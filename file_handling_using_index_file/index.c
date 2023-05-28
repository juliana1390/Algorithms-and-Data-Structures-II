#include "index.h"
#include "library.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ==== compares left hand side and right hand side ====
int compareById(void const *lhs, void const *rhs) {
  Index1 const *const l = lhs;
  Index1 const *const r = rhs;

  if (l->id < r->id)
    return -1;
  else if (l->id > r->id)
    return 1;
  else
    return 0;
}

// ==== searches by ID through binary search ====
Index1 *searchById(int id, IndexArray1 *indexes1) {
  if (indexes1->size < 1) {
    return indexes1->items[1].id == id ? &indexes1->items[1] : (Index1 *)NULL;
  }

  Index1 key = {.id = id};
  qsort(indexes1->items, indexes1->size, sizeof(Index1), compareById);
  return bsearch(&key, indexes1->items, indexes1->size, sizeof(Index1),
                 compareById);
}

// ==== prints a register ====
void printBook(Book *book) {
  printf("%d - %s - %s\n", book->id, book->title, book->author);
}

// ==== reads data from data file ====
Book readDataFile(FILE *file, long byteOffSet) { // VERIFICAR
  Book book;
  char ch;

  fseek(file, byteOffSet, SEEK_SET);
  fread(&book.id, sizeof(int), 1, file);

  int i = 0;
  while ((ch = fgetc(file)) != EOF) {
    if (ch == DELIMITER_CHAR) // |
      break;
    book.title[i++] = ch;
  }
  book.title[i++] = '\0';

  i = 0;
  while ((ch = fgetc(file)) != EOF) {
    if (ch == DELIMITER_REG) // #
      break;
    book.author[i++] = ch;
  }

  book.author[i++] = '\0';
  return book;
}

// ==== removes a chosen regiter from primary index ====
void removeIndex1(Index1 *idx, IndexArray1 *array) {
  for (Index1 *it = idx + 1; it < array->items + array->size; it++) {
    (it - 1)->id = it->id;
    (it - 1)->byteOffSet = it->byteOffSet;
  }
  array->size--;
}

// ==== removes a chosen regiter from secondary index ====
void removeIndex2(Index2 *idx, IndexArray2 *array) {
  for (Index2 *it = idx + 1; it < array->items + array->size; it++) {
    (it - 1)->id = it->id;
    strcpy((it - 1)->author, it->author);
  }
  array->size--;
}

// ==== writes primary index file ====
void writeIndex1(FILE *file, IndexArray1 *array1) {
  fseek(file, 0, SEEK_SET);
  for (Index1 *it = array1->items; it < array1->items + array1->size; it++) {
    fwrite(&it->id, sizeof(int), 1, file);
    fwrite(&it->byteOffSet, sizeof(long), 1, file);
    fputc(DELIMITER_REG, file);
  }
}

// ==== writes secondary index file ====
void writeIndex2(FILE *file, IndexArray2 *array2) {
  fseek(file, 0, SEEK_SET);
  for (Index2 *it = array2->items; it < array2->items + array2->size; it++) {
    fwrite(&it->id, sizeof(int), 1, file);
    fwrite(it->author, sizeof(char), strlen(it->author) + 1, file);
    fputc(DELIMITER_REG, file);
  }
}