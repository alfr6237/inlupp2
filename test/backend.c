#include "../src/backend/merch.h"
#include "../src/backend/store.h"

#include <CUnit/Basic.h>

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

void test_create_merch(void) {
  ioopm_merch_t *m = ioopm_merch_create("hej", "h", 12);
  ioopm_merch_destroy(m);
}

void test_create_store(void) {
  ioopm_store_t *store = ioopm_store_create();
  ioopm_store_destroy(store);
}

void test_add_remove_merch(void) {

  ioopm_store_t *store = ioopm_store_create();

  ioopm_store_add_merch(store, "hej", "h", 12);
  ioopm_merch_t *mer = ioopm_store_get_merch(store, "hej");

  CU_ASSERT(strcmp("hej", mer->name) == 0);
  CU_ASSERT(strcmp("h", mer->desc) == 0);
  CU_ASSERT_EQUAL(12, mer->price);

  ioopm_store_remove_merch(store, "hej");

  CU_ASSERT_PTR_NULL(ioopm_store_get_merch(store, "hej"));

  ioopm_store_destroy(store);
}

void test_replenish_merch_not_found(void)
{
  ioopm_store_t *store = ioopm_store_create();

  ioopm_shelf_t shelf = {.c = 'A', .n = 42};

  CU_ASSERT_EQUAL(ioopm_store_replenish_merch(store, "hej", shelf, 53), MERCH_NOT_FOUND);

  ioopm_store_destroy(store);
}

void test_replenish_merch_conflict(void)
{
  ioopm_store_t *store = ioopm_store_create();

  ioopm_shelf_t shelf = {.c = 'A', .n = 42};

  ioopm_store_add_merch(store, "hej", "h", 12);
  ioopm_store_add_merch(store, "då", "d", 113);

  CU_ASSERT_EQUAL(ioopm_store_replenish_merch(store, "hej", shelf, 53), SUCESS);
  CU_ASSERT_EQUAL(ioopm_store_replenish_merch(store, "då", shelf, 53), MERCH_CONFLICT);

  ioopm_store_destroy(store);
}

void test_replenish_many(void)
{
  ioopm_store_t *store = ioopm_store_create();

  ioopm_shelf_t shelf = {.c = 'A', .n = 42};
  ioopm_store_add_merch(store, "hej", "h", 12);

  shelf_storage_t storage;

  CU_ASSERT_EQUAL(ioopm_store_replenish_merch(store, "hej", shelf, 53), SUCESS);
  CU_ASSERT_TRUE(ioopm_get_shelf_storage(store, shelf, &storage));
  CU_ASSERT_TRUE(!strcmp(storage.merch_name, "hej"));
  CU_ASSERT_EQUAL(storage.quantity, 53);

  CU_ASSERT_EQUAL(ioopm_store_replenish_merch(store, "hej", shelf, 53), SUCESS);
  CU_ASSERT_TRUE(ioopm_get_shelf_storage(store, shelf, &storage));
  CU_ASSERT_TRUE(!strcmp(storage.merch_name, "hej"));
  CU_ASSERT_EQUAL(storage.quantity, 106);

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
  if ((CU_add_test(backend_test_suite, "Create merch", test_create_merch) ==
       NULL) ||

      (CU_add_test(backend_test_suite, "Create store", test_create_store) ==
       NULL) ||
      (CU_add_test(backend_test_suite, "Add/remove merch to/from store",
                   test_add_remove_merch) == NULL) ||
      (CU_add_test(backend_test_suite, "Replenish merch not found",
                   test_replenish_merch_not_found) == NULL) ||
      (CU_add_test(backend_test_suite, "Replenish shelf occupied by other merch",
                   test_replenish_merch_conflict) == NULL) ||
      (CU_add_test(backend_test_suite, "Replenish same merch and shelf",
                   test_replenish_many) == NULL) ||
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
