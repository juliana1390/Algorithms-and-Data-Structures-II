#include "index.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void printIndexList(IndexArray2 *list) { // TESTE
  for (int i = 0; i < list->size; i++) {
    Index2 *it = &list->items[i];
    printf("%d - %s\n", it->id, it->author);
  }
}

// === MAIN ===
int main() {
  Book book;
  IndexArray1 indexes1; // array to keep primary indexes
  IndexArray2 indexes2; // array to keep secondary indexes
  indexes1.size = 0;
  indexes2.size = 0;

  // opens the data file or creates one if there is none
  FILE *dataFile;
  if ((dataFile = fopen("library.bin", "wb+")) == NULL) {
    printf("error creating/opening data file\n");
    exit(1);
  }

  // opens the primary index file or creates one if there is none
  FILE *indexFile1;
  if ((indexFile1 = fopen("index1.bin", "wb+")) == NULL) {
    printf("error creating/opening index1 file\n");
    exit(1);
  }

  // opens the secondary index file or creates one if there is none
  FILE *indexFile2;
  if ((indexFile2 = fopen("index2.bin", "wb+")) == NULL) {
    printf("error creating/opening index2 file\n");
    exit(1);
  }

  // === INPUT COMMANDS ===
  char command[100];
  char field[10];
  char value[100];

  while (1) {
    fgets(command, sizeof(command), stdin);
    command[strcspn(command, "\n")] = '\0'; // remove newline character

    // === EXIT COMMAND ===
    if (strcmp(command, "EXIT") == 0) {
      writeIndex1(indexFile1, &indexes1);
      writeIndex2(indexFile2, &indexes2);
      break;
    }
    // ============ INSERTION COMMAND ============
    else if (strstr(command, "ADD") != NULL) {
      sscanf(command, "ADD id='%d' titulo='%[^']' autor='%[^']'", &book.id,
             book.title, book.author);

      printf(DASHES);
      Index1 *idx = searchById(book.id, &indexes1);
      // if the register already exists
      if (idx) {
        printf("Erro ao inserir registro, chave primária duplicada\n");
      }
      // if the register doesn't exists yet
      else {
        fseek(dataFile, 0, SEEK_END);

        // new index
        // updating indexes in memory
        indexes1.items[indexes1.size].id = book.id;
        indexes1.items[indexes1.size].byteOffSet = ftell(dataFile);
        indexes2.items[indexes2.size].id = book.id;
        strcpy(indexes2.items[indexes2.size].author, book.author);
        indexes1.size++;
        indexes2.size++;

        // writes in data file
        fwrite(&book.id, sizeof(int), 1, dataFile);
        fwrite(book.title, sizeof(char), strlen(book.title) + 1, dataFile);
        fputc(DELIMITER_CHAR, dataFile);
        fwrite(book.author, sizeof(char), strlen(book.author) + 1, dataFile);
        fputc(DELIMITER_REG, dataFile);
        printf("Registro inserido\n");
      }
    }
    // ============ SEARCH COMMAND ============
    else if (strstr(command, "SEARCH") != NULL) {
      Index1 *idx1 = NULL;

      sscanf(command, "SEARCH %[^=]= '%[^']'", field, value);

      printf(DASHES);
      if (strcmp(field, "id") == 0) { // SEARCH BY ID
        book.id = atoi(value);
        idx1 = searchById(book.id, &indexes1);
        if (idx1) {

          book = readDataFile(dataFile, idx1->byteOffSet);
          printBook(&book);
        }
      } else if (strcmp(field, "autor") == 0) { // SEARCH BY AUTHOR
        Index2 *idx2 = NULL;
        for (int i = 0; i < indexes2.size; i++) {
          if (strcmp(value, indexes2.items[i].author) == 0) {
            idx2 = &indexes2.items[i];
            idx1 = searchById(idx2->id, &indexes1);
            book = readDataFile(dataFile, idx1->byteOffSet);
            printBook(&book);
          }
        }
      }

      if (!idx1) {
        printf("Não encontrado\n");
      }
    }
    // ============ REMOVAL COMMAND ============
    else if (strstr(command, "REMOVE") != NULL) {
      sscanf(command, "REMOVE %[^=]= '%[^']'", field, value);
      printf(DASHES);
      int removed = 0;
      if (strcmp(field, "id") == 0) { // REMOVAL BY ID
        book.id = atoi(value);
        Index1 *idx1 = searchById(book.id, &indexes1);

        if (idx1) {
          for (Index2 *idx2 = indexes2.items;
               idx2 < indexes2.items + indexes2.size; idx2++) {
            if (idx1->id == idx2->id) {
              removeIndex2(idx2, &indexes2); // removes from secondary index
              break;
            }
          }
          removeIndex1(idx1, &indexes1); // removes from primary index
          removed = 1;
          printf("Registro removido\n");
        }
      } else if (strcmp(field, "autor") == 0) { // REMOVAL BY AUTHOR
        Index2 *it = indexes2.items;
        Index1 *it1 = NULL;
        while (it < indexes2.items + indexes2.size) {
          if (strcmp(value, it->author) != 0) { // if different
            it++;                               // goes next
          } else {                              // if equal
            it1 = searchById(it->id, &indexes1);
            removeIndex1(it1, &indexes1);       // removes from primary index
            removeIndex2(it, &indexes2);        // removes from secondary index
  
            
            removed = 1;
            printf("Registro removido\n");
          }
        }
      }

      if (!removed) {
        printf("Erro ao remover\n");
      }
    }
  }

  fclose(dataFile);
  fclose(indexFile1);
  fclose(indexFile2);

  return 0;
}
