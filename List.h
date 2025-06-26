#ifndef LIST
#define LIST

#include <stdlib.h>
#include <stdio.h>
#include <string.h>


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
    char** data; // pointer to array of char pointers
    size_t data_size;

    size_t start;
    size_t end;
    size_t len;
} List;

List* new_list() {
    List *p_list = malloc(sizeof(List));
    if (p_list == NULL) {
        // handle malloc failure
        return NULL;
    }
    p_list->data = malloc(10 * sizeof(char*));
    if (p_list->data == NULL) {
        free(p_list);
        return NULL;
    }
    p_list->data_size = 10;
    p_list->start = 0;
    p_list->end = 0;
    p_list->len = 0;

    return p_list;
}

/*
    returns the length of a list
*/
size_t len(List* list) {
    return list->len;
}


/*
    doubles the internal array of the list

    returns 0 if successful
*/
int resize(List* list) {

    /*
        int size = size();
        T[] newData = (T[]) new Object[data.length * 2];
        if (startIndex > endIndex) {
            int startSize = this.data.length - startIndex;
            System.arraycopy(this.data, startIndex, newData, 0, startSize);
            System.arraycopy(this.data, 0, newData, startSize, endIndex);
        }
        else if (startIndex == endIndex) {
            // do nothing empty array
        }
        else {
            System.arraycopy(this.data, startIndex, newData, 0, size);
        }

        data = newData;
        startIndex = 0;
        endIndex = size;
    */

    size_t new_size = list->data_size * 2;
    char** new_data = malloc(new_size * sizeof(char*));
    if (new_data == NULL) {
        fprintf(stderr, "List couldn't get more memory on the system! Exiting...");
        exit(EXIT_FAILURE);
    }

    if (list->start > list->end) {

        // copy start index to array end
        memcpy(
            new_data, 
            list->data[list->start], 
            (list->data_size - list->start) * sizeof(char*)
        );
        // copy array start to end index
        memcpy(
            new_data, 
            list->data, 
            list->end * sizeof(char*)
        );
    }
    else {
        memcpy(
            new_data, 
            list->data[list->start], 
            (list->end - list->start) * sizeof(char*)
        );
    }

    // freeing list but not contents
    free(list->data);
    list->data = new_data;
    list->data_size = new_size;
    list->start = 0;
    list->end = list->len;

    return 0;
}

/*
    function to add to the end of a list.
    may resize interanl size of the list,
    but amoritized is a constant operation
*/
void push(List* list, void* data) {
    
    if (list->len == list->data_size - 1) {
        resize(list);
    }
    list->data[list->end] = data;
    list->end = (list->end + 1) % list->data_size;
    list->len = list->len + 1;
}

/*
    function to add to the front of a list.
    may resize interanl size of the list,
    but amoritized is a constant operation
*/
void push_front(List* list, void* data) {

    if (list->len == list->data_size - 1) {
        resize(list);
    }
    list->start = (list->start - 1 + list->data_size) % list->data_size;
    list->data[list->start] = data;
    list->len = list->len + 1;
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
    list->len = list->len - 1;

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
    list->len = list->len - 1;

    return data;
}

/*
    frees the list and stored objects
*/
void free_list(List* list) {
    // Free each string inside data
    for (size_t i = 0; i < list->data_size; i++) {
        free(list->data[i]);
    }
    // Free data
    free(list->data);
    // Free the struct itself
    free(list);
}




char* g_data(void* ptr, size_t size) {
    char *data = malloc(size);
    if (data == NULL) {
        fprintf(stderr, "Error allocating memory in! Exiting...");
        exit(EXIT_FAILURE);
    }

    memcpy(data, ptr, size);

    return data;
}


void p_data(void* dest, size_t size, char* data) {

    memcpy(dest, data, size);
    free(data);
}

#endif