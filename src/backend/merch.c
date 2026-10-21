#include "merch.h"
#include <stdlib.h>
#include <string.h>

merch_t *merch_create(const char *name, const char *desc, const size_t price) {
  merch_t *merch = calloc(1, sizeof(merch_t));
  merch->name = strdup(name);
  merch->desc = strdup(desc);
  merch->price = price;
  return merch;
}
void merch_destroy(merch_t *merch) {
  free(merch->name);
  free(merch->desc);
  free(merch);
}
