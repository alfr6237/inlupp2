#pragma once
#include "../../vendor/hash_table.h"

#include "result.h"

ioopm_hash_table_t *cart_create();
void cart_destroy(ioopm_hash_table_t *cart);
