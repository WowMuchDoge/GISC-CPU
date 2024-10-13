#include <stdio.h>

#include "assembler.h"
#include "table.h"

int main() {
    Table table;

    initTable(&table);

    addElement(&table, "Funny", 234);

    printf("%d\n", getElement(&table, "nny"));
}
