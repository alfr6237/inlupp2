
#include "merch.h"
#include <stdlib.h>

merch_t *ioopm_merch_create(const char *name, const char *desc, size_t price) {
  merch_t *merch = calloc(sizeof(merch_t), 1);
  merch->name = name;
  merch->desc = desc;
  merch->price = price;
  return merch;
}
void ioopm_merch_destroy(merch_t *merch) { free(merch); }
