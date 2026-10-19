#include "store.h"
#include "../../vendor/linked_list_iterator.h"
#include "./merch.h"
#include "result.h"
#include "shelf.h"
#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

typedef RESULT(ioopm_merch_t *, search_error_t) get_merch_result_t;

static bool shelf_eql_fn(elem_t a, elem_t b);

static size_t string_hash_function(elem_t key);
static bool string_eq_function(elem_t a, elem_t b);
static size_t shelf_hash_fn(elem_t shelf);

static bool size_t_eql_fn(elem_t a, elem_t b);
static size_t size_t_hash_fn(elem_t s);

// static void remove_string(elem_t string);

static void remove_merch_value(elem_t merch);
static void remove_any_elem(elem_t elem);

static void remove_list_value(elem_t list);

static void stock_remove_fn(elem_t elem);

// static bool ioopm_get_shelf_storage(const ioopm_store_t *store,
//                                     const shelf_name_t shelf,
//                                     shelf_storage_t *storage_out);

static void add_shelf(ioopm_store_t *store, shelf_name_t shelf_name,
                      size_t merch_id, size_t quantity);

static bool increase_quantity(ioopm_store_t *store, size_t shelf_id,
                              size_t quantity);

ioopm_store_t *ioopm_store_create(void) {
  ioopm_store_t *store = calloc(1, sizeof(ioopm_store_t));

  store->merch_ids = ioopm_hash_table_create(string_hash_function,
                                             string_eq_function, NULL, NULL);
  store->shelf_ids =
      ioopm_hash_table_create(shelf_hash_fn, shelf_eql_fn, NULL, NULL);
  store->merch = ioopm_hash_table_create(size_t_hash_fn, size_t_eql_fn, NULL,
                                         remove_merch_value);
  store->shelfs = ioopm_hash_table_create(size_t_hash_fn, size_t_eql_fn, NULL,
                                          remove_any_elem);
  store->locations = ioopm_hash_table_create(size_t_hash_fn, size_t_eql_fn,
                                             NULL, remove_list_value);
  store->current_id = 0;

  return store;
}

void ioopm_store_destroy(ioopm_store_t *store) {

  ioopm_hash_table_destroy(store->shelfs);
  ioopm_hash_table_destroy(store->merch);
  ioopm_hash_table_destroy(store->shelf_ids);
  ioopm_hash_table_destroy(store->merch_ids);
  ioopm_hash_table_destroy(store->locations);

  free(store);
}

bool ioopm_merch_exists(ioopm_store_t *store, const char *merch_name) {
  (void)store;
  (void)merch_name;
  return false;
}

// kanske olika fkn sviter för de olika ht:na i store? återanvändning?
//  högre nivå för check? lär vara bra, slipper standard outputs
const ioopm_merch_t *ioopm_store_get_merch(ioopm_store_t *store,
                                           const char *merch_name) {
  elem_t merch_id = size_elem(0);
  bool merch_found = ioopm_hash_table_lookup(store->merch_ids,
                                             c_str_elem(merch_name), &merch_id);

  if (merch_found) {
    elem_t merch_gotten = ptr_elem(NULL);
    assert(ioopm_hash_table_lookup(store->merch, merch_id, &merch_gotten));
    return merch_gotten.p;
  }

  return NULL;
}

// TODO: When do we check if merch name is ok to add, no dup? static?
bool ioopm_store_add_merch(ioopm_store_t *store, const char *name,
                           const char *desc, int price) {
  elem_t merch_id = size_elem(0);
  bool merch_found =
      ioopm_hash_table_lookup(store->merch_ids, c_str_elem(name), &merch_id);

  if (merch_found) {
    return false;
  }

  ioopm_merch_t *merch = ioopm_merch_create(name, desc, price);

  merch_id = size_elem(store->current_id++);

  ioopm_hash_table_insert(store->merch_ids, str_elem(merch->name), merch_id);

  ioopm_hash_table_insert(store->merch, merch_id, ptr_elem(merch));
  ioopm_list_t *locations = ioopm_list_create(NULL);
  ioopm_hash_table_insert(store->locations, merch_id, ptr_elem(locations));

  return true;
}

// returns false if there exist other type of merch on the specified shelf or
// quantity is 0
// TODO: update location of merch (change to ludvigs list impl)
replenish_result_t ioopm_store_replenish_merch(ioopm_store_t *store,
                                               const char *merch_name,
                                               const shelf_name_t shelf_name,
                                               size_t quantity) {
  assert(quantity != 0);

  elem_t merch_id;
  bool merch_found = ioopm_hash_table_lookup(
      store->merch_ids, str_elem((char *)merch_name), &merch_id);

  if (!merch_found) {
    return (replenish_result_t){.is_ok = false, .error = MERCH_NOT_FOUND};
  }

  elem_t shelf_id;
  bool shelf_found = ioopm_hash_table_lookup(
      store->shelf_ids, ptr_elem((shelf_name_t *)&shelf_name), &shelf_id);

  if (!shelf_found) {
    shelf_id.sz = store->current_id++;

    shelf_storage_t *storage =
        create_shelf_storage(shelf_name, merch_id.sz, quantity);

    ioopm_hash_table_insert(store->shelfs, shelf_id, ptr_elem(storage));
    ioopm_hash_table_insert(store->shelf_ids, ptr_elem(&storage->shelf_name),
                            shelf_id);

    // NOTE: SEGAFAULT HERE IF REMOVED
    elem_t locations;
    assert(ioopm_hash_table_lookup(store->locations, merch_id, &locations));
    ioopm_list_append(locations.p, shelf_id);

    return (replenish_result_t){.is_ok = true};
  }

  elem_t storage_elem;
  assert(ioopm_hash_table_lookup(store->shelfs, shelf_id, &storage_elem));

  return replenish_merch(storage_elem.p, merch_id.sz, quantity);
}

// TODO: When is name check for existence done? prolly not this lvl, kanske
// static? PRE: should not be ran before merchandise has been removed from all
// shelves
bool ioopm_store_remove_merch(ioopm_store_t *store, const char *merch_name) {
  elem_t merch_id = int_elem(-1); // initialization?
  bool merch_found = ioopm_hash_table_remove(store->merch_ids,
                                             c_str_elem(merch_name), &merch_id);

  if (!merch_found) {
    return false;
  }

  elem_t locations;
  assert(ioopm_hash_table_remove(store->locations, merch_id, &locations));

  elem_t _;

  assert(ioopm_hash_table_remove(store->merch, merch_id, &_));

  return true;
}

edit_result_t ioopm_store_edit_merch(ioopm_store_t *store, const char *old_name,
                                     const char *new_name, const char *new_desc,
                                     const size_t new_price) {

  if (!ioopm_hash_table_has_key(store->merch_ids, c_str_elem(old_name))) {
    return (edit_result_t){.is_ok = false, .error = OLD_NAME_NOT_FOUND};
  }

  if (strcmp(old_name, new_name) != 0) {
    if (ioopm_hash_table_has_key(store->merch_ids, c_str_elem(new_name))) {
      return (edit_result_t){.is_ok = false, .error = NEW_NAME_EXIST};
    }
  }

  elem_t merch_id;
  assert(ioopm_hash_table_remove(store->merch_ids, c_str_elem(old_name),
                                 &merch_id));

  ioopm_merch_t *merch = ioopm_merch_create(new_name, new_desc, new_price);

  ioopm_hash_table_insert(store->merch, merch_id, ptr_elem(merch));
  ioopm_hash_table_insert(store->merch_ids, c_str_elem(new_name), merch_id);

  return (edit_result_t){.is_ok = true};
}

// static bool ioopm_get_shelf_storage(const ioopm_store_t *store,
//                                     const shelf_name_t shelf,
//                                     shelf_storage_t *storage_out) {
//   elem_t id;
//   if (!ioopm_hash_table_lookup(store->shelf_ids, c_ptr_elem(&shelf), &id)) {
//     return false;
//   }
//
//   elem_t shelf_storage;
//   assert(ioopm_hash_table_lookup(store->shelfs, id, &shelf_storage));
//
//   memcpy(storage_out, shelf_storage.cp, sizeof(shelf_storage_t));
//
//   return true;
// }

ioopm_list_t *ioopm_store_get_stock(const ioopm_store_t *store,
                                    const char *merch_name) {
  elem_t merch_id_elem;
  if (!ioopm_hash_table_lookup(store->merch_ids, c_str_elem(merch_name),
                               &merch_id_elem)) {
    return NULL;
  }
  elem_t locations_elem;
  assert(ioopm_hash_table_lookup(store->locations, merch_id_elem,
                                 &locations_elem));

  ioopm_list_t *stock = ioopm_list_create(stock_remove_fn);
  ioopm_list_iterator_t *iter = ioopm_list_iterator_create(locations_elem.p);

  while (!ioopm_list_iterator_at_end(iter)) {
    elem_t shelf_id = ioopm_list_iterator_current(iter);

    elem_t storage_elem;
    assert(ioopm_hash_table_lookup(store->shelfs, shelf_id, &storage_elem));
    shelf_storage_t *storage = storage_elem.p;

    shelf_t *shelf = calloc(1, sizeof(shelf_t));
    *shelf = (shelf_t){.quantity = storage->quantity,
                       .shelf_name = storage->shelf_name};

    ioopm_list_append(stock, ptr_elem(shelf));

    ioopm_list_iterator_advance(iter);
  }
  ioopm_list_iterator_destroy(iter);
  return stock;
}

static void stock_remove_fn(elem_t elem) { free(elem.p); }

static bool string_eq_function(elem_t a, elem_t b) {
  return strcmp(a.s, b.s) == 0;
}

static size_t string_hash_function(elem_t key) {
  const char *str = key.cs;

  size_t result = 0;
  while (*str != '\0') {
    result = result * 31 + ((unsigned char)*str);
    str++;
  }
  return result;
}

static bool shelf_eql_fn(elem_t a, elem_t b) {
  shelf_name_t *shelf_a = (shelf_name_t *)a.p;
  shelf_name_t *shelf_b = (shelf_name_t *)b.p;

  return shelf_a->c == shelf_b->c && shelf_a->n == shelf_b->n;
}

static size_t shelf_hash_fn(elem_t shelf) {

  shelf_name_t *s = (shelf_name_t *)shelf.p;
  return (size_t)s->c + s->n;
}

static bool size_t_eql_fn(elem_t a, elem_t b) { return a.sz == b.sz; }
static size_t size_t_hash_fn(elem_t s) { return s.sz; }

// static void remove_string(elem_t string) { free(string.s); }
static void remove_any_elem(elem_t elem) { free(elem.p); }

static void remove_merch_value(elem_t merch) { ioopm_merch_destroy(merch.p); }

static void remove_list_value(elem_t list) { ioopm_list_destroy(list.p); }

static void add_shelf(ioopm_store_t *store, shelf_name_t shelf_name,
                      size_t merch_id, size_t quantity) {

  shelf_storage_t *storage = calloc(1, sizeof(shelf_storage_t));
  storage->merch_id = merch_id;
  storage->quantity = quantity;
  storage->shelf_name = shelf_name;

  elem_t shelf_id = size_elem(store->current_id++);
  ioopm_hash_table_insert(store->shelfs, shelf_id, ptr_elem(storage));

  ioopm_hash_table_insert(store->shelf_ids, c_ptr_elem(&storage->shelf_name),
                          shelf_id);

  elem_t locations;
  assert(ioopm_hash_table_lookup(store->locations, size_elem(merch_id),
                                 &locations));
  ioopm_list_append(locations.p, shelf_id);
}

static bool increase_quantity(ioopm_store_t *store, size_t shelf_id,
                              size_t quantity) {

  elem_t storage_elem;
  if (!ioopm_hash_table_lookup(store->shelfs, size_elem(shelf_id),
                               &storage_elem)) {
    return false;
  }
  shelf_storage_t *storage = storage_elem.p;
  storage->quantity += quantity;
  return true;
}
