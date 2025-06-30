#include <stdio.h>
#include "List.h"
#include "Map.h"

/*

valgrind --leak-check=full --show-leak-kinds=all ./test_exe

leaks -atExit -- ./test_exe
*/

char* ANSII_RESET = "\033[0m";
void assert(int condition, char* message) {
    if (!condition) {
        fprintf(stderr, "\033[38;2;255;0;0m");
        fprintf(stderr, "failed: ");
        fprintf(stderr, message);
        fprintf(stderr, ANSII_RESET);
        fprintf(stderr, "\n");
        exit(EXIT_FAILURE);
    }
    else {
        fprintf(stderr, "\033[38;2;40;133;0m");
        fprintf(stderr, "passed: ");
        fprintf(stderr, message);
        fprintf(stderr, ANSII_RESET);
        fprintf(stderr, "\n");
    }
}


void* copy_data(void* data, size_t size) {
    void* ptr = malloc(size);
    // int c = *(int*) data;
    memcpy(ptr, data, size);
    return ptr;
}

void list_test_push_push_front_pop_pop_front_resize_get_set() {

    List* list = new_list();


    int i = 1;
    push(list, &i, sizeof(i));
    i = 2;
    push(list, &i, sizeof(int));
    i = 3;
    push(list, &i, sizeof(int));
    i = 4;
    push(list, &i, sizeof(int));
    i = 5;
    push(list, &i, sizeof(int));
    i = 6;
    push(list, &i, sizeof(int));
    i = 7;
    push(list, &i, sizeof(int));
    i = 8;
    push(list, &i, sizeof(int));
    i = 9;
    push(list, &i, sizeof(int));
    i = 10;
    push(list, &i, sizeof(int));
    i = 11;
    push(list, &i, sizeof(int));
    i = 12;
    push(list, &i, sizeof(int));


    int b = 0;
    pop(list, &b, sizeof(b));
    assert(12 == b, "push pop same");
    pop(list, &b, sizeof(b));
    assert(11 == b, "push pop same");
    pop(list, &b, sizeof(b));
    assert(10 == b, "push pop same");
    pop(list, &b, sizeof(b));
    assert(9 == b, "push pop same");
    pop(list, &b, sizeof(b));
    assert(8 == b, "push pop same");
    pop(list, &b, sizeof(b));
    assert(7 == b, "push pop same");
    pop(list, &b, sizeof(b));
    assert(6 == b, "push pop same");
    pop(list, &b, sizeof(b));
    assert(5 == b, "push pop same");
    pop(list, &b, sizeof(b));
    assert(4 == b, "push pop same");
    pop(list, &b, sizeof(b));
    assert(3 == b, "push pop same");
    pop(list, &b, sizeof(b));
    assert(2 == b, "push pop same");
    pop(list, &b, sizeof(b));
    assert(1 == b, "push pop same");

    free_list(list);




    list = new_list();

    i = 1;
    push_front(list, &i, sizeof(i));
    i = 2;
    push_front(list, &i, sizeof(int));
    i = 3;
    push(list, &i, sizeof(int));
    i = 4;
    push(list, &i, sizeof(int));
    i = 5;
    push(list, &i, sizeof(int));
    i = 6;
    push(list, &i, sizeof(int));
    i = 7;
    push(list, &i, sizeof(int));
    i = 8;
    push(list, &i, sizeof(int));
    i = 9;
    push(list, &i, sizeof(int));
    i = 10;
    push(list, &i, sizeof(int));
    i = 11;
    push(list, &i, sizeof(int));
    i = 12;
    push(list, &i, sizeof(int));

    b = 0;
    pop_front(list, &b, sizeof(b));
    assert(2 == b, "push pop same");
    pop_front(list, &b, sizeof(b));
    assert(1 == b, "push pop same");
    pop(list, &b, sizeof(b));
    assert(12 == b, "push pop same");
    pop(list, &b, sizeof(b));
    assert(11 == b, "push pop same");
    pop(list, &b, sizeof(b));
    assert(10 == b, "push pop same");
    pop(list, &b, sizeof(b));
    assert(9 == b, "push pop same");
    pop(list, &b, sizeof(b));
    assert(8 == b, "push pop same");
    pop(list, &b, sizeof(b));
    assert(7 == b, "push pop same");
    pop(list, &b, sizeof(b));
    assert(6 == b, "push pop same");
    pop(list, &b, sizeof(b));
    assert(5 == b, "push pop same");
    pop(list, &b, sizeof(b));
    assert(4 == b, "push pop same");
    pop(list, &b, sizeof(b));
    assert(3 == b, "push pop same");

    free_list(list);
    



    list = new_list();

    i = 1;
    push_front(list, &i, sizeof(i));
    i = 2;
    push_front(list, &i, sizeof(int));
    i = 3;
    push(list, &i, sizeof(int));
    i = 4;
    push(list, &i, sizeof(int));
    i = 5;
    push(list, &i, sizeof(int));
    i = 6;
    push(list, &i, sizeof(int));
    i = 7;
    push(list, &i, sizeof(int));
    i = 8;
    push(list, &i, sizeof(int));
    i = 9;
    push(list, &i, sizeof(int));
    i = 10;
    push(list, &i, sizeof(int));
    i = 11;
    push(list, &i, sizeof(int));
    i = 12;
    push(list, &i, sizeof(int));


    // 2 1 3 4 5 6 7 8 9 10 11 12
    b = 0;
    pop_front(list, &b, sizeof(b));
    pop(list, &b, sizeof(b));
    pop(list, &b, sizeof(b));

    // 1 3 4 5 6 7 8 9 10
    i = 13;
    push(list, &i, sizeof(int));
    i = 14;
    push(list, &i, sizeof(int));
    i = 15;
    push(list, &i, sizeof(int));

    // 1 3 4 5 6 7 8 9 10 13 14 15
    int* t = (int*) get(list, 0);
    assert(*t == 1, "index get");
    t = (int*) get(list, 1);
    assert(*t == 3, "index get");
    t = (int*) get(list, -1);
    assert(*t == 15, "index get");

    List* slice_list = slice(list, 1, 4, sizeof(i));
    t = (int*) get(slice_list, 0);
    assert(*t == 3, "slice check");
    t = (int*) get(slice_list, 1);
    assert(*t == 4, "slice check");
    t = (int*) get(slice_list, -1);
    assert(*t == 5, "slice check");
    free_list(slice_list);


    free_list(list);


}

void map_resizing_all_methods() {

    // // Testing with Structs
    // typedef struct MyStruct {
    //     int x;
    //     int y;
    // } MyStruct;

    // Map* map = new_map();
    // MyStruct object = {1, 3};
    // put(map, "my_key", &object);

    // MyStruct back_out = *(MyStruct*) at(map, "my_key");

    // erase(map, "my_key");

    // printf("%d\n", map->len);


    // MyStruct object2 = {1, 3};
    // put(map, "my_key", &object2);
    // MyStruct object3 = {2, 4};
    // put(map, "my_key2", &object3);

    // Element** items = map_items(map);
    // for (int i = 0; i < map->len; ++i) {
    //     Element* item = items[i];
    //     char* key = item->key;
    //     MyStruct obj = *(MyStruct*) item->data;

    //     printf("%s{%d,%d}\n", key, obj.x, obj.y);
    // }
    // free(items);

    // free_map(map);



    // // testing resize
    // map = new_map();
    // for (int i = 0; i < PRIMES[2]; ++i) {
    //     int *data = malloc(sizeof(int));
    //     *data = i * 2;
    //     int_put(map, i, data);
    // }

    // for (int i = 0; i < PRIMES[2]; ++i) {
    //     int* data = (int*) int_erase(map, i);        
    //     assert(*data == i * 2, "check int map");

    //     free(data);
    // }
    // free_map(map);



    // 
    Map* checker = new_map();

    char* key = "hi";
    size_t key_size = (strlen(key) + 1) * sizeof(char);
    size_t table_size = 57;
    size_t index = hash(key, key_size) % table_size;
    int* hash_collisions = malloc(sizeof(int));
    *hash_collisions = 0;
    for(int i = 0; i < table_size * 2; ++i) {
        ++(*hash_collisions);
        index = (index + hash3(*hash_collisions)) % table_size;

        int count = 0;
        if (int_contains(checker, index)) {
            void* ptr = int_at(checker, index);
            count = *(int*) ptr;
            free(ptr);
        }
        count = count + 1;
        void* copy = copy_data(&count, sizeof(int));
        
        int_put(checker, index, copy);
        // int d = *(int*) int_at(checker, index);
    }
    free(hash_collisions);
    


    for(int i = 0; i < table_size; ++i) {
        void* ptr = int_erase(checker, i);
        int count = 0;
        if (ptr != NULL) {
            count = *(int*) ptr;
        }
        printf("%d %d\n", i, count);
        free(ptr);
    }
    free_map(checker);

}

int main() {

    map_resizing_all_methods();
    
    return 0;
}