#include "pch.h"
#include "idgen.h"


typedef struct idgen_t
{
	UINT32 min_id;
	UINT32 max_id;
	UINT32 total_count;
	UINT32 curr_idx;
	UINT32* table;
} tag_idgen;

HANDLE idgen_create(
	UINT32 min_id,
	UINT32 max_id
)
{
	struct idgen_t* idgen = (struct idgen_t*)malloc(sizeof(struct idgen_t));
	if (!idgen)
		return NULL;

	idgen->min_id = min_id;
	idgen->max_id = max_id;
	idgen->total_count = (UINT32)(max_id - min_id + 1);
	idgen->curr_idx = 0;
	idgen->table = (INT32*)malloc(sizeof(INT32) * idgen->total_count);

	if (!idgen->table)
	{
		free(idgen);
		return NULL;
	}

	UINT32 idx = 0;
	for (UINT32 i = min_id; i < max_id + 1; ++i, ++idx)
		idgen->table[idx] = i;

	return (HANDLE)idgen;
}

void idgen_destroy(
	HANDLE idgen
)
{
	if (!idgen) return;

	struct idgen_t* pIDGen = (struct idgen_t*)idgen;
	if (pIDGen->table)
		free(pIDGen->table);
	free(pIDGen);
}

UINT32 idgen_getid(
	HANDLE idgen
)
{
	if (!idgen) return INVALID_UNIQUEID;

	struct idgen_t* pIDGen = (struct idgen_t*)idgen;

	if (pIDGen->total_count <= pIDGen->curr_idx)
		return INVALID_UNIQUEID;

	UINT32 ID = pIDGen->table[pIDGen->curr_idx];
	pIDGen->curr_idx++;

	return ID;
}

void idgen_release(
	HANDLE hIDGen,
	UINT32 id
)
{
	if (!hIDGen || INVALID_UNIQUEID == id) return;

	struct idgen_t* pIDGen = (struct idgen_t*)hIDGen;
	if (pIDGen->max_id < id)
		return;

	if (!pIDGen->curr_idx)
		return;

	pIDGen->curr_idx--;
	pIDGen->table[pIDGen->curr_idx] = id;
}