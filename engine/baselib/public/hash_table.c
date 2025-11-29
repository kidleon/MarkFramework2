#include "pch.h"
#include "hash_table.h"


/*
HASH_TABLE* create_hash_table(
	size_t bucket_size,
	HASH_NODE* (*create_node)(int64 key, void* data), // 노드 생성 함수 포인터
	void (*release_node)(HASH_NODE* node) // 노드 해제 함수 포인터
)
*/
struct HASH_TABLE* create_hash_table(
	size_t bucket_size
)
{
	if (bucket_size == 0)
		return NULL;

	struct HASH_TABLE* hash_table = (struct HASH_TABLE*)malloc(sizeof(struct HASH_TABLE));
	if (!hash_table)
		return NULL;

	hash_table->bucket_size = bucket_size;
	hash_table->node_count = 0;
	hash_table->buckets = (struct HASH_NODE**)calloc(bucket_size, sizeof(struct HASH_NODE*));
	if (!hash_table->buckets)
	{
		free(hash_table);
		return NULL;
	}

	//hash_table->create_node = create_node;
	//hash_table->release_node = release_node;

	return hash_table;
}

void delete_hash_table(
	struct HASH_TABLE* hash_table
)
{
	if (!hash_table)
		return;

	for (size_t i = 0; i < hash_table->bucket_size; i++)
	{
		struct HASH_NODE* node = hash_table->buckets[i];
		while (node)
		{
			struct HASH_NODE* next_node = node->next;
			node->prev = NULL;
			node->next = NULL;
			node = next_node;
		}
	}

	free(hash_table->buckets);
	free(hash_table);
}

int insert_hash_node(
	struct HASH_TABLE* hash_table,
	int64 key,
	struct HASH_NODE* node
)
{
	if (!hash_table || !node)
		return -1;

	node->key = key;

	size_t index = key % hash_table->bucket_size;
	struct HASH_NODE* b_node = hash_table->buckets[index];
	while (b_node)
	{
		if (b_node->key == key)
		{
			//if (hash_table->release_node)
			//	hash_table->release_node(node);
			
			// 기존 노드가 있으면 1반환
			return 1;
		}
		b_node = b_node->next;
	}

	node->next = hash_table->buckets[index];
	if (hash_table->buckets[index])
		hash_table->buckets[index]->prev = node;

	node->prev = NULL;
	hash_table->buckets[index] = node;
	hash_table->node_count++;

	return 0;
}

int delete_hash_node(
	struct HASH_TABLE* hash_table,
	int64 key
)
{
	if (!hash_table)
		return -1;

	size_t index = key % hash_table->bucket_size;

	if (hash_table->buckets[index] == NULL)
		return -1;

	struct HASH_NODE* node = hash_table->buckets[index];
	while (node)
	{
		if (node->key == key)
		{
			if (node->prev)
				node->prev->next = node->next;
			else
				hash_table->buckets[index] = node->next;
			if (node->next)
				node->next->prev = node->prev;
			hash_table->node_count--;

			node->prev = NULL;
			node->next = NULL;

			return 0;
		}
		node = node->next;
	}

	return -1;
}

void* query_hash_node(
	struct HASH_TABLE* hash_table,
	int64 key
)
{
	if (!hash_table)
		return NULL;

	size_t index = key % hash_table->bucket_size;
	struct HASH_NODE* node = hash_table->buckets[index];
	while (node)
	{
		if (node->key == key)
			return node->data;
		node = node->next;
	}

	return NULL;
}