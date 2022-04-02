#include "common.h"
#include "module.h"

#define INIT_MALLOC_SIZE 1024

void **malloc_table = NULL;
size_t malloc_table_index = 0;
size_t malloc_table_size = 0;

void *log_malloc(size_t size) {
  if (malloc_table_size == 0) {
    malloc_table = (void **)malloc(INIT_MALLOC_SIZE * sizeof(void *));
    malloc_table_size = INIT_MALLOC_SIZE;
  }

  if (malloc_table_index == malloc_table_size) {
    malloc_table =
        (void **)realloc(malloc_table, INIT_MALLOC_SIZE * 2 * sizeof(void *));
    malloc_table_size *= 2;
  }

  void *res = malloc(size);
  malloc_table[malloc_table_index] = res;
  ++malloc_table_index;
  return res;
}

static void clear_malloc() {
  for (size_t i = 0; i < malloc_table_index; ++i) {
    free(malloc_table[i]);
    malloc_table[i] = NULL;
  }
  free(malloc_table);
  malloc_table = NULL;
}

MODULE_DEF(mm) = {
    .log_malloc = log_malloc,
    .clear_malloc = clear_malloc,
};
