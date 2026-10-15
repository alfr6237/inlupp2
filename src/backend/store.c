#include "store.h"
#include "./merch.h"
#include "result.h"
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

ioopm_store_t *ioopm_store_create(void) {
  ioopm_store_t *store = calloc(1, sizeof(ioopm_store_t));

  store->merch_ids = ioopm_hash_table_create(string_hash_function,
                                             string_eq_function, NULL, NULL);
  store->shelf_ids = ioopm_hash_table_create(shelf_hash_fn, shelf_eql_fn,
                                             remove_any_elem, NULL);
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
  ioopm_list_t *locations = ioopm_list_create();
  ioopm_hash_table_insert(store->locations, merch_id, ptr_elem(locations));

  return true;
}

// returns false if there exist other type of merch on the specified shelf or
// quantity is 0
// TODO: update location of merch (change to ludvigs list impl)
replenish_result_t ioopm_store_replenish_merch(ioopm_store_t *store,
                                               const char *merch_name,
                                               const ioopm_shelf_t shelf,
                                               size_t quantity) {
  assert(quantity != 0);
  elem_t merch_id = size_elem(0);
  bool merch_found = ioopm_hash_table_lookup(
      store->merch_ids, str_elem((char *)merch_name), &merch_id);

  if (!merch_found) {
    return (replenish_result_t){.is_ok = false, .error = MERCH_NOT_FOUND};
  }

  elem_t shelf_id = size_elem(0);
  bool shelf_found = ioopm_hash_table_lookup(
      store->shelf_ids, ptr_elem((ioopm_shelf_t *)&shelf), &shelf_id);

  shelf_storage_t *shelf_storage;

  if (shelf_found) {
    elem_t shelf_elem;
    assert(ioopm_hash_table_lookup(store->shelfs, shelf_id, &shelf_elem));
    shelf_storage = shelf_elem.p;

    if (strcmp(shelf_storage->merch_name, merch_name) != 0) {
      return (replenish_result_t){.is_ok = false, .error = MERCH_CONFLICT};
    }
  } else {
    ioopm_shelf_t *shelf_ptr = calloc(1, sizeof(ioopm_shelf_t));
    shelf_id.sz = store->current_id++;
    memcpy(shelf_ptr, &shelf, sizeof(ioopm_shelf_t));

    ioopm_hash_table_insert(store->shelf_ids, ptr_elem(shelf_ptr), shelf_id);

    shelf_storage = calloc(1, sizeof(shelf_storage_t));

    elem_t merch;
    assert(ioopm_hash_table_lookup(store->merch, merch_id, &merch));

    shelf_storage->merch_name = ((ioopm_merch_t *)merch.p)->name;
    shelf_storage->quantity = 0;

    ioopm_hash_table_insert(store->shelfs, shelf_id, ptr_elem(shelf_storage));
  }

  shelf_storage->quantity += quantity;
  elem_t locations;
  assert(ioopm_hash_table_lookup(store->locations, merch_id, &locations));

  ioopm_list_append(locations.p, shelf_id);

  return (replenish_result_t){.is_ok = true};
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

  // NOTE: Bug introduced
  //

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

get_shelf_storage_result_t ioopm_get_shelf_storage(ioopm_store_t *store,
                                                   const ioopm_shelf_t shelf) {
  elem_t id;
  if (!ioopm_hash_table_lookup(store->shelf_ids, c_ptr_elem(&shelf), &id)) {
    return (get_shelf_storage_result_t){.is_ok = false, .error = NOT_FOUND};
  }

  elem_t shelf_storage;
  assert(ioopm_hash_table_lookup(store->shelfs, id, &shelf_storage));

  get_shelf_storage_result_t result = {.is_ok = true};
  memcpy(&result.value, shelf_storage.cp, sizeof(shelf_storage_t));

  return result;
}

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
  ioopm_shelf_t *shelf_a = (ioopm_shelf_t *)a.p;
  ioopm_shelf_t *shelf_b = (ioopm_shelf_t *)b.p;

  return shelf_a->c == shelf_b->c && shelf_a->n == shelf_b->n;
}

static size_t shelf_hash_fn(elem_t shelf) {

  ioopm_shelf_t *s = (ioopm_shelf_t *)shelf.p;
  return (size_t)s->c + s->n;
}

static bool size_t_eql_fn(elem_t a, elem_t b) { return a.sz == b.sz; }
static size_t size_t_hash_fn(elem_t s) { return s.sz; }

// static void remove_string(elem_t string) { free(string.s); }
static void remove_any_elem(elem_t elem) { free(elem.p); }

static void remove_merch_value(elem_t merch) { ioopm_merch_destroy(merch.p); }

static void remove_list_value(elem_t list) { ioopm_list_destroy(list.p); }
