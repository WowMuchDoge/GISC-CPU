#include "table.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define START_SIZE 16
#define EMPTY_ELEMENT                                                          \
  (Element) { 0, NULL }
#define NEW_SIZE(n) (n * 2)

static int hash(char *str, int size) {
  int index = 0;
  int len = strlen(str);
  int hash = 0;

  for (int i = 0; i < len; i++) {
    hash = (hash * 31 + str[i]) % size;
  }

  return hash;
}

static void putElement(Table *table, int index, Element element) {
  while (table->elements[index].str) {
    if (index == table->size)
      index = -1;
    index++;
  }

  table->elements[index] = element;
}

void initTable(Table *table) {
  table->elements = calloc(START_SIZE, sizeof(Element));

  table->count = 0;
  table->size = START_SIZE;
}

void addElement(Table *table, char *str, uint16_t value) {
  if (table->size == table->count + 1) {
    // Reallocate elements pointer
    table->elements = realloc(table->elements, NEW_SIZE(table->size));
    table->size = NEW_SIZE(table->size);

    // Now we gotta re-hash everything
    for (int i = 0; i < table->size; i++) {
      if (table->elements[i].str) {
        putElement(table, hash(str, table->size), table->elements[i]);
        table->elements[i] = EMPTY_ELEMENT;
      }
    }
  }

  char *newStr = malloc((strlen(str) + 1) * sizeof(char));

  memcpy(newStr, str, strlen(str) + 1);

  table->elements[hash(str, table->size)] = (Element){value, newStr};
}

uint16_t getElement(Table *table, char *str) {
  int index = hash(str, table->size);

  while (table->elements[index].str && memcmp(str, table->elements[index].str, strlen(str)) != 0) {
    if (index == table->size - 1)
      index = -1;
    index++;
  }

  if (!table->elements[index].str) {
    printf("Warning: Element '%s' does not exist.\n", str);
    return 0;
  }

  return table->elements[index].element;
}

bool checkElement(Table *table, char *str) {
  return table->elements[hash(str, table->size)].str;
}

void freeTable(Table *table) {
  for (int i = 0; i < table->size; i++) {
    if (table->elements[i].str) {
      free(table->elements[i].str);
    }
  }

  free(table->elements);
}
