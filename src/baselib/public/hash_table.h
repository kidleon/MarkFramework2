#ifndef __HASH_TABLE_H__
#define __HASH_TABLE_H__


struct HASH_NODE
{
	int64 key; // key 값
	void* data; // data 값
	struct HASH_NODE* prev;	// 이전 노드 포인터
	struct HASH_NODE* next; // 다음 노드 포인터
};

struct HASH_TABLE
{
	size_t bucket_size; // 버킷 크기
	size_t node_count; // 노드 카운트
	HASH_NODE** buckets; // 버킷 포인터
	//HASH_NODE* (*create_node)(int64 key, void* data); // 노드 생성 함수 포인터
	//void (*release_node)(HASH_NODE* node); // 노드 해제 함수 포인터
};

/**
* @brief 해시 테이블을 생성합니다.
* @param bucket_size 해시 테이블의 버킷 크기
* @return 생성된 해시 테이블 포인터
* @note 버킷 크기는 해시 테이블의 성능에 영향을 미칩니다. 적절한 버킷 크기를 설정해야 합니다.
*/
MARKENGINE_C_API HASH_TABLE* create_hash_table(
	size_t bucket_size
);
	//HASH_NODE* (*create_node)(int64 key, void* data), // 노드 생성 함수 포인터
	//void (*release_node)(HASH_NODE* node) // 노드 해제 함수 포인터

/**
* @brief 해시 테이블을 삭제합니다.
* @param hash_table 삭제할 해시 테이블 포인터
* @return 없음
* @note 해시 테이블의 모든 노드가 삭제됩니다. 해시 테이블을 사용한 후에는 반드시 이 함수를 호출하여 메모리를 해제해야 합니다.
*/
MARKENGINE_C_API void delete_hash_table(
	HASH_TABLE* hash_table
);

/**
* @brief 해시 테이블에 노드를 추가합니다.
* @param hash_table 해시 테이블 포인터
* @param key 노드의 키 값
* @param node 추가할 노드 포인터
* @return 0 성공, -1 실패
* @note 해시 테이블에 이미 존재하는 키 값이면 기존 노드를 덮어씁니다.
*/
MARKENGINE_C_API int insert_hash_node(
	HASH_TABLE* hash_table, 
	int64 key,
	HASH_NODE* node
);

/**
* @brief 해시 테이블에서 노드를 삭제합니다.
* @param hash_table 해시 테이블 포인터
* @param key 삭제할 노드의 키 값
* @return 0 성공, -1 실패
* @note 삭제된 노드의 데이터는 해시 테이블에서 제거되지만, 메모리는 해제되지 않습니다.
*/
MARKENGINE_C_API int delete_hash_node(
	HASH_TABLE* hash_table,
	int64 key
);

// 해시 테이블에서 노드 검색
/**
* @brief 해시 테이블에서 노드를 검색합니다.
* @param hash_table 해시 테이블 포인터
* @param key 검색할 노드의 키 값
* @return 검색된 노드의 데이터 포인터
* @note 검색된 노드가 없으면 NULL을 반환합니다.
*/
MARKENGINE_C_API void* query_hash_node(
	HASH_TABLE* hash_table, 
	int64 key
);


#endif // __HASH_TABLE_H__
