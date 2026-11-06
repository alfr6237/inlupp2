#include "../src/backend/store.h"
#include "../vendor/linked_list_iterator.h"

#include <CUnit/Basic.h>

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

static const char *merch_names[4] = {
    "banan",
    "citron",
    "kiwi",
    "melon",
};

static const char *descriptions[4] = {
    "guleböj",
    "exotisk frukt",
    "fågel",
    "citrus",
};

static const ioopm_shelf_name_t shelf_names[4] = {
    {.c = 'A', .n = 1},
    {.c = 'B', .n = 2},
    {.c = 'C', .n = 3},
    {.c = 'D', .n = 4},
};

static const size_t prices[4] = {1, 2, 3, 4};
static const size_t quantities[4] = {64, 128, 256, 512};

static bool shelf_name_eql(ioopm_shelf_name_t shelf_a,
                           ioopm_shelf_name_t shelf_b) {
  return shelf_a.c == shelf_b.c && shelf_a.n == shelf_b.n;
}

void test_create_store(void) {
  ioopm_store_t *store = ioopm_store_create();
  ioopm_store_destroy(store);
}

void test_add_remove_merch(void) {

  ioopm_store_t *store = ioopm_store_create();

  ioopm_store_add_merch(store, merch_names[0], descriptions[0], prices[0]);
  const ioopm_merch_t *merch = ioopm_store_get_merch(store, merch_names[0]);

  CU_ASSERT(strcmp(merch_names[0], merch->name) == 0);
  CU_ASSERT(strcmp(descriptions[0], merch->desc) == 0);
  CU_ASSERT_EQUAL(merch->price, prices[0]);

  ioopm_store_remove_merch(store, merch_names[0]);

  CU_ASSERT_PTR_NULL(ioopm_store_get_merch(store, merch_names[0]));

  ioopm_store_destroy(store);
}

void test_replenish_merch_not_found(void) {
  ioopm_store_t *store = ioopm_store_create();

  const ioopm_replenish_result_t result = ioopm_store_replenish_merch(
      store, merch_names[0], shelf_names[0], quantities[0]);

  CU_ASSERT_FALSE(result.is_ok)
  CU_ASSERT_EQUAL(result.error, MERCH_NOT_FOUND);

  ioopm_store_destroy(store);
}

void test_replenish_merch_conflict(void) {
  ioopm_store_t *store = ioopm_store_create();

  ioopm_store_add_merch(store, merch_names[0], descriptions[0], prices[0]);
  ioopm_store_add_merch(store, merch_names[1], descriptions[1], prices[1]);

  ioopm_replenish_result_t result = ioopm_store_replenish_merch(
      store, merch_names[0], shelf_names[0], quantities[0]);
  CU_ASSERT_TRUE(result.is_ok);
  result = ioopm_store_replenish_merch(store, merch_names[1], shelf_names[0],
                                       quantities[0]);
  CU_ASSERT_FALSE(result.is_ok);
  CU_ASSERT_EQUAL(result.error, MERCH_CONFLICT);

  ioopm_store_destroy(store);
}

void test_replenish_many(void) {
  ioopm_store_t *store = ioopm_store_create();

  ioopm_store_add_merch(store, merch_names[0], descriptions[0], prices[1]);

  ioopm_replenish_result_t rep_result = ioopm_store_replenish_merch(
      store, merch_names[0], shelf_names[0], quantities[0]);

  CU_ASSERT_TRUE(rep_result.is_ok);
  ioopm_list_t *list = ioopm_store_get_stock(store, merch_names[0]);
  elem_t shelf_elem = ioopm_list_head(list);
  ioopm_shelf_t *shelf = shelf_elem.p;
  CU_ASSERT_TRUE(shelf_name_eql(shelf->shelf_name, shelf_names[0]));
  CU_ASSERT_EQUAL(shelf->quantity, quantities[0]);

  rep_result = ioopm_store_replenish_merch(store, merch_names[0],
                                           shelf_names[0], quantities[1]);

  ioopm_list_destroy(list);
  CU_ASSERT_TRUE(rep_result.is_ok);
  list = ioopm_store_get_stock(store, merch_names[0]);
  shelf_elem = ioopm_list_last(list);
  shelf = shelf_elem.p;
  CU_ASSERT_TRUE(shelf_name_eql(shelf->shelf_name, shelf_names[0]));
  CU_ASSERT_EQUAL(shelf->quantity, quantities[0] + quantities[1]);

  ioopm_list_destroy(list);

  ioopm_store_destroy(store);
}

void test_merch_edit_name_not_found(void) {

  ioopm_store_t *store = ioopm_store_create();
  ioopm_store_add_merch(store, merch_names[0], descriptions[0], prices[0]);

  const ioopm_edit_result_t result = ioopm_store_edit_merch(
      store, merch_names[1], merch_names[0], descriptions[1], 106);
  CU_ASSERT_FALSE(result.is_ok);
  CU_ASSERT_EQUAL(result.error, OLD_NAME_NOT_FOUND);

  ioopm_store_destroy(store);
}

void test_merch_edit_name_conflict(void) {

  ioopm_store_t *store = ioopm_store_create();
  ioopm_store_add_merch(store, merch_names[0], descriptions[0], prices[0]);

  ioopm_store_add_merch(store, merch_names[1], descriptions[1], prices[1]);

  const ioopm_edit_result_t result = ioopm_store_edit_merch(
      store, merch_names[0], merch_names[1], descriptions[1], prices[1]);
  CU_ASSERT_FALSE(result.is_ok);
  CU_ASSERT_EQUAL(result.error, NEW_NAME_EXIST);

  ioopm_store_destroy(store);
}

void test_merch_edit_success(void) {
  ioopm_store_t *store = ioopm_store_create();
  ioopm_store_add_merch(store, merch_names[0], descriptions[0], prices[0]);

  const ioopm_edit_result_t result = ioopm_store_edit_merch(
      store, merch_names[0], merch_names[1], descriptions[1], prices[1]);
  CU_ASSERT_TRUE(result.is_ok);

  const ioopm_merch_t *old_merch = ioopm_store_get_merch(store, merch_names[0]);
  CU_ASSERT_PTR_NULL(old_merch);

  const ioopm_merch_t *new_merch = ioopm_store_get_merch(store, merch_names[1]);
  CU_ASSERT_PTR_NOT_NULL(new_merch);

  CU_ASSERT_TRUE(strcmp(new_merch->name, merch_names[1]) == 0);
  CU_ASSERT_TRUE(strcmp(new_merch->desc, descriptions[1]) == 0);
  CU_ASSERT_EQUAL(new_merch->price, prices[1]);

  ioopm_store_destroy(store);
}

void test_ioopm_store_get_stock_non_existing(void) {

  ioopm_store_t *store = ioopm_store_create();
  CU_ASSERT_PTR_NULL(ioopm_store_get_stock(store, merch_names[0]));

  ioopm_store_add_merch(store, merch_names[0], descriptions[0], prices[0]);
  ioopm_list_t *stock = ioopm_store_get_stock(store, merch_names[0]);
  CU_ASSERT_PTR_NOT_NULL(stock);
  CU_ASSERT_TRUE(ioopm_list_is_empty(stock));

  ioopm_list_destroy(stock);

  ioopm_store_destroy(store);
}

void test_ioopm_store_get_stock(void) {

  ioopm_store_t *store = ioopm_store_create();
  CU_ASSERT_PTR_NULL(ioopm_store_get_stock(store, merch_names[0]));

  ioopm_store_add_merch(store, merch_names[0], descriptions[0], prices[0]);

  ioopm_store_replenish_merch(store, merch_names[0], shelf_names[0],
                              quantities[0]);
  ioopm_store_replenish_merch(store, merch_names[0], shelf_names[2],
                              quantities[2]);
  ioopm_store_replenish_merch(store, merch_names[0], shelf_names[3],
                              quantities[3]);
  ioopm_store_replenish_merch(store, merch_names[0], shelf_names[1],
                              quantities[1]);

  ioopm_list_t *stock = ioopm_store_get_stock(store, merch_names[0]);
  CU_ASSERT_PTR_NOT_NULL(stock);
  CU_ASSERT_FALSE(ioopm_list_is_empty(stock));

  ioopm_list_iterator_t *iter = ioopm_list_iterator_create(stock);

  for (size_t i = 0; !ioopm_list_iterator_at_end(iter); i++) {

    elem_t current_elem = ioopm_list_iterator_current(iter);
    ioopm_shelf_t *shelf = current_elem.p;
    CU_ASSERT_EQUAL(shelf->shelf_name.c, shelf_names[i].c);
    CU_ASSERT_EQUAL(shelf->shelf_name.n, shelf_names[i].n);
    CU_ASSERT_EQUAL(shelf->quantity, quantities[i]);

    ioopm_list_iterator_advance(iter);
  }

  ioopm_list_iterator_destroy(iter);
  ioopm_list_destroy(stock);

  ioopm_store_destroy(store);
}

void test_merch_list(void) {

  ioopm_store_t *store = ioopm_store_create();

  ioopm_list_t *empty_list = ioopm_store_merch_list(store);
  CU_ASSERT_PTR_NOT_NULL(empty_list);
  CU_ASSERT_TRUE(ioopm_list_is_empty(empty_list));

  ioopm_list_destroy(empty_list);

  for (size_t i = 0; i < 4; i++) {
    ioopm_store_add_merch(store, merch_names[i], descriptions[i], prices[i]);
  }

  ioopm_list_t *non_empty_list = ioopm_store_merch_list(store);
  ioopm_list_iterator_t *iter = ioopm_list_iterator_create(non_empty_list);
  for (size_t i = 0; i < 4; i++) {

    elem_t current = ioopm_list_iterator_current(iter);
    ioopm_merch_t *merch = current.p;

    CU_ASSERT_TRUE(strcmp(merch_names[i], merch->name) == 0);
    CU_ASSERT_TRUE(strcmp(descriptions[i], merch->desc) == 0);
    CU_ASSERT_EQUAL(prices[i], merch->price);

    CU_ASSERT_FALSE(ioopm_list_iterator_at_end(iter));
    ioopm_list_iterator_advance(iter);
  }

  ioopm_list_iterator_destroy(iter);
  ioopm_list_destroy(non_empty_list);

  ioopm_store_destroy(store);
}

void test_cart_add_remove(void) {
  ioopm_store_t *store = ioopm_store_create();

  const size_t random_cart_id = 77777777;

  CU_ASSERT_FALSE(ioopm_store_cart_remove(store, random_cart_id));

  const size_t cart_id = ioopm_store_add_cart(store);

  CU_ASSERT_TRUE(ioopm_store_cart_remove(store, cart_id));

  ioopm_store_destroy(store);
}

void test_cart_add_remove_merch(void) {

  ioopm_store_t *store = ioopm_store_create();

  ioopm_store_add_merch(store, merch_names[0], descriptions[0], prices[0]);

  const size_t random_cart_id = 77777777;

  ioopm_cart_result_t result =
      ioopm_store_cart_add_merch(store, random_cart_id, merch_names[0], 1);
  CU_ASSERT_FALSE(result.is_ok);
  CU_ASSERT_TRUE(result.error == CART_NOT_FOUND);

  result =
      ioopm_store_cart_remove_merch(store, random_cart_id, merch_names[0], 0);
  CU_ASSERT_FALSE(result.is_ok);
  CU_ASSERT_TRUE(result.error == CART_NOT_FOUND);

  const size_t cart_id = ioopm_store_add_cart(store);

  result = ioopm_store_cart_add_merch(store, cart_id, "FINNS INTE", 1);
  CU_ASSERT_FALSE(result.is_ok);
  CU_ASSERT_TRUE(result.error == MERCH_NOT_IN_STORE);

  result = ioopm_store_cart_remove_merch(store, cart_id, "FINNS INTE", 0);
  CU_ASSERT_FALSE(result.is_ok);
  CU_ASSERT_TRUE(result.error == MERCH_NOT_IN_STORE);

  result = ioopm_store_cart_remove_merch(store, cart_id, merch_names[0], 0);
  CU_ASSERT_FALSE(result.is_ok);
  CU_ASSERT_TRUE(result.error == MERCH_NOT_IN_CART);

  result =
      ioopm_store_cart_add_merch(store, cart_id, merch_names[0], quantities[0]);
  CU_ASSERT_FALSE(result.is_ok);
  CU_ASSERT_TRUE(result.error == QUANTITY_TOO_LARGE);

  ioopm_store_replenish_merch(store, merch_names[0], shelf_names[0],
                              quantities[0]);

  result =
      ioopm_store_cart_add_merch(store, cart_id, merch_names[0], quantities[0]);
  CU_ASSERT_TRUE(result.is_ok);

  result = ioopm_store_cart_remove_merch(store, cart_id, merch_names[0],
                                         quantities[0] + 1);
  CU_ASSERT_FALSE(result.is_ok);
  CU_ASSERT_TRUE(result.error == QUANTITY_TOO_LARGE);

  result = ioopm_store_cart_remove_merch(store, cart_id, merch_names[0],
                                         quantities[0]);
  CU_ASSERT_TRUE(result.is_ok);

  ioopm_store_destroy(store);
}

void test_cart_calculate_cost(void) {

  const size_t random_cart_id = 77777777;
  size_t cost = 0;

  ioopm_store_t *store = ioopm_store_create();

  ioopm_store_add_merch(store, merch_names[0], descriptions[0], prices[0]);

  ioopm_store_replenish_merch(store, merch_names[0], shelf_names[0],
                              quantities[0]);
  CU_ASSERT_FALSE(ioopm_store_cart_cost(store, random_cart_id, &cost));

  const size_t cart_id = ioopm_store_add_cart(store);

  CU_ASSERT_TRUE(ioopm_store_cart_cost(store, cart_id, &cost));
  CU_ASSERT_EQUAL(cost, 0);

  ioopm_cart_result_t result =
      ioopm_store_cart_add_merch(store, cart_id, merch_names[0], quantities[0]);
  CU_ASSERT_TRUE(result.is_ok);

  ioopm_store_cart_cost(store, cart_id, &cost);
  CU_ASSERT_TRUE(ioopm_store_cart_cost(store, cart_id, &cost));
  CU_ASSERT_EQUAL(cost, prices[0] * quantities[0]);

  ioopm_store_destroy(store);
}

void test_cart_checkout(void) {
  ioopm_store_t *store = ioopm_store_create();

  size_t random_cart_id = 7777777;

  CU_ASSERT_FALSE(ioopm_store_cart_checkout(store, random_cart_id));

  ioopm_store_add_merch(store, merch_names[0], descriptions[0], prices[0]);

  ioopm_store_replenish_merch(store, merch_names[0], shelf_names[0],
                              quantities[0]);
  ioopm_store_replenish_merch(store, merch_names[0], shelf_names[1],
                              quantities[1]);

  size_t cart_id = ioopm_store_add_cart(store);

  size_t merch_stock = ioopm_store_count_stock(store, merch_names[0]);

  CU_ASSERT_EQUAL(merch_stock, quantities[0] + quantities[1]);

  ioopm_store_cart_add_merch(store, cart_id, merch_names[0],
                             (quantities[0] + quantities[1]) / 2);

  ioopm_store_cart_checkout(store, cart_id);

  merch_stock = ioopm_store_count_stock(store, merch_names[0]);
  CU_ASSERT_EQUAL(merch_stock, (quantities[0] + quantities[1]) / 2);

  ioopm_store_destroy(store);
}

void test_store_to_cart_remove(void) {

  ioopm_store_t *store = ioopm_store_create();

  ioopm_store_add_merch(store, merch_names[0], descriptions[0], prices[0]);

  ioopm_store_replenish_merch(store, merch_names[0], shelf_names[0],
                              quantities[0]);

  size_t cart_id = ioopm_store_add_cart(store);

  ioopm_store_cart_add_merch(store, cart_id, merch_names[0], quantities[0]);

  ioopm_store_remove_merch(store, merch_names[0]);

  ioopm_cart_result_t result =
      ioopm_store_cart_remove_merch(store, cart_id, merch_names[0], 0);
  CU_ASSERT_FALSE(result.is_ok);
  CU_ASSERT_EQUAL(MERCH_NOT_IN_STORE, result.error);

  ioopm_store_destroy(store);
}

int init_suite(void) {
  // Change this function if you want to do something *before* you
  // run a test suite
  return 0;
}

int clean_suite(void) {
  // Change this function if you want to do something *after* you
  // run a test suite
  return 0;
}

int main(void) {
  // First we try to set up CUnit, and exit if we fail
  if (CU_initialize_registry() != CUE_SUCCESS)
    return CU_get_error();

  // We then create an empty test suite and specify the name and
  // the init and cleanup functions
  CU_pSuite backend_test_suite =
      CU_add_suite("Backend test suite", init_suite, clean_suite);
  if (backend_test_suite == NULL) {
    // If the test suite could not be added, tear down CUnit and exit
    CU_cleanup_registry();
    return CU_get_error();
  }
  if ((CU_add_test(backend_test_suite, "Create store", test_create_store) ==
       NULL) ||
      (CU_add_test(backend_test_suite, "Add/remove merch to/from store",
                   test_add_remove_merch) == NULL) ||
      (CU_add_test(backend_test_suite, "Replenish merch not found",
                   test_replenish_merch_not_found) == NULL) ||
      (CU_add_test(backend_test_suite,
                   "Replenish shelves occupied by other merch",
                   test_replenish_merch_conflict) == NULL) ||
      (CU_add_test(backend_test_suite, "Replenish same merch and shelves",
                   test_replenish_many) == NULL) ||
      (CU_add_test(backend_test_suite, "Edit merch non existant merch",
                   test_merch_edit_name_not_found) == NULL) ||
      (CU_add_test(backend_test_suite, "Edit merch with a confliciting name",
                   test_merch_edit_name_conflict) == NULL) ||
      (CU_add_test(backend_test_suite, "Edit merch succesfully",
                   test_merch_edit_success) == NULL) ||
      (CU_add_test(backend_test_suite, "Stock of merch that isnt stocked",
                   test_ioopm_store_get_stock_non_existing) == NULL) ||
      (CU_add_test(backend_test_suite, "Stock of merch that is stocked",
                   test_ioopm_store_get_stock) == NULL) ||
      (CU_add_test(backend_test_suite, "Test list of merch", test_merch_list) ==
       NULL) ||
      (CU_add_test(backend_test_suite, "Add and remove cart to/from store",
                   test_cart_add_remove) == NULL) ||
      (CU_add_test(backend_test_suite, "Add and remove merch to/from cart",
                   test_cart_add_remove_merch) == NULL) ||
      (CU_add_test(backend_test_suite, "Test cost calculation of cart",
                   test_cart_calculate_cost) == NULL) ||
      (CU_add_test(backend_test_suite, "Test checkout of cart",
                   test_cart_checkout) == NULL) ||
      (CU_add_test(backend_test_suite,
                   "Test removal from cart when remove from store",
                   test_store_to_cart_remove) == NULL) ||

      0) {
    // If adding any of the tests fails, we tear down CUnit and exit
    CU_cleanup_registry();
    return CU_get_error();
  }

  // Set the running mode. Use CU_BRM_VERBOSE for maximum output.
  // Use CU_BRM_NORMAL to only print errors and a summary
  CU_basic_set_mode(CU_BRM_VERBOSE);

  // This is where the tests are actually run!
  CU_basic_run_tests();

  // Tear down CUnit before exiting
  CU_cleanup_registry();
  return CU_get_error();
}
