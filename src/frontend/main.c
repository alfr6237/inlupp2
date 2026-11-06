#include "../backend/store.h"
#include "tui.h"

int main(void) {
  store_t *store = store_create();

  event_loop(store);

  store_destroy(store);
  return 0;
}
