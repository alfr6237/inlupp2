#pragma once
#include "../../vendor/linked_list.h"
#include <stddef.h>

typedef struct merch merch_t;

struct merch {
  char *name;
  char *desc;
  size_t price;
};

merch_t *merch_create(const char *name, const char *desc, const size_t price);
void merch_destroy(merch_t *merch);

void merch_edit_name(merch_t *merch, const char *new_name);
void merch_edit_desc(merch_t *merch, const char *new_desc);
void merch_edit_price(merch_t *merch, const size_t new_price);
