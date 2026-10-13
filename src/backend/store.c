#include "store.h"
#include "./merch.h"
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

typedef struct shelf_storage {
  size_t merch_id;
  size_t quantity;
} shelf_storage_t;

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
  store->current_id = 0;

  return store;
}

void ioopm_store_destroy(ioopm_store_t *store) {

  ioopm_hash_table_destroy(store->shelfs);
  ioopm_hash_table_destroy(store->merch);
  ioopm_hash_table_destroy(store->shelf_ids);
  ioopm_hash_table_destroy(store->merch_ids);

  free(store);
}

bool ioopm_merch_exists(ioopm_store_t *store, const char *merch_name) {
  return false;
}

//kanske olika fkn sviter för de olika ht:na i store? återanvändning?
// högre nivå för check? lär vara bra, slipper standard outputs
ioopm_merch_t *ioopm_store_get_merch(ioopm_store_t *store,
                                     const char *merch_name)
{
  elem_t m_id = size_elem(0);
  bool merch_found = ioopm_hash_table_lookup(store->merch_ids, str_elem(merch_name), &m_id);

  if (merch_found)
  {
    elem_t merch_gotten = ptr_elem(NULL);
    assert(ioopm_hash_table_lookup(store->merch, m_id, &merch_gotten));
    return merch_gotten.p;
  }

  return NULL;
}

// TODO: When do we check if merch name is ok to add, no dup? static?
bool ioopm_store_add_merch(ioopm_store_t *store, char *name, char *desc, int price) 
{  
  elem_t m_id = size_elem(0);
  bool merch_found = ioopm_hash_table_lookup(store->merch_ids, str_elem(name), &m_id);

  if(merch_found)
  {
    return false;
  }

  ioopm_merch_t *merch = ioopm_merch_create(name, desc, price);

  size_t merch_id = store->current_id++;

  ioopm_hash_table_insert(store->merch_ids, str_elem(merch->name), size_elem(merch_id));

  ioopm_hash_table_insert(store->merch, size_elem(merch_id), ptr_elem(merch));

  return true;
}

// returns false if there exist other type of merch on the specified shelf or
// quantity is 0
enum replenish_result (ioopm_store_t *store, const char *merch_name,
                                 const ioopm_shelf_t shelf, size_t quantity)
{
  elem_t m_id = size_elem(0);
  bool merch_found = ioopm_hash_table_lookup(store->merch_ids, str_elem(merch_name), &m_id);

  if(merch_found)
  {
    return MERCH_NOT_FOUND;
  }

  elem_t s_id = size_elem(0);
  bool shelf_found = ioopm_hash_table_lookup(store->shelf_ids, ptr_elem(&shelf), &s_id);

  shelf_storage_t *shelf_storage;

  if(shelf_found)
  {
    elem_t shelf_elem;
    assert(ioopm_hash_table_lookup(store->shelfs, s_id, &shelf_elem));
    shelf_storage = shelf_elem.p;

    if(shelf_storage->merch_id != m_id.sz)
    {
      return MERCH_CONFLICT;
    }
  }
  else
  {
    ioopm_shelf_t *shelf_ptr = calloc(sizeof(ioopm_shelf_t), 1);
    s_id.sz = store->current_id++;
    memcpy(shelf_ptr, &shelf, sizeof(ioopm_shelf_t));

    ioopm_hash_table_insert(store->shelf_ids, ptr_elem(shelf_ptr), s_id);
    
    shelf_storage = calloc(sizeof(shelf_storage_t), 1);
    shelf_storage->merch_id = m_id.sz;
    shelf_storage->quantity = 0;

    ioopm_hash_table_insert(store->shelfs, s_id, ptr_elem(shelf_storage));
  }

  shelf_storage->quantity += quantity;

  return SUCESS;
}

// TODO: When is name check for existence done? prolly not this lvl, kanske static?
// PRE: should not be ran before merchandise has been removed from all shelves
bool ioopm_store_remove_merch(ioopm_store_t *store, const char *merch_name) 
{
  elem_t m_id = int_elem(-1); //initialization?
  bool merch_found = ioopm_hash_table_remove(store->merch_ids, str_elem(merch_name), &m_id);

  if(!merch_found)
  {
    return false;
  }
  elem_t _;
  assert(ioopm_hash_table_remove(store->merch, m_id, &_));

  return true;
}

void ioopm_store_edit_merch_name(ioopm_store_t *store,
                                 const char *old_merch_name,
                                 const char *new_merch_name);

void ioopm_store_edit_merch_desc(ioopm_store_t *store, const char *merch_name,
                                 const char *new_desc)
{

}

void ioopm_store_edit_price(ioopm_store_t *store, const char *merch_name,
                            size_t new_price)
{

}

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
