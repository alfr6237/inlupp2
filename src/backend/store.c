#include "store.h"
#include "../../vendor/linked_list_iterator.h"
#include "cart.h"
#include "merch.h"
#include "result.h"
#include <assert.h>
#include <ctype.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

static bool shelf_eql_fn(elem_t a, elem_t b);

static size_t string_hash_function(elem_t key);
static bool string_eq_function(elem_t a, elem_t b);
static size_t shelf_hash_fn(elem_t shelf);

static bool shelf_less_than(elem_t a, elem_t b);

static bool size_t_eql_fn(elem_t a, elem_t b);
static size_t size_t_hash_fn(elem_t s);

// static void remove_string(elem_t string);

static void remove_merch_value(elem_t merch);
static void remove_any_elem(elem_t elem);

static void remove_ioopm_list_value(elem_t list);

static void stock_remove_fn(elem_t elem);

store_t *store_create(void) {
  store_t *store = calloc(1, sizeof(store_t));

  store->merch_ids = ioopm_hash_table_create(string_hash_function,
                                             string_eq_function, NULL, NULL);
  store->shelf_ids =
      ioopm_hash_table_create(shelf_hash_fn, shelf_eql_fn, NULL, NULL);
  store->merch = ioopm_hash_table_create(size_t_hash_fn, size_t_eql_fn, NULL,
                                         remove_merch_value);
  store->shelfs = ioopm_hash_table_create(size_t_hash_fn, size_t_eql_fn, NULL,
                                          remove_any_elem);
  store->locations = ioopm_hash_table_create(size_t_hash_fn, size_t_eql_fn,
                                             NULL, remove_ioopm_list_value);

  store->carts =
      ioopm_hash_table_create(size_t_hash_fn, size_t_eql_fn, NULL, NULL);

  store->reserved_stock =
      ioopm_hash_table_create(size_t_hash_fn, size_t_eql_fn, NULL, NULL);

  store->current_id = 0;
  store->current_cart_id = 0;

  return store;
}

void store_destroy(store_t *store) {

  destroy_cart_table(store);

  ioopm_hash_table_destroy(store->shelfs);
  ioopm_hash_table_destroy(store->merch);
  ioopm_hash_table_destroy(store->shelf_ids);
  ioopm_hash_table_destroy(store->merch_ids);
  ioopm_hash_table_destroy(store->locations);
  ioopm_hash_table_destroy(store->reserved_stock);

  free(store);
}

ioopm_list_t *store_get_stock(const store_t *store, const char *merch_name) {
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

  ioopm_list_sort(stock, shelf_less_than);

  return stock;
}

size_t store_count_stock(store_t *store, const char *merch_name) {
  size_t count = 0;
  // handling om stock inte finns
  ioopm_list_t *stock = store_get_stock(store, merch_name);
  if (stock == NULL) {
    return 0;
  }
  ioopm_list_iterator_t *iter = ioopm_list_iterator_create(stock);

  while (!ioopm_list_iterator_at_end(iter)) {
    elem_t current_elem = ioopm_list_iterator_current(iter);
    shelf_t *shelf = current_elem.p;

    count += shelf->quantity;
    ioopm_list_iterator_advance(iter);
  }

  ioopm_list_iterator_destroy(iter);
  ioopm_list_destroy(stock);

  return count;
}

static bool shelf_less_than(elem_t a, elem_t b) {
  shelf_t *shelf_a = (shelf_t *)a.p;
  shelf_t *shelf_b = (shelf_t *)b.p;

  if (shelf_a->shelf_name.c == shelf_b->shelf_name.c) {
    return shelf_a->shelf_name.n < shelf_b->shelf_name.n;
  }

  return shelf_a->shelf_name.c < shelf_b->shelf_name.c;
}

static size_t get_stock_size_impl(link_t *link, size_t sum);

size_t get_stock_size(ioopm_list_t *stock) {
  return get_stock_size_impl(stock->first, 0);
}

static size_t get_stock_size_impl(link_t *link, size_t sum) {
  if (link == NULL) {
    return sum;
  }
  shelf_t *shelf = (shelf_t *)link->element.p;
  sum += shelf->quantity;
  link_t *next = link->next;
  return get_stock_size_impl(next, sum);
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

static void remove_merch_value(elem_t merch) { merch_destroy(merch.p); }

static void remove_ioopm_list_value(elem_t list) { ioopm_list_destroy(list.p); }
