#include "../src/backend/cart.h"
#include "../src/backend/merch.h"
#include "../src/backend/store.h"
#include "../vendor/linked_list.h"

#include <CUnit/Basic.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

void test_create_merch(void) {
  merch_t *m = ioopm_merch_create("hej", "h", 12);
  ioopm_merch_destroy(m);

  ioopm_list_t *l = ioopm_list_create();
  ioopm_list_destroy(l);
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
  CU_pSuite ht_test_suite =
      CU_add_suite("Hash table test suite", init_suite, clean_suite);
  if (ht_test_suite == NULL) {
    // If the test suite could not be added, tear down CUnit and exit
    CU_cleanup_registry();
    return CU_get_error();
  }
  if ((CU_add_test(ht_test_suite, "test", test_create_merch) == NULL) || 0) {
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
