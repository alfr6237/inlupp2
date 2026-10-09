#include "merch.h"
#include "../../vendor/linked_list.h"
#include <stdlib.h>
#include <string.h>

ioopm_merch_t *ioopm_merch_create(const char *name, const char *desc,
                                  size_t price) {
  ioopm_merch_t *merch = calloc(sizeof(ioopm_merch_t), 1);
  merch->name = strdup(name);
  merch->desc = strdup(desc);
  merch->price = price;
  merch->locations = ioopm_list_create();
  return merch;
}
void ioopm_merch_destroy(ioopm_merch_t *merch) {
  free(merch->name);
  free(merch->desc);
  ioopm_list_destroy(merch->locations);
  free(merch);
}
