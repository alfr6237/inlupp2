#include "../../vendor/hash_table.h"
#include "../../vendor/hash_table_iterator.h"
#include "../../vendor/linked_list_iterator.h"
#include "common.h"
#include "store.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

static size_t hash_fn(elem_t elem) { return elem.sz; }
static bool key_eql_fn(elem_t a, elem_t b) { return a.sz == b.sz; }

static void remove_cart(store_t *store, ioopm_hash_table_t *cart);

static ioopm_hash_table_t *cart_create(void);
static void cart_destroy(ioopm_hash_table_t *cart);

size_t store_add_cart(store_t *store) {

  size_t cart_id = ++(store->current_cart_id);

  ioopm_hash_table_t *cart = cart_create();

  ioopm_hash_table_insert(store->carts, size_elem(cart_id), ptr_elem(cart));

  return cart_id;
}

bool store_cart_remove(store_t *store, size_t cart_id) {
  elem_t cart_elem;

  if (!ioopm_hash_table_remove(store->carts, size_elem(cart_id), &cart_elem)) {
    return false;
  }
  ioopm_hash_table_t *cart = cart_elem.p;
  remove_cart(store, cart);

  return true;
}

cart_result_t store_cart_add_merch(store_t *store, size_t cart_id,
                                   const char *merch_name, size_t quantity) {
  assert(quantity > 0);
  elem_t cart_elem;
  if (!ioopm_hash_table_lookup(store->carts, size_elem(cart_id), &cart_elem)) {
    return (cart_result_t){.is_ok = false, .error = CART_NOT_FOUND};
  }

  elem_t merch_id_elem;
  if (!ioopm_hash_table_lookup(store->merch_ids, c_str_elem(merch_name),
                               &merch_id_elem)) {
    return (cart_result_t){.is_ok = false, .error = MERCH_NOT_IN_STORE};
  }

  elem_t reserved_elem;
  assert(ioopm_hash_table_lookup(store->reserved_stock, merch_id_elem,
                                 &reserved_elem));

  size_t reserved = reserved_elem.sz;

  if ((store_count_stock(store, merch_name) - reserved) < quantity) {
    return (cart_result_t){.is_ok = false, .error = QUANTITY_TOO_LARGE};
  }

  ioopm_hash_table_t *cart = cart_elem.p;

  ioopm_hash_table_insert(store->reserved_stock, merch_id_elem,
                          size_elem(reserved + quantity));

  elem_t cart_quant_elem;
  size_t total_quantity = quantity;
  if (ioopm_hash_table_lookup(cart, merch_id_elem, &cart_quant_elem)) {
    total_quantity += cart_quant_elem.sz;
  };
  ioopm_hash_table_insert(cart, merch_id_elem, size_elem(total_quantity));

  return (cart_result_t){.is_ok = true};
}

cart_result_t store_cart_remove_merch(store_t *store, size_t cart_id,
                                      const char *merch_name, size_t quantity) {
  elem_t cart_elem;
  if (!ioopm_hash_table_lookup(store->carts, size_elem(cart_id), &cart_elem)) {
    return (cart_result_t){.is_ok = false, .error = CART_NOT_FOUND};
  }

  elem_t merch_id_elem;
  if (!ioopm_hash_table_lookup(store->merch_ids, c_str_elem(merch_name),
                               &merch_id_elem)) {
    return (cart_result_t){.is_ok = false, .error = MERCH_NOT_IN_STORE};
  }

  ioopm_hash_table_t *cart = cart_elem.p;
  elem_t cart_quant_elem;
  if (!ioopm_hash_table_lookup(cart, merch_id_elem, &cart_quant_elem)) {
    // add another error type specific for remove
    return (cart_result_t){.is_ok = false, .error = MERCH_NOT_IN_CART};
  }

  size_t cart_quant = cart_quant_elem.sz;

  if (cart_quant < quantity) {
    return (cart_result_t){.is_ok = false, .error = QUANTITY_TOO_LARGE};
  }

  if (cart_quant == quantity) {
    elem_t _;
    assert(ioopm_hash_table_remove(cart, merch_id_elem, &_));
  } else {
    ioopm_hash_table_insert(cart, merch_id_elem,
                            size_elem(cart_quant - quantity));
  }

  elem_t reserved_elem;
  assert(ioopm_hash_table_lookup(store->reserved_stock, merch_id_elem,
                                 &reserved_elem));
  size_t reserved = reserved_elem.sz;
  assert(reserved >= quantity);

  ioopm_hash_table_insert(store->reserved_stock, merch_id_elem,
                          size_elem(reserved - quantity));

  return (cart_result_t){.is_ok = true};
}

bool store_cart_cost(store_t *store, size_t cart_id, size_t *out_cost) {

  size_t cost = 0;

  elem_t cart_elem;
  if (!ioopm_hash_table_lookup(store->carts, size_elem(cart_id), &cart_elem)) {
    return false;
  }

  ioopm_hash_table_t *cart = cart_elem.p;
  ioopm_hash_table_iterator_t *iter = ioopm_hash_table_iterator_create(cart);

  while (!ioopm_hash_table_iterator_at_end(iter)) {
    elem_t merch_id_elem = ioopm_hash_table_iterator_current_key(iter);
    elem_t quantity_elem = ioopm_hash_table_iterator_current_value(iter);

    elem_t merch_elem;
    assert(ioopm_hash_table_lookup(store->merch, merch_id_elem, &merch_elem));

    size_t quantity = quantity_elem.sz;
    merch_t *merch = merch_elem.p;

    cost += (quantity * (merch->price));

    ioopm_hash_table_iterator_advance(iter);
  }
  ioopm_hash_table_iterator_destroy(iter);

  *out_cost = cost;

  return true;
}

bool store_cart_checkout(store_t *store, size_t cart_id) {

  elem_t cart_elem;

  if (!ioopm_hash_table_lookup(store->carts, size_elem(cart_id), &cart_elem)) {
    return false;
  }

  ioopm_hash_table_t *cart = cart_elem.p;
  ioopm_hash_table_iterator_t *cart_iter =
      ioopm_hash_table_iterator_create(cart);

  while (!ioopm_hash_table_iterator_at_end(cart_iter)) {
    elem_t merch_id_elem = ioopm_hash_table_iterator_current_key(cart_iter);
    elem_t quantity_elem = ioopm_hash_table_iterator_current_value(cart_iter);
    size_t quantity = quantity_elem.sz;

    elem_t locations_elem;

    assert(ioopm_hash_table_lookup(store->locations, merch_id_elem,
                                   &locations_elem));

    ioopm_list_iterator_t *location_iter =
        ioopm_list_iterator_create(locations_elem.p);

    while (quantity > 0 && !ioopm_list_iterator_at_end(location_iter)) {
      elem_t shelf_id_elem = ioopm_list_iterator_current(location_iter);

      elem_t shelf_elem;
      assert(
          ioopm_hash_table_lookup(store->shelfs, shelf_id_elem, &shelf_elem));
      shelf_storage_t *shelf = shelf_elem.p;

      if (shelf->quantity <= quantity) {
        quantity -= shelf->quantity;
        ioopm_list_iterator_remove(location_iter);
      } else {
        shelf->quantity -= quantity;
        quantity = 0;
      }
    }

    ioopm_list_iterator_destroy(location_iter);

    ioopm_hash_table_iterator_advance(cart_iter);
  }

  ioopm_hash_table_iterator_destroy(cart_iter);
  store_cart_remove(store, cart_id);

  return true;
}

void remove_cart(store_t *store, ioopm_hash_table_t *cart) {

  ioopm_hash_table_iterator_t *iter = ioopm_hash_table_iterator_create(cart);

  while (!ioopm_hash_table_iterator_at_end(iter)) {
    elem_t merch_id_elem = ioopm_hash_table_iterator_current_key(iter);
    elem_t quantity_elem = ioopm_hash_table_iterator_current_value(iter);

    elem_t resered_elem;
    assert(ioopm_hash_table_lookup(store->reserved_stock, merch_id_elem,
                                   &resered_elem));

    size_t quantity = quantity_elem.sz;
    size_t reserved = resered_elem.sz;
    assert(quantity_elem.sz <= resered_elem.sz);

    ioopm_hash_table_insert(store->reserved_stock, merch_id_elem,
                            size_elem(reserved - quantity));

    ioopm_hash_table_iterator_advance(iter);
  }
  ioopm_hash_table_iterator_destroy(iter);
  cart_destroy(cart);
}

void destroy_cart_table(store_t *store) {
  ioopm_hash_table_iterator_t *iter =
      ioopm_hash_table_iterator_create(store->carts);

  while (!ioopm_hash_table_iterator_at_end(iter)) {
    elem_t cart_elem = ioopm_hash_table_iterator_current_value(iter);
    ioopm_hash_table_t *cart = cart_elem.p;
    // assert(ioopm_hash_table_lookup(store->carts, cart_id_elem, &cart_elem));

    remove_cart(store, cart);
    ioopm_hash_table_iterator_advance(iter);
  }

  ioopm_hash_table_iterator_destroy(iter);
  ioopm_hash_table_destroy(store->carts);
}

static ioopm_hash_table_t *cart_create(void) {
  return ioopm_hash_table_create(hash_fn, key_eql_fn, NULL, NULL);
}

static void cart_destroy(ioopm_hash_table_t *cart) {
  ioopm_hash_table_destroy(cart);
}
