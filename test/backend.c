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

  ioopm_merch_t *m = ioopm_merch_create("hej", "h", 12);

  ioopm_store_add_merch(store, m);
  ioopm_merch_t *mer = ioopm_store_get_merch(store, "hej");

  CU_ASSERT(strcmp(m->name, mer->name) == 0);
  CU_ASSERT(strcmp(m->desc, mer->desc) == 0);
  CU_ASSERT_EQUAL(m->price, mer->price);

  ioopm_store_remove_merch(store, m->name);

  CU_ASSERT_PTR_NULL(ioopm_store_get_merch(store, "hej"));
  ioopm_merch_destroy(m);

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
