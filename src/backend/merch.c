#include "merch.h"
#include "../../vendor/linked_list.h"
#include <stdlib.h>
#include <string.h>

ioopm_merch_t *ioopm_merch_create(const char *name, const char *desc,
                                  const size_t price) {
  ioopm_merch_t *merch = calloc(1, sizeof(ioopm_merch_t));
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

void ioopm_merch_edit_name(ioopm_merch_t *merch, const char *new_name) {
  (void)merch;
  (void)new_name;
}
void ioopm_merch_edit_desc(ioopm_merch_t *merch, const char *new_desc) {
  (void)merch;
  (void)new_desc;
}
void ioopm_merch_edit_price(ioopm_merch_t *merch, const size_t price) {
  (void)merch;
  (void)price;
}
