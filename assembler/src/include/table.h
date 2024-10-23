#ifndef TABLE_H_
#define TABLE_H_

#include <stdbool.h>
#include <stdint.h>

struct Element {
  uint16_t element;
  char *str;
};

typedef struct Element Element;

struct Table {
  Element *elements;
  int count;
  int size;
};

typedef struct Table Table;

// Initialize Table
void initTable(Table *table);

// Add element to table
void addElement(Table *table, char *str, uint16_t value);

// Get element from the table
uint16_t getElement(Table *table, char *str);

// Check if an element is in the table
bool checkElement(Table *table, char *str);

// Free the memory allocated to a table
void freeTable(Table *table);

#endif
