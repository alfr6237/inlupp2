#include "merch.h"
#include "../../vendor/hash_table_iterator.h"
#include "common.h"
#include "shelf.h"
#include "store.h"
#include <assert.h>
#include <ctype.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

static ioopm_merch_t *merch_create(const char *name, const char *desc,
                                   const size_t price);

static void remove_merch_value(elem_t merch);

static bool merch_less_than(elem_t merch_elem_a, elem_t merch_elem_b);

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
                           const char *desc, const size_t price) {
  elem_t merch_id = size_elem(0);
  bool merch_found =
      ioopm_hash_table_lookup(store->merch_ids, c_str_elem(name), &merch_id);

  if (merch_found) {
    return false;
  }

  ioopm_merch_t *merch = merch_create(name, desc, price);

  merch_id = size_elem(store->current_id++);

  ioopm_hash_table_insert(store->merch_ids, str_elem(merch->name), merch_id);
  ioopm_hash_table_insert(store->merch, merch_id, ptr_elem(merch));
  ioopm_list_t *locations = ioopm_list_create(NULL);
  ioopm_hash_table_insert(store->locations, merch_id, ptr_elem(locations));
  ioopm_hash_table_insert(store->reserved_stock, merch_id, size_elem(0));

  return true;
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

  // TODO: extract to a helper function to inrease readabillity
  ioopm_hash_table_iterator_t *carts_iter =
      ioopm_hash_table_iterator_create(store->carts);

  while (!ioopm_hash_table_iterator_at_end(carts_iter)) {
    elem_t current_elem = ioopm_hash_table_iterator_current_value(carts_iter);

    ioopm_hash_table_t *cart = current_elem.p;

    elem_t _;
    ioopm_hash_table_remove(cart, merch_id, &_);

    ioopm_hash_table_iterator_advance(carts_iter);
  }
  ioopm_hash_table_iterator_destroy(carts_iter);

  assert(ioopm_hash_table_remove(store->reserved_stock, merch_id, &_));

  return true;
}

ioopm_list_t *ioopm_store_merch_list(const ioopm_store_t *store) {
  ioopm_list_t *list = ioopm_list_create(remove_merch_value);

  ioopm_hash_table_iterator_t *merch_iter =
      ioopm_hash_table_iterator_create(store->merch);
  while (!ioopm_hash_table_iterator_at_end(merch_iter)) {

    const elem_t current_elem =
        ioopm_hash_table_iterator_current_value(merch_iter);
    const ioopm_merch_t *current_merch = (ioopm_merch_t *)current_elem.p;

    ioopm_merch_t *list_item = calloc(1, sizeof(ioopm_merch_t));

    list_item->name = strdup(current_merch->name);
    list_item->desc = strdup(current_merch->desc);
    list_item->price = current_merch->price;

    elem_t list_elem = (elem_t){.p = list_item};

    ioopm_list_append(list, list_elem);

    ioopm_hash_table_iterator_advance(merch_iter);
  }

  ioopm_hash_table_iterator_destroy(merch_iter);
  ioopm_list_sort(list, merch_less_than);

  return list;
}

// returns false if there exist other type of merch on the specified shelf or
// quantity is 0
// TODO: update location of merch (change to ludvigs list impl)
ioopm_replenish_result_t
ioopm_store_replenish_merch(ioopm_store_t *store, const char *merch_name,
                            const ioopm_shelf_name_t shelf_name,
                            const size_t quantity) {
  assert(quantity != 0);

  elem_t merch_id;
  bool merch_found = ioopm_hash_table_lookup(
      store->merch_ids, str_elem((char *)merch_name), &merch_id);

  if (!merch_found) {
    return (ioopm_replenish_result_t){.is_ok = false, .error = MERCH_NOT_FOUND};
  }

  elem_t shelf_id;
  bool shelf_found = ioopm_hash_table_lookup(
      store->shelf_ids, ptr_elem((ioopm_shelf_name_t *)&shelf_name), &shelf_id);

  if (!shelf_found) {
    shelf_id.sz = store->current_id++;

    ioopm_shelf_storage_t *storage =
        ioopm_create_shelf_storage(shelf_name, merch_id.sz, quantity);

    ioopm_hash_table_insert(store->shelfs, shelf_id, ptr_elem(storage));
    ioopm_hash_table_insert(store->shelf_ids, ptr_elem(&storage->shelf_name),
                            shelf_id);

    // NOTE: SEGAFAULT HERE IF REMOVED
    elem_t locations;
    assert(ioopm_hash_table_lookup(store->locations, merch_id, &locations));
    ioopm_list_append(locations.p, shelf_id);

    return (ioopm_replenish_result_t){.is_ok = true};
  }

  elem_t storage_elem;
  assert(ioopm_hash_table_lookup(store->shelfs, shelf_id, &storage_elem));

  return ioopm_replenish_merch(storage_elem.p, merch_id.sz, quantity);
}

ioopm_edit_result_t ioopm_store_edit_merch(ioopm_store_t *store,
                                           const char *old_name,
                                           const char *new_name,
                                           const char *new_desc,
                                           const size_t new_price) {

  if (!ioopm_hash_table_has_key(store->merch_ids, c_str_elem(old_name))) {
    return (ioopm_edit_result_t){.is_ok = false, .error = OLD_NAME_NOT_FOUND};
  }

  if (strcmp(old_name, new_name) != 0) {
    if (ioopm_hash_table_has_key(store->merch_ids, c_str_elem(new_name))) {
      return (ioopm_edit_result_t){.is_ok = false, .error = NEW_NAME_EXIST};
    }
  }

  elem_t merch_id;
  assert(ioopm_hash_table_remove(store->merch_ids, c_str_elem(old_name),
                                 &merch_id));

  ioopm_merch_t *merch = merch_create(new_name, new_desc, new_price);

  ioopm_hash_table_insert(store->merch, merch_id, ptr_elem(merch));
  // ht har ägandeskap över name istället för callern (new_name)
  ioopm_hash_table_insert(store->merch_ids, c_str_elem(merch->name), merch_id);

  return (ioopm_edit_result_t){.is_ok = true};
}

void ioopm_merch_destroy(ioopm_merch_t *merch) {
  free(merch->name);
  free(merch->desc);
  free(merch);
}
static ioopm_merch_t *merch_create(const char *name, const char *desc,
                                   const size_t price) {
  ioopm_merch_t *merch = calloc(1, sizeof(ioopm_merch_t));
  merch->name = strdup(name);
  merch->desc = strdup(desc);
  merch->price = price;
  return merch;
}

static void remove_merch_value(elem_t merch) { ioopm_merch_destroy(merch.p); }

// optimized merch_less_than
static bool merch_less_than(elem_t merch_elem_a, elem_t merch_elem_b) {
  ioopm_merch_t *merch_a = merch_elem_a.p;
  ioopm_merch_t *merch_b = merch_elem_b.p;

  const unsigned char *a = (const unsigned char *)merch_a->name;
  const unsigned char *b = (const unsigned char *)merch_b->name;

  while (*a && *b) {
    int char_a = toupper(*a);
    int char_b = toupper(*b);

    if (char_a != char_b) {
      return char_a < char_b;
    }

    a++;
    b++;
  }

  return *a < *b;
}
