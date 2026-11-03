#include "store.h"
#include "../../vendor/hash_table_iterator.h"
#include "../../vendor/linked_list_iterator.h"
#include "./merch.h"
#include "cart.h"
#include "result.h"
#include "shelf.h"
#include <string.h>
#include <assert.h>
#include <ctype.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

typedef RESULT(merch_t *, search_error_t) get_merch_result_t;

static bool shelf_eql_fn(elem_t a, elem_t b);

static size_t string_hash_function(elem_t key);
static bool string_eq_function(elem_t a, elem_t b);
static size_t shelf_hash_fn(elem_t shelf);

static bool size_t_eql_fn(elem_t a, elem_t b);
static size_t size_t_hash_fn(elem_t s);

// static void remove_string(elem_t string);

static void remove_merch_value(elem_t merch);
static void remove_any_elem(elem_t elem);
static void remove_cart(elem_t cart_elem);

static bool merch_less_than(elem_t merch_elem_a, elem_t merch_elem_b);

static void remove_ioopm_list_value(elem_t list);
static void remove_merch_form_list(elem_t merch_elem);

static void stock_remove_fn(elem_t elem);

// static bool get_shelf_storage(const store_t *store,
//                                     const shelf_name_t shelf,
//                                     shelf_storage_t *storage_out);

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
      ioopm_hash_table_create(size_t_hash_fn, size_t_eql_fn, NULL, remove_cart);

  store->reserved_stock =
      ioopm_hash_table_create(size_t_hash_fn, size_t_eql_fn, NULL, NULL);

  store->current_id = 0;
  store->current_cart_id = 0;

  return store;
}

void store_destroy(store_t *store) {

  ioopm_hash_table_destroy(store->shelfs);
  ioopm_hash_table_destroy(store->merch);
  ioopm_hash_table_destroy(store->shelf_ids);
  ioopm_hash_table_destroy(store->merch_ids);
  ioopm_hash_table_destroy(store->locations);

  free(store);
}

bool merch_exists(store_t *store, const char *merch_name) {
  (void)store;
  (void)merch_name;
  return false;
}

// kanske olika fkn sviter för de olika ht:na i store? återanvändning?
//  högre nivå för check? lär vara bra, slipper standard outputs
const merch_t *store_get_merch(store_t *store, const char *merch_name) {
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
bool store_add_merch(store_t *store, const char *name, const char *desc,
                     const size_t price) {
  elem_t merch_id = size_elem(0);
  bool merch_found =
      ioopm_hash_table_lookup(store->merch_ids, c_str_elem(name), &merch_id);

  if (merch_found) {
    return false;
  }

  merch_t *merch = merch_create(name, desc, price);

  merch_id = size_elem(store->current_id++);

  ioopm_hash_table_insert(store->merch_ids, str_elem(merch->name), merch_id);

  ioopm_hash_table_insert(store->merch, merch_id, ptr_elem(merch));
  ioopm_list_t *locations = ioopm_list_create(NULL);
  ioopm_hash_table_insert(store->locations, merch_id, ptr_elem(locations));
  ioopm_hash_table_insert(store->reserved_stock, merch_id, size_elem(0));

  return true;
}

ioopm_list_t *store_merch_list(const store_t *store) {
  ioopm_list_t *list = ioopm_list_create(remove_merch_value);

  ioopm_hash_table_iterator_t *merch_iter =
      ioopm_hash_table_iterator_create(store->merch);
  while (!ioopm_hash_table_iterator_at_end(merch_iter)) {

    const elem_t current_elem =
        ioopm_hash_table_iterator_current_value(merch_iter);
    const merch_t *current_merch = (merch_t *)current_elem.p;

    merch_t *list_item = calloc(1, sizeof(merch_t));

    list_item->name = strdup(current_merch->name);
    list_item->desc = strdup(current_merch->desc);
    list_item->price = current_merch->price;

    elem_t list_elem = (elem_t){.p = list_item};

    ioopm_list_append(list, list_elem);

    ioopm_hash_table_iterator_advance(merch_iter);
  }

  ioopm_list_sort(list, merch_less_than);

  return list;
}

// returns false if there exist other type of merch on the specified shelf or
// quantity is 0
// TODO: update location of merch (change to ludvigs list impl)
replenish_result_t store_replenish_merch(store_t *store, const char *merch_name,
                                         const shelf_name_t shelf_name,
                                         const size_t quantity) {
  assert(quantity != 0);

  elem_t merch_id;
  bool merch_found = ioopm_hash_table_lookup(
      store->merch_ids, str_elem((char *)merch_name), &merch_id);

  if (!merch_found) {
    return (replenish_result_t){.is_ok = false, .error = MERCH_NOT_IN_STORE};
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
bool store_remove_merch(store_t *store, const char *merch_name) {
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

edit_result_t store_edit_merch(store_t *store, const char *old_name,
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

  merch_t *merch = merch_create(new_name, new_desc, new_price);

  ioopm_hash_table_insert(store->merch, merch_id, ptr_elem(merch));
  ioopm_hash_table_insert(store->merch_ids, c_str_elem(new_name), merch_id);

  return (edit_result_t){.is_ok = true};
}

size_t store_add_cart(store_t *store) {
  
  size_t cart_id = ++(store->current_cart_id);

  ioopm_hash_table_t *cart = cart_create();

  ioopm_hash_table_insert(store->carts, size_elem(cart_id), ptr_elem(cart));

  return cart_id;
}

bool store_cart_remove(store_t *store, size_t cart_id) {
  elem_t _;
  return ioopm_hash_table_remove(store->carts, size_elem(cart_id), &_);  
}

size_t store_count_stock(store_t *store, const char *merch_name) {
  size_t count = 0;
  ioopm_list_t *stock = store_get_stock(store, merch_name);

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

cart_result_t store_cart_add_merch(store_t *store, size_t cart_id,
                                   const char *merch_name, size_t quantity)
{
  assert(quantity > 0);
  elem_t cart_elem;
  if(!ioopm_hash_table_lookup(store->carts, size_elem(cart_id), &cart_elem))
  {
    return (cart_result_t) {.is_ok = false, .error = CART_NOT_FOUND};
  }

  elem_t merch_id_elem;
  if(!ioopm_hash_table_lookup(store->merch_ids, c_str_elem(merch_name), &merch_id_elem))
  {
    return (cart_result_t) {.is_ok = false, .error = MERCH_NOT_FOUND};
  }

  elem_t reserved_elem;
  assert(ioopm_hash_table_lookup(store->reserved_stock, merch_id_elem, &reserved_elem));

  size_t reserved = reserved_elem.sz;

  if((store_count_stock(store, merch_name) - reserved) < quantity)
  {
    return (cart_result_t) {.is_ok = false, .error = QUANTITY_TOO_LARGE};
  }

  ioopm_hash_table_t *cart = cart_elem.p;

  ioopm_hash_table_insert(store->reserved_stock, merch_id_elem, size_elem(reserved + quantity));

  elem_t cart_quant_elem;
  size_t total_quantity = quantity; 
  if(ioopm_hash_table_lookup(cart, merch_id_elem, &cart_quant_elem))
  {
    total_quantity += cart_quant_elem.sz;
  }
  ioopm_hash_table_insert(cart, merch_id_elem, size_elem(total_quantity));

  return (cart_result_t){.is_ok = true};
}

cart_result_t store_cart_remove_merch(store_t *store, size_t cart_id,
                                      const char *merch_name, size_t quantity)
{
  elem_t cart_elem;
  if(!ioopm_hash_table_lookup(store->carts, size_elem(cart_id), &cart_elem))
  {
    return (cart_result_t) {.is_ok = false, .error = CART_NOT_FOUND};
  }

  elem_t merch_id_elem;
  if(!ioopm_hash_table_lookup(store->merch_ids, c_str_elem(merch_name), &merch_id_elem))
  {
    return (cart_result_t) {.is_ok = false, .error = MERCH_NOT_FOUND};
  }

  ioopm_hash_table_t *cart = cart_elem.p;
  elem_t cart_quant_elem;
  if(!ioopm_hash_table_lookup(cart, merch_id_elem, &cart_quant_elem))
  {
    //add another error type specific for remove
    return (cart_result_t) {.is_ok = false, .error = MERCH_NOT_FOUND};
  }

  size_t cart_quant = cart_quant_elem.sz;

  if(cart_quant < quantity)
  {
    return (cart_result_t) {.is_ok = false, .error = QUANTITY_TOO_LARGE};
  }
  
  if(cart_quant == quantity)
  {
    elem_t _;
    assert(ioopm_hash_table_remove(cart, merch_id_elem, &_));
  }
  else
  {
    ioopm_hash_table_insert(cart, merch_id_elem, size_elem(cart_quant - quantity));
  }

  elem_t reserved_elem;
  assert(ioopm_hash_table_lookup(store->reserved_stock, merch_id_elem, &reserved_elem));
  size_t reserved = reserved_elem.sz;
  assert(reserved >= quantity);

  ioopm_hash_table_insert(store->reserved_stock, merch_id_elem, size_elem(reserved - quantity));

  return (cart_result_t){.is_ok = true};
}

bool store_cart_cost(store_t *store, size_t cart_id, size_t *out_cost) {
  
  size_t cost = 0;

  elem_t cart_elem;
  if(!ioopm_hash_table_lookup(store->carts, size_elem(cart_id), &cart_elem))
  {
    return false;
  }

  ioopm_hash_table_t *cart = cart_elem.p;
  ioopm_hash_table_iterator_t *iter = ioopm_hash_table_iterator_create(cart);

  while(!ioopm_hash_table_iterator_at_end(iter))
  {
    elem_t merch_id_elem = ioopm_hash_table_iterator_current_key(iter);
    elem_t quantity_elem = ioopm_hash_table_iterator_current_value(iter);

    elem_t merch_elem;
    assert(ioopm_hash_table_lookup(store->merch, merch_id_elem, &merch_elem));

    size_t quantity = quantity_elem.sz;
    merch_t *merch = merch_elem.p;
    
    cost += (quantity * (merch->price));

    ioopm_hash_table_iterator_advance(iter);
  }


  *out_cost = cost;
  return true;
}

bool store_cart_checkout(store_t *store, size_t cart_id) {
  (void)store;
  (void)cart_id;
  return false;
}

static bool shelf_less_than(elem_t a, elem_t b) {
  shelf_t *shelf_a = (shelf_t *)a.p;
  shelf_t *shelf_b = (shelf_t *)b.p;

  if (shelf_a->shelf_name.c == shelf_b->shelf_name.c) {
    return shelf_a->shelf_name.n < shelf_b->shelf_name.n;
  }

  return shelf_a->shelf_name.c < shelf_b->shelf_name.c;
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

static char *toupperDupe(const char *str);

static bool merch_less_than(elem_t merch_elem_a, elem_t merch_elem_b) {
  merch_t *merch_a = (merch_t *)merch_elem_a.p;
  merch_t *merch_b = (merch_t *)merch_elem_b.p;

  char *upper_name_a = toupperDupe(merch_a->name);
  char *upper_name_b = toupperDupe(merch_b->name);

  int result = strcmp(upper_name_a, upper_name_b);

  free(upper_name_a);
  free(upper_name_b);

  return result < 0;
}

static char *toupperDupe(const char *str) {

  const size_t len = strlen(str);
  char *upper = calloc(len + 1, sizeof(char));

  for (size_t i = 0; i < len; i++) {
    upper[i] = toupper(str[i]);
  }
  upper[len] = 0;
  return upper;
}

static void remove_merch_from_list(elem_t merch_elem) {
  merch_t *merch = (merch_t *)merch_elem.p;
  free(merch->name);
  free(merch->desc);
  free(merch);
}

static void remove_cart(elem_t cart_elem) {
  ioopm_hash_table_t *cart = cart_elem.p;
  ioopm_hash_table_destroy(cart);
}
