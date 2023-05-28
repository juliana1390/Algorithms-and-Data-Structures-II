#ifndef INDEXFILES_H
#define INDEXFILES_H
#define DELIMITER_CHAR '|'
#define DELIMITER_REG '#'
#define DASHES "----------------------------------------------------------\n"

// === REGISTER ===
typedef struct Book {
  int id;
  char title[100];
  char author[100];
} Book;

// === PRIMARY INDEX ===
typedef struct Index1 {
  int id;
  long byteOffSet;
} Index1;

// === SECONDARY INDEX ===
typedef struct Index2 {
  int id;
  char author[100];
} Index2;

// === ARRAY OF INDEXES 1 ===
typedef struct IndexArray1 {
  Index1 items[1001];
  int size;
} IndexArray1;

// === ARRAY OF INDEXES 1 ===
typedef struct IndexArray2 {
  Index2 items[1001];
  int size;
} IndexArray2;

typedef struct _IO_FILE FILE;

// METHODS
int compareById(void const *lhs, void const *rhs);
Index1 *searchById(int id, IndexArray1 *indexes1);
Book readDataFile(FILE *file, long byteOffSet);
void printBook(Book *book);
void removeIndex1(Index1 *idx, IndexArray1 *array);
void removeIndex2(Index2 *idx, IndexArray2 *array);
void writeIndex1(FILE *file, IndexArray1 *array);
void writeIndex2(FILE *file, IndexArray2 *array);

#endif