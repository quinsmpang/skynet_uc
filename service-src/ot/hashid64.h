#ifndef __HASHID64__H__
#define __HASHID64__H__

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

struct hashid64_node {
	uint64_t id64;
	struct hashid64_node *next;
};

struct hashid64 {
	int hashmod;
	int cap;
	int count;
	struct hashid64_node *id64;
	struct hashid64_node **hash;
};

static void
hashid64_init(struct hashid64 *hi, int max) {
	int i;
	int hashcap;
	hashcap = 16;
	while (hashcap < max) {
		hashcap *= 2;
	}
	hi->hashmod = hashcap - 1;
	hi->cap = max;
	hi->count = 0;
	hi->id64 = skynet_malloc(max * sizeof(struct hashid64_node));
	for (i=0;i<max;i++) {
		hi->id64[i].id64 = -1;
		hi->id64[i].next = NULL;
	}
	hi->hash = skynet_malloc(hashcap * sizeof(struct hashid64_node *));
	memset(hi->hash, 0, hashcap * sizeof(struct hashid64_node *));
}

static void
hashid64_clear(struct hashid64 *hi) {
	skynet_free(hi->id64);
	skynet_free(hi->hash);
	hi->id64 = NULL;
	hi->hash = NULL;
	hi->hashmod = 1;
	hi->cap = 0;
	hi->count = 0;
}

static int
hashid64_lookup(struct hashid64 *hi, uint64_t id64) {
	int h = id64 & hi->hashmod;
	struct hashid64_node * c = hi->hash[h];
	while(c) {
		if (c->id64 == id64)
			return c - hi->id64;
		c = c->next;
	}
	return -1;
}

static int
hashid64_remove(struct hashid64 *hi, uint64_t id64) {
	int h = id64 & hi->hashmod;
	struct hashid64_node * c = hi->hash[h];
	if (c == NULL)
		return -1;
	if (c->id64 == id64) {
		hi->hash[h] = c->next;
		goto _clear;
	}
	while(c->next) {
		if (c->next->id64 == id64) {
			struct hashid64_node * temp = c->next;
			c->next = temp->next;
			c = temp;
			goto _clear;
		}
		c = c->next;
	}
	return -1;
_clear:
	c->id64 = -1;
	c->next = NULL;
	--hi->count;
	return c - hi->id64;
}

static int
hashid64_insert(struct hashid64 * hi, uint64_t id64) {
	struct hashid64_node *c = NULL;
	int i;
	for (i=0;i<hi->cap;i++) {
		int index = (i+id64) % hi->cap;
		if (hi->id64[index].id64 == -1) {
			c = &hi->id64[index];
			break;
		}
	}
	assert(c);
	++hi->count;
	c->id64 = id64;
	assert(c->next == NULL);
	int h = id64 & hi->hashmod;
	if (hi->hash[h]) {
		c->next = hi->hash[h];
	}
	hi->hash[h] = c;
	
	return c - hi->id64;
}

static inline int
hashid64_full(struct hashid64 *hi) {
	return hi->count == hi->cap;
}

#endif  /* __HASHID64__H__ */
