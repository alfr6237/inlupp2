#include "merch.h"
#include <stdlib.h>
#include <string.h>

ioopm_merch_t *ioopm_merch_create(const char *name, const char *desc,
                                  const size_t price) {
  ioopm_merch_t *merch = calloc(1, sizeof(ioopm_merch_t));
  merch->name = strdup(name);
  merch->desc = strdup(desc);
  merch->price = price;
  return merch;
}
void ioopm_merch_destroy(ioopm_merch_t *merch) {
  free(merch->name);
  free(merch->desc);
  free(merch);
}
