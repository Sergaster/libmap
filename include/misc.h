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

#ifndef MAP_MISC_H
#define MAP_MISC_H

#include <string.h>

#include "map.h"

static inline int map_sv_select(MAP *map, char *k, void **v, uint64_t *vlen)
{
	return map_select(map, k, strlen(k) + 1, v, vlen);
}
static inline int map_sv_insert(MAP *map, char *k, void *v, uint64_t vlen)
{
	return map_insert(map, k, strlen(k) + 1, v, vlen);
}
static inline int map_sv_update(MAP *map, char *k, void *v, uint64_t vlen)
{
	return map_update(map, k, strlen(k) + 1, v, vlen);
}
static inline int map_sv_delete(MAP *map, char *k)
{
	return map_delete(map, k, strlen(k) + 1);
}

int map_uv_select(MAP *map, uint64_t k, void **v, uint64_t *vlen);
int map_uv_insert(MAP *map, uint64_t k, void *v, uint64_t vlen);
int map_uv_update(MAP *map, uint64_t k, void *v, uint64_t vlen);
int map_uv_delete(MAP *map, uint64_t k);

int map_vu_select(MAP *map, void *k, uint64_t klen, uint64_t *v);
int map_vu_insert(MAP *map, void *k, uint64_t klen, uint64_t v);
int map_vu_update(MAP *map, void *k, uint64_t klen, uint64_t v);
int map_vu_delete(MAP *map, void *k, uint64_t klen);

int map_uu_select(MAP *map, uint64_t k, uint64_t *v);
int map_uu_insert(MAP *map, uint64_t k, uint64_t v);
int map_uu_update(MAP *map, uint64_t k, uint64_t v);
int map_uu_delete(MAP *map, uint64_t k);

#endif
