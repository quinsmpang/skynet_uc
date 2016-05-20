


#ifndef __OT_DEF_H__
#define __OT_DEF_H__


/***************************

	basic type def

****************************/

typedef signed char  s8_t;
typedef signed short s16_t;
typedef signed long  s32_t;
typedef signed long long s64_t;
typedef unsigned char  u8_t;
typedef unsigned short u16_t;
typedef unsigned long  u32_t;
typedef unsigned long long u64_t;

typedef unsigned int size_t;


/***************************

	const def

****************************/

#define OT_PROTO_VER_V3A 0x3121
#define OT_PROTO_VER_V3U 0x3021

#define  OT_PROTO_DEVICE_ID_SIZE 8
#define  OT_PROTO_MD5_SIZE 16


/***************************

	configable def

****************************/

#ifndef OT_PACKET_SIZE_MAX
#define OT_PACKET_SIZE_MAX 1000
#endif



/***************************

	sructure def

****************************/


struct  ot_pkt_hdr{
	u16_t proto_ver;
	u16_t len;
	union {
		u8_t byte[OT_PROTO_DEVICE_ID_SIZE];
		uint64_t u64;
	}did;
	u32_t ts;
	u8_t sign[OT_PROTO_MD5_SIZE];
};

typedef struct ot_pkt_hdr ot_pkt_hdr_t;




/***************************

	ultility macro

****************************/

#define IS_ALL_FF(ptr, len) ((len) == memchcmp((ptr), 0xFF, (len)))
#define IS_ALL_00(ptr, len) ((len) == memchcmp((ptr), 0x00, (len)))


#endif /* __OT_DEF_H__ */


