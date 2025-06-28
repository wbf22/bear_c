#include <stdio.h>
#include "List.h"
#include "Map.h"

/*

valgrind --leak-check=full --show-leak-kinds=all ./test_exe

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


int main() {

    // Map* map;

    // insert(map, "key", "value", 3 * )


    int i = 0;
    char key[10];
    make_key(&i, sizeof(i), key, sizeof(key));
    printf("%s\n", key);

    i = 1;
    make_key(&i, sizeof(i), key, sizeof(key));
    printf("%s\n", key);


    i = 2;
    make_key(&i, sizeof(i), key, sizeof(key));
    printf("%s\n", key);

    i = 3;
    make_key(&i, sizeof(i), key, sizeof(key));
    printf("%s\n", key);

    i = 4;
    make_key(&i, sizeof(i), key, sizeof(key));
    printf("%s\n", key);

    i = 1235;
    make_key(&i, sizeof(i), key, sizeof(key));
    printf("%s\n", key);

    return 0;
}