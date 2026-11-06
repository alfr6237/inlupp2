#include "../../vendor/linked_list.h"
#include "../../vendor/linked_list_iterator.h"
#include "../../vendor/utils.h"
#include "../backend/store.h"
#include <assert.h>
#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
static void list_merch(store_t *store);
static void remove_merch(store_t *store);
static void edit_merch(store_t *store);
static void show_stock(const store_t *store);
static void replenish(store_t *store);
static void print_stock(ioopm_list_t *stock_list);
static void print_merch(merch_t *merch);
static void create_cart(store_t *store);
static void cart_remove(store_t *store);
static void add_to_cart(store_t *store);
static void remove_from_cart(store_t *store);
static void calculate_cost(store_t *store);
static void checkout(store_t *store);

static bool confirmation(unsigned char code, const char *format, ...);
static bool is_shelf_name(const char *str);
static shelf_name_t make_shelf_name(const char *str);
static shelf_name_t ask_question_shelf_name(const char *question);

answer_t convert_action_fn(const char *str);

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
    list_merch(store);
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
  case CREATE_CART:
    puts("\nCreate cart");
    create_cart(store);
    break;
  case REMOVE_CART:
    puts("\nRemove cart");
    cart_remove(store);
    break;
  case ADD_TO_CART:
    puts("\nAdd to cart");
    add_to_cart(store);
    break;
  case REMOVE_FROM_CART:
    puts("\nRemove from cart");
    remove_from_cart(store);
    break;
  case CALCULATE_COST:
    puts("\nCalculate cost");
    calculate_cost(store);
    break;
  case CHECKOUT:
    puts("\nCheckout");
    checkout(store);
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
                         "[C] Create a new cart\n"
                         "[R] Remove a cart\n"
                         "[+] Add to cart\n"
                         "[-] Remove from cart\n"
                         "[=] Calculate cost\n"
                         "[O] Checkout cart from store\n"
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
  case 'C':
    a = CREATE_CART;
    break;
  case 'R':
    a = REMOVE_CART;
    break;
  case '+':
    a = ADD_TO_CART;
    break;
  case '-':
    a = REMOVE_FROM_CART;
    break;
  case '=':
    a = CALCULATE_COST;
    break;
  case 'O':
    a = CHECKOUT;
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
  case 'C':
  case 'R':
  case '+':
  case '-':
  case '=':
  case 'O':
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

static void list_merch(store_t *store) {
  ioopm_list_t *list = store_merch_list(store);

  ioopm_list_iterator_t *iter = ioopm_list_iterator_create(list);

  puts("");
  bool keep_going = false;
  size_t loop_count = 0;
  do {
    size_t i = 0;
    while (!ioopm_list_iterator_at_end(iter) && i < 20) {

      elem_t merch_elem = ioopm_list_iterator_current(iter);
      merch_t *merch = (merch_t *)merch_elem.p;

      size_t merch_list_number = loop_count * 20 + i + 1;
      printf("%zu. ", merch_list_number);
      print_merch(merch);
      puts("");
      ioopm_list_iterator_advance(iter);
      i++;
    }

    if (ioopm_list_iterator_at_end(iter)) {
      keep_going = false;
    } else {
      keep_going =
          confirmation('N', "Do you whish to continue to list merchendise?");
    }
    loop_count++;
  } while (keep_going);

  ioopm_list_iterator_destroy(iter);
  ioopm_list_destroy(list);
}

static void remove_merch(store_t *store) {
  char *name = ask_question_string("Name: ");

  const char *confirm_fmt = "Do you really wish to remove %s?\n";

  if (confirmation('Y', confirm_fmt, name)) {
    if (!store_remove_merch(store, name)) {
      printf("Merch with name %s doesn't exist, merch not removed\n", name);
    }
  }

  free(name);
}

static void edit_merch(store_t *store) {
  char *name = ask_question_string("Name: ");

  char *new_name = ask_question_string("New name: ");
  char *new_desc = ask_question_string("New description: ");
  size_t new_price = ask_question_size("New price: ");

  const char *confirm_fmt = "Do you really wish to edit merch %s?\n";

  if (confirmation('Y', confirm_fmt, name)) {
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

  free(new_desc);
  free(new_name);
  free(name);
}

static bool confirmation(unsigned char code, const char *format, ...) {
  va_list args;
  va_start(args, format);

  // Calculate required buffer size
  va_list args_copy;
  va_copy(args_copy, args);
  int length = vsnprintf(NULL, 0, format, args_copy);
  va_end(args_copy);

  if (length < 0) {
    va_end(args);
    return false;
  }

  // Extra space for " [X/x]" and null terminator
  const int extra_space = 7;
  char *question = malloc((size_t)length + extra_space);
  if (!question) {
    va_end(args);
    return false;
  }

  vsnprintf(question, (size_t)length + 1, format, args);
  va_end(args);

  // Append confirmation code
  char upper = (char)toupper((unsigned char)code);
  char lower = (char)tolower((unsigned char)code);

  snprintf(question + length, extra_space, " [%c/%c]", upper, lower);

  char *answer = ask_question_string(question);
  bool result = answer && toupper((unsigned char)answer[0]) == upper;

  free(answer);
  free(question);

  return result;
}

static void replenish(store_t *store) {
  shelf_name_t shelf_name = ask_question_shelf_name("Shelf to replenish: ");
  char *name = ask_question_string("Merch to replenish: ");
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

static void show_stock(const store_t *store) {
  char *name = ask_question_string("Merch name to show stock of: ");

  ioopm_list_t *stock_list = store_get_stock(store, name);

  puts("");
  if (stock_list) {
    if (ioopm_list_is_empty(stock_list)) {
      printf("%s is not stocked\n", name);
    }
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
  printf("%s\nDescription: %s\nPrice: %zd\n", merch->name, merch->desc,
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

static void create_cart(store_t *store) {
  size_t cart_id = store_add_cart(store);
  puts("\nA new cart has been added to the store");
  printf("\nID of new cart: %zu\n", cart_id);
}

static void cart_remove(store_t *store) {
  // printa id:n på carts?
  size_t cart_id = ask_question_size("ID of cart to be removed: ");

  const char *confirm_fmt = "Do you really wish to remove %zu?\n";

  if (confirmation('Y', confirm_fmt, cart_id)) {
    if (!store_cart_remove(store, cart_id)) {
      printf("Merch with name %zu doesn't exist, merch not removed\n", cart_id);
    }
  }
}

static void add_to_cart(store_t *store) {
  size_t cart_id = ask_question_size("Cart ID: ");
  char *merch_name = ask_question_string("Name of merch: ");
  size_t quantity = ask_question_size("Quantity to add to cart: ");

  const cart_result_t result =
      store_cart_add_merch(store, cart_id, merch_name, quantity);

  if (!result.is_ok) {
    switch (result.error) {
    case CART_NOT_FOUND:
      printf("Cart with ID %zu does not exist in store, action failed\n",
             cart_id);
      break;
    case MERCH_NOT_IN_STORE:
      printf("Merch with name %s does not exist in store, action failed\n",
             merch_name);
      break;
    case QUANTITY_TOO_LARGE:
      printf("Quantity of size %zu for merch %s is not available for "
             "reservation, action failed\n",
             quantity, merch_name);
      break;
    case MERCH_NOT_IN_CART:
      // dummy field, never get here
      break;
    }
  }

  free(merch_name);
}

static void remove_from_cart(store_t *store) {
  size_t cart_id = ask_question_size("Cart ID: ");
  char *merch_name = ask_question_string("Name of merch: ");
  size_t quantity = ask_question_size("Quantity to remove from cart: ");

  const cart_result_t result =
      store_cart_remove_merch(store, cart_id, merch_name, quantity);

  if (!result.is_ok) {
    switch (result.error) {
    case CART_NOT_FOUND:
      printf("Cart with ID %zu does not exist in store, action failed\n",
             cart_id);
      break;
    case MERCH_NOT_IN_STORE:
      printf("Merch with name %s does not exist in store, action failed\n",
             merch_name);
      break;
    case MERCH_NOT_IN_CART:
      printf("Merch with name %s was not found in cart with ID: %zu, action "
             "failed\n",
             merch_name, cart_id);
      break;
    case QUANTITY_TOO_LARGE:
      printf("Quantity of size %zu for merch %s is larger than quantity in "
             "cart with ID: %zu, action failed\n",
             quantity, merch_name, cart_id);
      break;
    }
  }

  free(merch_name);
}

static void calculate_cost(store_t *store) {
  size_t cart_id = ask_question_size("Cart ID: ");

  size_t cart_cost = 0;
  bool success = store_cart_cost(store, cart_id, &cart_cost);

  if (success) {
    printf("The cost of cart with ID: %zu is %zu\n", cart_id, cart_cost);
  } else {
    printf("A cart with ID: %zu does not exist in store\n", cart_id);
  }
}

static void checkout(store_t *store) {
  size_t cart_id = ask_question_size("Cart ID to checkout: ");

  bool sucess = store_cart_checkout(store, cart_id);

  if (sucess) {
    printf("Cart with ID: %zu has sucessfully been checked out\n", cart_id);
  } else {
    printf("Cart with ID: %zu does not exist in store\n", cart_id);
  }
}
