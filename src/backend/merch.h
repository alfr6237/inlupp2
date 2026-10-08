#include <stddef.h>

typedef struct merch merch_t;

struct merch {
  char *name;
  char *desc;
  size_t price;
};

merch_t *ioopm_merch_create(const char *name, const char *desc, size_t price);
void ioopm_merch_destroy(merch_t *merch);
