#include "../../vendor/utils.h"
#include "../backend/store.h"
#include <_string.h>
#include <assert.h>
#include <stdbool.h>

typedef enum action {
  ADD_MERCH,
  LIST_MERCH,
  REMOVE_MERCH,
  EDIT_MERCH,
  SHOW_STOCK,
  REPLENISH,
  CREATE_CART,
  REMOVE_CART,
  ADD_TO_CART,
  REMOVE_FROM_CART,
  CALCULATE_COST,
  CHECKOUT,
  QUIT,
} action_t;

void event_loop(ioopm_store_t *store);
action_t get_action(void);
bool execute_action(action_t action, ioopm_store_t *store);
bool check_action_fn(const char *str);

answer_t convert_action_fn(const char *str);

int main(void) {
  ioopm_store_t *store = ioopm_store_create();
  event_loop(store);
  ioopm_store_destroy(store);
}

void event_loop(ioopm_store_t *store) {
  bool quit = false;
  do {
    action_t action = get_action();
    quit = execute_action(action, store);
  } while (!quit);
}

bool execute_action(action_t action, ioopm_store_t *store) {
  switch (action) {
  case ADD_MERCH:
    // ioopm_store_add_merch
    break;
  case LIST_MERCH:
    // TODO: implement for backend
    break;
  case REMOVE_MERCH:
    // ioopm_store_remove_merch
    break;
  case EDIT_MERCH:
    // ioopm_store_edit_merch
    break;
  case SHOW_STOCK:
    // ioopm_store_get_stock
    break;
  case REPLENISH:
    // ioopm_store_replenish_merch
    break;
  case QUIT:
    return false;
  default:
    assert(false && "Not implemented yet\n");
  }
  return true;
}

action_t get_action(void) {
  const char *question = "[A] Add merchandise\n"
                         "[L] List merchendise\n"
                         "[D] Remove merchendise\n"
                         "[E] Edit merhcendise\n"
                         "[S] Show stock\n"
                         "[P] Replenish merchendise\n";

  answer_t answer = ask_question(question, check_action_fn, convert_action_fn);
  return (action_t)answer.int_value;
}

answer_t convert_action_fn(const char *str) {

  action_t a;
  switch (str[0]) {
  case 'A':
    a = ADD_MERCH;
    break;
  case 'L':
    a = LIST_MERCH;
    break;
  case 'D':
    a = REMOVE_MERCH;
    break;
  case 'E':
    a = EDIT_MERCH;
    break;
  case 'S':
    a = SHOW_STOCK;
    break;
  case 'P':
    a = REPLENISH;
    break;
  }
  return (answer_t){.int_value = (int)a};
}

bool check_action_fn(const char *str) {
  if (strlen(str) != 1) {
    return false;
  }
  const char c = str[0];

  switch (c) {
  case 'A':
  case 'L':
  case 'D':
  case 'E':
  case 'S':
  case 'P':
    return true;
  default:
    return false;
  }
}

bool conferm_question(const char *question, const char c) { return false; }
