/* C library for manipulating key/value data sets
   Copyright (C) 2021 Sergey V. Kostyuk

   This file was written by Sergey V. Kostyuk <kostyuk.sergey79@gmail.com>

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, see
   <https://www.gnu.org/licenses/>.  */

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

MAP *map_new0(void);
void map_free(MAP *map);

int map_select(MAP *map, void *k, uint64_t klen, void **v, uint64_t *vlen);
int map_insert(MAP *map, void *k, uint64_t klen, void *v, uint64_t vlen);
int map_update(MAP *map, void *k, uint64_t klen, void *v, uint64_t vlen);
int map_delete(MAP *map, void *k, uint64_t klen);

typedef int (*map_foreach_cb) (MAP *map, void *k, uint64_t klen, void *v, uint64_t vlen, void *user);
int map_foreach(MAP *map, map_foreach_cb cb, void *user);

#endif
