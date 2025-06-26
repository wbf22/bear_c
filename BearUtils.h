#ifndef LIST
#define LIST

#include <stdlib.h>


/*
    A list of elements of any type with the following operation complexities
    - len(List* list) -> O(1)
    - push(List* list, void* data) -> O(1) amoritized
    - push_front(List* list, void* data) -> O(1) amoritized
    - pop(List* list) -> O(1)
    - pop_front(List* list) -> O(1)
    - clear() -> O(1)
    - free() -> O(n)


    this list implementation does not manage memory for you, and data passed in
    will not be freed unless you use the free() function to free the entire list.

*/
typedef struct List {
    char* data[10];
    size_t start;
    size_t end;
    size_t len;
} List;

/*
    returns the length of a list
*/
size_t len(List* list) {
    return list->len;
}


static void calc_len_and_set(List* list) {
    if (list->start < list->end) {
        list->len = list->end - list->start;   
    }
    else {
        size_t array_len = sizeof(list->data) / sizeof(list->data[0]);
        list->len = array_len - list->start + list->end;
    }
}


/*
    function to add to the end of a list.
    may resize interanl size of the list,
    but amoritized is a constant operation
*/
void push(List* list, void* data) {

}

/*
    function to add to the front of a list.
    may resize interanl size of the list,
    but amoritized is a constant operation
*/
void push_front(List* list, void* data) {

}


/*
    removes and retrieves that last element of the list.
    returns NULL when the list is empty
*/
char* pop(List* list) {

    if (list->len == 0) {
        return NULL;
    }

    list->end = list->end - 1;
    char* data = list->data[list->end];
    list->data[list->end] = NULL;

    calc_len_and_set(list);

    return data;
}

/*
    removes and retrieves that first element of the list.
    returns NULL when the list is empty
*/
char* pop_front(List* list) {

    if (list->len == 0) {
        return NULL;
    }

    char* data = list->data[list->start];
    list->data[list->start] = NULL;
    list->start = list->start + 1;

    calc_len_and_set(list);

    return data;
}



#endif