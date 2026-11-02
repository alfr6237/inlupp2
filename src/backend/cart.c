#include "cart.h"
#include "../../vendor/hash_table.h"
#include "assert.h"
#include "stdlib.h"
#include "string.h"
#include <shelf.h>

static size_t hash_fn(elem_t elem) { return elem.sz; }
static bool key_eql_fn(elem_t a, elem_t b) { return a.sz == b.sz; }

cart_t cart_create() {
  ioopm_hash_table_t *items =
      ioopm_hash_table_create(hash_fn, key_eql_fn, NULL, NULL);

  return (cart_t){.items = items};
}

void cart_destroy(cart_t cart) { ioopm_hash_table_destroy(cart.items); }

void cart_add_merch(cart_t cart, size_t merch_id, size_t count) {

  elem_t current_quant = size_elem(0);
  size_t total_quant = 0;
  if (ioopm_hash_table_lookup(cart.items, size_elem(merch_id),
                              &current_quant)) {
    total_quant = current_quant.sz + count;
  } else {
    total_quant = count;
  }

  ioopm_hash_table_insert(cart.items, size_elem(merch_id),
                          size_elem(total_quant));
}
remove_result_t cart_remove_merch(cart_t cart, size_t merch_id, size_t count) {
  elem_t current_quant = size_elem(0);
  size_t total_quant = 0;
  if (!ioopm_hash_table_lookup(cart.items, size_elem(merch_id),
                               &current_quant)) {
    return (remove_result_t){.is_ok = false, .error = MERCH_NOT_IN_CART};
  }

  if (count > current_quant.sz) {
    return (remove_result_t){.is_ok = false, .error = COUNT_TOO_LARGE};
  }

  if (count == current_quant.sz) {
    elem_t _;
    assert(ioopm_hash_table_remove(cart.items, size_elem(merch_id), &_));
  } else {
    total_quant = current_quant.sz - count;
    ioopm_hash_table_insert(cart.items, size_elem(merch_id),
                            size_elem(total_quant));
  }

  return (remove_result_t){.is_ok = true};
}
