#ifndef RS_MAP_H
#define RS_MAP_H

#include <stdint.h>

typedef struct rs_keyval_t {
	void *key;
	uint64_t key_len;
	void *value;
	uint64_t val_len;
} rs_keyval;

typedef struct rs_bucket_t {
	rs_keyval *arr;
	uint64_t count;
} rs_bucket;

typedef struct rs_map_t {
	int index;
	int size;
	uint64_t count;
	rs_bucket *table;
} MAP;

MAP *rs_map_new0(void);
void map_free(MAP *map);

int map_select(MAP *map, void *k, uint64_t klen, void **v, uint64_t *vlen);
int map_insert(MAP *map, void *k, uint64_t klen, void *v, uint64_t vlen);
int map_update(MAP *map, void *k, uint64_t klen, void *v, uint64_t vlen);
int map_delete(MAP *map, void *k, uint64_t klen);

typedef int (*map_foreach_cb) (MAP *map, void *k, uint64_t klen, void *v, uint64_t vlen, void *user);
int map_foreach(MAP *map, map_foreach_cb cb, void *user);

#endif
