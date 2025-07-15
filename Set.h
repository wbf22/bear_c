#ifndef SET
#define SET

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct Item {
    void* data;
    size_t data_size;
} Item;

/*
    

*/
typedef struct Set {
    Item** data; // pointer to array of pointers
    size_t data_size;
    size_t len;
} Set;

const char* SET_DELETED_KEY = "<DELETED>";

const size_t SET_PRIMES[] = {
    127,
    257,
    509,
    1021,
    2053,
    4099,
    9311,
    18253,
    37633,
    65713,
    115249,
    193939,
    505447,
    1062599,
    2017963,
    4393139,
    6972593,
    13466917,
    30402457,
    57885161,
    99990001,
    370248451,
    492366587,
    715827883,
    6643838879,
    8589935681,
    32212254719,
    51539607551, // 412 GB of 8 byte elements
    
    // just for fun probably
    80630964769,
    119327070011,
    228204732751,
    1171432692373,
    1398341745571,
    9809862296159,
    15285151248481,
    304250263527209,
    1746860020068409,
    10657331232548839,
    790738119649411319,
    2305843009213693951 // 18,446 PB of 8 byte elements, more than largest super computers have in ram
};

static void set_mem_error_exit_failing() {
    fprintf(stderr, "Set couldn't get more memory on the system! Exiting...");
    exit(EXIT_FAILURE);
}


static Set* new_set_s(size_t size) {

    Set *set = malloc(sizeof(Set));
    if (set == NULL) {
        set_mem_error_exit_failing();
    }
    set->data = malloc(size * sizeof(void*));
    if (set->data == NULL) {
        free(set);
        set_mem_error_exit_failing();
    }
    set->data_size = size;
    for (int i = 0; i < size; ++i) {
        set->data[i] = NULL;
    }
    set->len = 0;

    return set;
}

/*
    Creates an empty set
*/
Set* new_set() {
    return new_set_s(SET_PRIMES[0]);
}


static size_t hash_s(void* key, size_t key_size) {
    unsigned char* bytes = (unsigned char*)key;
    size_t hash = 5381;

    for (size_t i = 0; i < key_size; i++)
        hash = ((hash << 5) + hash) + bytes[i];

    return (size_t)hash;
}

static size_t hash_s_3(int failures) {
    size_t hash = 5381;

    for (size_t i = 0; i < failures; i++)
        hash = (hash * 131) + failures;

    // not odd or zero
    if (hash % 2 == 0) {
        hash += 1;
    }

    return (size_t)hash;
}

static size_t probe_s(Set* set, char* data, size_t data_size, int* hash_collisions) {
    size_t index = hash_s(data, data_size) % set->data_size;
    size_t first_index = index;

    int open_or_match = set->data[index] == NULL;
    if (set->data[index] != NULL && set->data[index]->data_size == data_size) {
        open_or_match = open_or_match || memcmp(set->data[index]->data, data, data_size) == 0;
    }
    while (!open_or_match) {
        ++(*hash_collisions);
        index = (first_index + hash_s_3(*hash_collisions)) % set->data_size;

        open_or_match = set->data[index] == NULL;
        if (set->data[index] != NULL && set->data[index]->data_size == data_size) {
            open_or_match = open_or_match || memcmp(set->data[index]->data, data, data_size) == 0;
        }
    }

    return index;
}


static void free_set_data(Set* set) {
    for (size_t i = 0; i < set->data_size; ++i) {
        if (set->data[i] != NULL) {
            free( set->data[i]);
        }
    }
    free(set->data);
}

/*
    Used for freeing the set's meta data. You should clean up set objects yourself,
    as the set cannot discover and clean up pointers to objects within objects. This
    function only cleans up the sets structural data.

    So free your objects, then call this to free the set, and everything should be
    cleaned up.
*/
void free_set(Set* set) {
    free_set_data(set);
    free(set);
}


static void add_no_resize(Set* set, void* data, size_t data_size) {
    int hash_collisions = 0;
    size_t index = probe_s(set, data, data_size, &hash_collisions);


    // if new element
    if (set->data[index] == NULL) {


        // make a new item
        Item *item = malloc(sizeof(Item));
        if (item == NULL) {
            set_mem_error_exit_failing();
        }
        item->data = data;
        item->data_size = data_size;
        set->data[index] = item;

        ++set->len;
    }
    else {
        memcpy(set->data[index]->data, data, data_size);
    }


}

static void resize_set(Set* set) {
    size_t NUM_SET_PRIMES = sizeof(SET_PRIMES) / sizeof(SET_PRIMES[0]);

    // get next table size
    size_t new_table_size = SET_PRIMES[0];
    for (int i = 0; i < NUM_SET_PRIMES; ++i) {
        if (SET_PRIMES[i] > set->data_size) {
            new_table_size = SET_PRIMES[i];
            break;
        }
    }

    // make new table and insert each element
    size_t SET_DELETED_KEY_SIZE = strlen(SET_DELETED_KEY) + 1;
    Set* new_set = new_set_s(new_table_size);
    for (size_t i = 0; i < set->data_size; ++i) {

        // if an element insert into new set
        if (set->data[i] != NULL) {

            // check if deleted
            int deleted = 0;
            if (set->data[i]->data_size == SET_DELETED_KEY_SIZE) {
                if (strcmp(set->data[i]->data, SET_DELETED_KEY) == 0) {
                    deleted = 1;
                }
            }

            // an element insert into set
            if (!deleted) {
                Item* item = set->data[i];
                add_no_resize(new_set, item->data, item->data_size);
            }
        }
    }

    free_set_data(set);
    set->data = new_set->data;
    set->data_size = new_set->data_size;
    set->len = new_set->len;

    new_set->data = NULL; // so it won't free the data array copied above
    free(new_set);
}


/*
    Function to insert an object in the set.

    This can be used like so:
    ```
    Set* set = new_set();
    MyStruct object = {1, "hi"};

    any_add(set, &object, sizeof(object));
    ```

*/
void any_add(Set* set, void* data, size_t data_size) {

    add_no_resize(set, data, data_size);

    // resize if neeeded
    if (set->data_size * 0.7 < set->len) {
        resize_set(set);
    }
}

/*
    Function to insert an int in the set.
*/
void int_add(Set* set, int data) {
    any_add(set, &data, sizeof(int));
}

/*
    Function to insert a string in the set.
*/
void add(Set* set, char* data) {
    any_add(set, data, (strlen(data) + 1) * sizeof(char));
}



/*
    Function to remove an object in the set.

    This can be used like so:
    ```
    Set* set = new_set();
    MyStruct object = {1, "hi"};
    any_remove(set, &object, sizeof(object));
    ```

    If the key doesn't exist nothing happens
*/
void* any_erase_s(Set* set, void* data, size_t data_size) {
    int hash_collisions = 0;
    size_t index = probe_s(set, data, data_size, &hash_collisions);

    if (set->data[index] == NULL) {
        return NULL;
    }

    Item* item = set->data[index];
    free(item);

    if (hash_collisions != 0) {

        // make deleted key
        size_t SET_DELETED_KEY_SIZE = strlen(SET_DELETED_KEY) + 1;
        char deleted_key_copy[SET_DELETED_KEY_SIZE];
        strcpy(deleted_key_copy, SET_DELETED_KEY);

        // set as deleted
        set->data[index]->data = deleted_key_copy;
        set->data[index]->data_size = SET_DELETED_KEY_SIZE;
        set->data[index]->data = NULL;

        --set->len;
    }
    else {
        set->data[index] = NULL;
        --set->len;
    }

    return data;
}

/*
    Function to remove an int in the set.

    If the key doesn't exist nothing happens
*/
void* int_erase_s(Set* set, int key) {
    return any_erase_s(set, &key, sizeof(key));
}

/*
    Function to remove a string in the set.

    If the key doesn't exist nothing happens
*/
void* erase_s(Set* set, char* key) {
    return any_erase_s(set, key, (strlen(key) + 1) * sizeof(char));
}


/*
    Returns the items in the set. Useful for iterating
    over set elements.

    These elements are only references to elements in the set.
    They should not be deleted or the set will be in a broken
    state. However, the array of elements must be cleaned up or
    there will be memory leaks. (so free the returned array, not the elements)
*/
void** set_items(Set* set) {
    void** array = malloc(set->len * sizeof(Item));

    int l = 0;
    size_t SET_DELETED_KEY_SIZE = strlen(SET_DELETED_KEY) + 1;
    for (size_t i = 0; i < set->data_size; ++i) {
        if (set->data[i] != NULL) {
            
            int deleted = 0;
            if (set->data[i]->data_size == SET_DELETED_KEY_SIZE) {
                 if (strcmp(set->data[i]->data, SET_DELETED_KEY) == 0) {
                    deleted = 1;
                }
            }

            if(!deleted) {
                array[l] = set->data[i]->data;
                ++l;
            }
        }
    }

    return array;
}


void clear_set(Set* set) {
    free_set(set);
    set = new_set();
}


/*
    Function to determine if an object is in the set.

    This can be used like so:
    ```
    Set* set = new_set();
    MyStruct key = {1, "hi"};
    if(any_has(set, &key, sizeof(key))) {
        printf("yay!");
    }
    ```
*/
int any_has(Set* set, void* data, size_t data_size) {

    int hash_collisions = 0;
    size_t index = probe_s(set, data, data_size, &hash_collisions);
    if (set->data[index] == NULL) {
        return 0; 
    }

    return 1;
}

/*
    Function to determine if an int is in the set.
*/
int int_has(Set* set, int data) {
    return any_has(set, &data, sizeof(data));
}

/*
    Function to determine if a string is in the set.
*/
int has(Set* set, char* data) {
    return any_has(set, data, (strlen(data) + 1) * sizeof(char));
}

#endif

