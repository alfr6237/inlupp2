#include "store.h"
#include <merch.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

size_t current_id = 0;

typedef struct shelf_storage {
  size_t merch_id;
  size_t quantity;
} shelf_storage_t;

static size_t nextId() { return current_id++; }
static bool shelf_eql_fn(elem_t a, elem_t b);

static size_t string_hash_function(elem_t key);
static bool string_eq_function(elem_t a, elem_t b);
static size_t shelf_hash_fn(elem_t shelf);

static bool size_t_eql_fn(elem_t a, elem_t b);
static size_t size_t_hash_fn(elem_t s);

static void remove_string(elem_t string);
static void remove_shelf_key(elem_t shelf);

static void remove_merch_value(elem_t merch);
static void remove_any_elem(elem_t elem);

ioopm_store_t *ioopm_store_create() {
  ioopm_store_t *store = calloc(sizeof(ioopm_store_t), 1);

  store->merch_ids = ioopm_hash_table_create(string_hash_function,
                                             string_eq_function, NULL, NULL);
  store->shelf_ids = ioopm_hash_table_create(shelf_hash_fn, shelf_eql_fn,
                                             remove_any_elem, NULL);
  store->merch = ioopm_hash_table_create(size_t_hash_fn, size_t_eql_fn, NULL,
                                         remove_merch_value);
  store->shelfs = ioopm_hash_table_create(size_t_hash_fn, size_t_eql_fn, NULL,
                                          remove_any_elem);

  return store;
}
void ioopm_store_destroy(ioopm_store_t *store) {

  ioopm_hash_table_destroy(store->shelfs);
  ioopm_hash_table_destroy(store->merch);

  ioopm_hash_table_destroy(store->merch);

  free(store);
}

bool ioopm_merch_exists(ioopm_store_t *store, const char *merch_name) {
  return false;
}

ioopm_merch_t *ioopm_store_get_merch(ioopm_store_t *store,
                                     const char *merch_name) {
  return NULL;
}

void ioopm_store_add_merch(ioopm_store_t *store, ioopm_merch_t *merch) {}

// returns false if there exist other type of merch on the specified shelf or
// quantity is 0
bool ioopm_store_replenish_merch(ioopm_store_t *store, const char *merch_name,
                                 const ioopm_shelf_t shelf, size_t quantity) {
  return false;
}

void ioopm_store_remove_merch(ioopm_store_t *store, const char *merch_name) {}

void ioopm_store_edit_merch_name(ioopm_store_t *store,
                                 const char *old_merch_name,
                                 const char *new_merch_name);

void ioopm_store_edit_merch_desc(ioopm_store_t *store, const char *merch_name,
                                 const char *new_desc) {}
void ioopm_store_edit_price(ioopm_store_t *store, const char *merch_name,
                            size_t new_price) {}

static bool string_eq_function(elem_t a, elem_t b) {
  return strcmp(a.s, b.s) == 0;
}

static size_t string_hash_function(elem_t key) {
  char *str = key.s;

  size_t result = 0;
  while (*str != '\0') {
    result = result * 31 + ((unsigned char)*str);
    str++;
  }
  return result;
}

static bool shelf_eql_fn(elem_t a, elem_t b) {
  ioopm_shelf_t *shelf_a = (ioopm_shelf_t *)a.p;
  ioopm_shelf_t *shelf_b = (ioopm_shelf_t *)a.p;

  return shelf_a->c == shelf_b->c && shelf_a->n == shelf_b->n;
}

static size_t shelf_hash_fn(elem_t shelf) {

  ioopm_shelf_t *s = (ioopm_shelf_t *)shelf.p;
  return (size_t)s->c + s->n;
}

static bool size_t_eql_fn(elem_t a, elem_t b) { return a.sz == b.sz; }
static size_t size_t_hash_fn(elem_t s) { return s.sz; }

static void remove_string(elem_t string) { free(string.s); }
static void remove_any_elem(elem_t elem) { free(elem.p); }

static void remove_merch_value(elem_t merch) { ioopm_merch_destroy(merch.p); }
