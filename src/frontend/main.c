#include "../backend/store.h"
#include "tui.h"

int main(void) {
  store_t *store = ioopm_store_create();

  ioopm_event_loop(store);

  ioopm_store_destroy(store);
  return 0;
}
