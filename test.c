#include <stdio.h>
#include "List.h"

int main() {
    List* list = new_list();

    free_list(list);

    return 0;
}