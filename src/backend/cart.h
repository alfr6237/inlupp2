#pragma once
#include "../../vendor/hash_table.h"

#include "result.h"

typedef struct cart {
  ioopm_hash_table_t *items;
} cart_t;

typedef enum result_error {
  MERCH_NOT_IN_CART,
  COUNT_TOO_LARGE,
} result_error_t;

typedef RESULT_VOID(result_error_t) remove_result_t;

cart_t cart_create();
void cart_destroy(cart_t cart);

void cart_add_merch(cart_t cart, size_t merch_id, size_t count);
remove_result_t cart_remove_merch(cart_t cart, size_t merch_id, size_t count);
