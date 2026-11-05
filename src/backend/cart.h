#pragma once
#include "../../vendor/hash_table.h"
#include "common.h"
#include "result.h"

void destroy_cart_table(store_t *store);

void remove_cart(store_t *store, ioopm_hash_table_t *cart);
