#include "pch.h"
#include "linked_list.h"


void init_linked_list(
    struct LINKED_LIST* linked_list
)
{
	linked_list->head = linked_list->tail = NULL;
	linked_list->size = 0;
}

void linked_list_push_front(
    struct LINKED_LIST* linked_list,
    struct LINK_NODE* node
)
{
    node->prev = NULL;
    node->next = linked_list->head;

    if (linked_list->head)
        linked_list->head->prev = node;
    else
        linked_list->tail = node;

    linked_list->head = node;
    linked_list->size++;
}

void linked_list_push_back(
    struct LINKED_LIST* linked_list,
    struct LINK_NODE* node
)
{
    node->next = NULL;
    node->prev = linked_list->tail;

    if (linked_list->tail)
        linked_list->tail->next = node;
    else
        linked_list->head = node;

    linked_list->tail = node;
    linked_list->size++;
}

struct LINK_NODE* linked_list_pop_front(
    struct LINKED_LIST* linked_list
)
{
    if (!linked_list->head)
        return NULL;

    struct LINK_NODE* node = linked_list->head;

    linked_list->head = node->next;
    if (linked_list->head)
        linked_list->head->prev = NULL;
    else
        linked_list->tail = NULL;

    linked_list->size--;
    
    return node;
}

struct LINK_NODE* linked_list_pop_back(
    struct LINKED_LIST* linked_list
)
{
    if (!linked_list->tail)
        return NULL;

    struct LINK_NODE* node = linked_list->tail;

    linked_list->tail = node->prev;
    if (linked_list->tail)
        linked_list->tail->next = NULL;
    else
        linked_list->head = NULL;

    linked_list->size--;
    return node;
}

void linked_list_insert_node(
    struct LINKED_LIST* linked_list,
    struct LINK_NODE* pos,
    struct LINK_NODE* node
)
{
    // 리스트가 비어있거나 pos가 NULL인 경우 push_back 사용
    if (!linked_list->head || !pos) 
    {
        linked_list_push_back(linked_list, node);
        return;
    }

    // pos 앞에 삽입
    node->next = pos;
    node->prev = pos->prev;

    if (pos->prev)
        pos->prev->next = node;
    else
        linked_list->head = node;  // pos가 head였던 경우

    pos->prev = node;
    linked_list->size++;
}

void linked_list_remove_node(
    struct LINKED_LIST* linked_list,
    struct LINK_NODE* node
)
{
    if (!linked_list || !node)
        return;

    // 이전 노드의 next를 node의 next로 연결
    if (node->prev) 
    {
        node->prev->next = node->next;
    }
    else 
    {
        // node가 head인 경우
        linked_list->head = node->next;
    }

    // 다음 노드의 prev를 node의 prev로 연결
    if (node->next)
    {
        node->next->prev = node->prev;
    }
    else
    {
        // node가 tail인 경우
        linked_list->tail = node->prev;
    }

    // 리스트의 크기 감소
    linked_list->size--;
}

BOOL linked_list_empty(
	struct LINKED_LIST* linked_list
)
{
    if (!linked_list)
        return TRUE;

	return (linked_list->size == 0);
}

void linked_list_sort(
    struct LINKED_LIST* linked_list,
    int (*comp)(void*, void*)
)
{
    if (!linked_list || linked_list->size < 2)
        return;

    int swapped;

    do 
    {
        swapped = 0;
        struct LINK_NODE* current = linked_list->head;

        while (current && current->next)
        {
            if (comp(current->data, current->next->data) > 0) 
            {
                // 노드의 data 포인터 교환
                void* temp = current->data;
                current->data = current->next->data;
                current->next->data = temp;
                swapped = 1;
            }
            current = current->next;
        }
    } while (swapped);
}