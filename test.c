#include <stdio.h>
#include "List.h"

/*

valgrind --leak-check=full --show-leak-kinds=all ./test_exe

*/

int main() {
    List* list = new_list();

    // int i = 32;
    // push(list, g_data(&i, sizeof(i)));

    // int b = 0;
    // p_data(
    //     &b,
    //     sizeof(b),
    //     pop(list)
    // );

    int i = 32;
    push(list, &i);

    int b = 0;
    memcpy(&b, pop(list), sizeof(b));


    free_list(list);

    return 0;
}