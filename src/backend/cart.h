#pragma once
#include "../../vendor/hash_table.h"

#include "result.h"

typedef enum result_error {
  MERCH_NOT_IN_CART,
  COUNT_TOO_LARGE,
} result_error_t;

typedef RESULT_VOID(result_error_t) remove_result_t;

ioopm_hash_table_t *cart_create();
void cart_destroy(ioopm_hash_table_t *cart);

void cart_add_merch(ioopm_hash_table_t *cart, size_t merch_id, size_t count);
remove_result_t cart_remove_merch(ioopm_hash_table_t *cart, size_t merch_id, size_t count);
