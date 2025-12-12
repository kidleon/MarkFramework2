#ifndef __LINKED_LIST_H__
#define __LINKED_LIST_H__


// 이중 연결 리스트 노드 구조체
struct LINK_NODE
{
	struct LINK_NODE* prev;
	struct LINK_NODE* next;
	void* data;
};


// 이중 연결 리스트 구조체
struct LINKED_LIST
{
	struct LINK_NODE* head;
	struct LINK_NODE* tail;
	int size;
};


/**
* init_linked_list
* @brief linked_list 객체를 NULL값으로 초기화 합니다.
* @param linked_list 초기화 할 LINKED_LIST 포인터
*/
MARK_BASELIB_C_API void init_linked_list(
	struct LINKED_LIST* linked_list
);

/**
* linked_list_push_front
* @brief LINKED_LIST에 Node를 앞에 추가 합니다.
* @param linked_list 작업 linked_list 포인터
* @param node 추가할 node
*/
MARK_BASELIB_C_API void linked_list_push_front(
	struct LINKED_LIST* linked_list,
	struct LINK_NODE* node
);

/**
* linked_list_push_back
* @brief LINKED_LIST에 Node를 맨 뒤에 추가 합니다.
* @param linked_list 작업 linked_list 포인터
* @param node 추가할 node
*/
MARK_BASELIB_C_API void linked_list_push_back(
	struct LINKED_LIST* linked_list,
	struct LINK_NODE* node
);

/**
* linked_list_pop_front
* @brief LINKED_LIST에서 제일 앞에 있는 노드를 반환합니다. 반환된 노드는 linked_list에서 삭제됩니다.
* @param linked_list 작업 linked_list 포인터
* @return 반환된 NODE 포인터, 실패시 NULL
*/
MARK_BASELIB_C_API struct LINK_NODE* linked_list_pop_front(
	struct LINKED_LIST* linked_list
);

/**
* linked_list_pop_back
* @brief LINKED_LIST에서 제일 뒤에 있는 노드를 반환합니다. 반환된 노드는 linked_list에서 삭제됩니다.
* @param linked_list 작업 linked_list 포인터
* @return 반환된 NODE 포인터, 실패시 NULL
*/
MARK_BASELIB_C_API struct LINK_NODE* linked_list_pop_back(
	struct LINKED_LIST* linked_list
);

/**
* linked_list_insert_node
* @brief pos노드 앞에 새노드(node)를 추가 합니다.
* @param linked_list 작업 linked_list 포인터
* @param pos 기준이 되는 노드.. pos노드 앞에 새 노드가 삽입됩니다.
* @param node 삽입할 노드
*/
MARK_BASELIB_C_API void linked_list_insert_node(
	struct LINKED_LIST* linked_list,
	struct LINK_NODE* pos,
	struct LINK_NODE* node
);

/**
* linked_list_remove_node
* @brief 노드를 삭제 합니다.
* @param linked_list 작업 linked_list 포인터
* @param node 삭제할 노드
*/
MARK_BASELIB_C_API void linked_list_remove_node(
	struct LINKED_LIST* linked_list,
	struct LINK_NODE* node
);

/**
* linked_list_empty
* @brief linked_list가 비어있는지 확인 합니다.
* @param linked_list 작업 linked_list 포인터
* @return 비어있거나 linked_list포인터가 NULL일 경우 TRUE, 아니면 FALSE
*/
MARK_BASELIB_C_API BOOL linked_list_empty(
	struct LINKED_LIST* linked_list
);

/**
* linked_list_sort
* @brief 노드를 함수 포인터(comp)를 이용하여 정렬 합니다.
* @param linked_list 작업 linked_list 포인터
* @param comp 정렬 비교를 위한 함수 포인터 (같을시 0 반환, 다를시 -1또는 1반환)
* @example
*	int compare_ints(void* a, void* b)
*	{
*		int int_a = *(int*)a;
*		int int_b = *(int*)b;
*		return int_a - int_b;
*	}

*	// 리스트 정렬
*	sort_list(&linked_list, compare_ints);
*/
MARK_BASELIB_C_API void linked_list_sort(
	struct LINKED_LIST* linked_list,
	int (*comp)(void*, void*)
);


#endif // __LINKED_LIST_H__