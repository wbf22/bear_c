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




void test_push_push_front_pop_pop_front_resize() {

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


}


int main() {

    test_push_push_front_pop_pop_front_resize();

    return 0;
}