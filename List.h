#ifndef LIST
#define LIST

#include <stdlib.h>
#include <stdio.h>
#include <string.h>


/*
    A list of elements of any type with the following operation complexities
    - len(...) -> O(1)
    - push(...) -> O(1) amoritized
    - push_front(...) -> O(1) amoritized
    - pop(...) -> O(1)
    - pop_front(...) -> O(1)
    - clear() -> O(n)
    - free_list() -> O(n)
    - slice() -> O(n)


    This list implementation maintains copies of each element passed in on the heap.
    When removed from the list, these copies are freed. 
    
    When done with your list yoou must call free_list() to avoid memory leaks.

*/
typedef struct List {
    void** data; // pointer to array of char pointers
    size_t data_size;

    size_t start;
    size_t end;
    size_t len;
} List;



static void list_mem_error_exit_failing() {
    fprintf(stderr, "List couldn't get more memory on the system! Exiting...");
    exit(EXIT_FAILURE);
}


List* new_list_s(size_t size) {
    List *p_list = malloc(sizeof(List));
    if (p_list == NULL) {
        list_mem_error_exit_failing();
    }
    p_list->data = malloc(size * sizeof(void*));
    if (p_list->data == NULL) {
        free(p_list);
        list_mem_error_exit_failing();
    }
    p_list->data_size = size;
    p_list->start = 0;
    p_list->end = 0;
    p_list->len = 0;

    return p_list;
}

List* new_list() {
    return new_list_s(10);
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
    void** new_data = malloc(new_size * sizeof(void*));
    if (new_data == NULL) {
        list_mem_error_exit_failing();
    }

    if (list->start > list->end) {

        // copy start index to array end
        memcpy(
            new_data, 
            list->data + list->start, 
            (list->data_size - list->start) * sizeof(void*)
        );
        // copy array start to end index
        memcpy(
            new_data + (list->data_size - list->start), 
            list->data, 
            list->end * sizeof(void*)
        );
    }
    else {
        memcpy(
            new_data, 
            list->data + list->start, 
            (list->end - list->start) * sizeof(void*)
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
    Function to add to the end of a list.
    May resize interanl size of the list,
    but amoritized is a constant operation.

    A reference to either a stack or heap variable
    can be passed in. This value's data will be
    copied to a new pointer on the heap. This data
    on the heap will be freed when calling pop or
    clearing the list. (or with the free_list function)
*/
void push(List* list, void* data, size_t size) {
    
    if (list->len == list->data_size - 1) {
        resize(list);
    }

    void* copy = malloc(size);
    if (copy == NULL) {
        list_mem_error_exit_failing();
    }
    memcpy(copy, data, size);
    list->data[list->end] = copy;
    list->end = (list->end + 1) % list->data_size;
    list->len = list->len + 1;
}

/*
    Function to add to the end of a list.
    May resize interanl size of the list,
    but amoritized is a constant operation.

    A reference to either a stack or heap variable
    can be passed in. This value's data will be
    copied to a new pointer on the heap. This data
    on the heap will be freed when calling pop or
    clearing the list. (or with the free_list function)
*/
void push_front(List* list, void* data, size_t size) {

    if (list->len == list->data_size - 1) {
        resize(list);
    }

    void* copy = malloc(size);
    if (copy == NULL) {
        list_mem_error_exit_failing();
    }
    memcpy(copy, data, size);
    list->start = (list->start - 1 + list->data_size) % list->data_size;
    list->data[list->start] = copy;
    list->len = list->len + 1;
}

/*
    Removes and retrieves that last element of the list
    copying the data into dest. The data in the list is
    then freed.
*/
void pop(List* list, void* dest, size_t size) {

    if (list->len == 0) {
        return;
    }


    list->end = (list->end - 1 + list->data_size) % list->data_size;
    list->len = list->len - 1;
    void* data = list->data[list->end];
    memcpy(dest, data, size);
    free(data);

    list->data[list->end] = NULL;
}

/*
    Removes and retrieves that first element of the list
    copying the data into dest. The data in the list is
    then freed.
*/
void pop_front(List* list, void* dest, size_t size) {

    if (list->len == 0) {
        return;
    }

    void* data = list->data[list->start];
    memcpy(dest, data, size);
    free(data);
    list->data[list->start] = NULL;
    list->start = (list->start + 1) % list->data_size;
    list->len = list->len - 1;
}



static int convert_index(List* list, int index) {

    while (index < 0) {
        index += list->len;
    }

    return (list->start + index) % list->data_size;
}

static void check_bounds(List* list, int index) {

    int in_bounds = index >= 0 && index < list->len;

    if (!in_bounds) {
        fprintf(stderr, "Index ");
        fprintf(stderr, "%d", index);
        fprintf(stderr, " out of bounds for len ");
        fprintf(stderr, "%d", list->len);
        exit(EXIT_FAILURE);
    }
}

/*
    Get's a pointer to a list element (so changes to pointer object
    will change element on the list)
*/
void* get(List* list, int index) {
    int real_index = convert_index(list, index);
    return list->data[real_index];
}

/*
    Replaces an element at the specified index, copying the data
    in 'data' to the heap. This new heap data will be freed with pop
    or clear functions (or with the free_list function).

    The existing element at the given index will also be freed.
*/
void set(List* list, int index, void* data, size_t size) {
    int real_index = convert_index(list, index);
    void* current_data = list->data[real_index];
    free(current_data);


    void* copy = malloc(size);
    if (copy == NULL) {
        list_mem_error_exit_failing();
    }
    memcpy(copy, data, size);
    list->data[real_index] = copy;

}




/*
    Clears the list, and frees all object copies on the heap
    created with pushes and sets
*/
void clear(List* list) {
    // Free each pointer inside data
    for (size_t i = 0; i < list->data_size; i++) {
        free(list->data[i]);
    }
    list->len = 0;
    list->start = 0;
    list->end = 0;
}

/*
    frees the list and stored objects
*/
void free_list(List* list) {
    clear(list);
    // Free data
    free(list->data);
    // Free the struct itself
    free(list);
}


/*
    Makes a new list from the specified range (inclusive exclusive). If copy_size
    is not 0, then each element in the first list will be copied
    and a copy will be inserted in the resulting list. 
    If you set copy_size to 0, then the resulting list will share
    objects with this list. (this is a faster operation though if 
    you don't care)

    copy_size is the size of the elements in the list. If the list
    contains elements of different size, then you'll have to slice the list
    without copying.
*/
List* slice(List* list, int start, int end, size_t copy_size) {
    while(start < 0) {
        start += list->len;
    }
    while(end < 0) {
        end += list->len;
    }
    int new_len = end - start;
    int real_start = convert_index(list, start);
    int real_end = convert_index(list, end);

    List* new_list = new_list_s(new_len * 2);

    if (copy_size != 0) {
        for (int i = start; i < end; i++) {
            void* element = get(list, i);
            push(new_list, element, copy_size);
        }
    }
    else {

        if (real_start > real_end) {

            // copy start index to array end
            memcpy(
                new_list->data, 
                list->data + real_start, 
                (list->data_size - real_start) * sizeof(void*)
            );
            // copy array start to end index
            memcpy(
                new_list->data + (list->data_size - real_start), 
                list->data, 
                real_end * sizeof(void*)
            );
        }
        else {
            memcpy(
                new_list->data, 
                list->data + real_start, 
                (real_end - real_start) * sizeof(void*)
            );
        }
    }


    new_list->start = 0;
    new_list->end = new_len;
    new_list->len = new_len;
}




void* g_data(void* ptr, size_t size) {
    char *data = malloc(size);
    if (data == NULL) {
        fprintf(stderr, "Error allocating memory in! Exiting...");
        exit(EXIT_FAILURE);
    }

    memcpy(data, ptr, size);

    return data;
}


void p_data(void* dest, size_t size, void* data) {

    memcpy(dest, data, size);
    free(data);
}




#endif