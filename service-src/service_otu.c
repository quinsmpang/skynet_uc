#include "skynet.h"
#include "skynet_socket.h"
#include "skynet_timer.h"
#include "ot/hashid64.h"

#include <arpa/inet.h>
 
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <stdarg.h>


#include "ot/ot.c"


#define htonll(x)   ((((uint64_t)htonl(x)) << 32) + htonl((x) >> 32))
#define ntohll(x)   ((((uint64_t)ntohl(x)) << 32) + ntohl((x) >> 32))



struct udp_peer {
	uint8_t KEY[16];
	uint8_t pKEY[16];
	uint8_t Addr[16];
};

struct otu {
	struct skynet_context *ctx;
	int service_id;
	uint32_t watchdog;
	uint32_t deviceDB;
	int client_tag;
	int max_peer;
	struct hashid64 hash;
	struct udp_peer *peer;
};

struct otu *
otu_create(void) {
	struct otu * u = skynet_malloc(sizeof(*u));
	memset(u,0,sizeof(*u));
	u->service_id = -1;
	return u;
}

void
otu_release(struct otu *u) {
	struct skynet_context *ctx = u->ctx;

	if (u->service_id >= 0) {
		skynet_socket_close(ctx, u->service_id);
	}

	hashid64_clear(&u->hash);
	skynet_free(u->peer);
	skynet_free(u);
}


static void
dispatch_socket_msg(struct otu *u, const struct skynet_socket_message * message, int sz) {
	struct skynet_context * ctx = u->ctx;

	switch(message->type) {

	case SKYNET_SOCKET_TYPE_CONNECT: {
		assert (message->id == u->service_id);
		break;
		}

	case SKYNET_SOCKET_TYPE_UDP: {
		char* pkt = message->buffer;
		int pkt_sz = message->ud;
		int addr_sz = 0;
		const char * addr = skynet_socket_udp_address(message, &addr_sz);
		char* last_err = NULL;

		//Ver len check
		if(pkt_sz < sizeof(ot_pkt_hdr_t)){
			last_err = "1.ShortHdr.";
			goto exit_drop;
		}

		ot_pkt_hdr_t *hdr = (ot_pkt_hdr_t *)pkt;
		u16_t hdr_len = ntohs(hdr->len);

		if(hdr_len > pkt_sz || hdr_len < sizeof(ot_pkt_hdr_t) || hdr_len > OT_PACKET_SIZE_MAX){		//len check
			last_err = "2.HdrLenErr.";
			goto exit_drop;
		}

		if(hdr->proto_ver != OT_PROTO_VER_V3A){
			last_err = "3.HdrVerErr.";
			goto exit_drop;
		}

#define HDR_ONLY (hdr_len == sizeof(ot_pkt_hdr_t))
#define DID_IS_BROADCAST (IS_ALL_FF(hdr->did.byte, sizeof(hdr->did.byte)))

		if(HDR_ONLY){

			//时间戳请求处理
			if(DID_IS_BROADCAST){	

				hdr->ts = htonl(skynet_now()/100 + skynet_starttime());

				//int err = 
				skynet_socket_udp_send(ctx, message->id, addr, pkt, pkt_sz);

			}

			//保活
			else {

 				uint64_t did64 = ntohll(hdr->did.u64);
				int idx = hashid64_lookup(&u->hash, did64);
				if (idx >= 0) {
					struct udp_peer *p = &u->peer[idx];
					(void)p;
					//dispatch_msg(u, p, message->id, message->buffer, message->ud);
					goto exit_drop;

				} else {

					//todo 提交DID未知请求
					goto exit_drop;
				}


			}


		}


		break;


		//get key, decrypt packet.

		//dispatch_msg


exit_drop:
		skynet_free(message->buffer);

		break;
	}


	default:
		skynet_error(ctx, "OTU: unknown type(%d)", message->type);
		break;
	}
}



static int
_cb_otu(struct skynet_context * ctx, void * ud, int type, int session, uint32_t source, const void * msg, size_t sz) {
	struct otu *u = ud;
	switch(type) {
	case PTYPE_TEXT:
//		_ctrl(u , msg , (int)sz);
		break;


/*

	case u->client_tag: {
		if (sz <=4 ) {
			skynet_error(ctx, "Invalid client message from %x",source);
			break;
		}
		// The last 4 bytes in msg are the id of socket, write following bytes to it
		const uint8_t * idbuf = msg + sz - 4;
		uint32_t uid = idbuf[0] | idbuf[1] << 8 | idbuf[2] << 16 | idbuf[3] << 24;
		int id = hashid_lookup(&u->hash, uid);
		if (id>=0) {
			// don't send id (last 4 bytes)
			skynet_socket_send(ctx, uid, (void*)msg, sz-4);
			// return 1 means don't free msg
			return 1;
		} else {
			skynet_error(ctx, "Invalid client id %d from %x",(int)uid,source);
			break;
		}

	}
*/

	case PTYPE_SOCKET:
		dispatch_socket_msg(u, msg, (int)(sz-sizeof(struct skynet_socket_message)));

		break;
	}
	return 0;
}



static int
start_service(struct otu *u, char * service_addr) {
	struct skynet_context * ctx = u->ctx;
	char * portstr = strchr(service_addr,':');
	const char * host = "";
	int port;
	if (portstr == NULL) {
		port = strtol(service_addr, NULL, 10);
		if (port <= 0) {
			skynet_error(ctx, "Invalid gate address %s",service_addr);
			return 1;
		}
	} else {
		port = strtol(portstr + 1, NULL, 10);
		if (port <= 0) {
			skynet_error(ctx, "Invalid gate address %s",service_addr);
			return 1;
		}
		portstr[0] = '\0';
		host = service_addr;
	}

	u->service_id = skynet_socket_udp(ctx, host, port) ;
	if (u->service_id < 0) {
		return 1;
	}
	skynet_socket_start(ctx, u->service_id);
	return 0;
}




int
otu_init(struct otu *u , struct skynet_context * ctx, char * parm) {
	if (parm == NULL)
		return 1;
	int max = 0;
	int sz = strlen(parm)+1;
	char watchdog[sz];
	char binding[sz];
	int client_tag = 0;

	int n = sscanf(parm, "%s %s %d %d", watchdog, binding, &client_tag, &max);
	if (n<4) {
		skynet_error(ctx, "Invalid otu parm %s", parm);
		return 1;
	}

	skynet_error(ctx,  "Watchdog:%s, Binding:%s, Protocol:%d, MaxPeer:%d", watchdog, binding, client_tag, max);

	if (max <=0 ) {
		skynet_error(ctx, "Need max connection");
		return 1;
	}

	if (client_tag == 0) {
		client_tag = PTYPE_CLIENT;
	}

	if (watchdog[0] == '!') {
		u->watchdog = 0;
	} else {
		u->watchdog = skynet_queryname(ctx, watchdog);
		if (u->watchdog == 0) {
			skynet_error(ctx, "Invalid watchdog %s",watchdog);
			return 1;
		}
	}

	u->ctx = ctx;

	hashid64_init(&u->hash, max);
	u->peer = skynet_malloc(max * sizeof(struct udp_peer));
	memset(u->peer, 0, max *sizeof(struct udp_peer));
	u->max_peer = max;

	int i;
	for (i=0;i<max;i++) {
	//	u->peer[i].id = -1;
	}
	
	u->client_tag = client_tag;

	skynet_callback(ctx,u,_cb_otu);

	return start_service(u,binding);
}































