#include "../../vendor/linked_list.h"
#include "../../vendor/linked_list_iterator.h"
#include "../../vendor/utils.h"
#include "../backend/store.h"
#include <_string.h>
#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

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

void event_loop(store_t *store);
action_t get_action(void);
bool execute_action(action_t action, store_t *store);
bool check_action_fn(const char *str);

static void add_merch(store_t *store);
static void remove_merch(store_t *store);
static void edit_merch(store_t *store);
static void show_stock(const store_t *store);
static void replenish(store_t *store);
static void print_stock(ioopm_list_t *stock_list);

static bool confirmation(const char *question);
static bool is_shelf_name(const char *str);
static shelf_name_t make_shelf_name(const char *str);
static shelf_name_t ask_question_shelf_name(const char *question);

answer_t convert_action_fn(const char *str);

int main(void) {
  store_t *store = store_create();
  event_loop(store);
  store_destroy(store);
}

void event_loop(store_t *store) {
  bool quit = false;
  do {
    action_t action = get_action();
    quit = execute_action(action, store);
    puts("");
  } while (!quit);
}

bool execute_action(action_t action, store_t *store) {
  switch (action) {
  case ADD_MERCH:
    puts("\nAdd merch");
    add_merch(store);
    break;
  case LIST_MERCH:
    // TODO: implement for backend
    break;
  case REMOVE_MERCH:
    puts("\nRemove merch");
    remove_merch(store);
    break;
  case EDIT_MERCH:
    puts("\nEdit merch");
    edit_merch(store);
    break;
  case SHOW_STOCK:
    puts("\nShow stock");
    show_stock(store);
    break;
  case REPLENISH:
    puts("\nReplenish merch");
    replenish(store);
    break;
  case QUIT:
    return true;
  default:
    assert(false && "Not implemented yet\n");
  }
  return false;
}
action_t get_action(void) {
  const char *question = "[A] Add merchandise\n"
                         "[L] List merchendise\n"
                         "[D] Remove merchendise\n"
                         "[E] Edit merhcendise\n"
                         "[S] Show stock\n"
                         "[P] Replenish merchendise\n"
                         "[Q] Quit\n\n"
                         "Action: ";

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
  case 'Q':
    a = QUIT;
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
  case 'Q':
    return true;
  default:
    return false;
  }
}
static void add_merch(store_t *store) {
  char *name = ask_question_string("Name: ");
  char *desc = ask_question_string("Description: ");
  const size_t price = ask_question_size("Price: ");

  if (!store_add_merch(store, name, desc, price)) {
    printf("Merch with name %s already exist, merch not added\n", name);
  }
  free(desc);
  free(name);
}

static void remove_merch(store_t *store) {
  char *name = ask_question_string("Name: ");

  const char *confirm_fmt = "Do you really wish to remove %s?";

  const int len = snprintf(NULL, 0, confirm_fmt, name);
  assert(len > 0);

  char *buffer = calloc(len + 1, sizeof(char));
  snprintf(buffer, len, confirm_fmt, name);

  const char *confirmation = ask_question_string(buffer);

  if (toupper(confirmation[0]) == 'Y') {
    if (!store_remove_merch(store, name)) {
      printf("Merch with name %s doesn't exist, merch not removed\n", name);
    }
  }

  free(buffer);
  free(name);
}

static void edit_merch(store_t *store) {
  char *name = ask_question_string("Name: ");

  char *new_name = ask_question_string("New name: ");
  char *new_desc = ask_question_string("New description: ");
  size_t new_price = ask_question_size("New price: ");

  const char *confrim_fmt = "Do you really wish to edit merch %s?\n";
  const int len = snprintf(NULL, 0, confrim_fmt, name);
  assert(len > 0);
  char *buffer = calloc(len + 1, sizeof(char));
  snprintf(buffer, len, confrim_fmt, name);

  if (confirmation(buffer)) {
    const edit_result_t result =
        store_edit_merch(store, name, new_name, new_desc, new_price);
    if (!result.is_ok) {
      switch (result.error) {
      case NEW_NAME_EXIST:
        printf("New name '%s' already exists in store, merch not edited\n",
               new_name);
        break;
      case OLD_NAME_NOT_FOUND:
        printf("Name of merch to be edited '%s' not found, merch not edited\n",
               name);
        break;
      }
    } else {
      printf("Merch succesfully edited\n");
    }
  }

  free(buffer);
  free(new_name);
  free(name);
}

static bool confirmation(const char *question) {
  char *answer = ask_question_string(question);
  bool result = toupper(answer[0]) == 'Y';

  free(answer);
  return result;
}

static void replenish(store_t *store) {
  char *name = ask_question_string("Merch to replenish: ");
  shelf_name_t shelf_name = ask_question_shelf_name("Shelf to replenish: ");
  size_t quantity = ask_question_size("Quantity: ");

  const replenish_result_t result =
      store_replenish_merch(store, name, shelf_name, quantity);

  if (!result.is_ok) {
    switch (result.error) {
    case MERCH_CONFLICT:
      printf("Shelf '%c%zu' is already occupied by other merch, replenish "
             "failed\n",
             shelf_name.c, shelf_name.n);
      break;
    case MERCH_NOT_FOUND:
      printf("Merch with name '%s' not found in store, replenish failed\n",
             name);
      break;
    }
  }

  free(name);
}

// TODO: sort the list in backend
static void show_stock(const store_t *store) {
  char *name = ask_question_string("Merch name to show stock of: ");

  ioopm_list_t *stock_list = store_get_stock(store, name);

  if (stock_list) {
    print_stock(stock_list);
    ioopm_list_destroy(stock_list);
  } else {
    printf("Merch with name '%s' does not exist in store\n", name);
  }
  free(name);
}

static void print_stock(ioopm_list_t *stock_list) {
  ioopm_list_iterator_t *iter = ioopm_list_iterator_create(stock_list);

  while (!ioopm_list_iterator_at_end(iter)) {
    elem_t shelf_elem = ioopm_list_iterator_current(iter);
    const shelf_t *shelf = (shelf_t *)shelf_elem.p;

    printf("%zu. shelf: %c%zu, stock: %zu\n", (iter->current_index + 1),
           shelf->shelf_name.c, shelf->shelf_name.n, shelf->quantity);

    ioopm_list_iterator_advance(iter);
  }

  ioopm_list_iterator_destroy(iter);
}

static void print_merch(merch_t *merch) {
  printf("Name: %s\nDescription: %s\nPrice: %zd\n", merch->name, merch->desc,
         merch->price);
}

static shelf_name_t ask_question_shelf_name(const char *question) {
  answer_t shelf_name_string =
      ask_question(question, is_shelf_name, make_string);
  const shelf_name_t shelf_name =
      make_shelf_name(shelf_name_string.string_value);

  free(shelf_name_string.string_value);
  return shelf_name;
}

static shelf_name_t make_shelf_name(const char *str) {
  assert(is_shelf_name(str));
  const char c = str[0];
  const size_t n = atoi(&str[1]);

  return (shelf_name_t){.c = c, .n = n};
}

static bool is_shelf_name(const char *str) {
  if (str[0] < 'A' || str[0] > 'Z') {
    return false;
  }
  if (!is_number(&str[1])) {
    return false;
  }
  int number = atoi(&str[1]);
  if (number < 0 || number > 99) {
    return false;
  }
  return true;
}
