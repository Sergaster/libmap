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

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "map.h"

#define CDB_HASHSTART	5381

static uint32_t map_hash(const char *s, int len)
{
	uint32_t h = CDB_HASHSTART;
	int i;

	if (s == NULL)
		return h;
	for (i = 0; i < len; i++)
		h = (h + (h << 5)) ^ s[i];
	return h;
}

static const uint32_t primes_sizes[] = {
	3, 7, 11, 17, 23, 29, 37, 47, 59, 71, 89, 107, 131,
	163, 197, 239, 293, 353, 431, 521, 631, 761, 919,
	1103, 1327, 1597, 1931, 2333, 2801, 3371, 4049, 4861,
	5839, 7013, 8419, 10103, 12143, 14591, 17519, 21023,
	25229, 30293, 36353, 43627, 52361, 62851, 75431, 90523,
	108631, 130363, 156437, 187751, 225307, 270371, 324449,
	389357, 467237, 560689, 672827, 807403, 968897, 1162687,
	1395263, 1674319, 2009191, 2411033, 2893249, 3471899,
	4166287, 4999559, 5999471, 7199369
};

#define LOAD_FACTOR		1
#define S_ARRAY_SIZE(x)		(sizeof(x) / sizeof(x[0]))

MAP *map_new(int index)
{
	MAP *map;
	rs_bucket *b;
	int i;
	uint32_t size;

	if (index >= S_ARRAY_SIZE(primes_sizes))
		index = S_ARRAY_SIZE(primes_sizes) - 1;
	size = primes_sizes[index];
	map = calloc(1, sizeof(*map));
	if (map == NULL)
		return NULL;
	map->index = index;
	map->size = size;
	map->count = 0;
	map->table = calloc(size, sizeof(*map->table));
	if (map->table == NULL)
		goto err;
	for (i = 0; i < size; i++) {
		b = &map->table[i];
		b->arr = NULL;
		b->count = 0;
	}
	return map;
err:
	free(map);
	return NULL;
}

MAP *map_new0(void)
{
	return map_new(0);
}

void map_free(MAP *map)
{
	rs_bucket *b;
	rs_keyval *kv;
	int i, j;

	for (i = 0; i < map->size; i++) {
		b = &map->table[i];
		for (j = 0; j < b->count; j++) {
			kv = &b->arr[j];
			free(kv->key);
			if (kv->val_len)
				free(kv->value);
		}
		free(b->arr);
	}
	free(map->table);
	free(map);
	return;
}

int map_select(MAP *map, void *k, uint64_t klen, void **v, uint64_t *vlen)
{
	rs_bucket *b;
	rs_keyval *kv;
	int j;
	uint32_t h;

	h = map_hash(k, klen);
	b = &map->table[h % map->size];
	for (j = 0; j < b->count; j++) {
		kv = &b->arr[j];
		if (klen == kv->key_len && !memcmp(k, kv->key, klen)) {
			*v = kv->value;
			*vlen = kv->val_len;
			return 0;
		}
	}
	return -1;
}

static void map_transform(MAP *map);

static int map_insert_aux(MAP *map, void *k, uint64_t klen, void *v, uint64_t vlen, bool grow)
{
	rs_bucket *b;
	rs_keyval *kv, *arr;
	int j;
	uint32_t h;

	if (v == NULL && vlen == 0)
		return -1;
	h = map_hash(k, klen);
	b = &map->table[h % map->size];
	for (j = 0; j < b->count; j++) {
		kv = &b->arr[j];
		if (klen == kv->key_len && !memcmp(k, kv->key, klen))
			return -1;
	}

	arr = realloc(kv, (b->count + 1) * sizeof(rs_keyval));
	if (arr == NULL)
		return -1;
	b->arr = arr;
	kv = &b->arr[b->count];
	kv->key = malloc(klen);
	if (kv->key == NULL)
		return -1;
	memcpy(kv->key, k, klen);
	if (vlen) {
		kv->value = malloc(vlen);
		if (kv->value == NULL)
			goto err;
		memcpy(kv->value, v, vlen);
	} else {
		kv->value = v;
	}
	kv->val_len = vlen;
	b->count++;
	map->count++;
	if (grow)
		map_transform(map);
	return 0;
err:
	free(kv->key);
	return -1;
}

static void map_transform(MAP *map)
{
	MAP *map2, swap;
	rs_bucket *b;
	rs_keyval *kv;
	int index, i, j;

	if (map->count < LOAD_FACTOR * map->size)
		return;
	index = map->index + 1;
	if (index > S_ARRAY_SIZE(primes_sizes) - 1)
		index = S_ARRAY_SIZE(primes_sizes) - 1;
	if (index == map->index)	// the biggest size
		return;
	map2 = map_new(index);
	if (map2 == NULL) /* the map is unoptimized left */
		return;
	for (i = 0; i < map->size; i++) {
		b = &map->table[i];
		for (j = 0; j < b->count; j++) {
			kv = &b->arr[j];
			if (map_insert_aux(map2, kv->key, kv->key_len, kv->value, kv->val_len, false))
				goto out;
		}
		free(b->arr);
	}
	swap = *map;
	*map = *map2;
	*map2 = swap;
out:
	map_free(map2);
	return;
}

int map_insert(MAP *map, void *k, uint64_t klen, void *v, uint64_t vlen)
{
	return map_insert_aux(map, k, klen, v, vlen, true);
}

int map_update(MAP *map, void *k, uint64_t klen, void *v, uint64_t vlen)
{
	rs_bucket *b;
	rs_keyval *kv;
	int j;
	uint32_t h;

	h = map_hash(k, klen);
	b = &map->table[h % map->size];
	for (j = 0; j < b->count; j++) {
		kv = &b->arr[j];
		if (klen == kv->key_len && !memcmp(k, kv->key, klen)) {
			if ((kv->val_len && vlen == 0) || (kv->val_len == 0 && vlen))	/* special case */
				return -1;
			if (vlen) {
				void *s;
				s = malloc(vlen);
				if (s == NULL)
					return -1;
				memcpy(s, v, vlen);
				free(kv->value);
				kv->value = s;
			} else {
				kv->value = v;
			}
			kv->val_len = vlen;
			return 0;
		}
	}
	return map_insert_aux(map, k, klen, v, vlen, true);
}

int map_delete(MAP *map, void *k, uint64_t klen)
{
	rs_bucket *b;
	rs_keyval *kv;
	int j;
	uint32_t h;

	h = map_hash(k, klen);
	b = &map->table[h % map->size];
	for (j = 0; j < b->count; j++) {
		kv = &b->arr[j];
		if (klen == kv->key_len && !memcmp(k, kv->key, klen)) {
			free(kv->key);
			if (kv->val_len)
				free(kv->value);
			memmove(kv, kv + 1, (b->count - j - 1) * sizeof(*kv));
			b->count--;
			map->count--;
			return 0;
		}
	}
	return -1;
}

int map_foreach(MAP *map, map_foreach_cb cb, void *user)
{
	rs_bucket *b;
	rs_keyval *kv;
	int i, j;

	for (i = 0; i < map->size; i++) {
		b = &map->table[i];
		for (j = 0; j < b->count; j++) {
			kv = &b->arr[j];
			if (cb(map, kv->key, kv->key_len, kv->value, kv->val_len, user))
				return -1;
		}
	}
	return 0;
}
