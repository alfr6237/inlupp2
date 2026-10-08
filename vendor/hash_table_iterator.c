#include "hash_table_iterator.h"
#include "common.h"
#include "hash_table_common.h"

#include <assert.h>
#include <stddef.h>
#include <stdlib.h>

struct hash_table_iterator
{
  ioopm_hash_table_t *ht;
  int current_bucket;
  entry_t *current_entry;
};

static void skip_empty_buckets(ioopm_hash_table_iterator_t *it);

ioopm_hash_table_iterator_t *
ioopm_hash_table_iterator_create(ioopm_hash_table_t *ht)
{
  ioopm_hash_table_iterator_t *it = malloc(sizeof(ioopm_hash_table_iterator_t));
  it->ht = ht;
  it->current_bucket = 0;
  it->current_entry = ht->buckets[0];

  skip_empty_buckets(it);
  return it;
}
void ioopm_hash_table_iterator_destroy(ioopm_hash_table_iterator_t *it)
{
  free(it);
}

bool ioopm_hash_table_iterator_at_end(ioopm_hash_table_iterator_t *it)
{
  return it->current_bucket == BUCKET_COUNT;
}

void ioopm_hash_table_iterator_advance(ioopm_hash_table_iterator_t *it)
{
  assert(!ioopm_hash_table_iterator_at_end(it));

  if (it->current_entry->next != NULL)
  {
    it->current_entry = it->current_entry->next;
    return;
  }

  it->current_bucket++;

  skip_empty_buckets(it);
}

elem_t ioopm_hash_table_iterator_current_key(ioopm_hash_table_iterator_t *it)
{
  return it->current_entry->key;
}

elem_t ioopm_hash_table_iterator_current_value(ioopm_hash_table_iterator_t *it)
{
  return it->current_entry->value;
}

static void skip_empty_buckets(ioopm_hash_table_iterator_t *it)
{
  while (it->current_bucket < BUCKET_COUNT &&
         it->ht->buckets[it->current_bucket] == NULL)
  {
    it->current_bucket++;
  }

  if (it->current_bucket < BUCKET_COUNT)
  {
    it->current_entry = it->ht->buckets[it->current_bucket];
  }
  else
  {
    it->current_entry = NULL;
  }
}
