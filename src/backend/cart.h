#pragma once
#include "../../vendor/hash_table.h"
#include "common.h"
#include "result.h"

void ioopm_destroy_cart_table(ioopm_store_t *store);

void ioopm_remove_cart(ioopm_store_t *store, ioopm_hash_table_t *cart);
