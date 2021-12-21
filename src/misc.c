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

#include <endian.h>

#include "misc.h"

int map_uv_select(MAP *map, uint64_t k, void **v, uint64_t *vlen)
{
	uint64_t bk;
	bk = htobe64(k);
	return map_select(map, &bk, sizeof(bk), v, vlen);
}

int map_uv_insert(MAP *map, uint64_t k, void *v, uint64_t vlen)
{
	uint64_t bk;
	bk = htobe64(k);
	return map_insert(map, &bk, sizeof(bk), v, vlen);
}

int map_uv_update(MAP *map, uint64_t k, void *v, uint64_t vlen)
{
	uint64_t bk, bv;
	bk = htobe64(k);
	return map_update(map, &bk, sizeof(bk), v, vlen);
}

int map_uv_delete(MAP *map, uint64_t k)
{
	uint64_t bk;
	bk = htobe64(k);
	return map_delete(map, &bk, sizeof(bk));
}

int map_vu_select(MAP *map, void *k, uint64_t klen, uint64_t *v)
{
	uint64_t *bv, vlen;
	void *val;
	int ret;
	
	ret = map_select(map, k, klen, &val, &vlen);
	if (ret || vlen != sizeof(*bv))
		return ret;
	bv = val;
	*v = be64toh(*bv);
	return 0;
}

int map_vu_insert(MAP *map, void *k, uint64_t klen, uint64_t v)
{
	uint64_t bv;
	bv = htobe64(v);
	return map_insert(map, k, klen, &bv, sizeof(bv));
}

int map_vu_update(MAP *map, void *k, uint64_t klen, uint64_t v)
{
	uint64_t bv;
	bv = htobe64(v);
	return map_update(map, k, klen, &bv, sizeof(bv));
}

int map_vu_delete(MAP *map, void *k, uint64_t klen)
{
	return map_delete(map, k, klen);
}

int map_uu_select(MAP *map, uint64_t k, uint64_t *v)
{
	uint64_t bk, *bv, vlen;
	void *val;
	int ret;
	
	bk = htobe64(k);
	ret = map_select(map, &bk, sizeof(bk), &val, &vlen);
	if (ret || vlen != sizeof(*bv))
		return ret;
	bv = val;
	*v = be64toh(*bv);
	return 0;
}

int map_uu_insert(MAP *map, uint64_t k, uint64_t v)
{
	uint64_t bk, bv;
	bk = htobe64(k);
	bv = htobe64(v);
	return map_insert(map, &bk, sizeof(bk), &bv, sizeof(bv));
}

int map_uu_update(MAP *map, uint64_t k, uint64_t v)
{
	uint64_t bk, bv;
	bk = htobe64(k);
	bv = htobe64(v);
	return map_update(map, &bk, sizeof(bk), &bv, sizeof(bv));
}

int map_uu_delete(MAP *map, uint64_t k)
{
	return map_uv_delete(map, k);
}
