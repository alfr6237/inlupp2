#include "hash_table.h"
#include "common.h"

#include "hash_table_common.h"
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUCKET_COUNT 17

static size_t bucket_size(entry_t *entry);
static entry_t *entry_create(elem_t key, elem_t value, entry_t *next);
static entry_t *entry_destroy(entry_t *entry);
static entry_t **find_entry_for_key(ioopm_hash_table_t *ht, elem_t key);

/// Allocate space for a ioopm_hash_table_t = 17 pointers to entry_t's
ioopm_hash_table_t *ioopm_hash_table_create(ioopm_hash_function *hash_fn,
                                            ioopm_eq_function *key_eq_fn)
{
  /// Allocate zeroed-out space for a ioopm_hash_table_t = 17 pointers to
  /// entry_t's
  ioopm_hash_table_t *ht = calloc(1, sizeof(ioopm_hash_table_t));
  ht->buckets = calloc(BUCKET_COUNT, sizeof(entry_t *));
  ht->hash_fn = hash_fn;
  ht->key_eq_fn = key_eq_fn;
  return ht;
}

void ioopm_hash_table_destroy(ioopm_hash_table_t *ht)
{
  for (size_t i = 0; i < BUCKET_COUNT; i++)
  {
    entry_t *cursor = ht->buckets[i];
    while (cursor != NULL)
    {
      cursor = entry_destroy(cursor);
    }
  }
  free(ht->buckets);
  free(ht);
}

void ioopm_hash_table_insert(ioopm_hash_table_t *ht, elem_t key, elem_t value)
{
  entry_t **entry = find_entry_for_key(ht, key);

  if ((*entry) != NULL)
  {
    (*entry)->value = value;
  }
  else
  {
    (*entry) = entry_create(key, value, NULL);
  }
}

bool ioopm_hash_table_lookup(ioopm_hash_table_t *ht, elem_t key, elem_t *result)
{
  entry_t **entry = find_entry_for_key(ht, key);

  if ((*entry) == NULL)
  {
    return false;
  }

  *result = (*entry)->value;
  return true;
}

bool ioopm_hash_table_remove(ioopm_hash_table_t *ht, elem_t key, elem_t *result)
{
  entry_t **entry = find_entry_for_key(ht, key);
  if ((*entry) == NULL)
  {
    return false;
  }

  *result = (*entry)->value;
  (*entry) = entry_destroy(*entry);
  return true;
}

bool ioopm_hash_table_has_key(ioopm_hash_table_t *ht, elem_t key)
{
  elem_t _;
  return ioopm_hash_table_lookup(ht, key, &_);
}

size_t ioopm_hash_table_size(ioopm_hash_table_t *ht)
{
  size_t size = 0;
  for (size_t i = 0; i < BUCKET_COUNT; i++)
  {
    size += bucket_size(ht->buckets[i]);
  }
  return size;
}

bool ioopm_hash_table_is_empty(ioopm_hash_table_t *ht)
{
  for (size_t i = 0; i < BUCKET_COUNT; i++)
  {
    if (ht->buckets[i] != NULL)
    {
      return false;
    }
  }
  return true;
}

static size_t bucket_size(entry_t *entry)
{
  size_t size = 0;
  while (entry != NULL)
  {
    size++;
    entry = entry->next;
  }
  return size;
}

static entry_t *entry_create(elem_t key, elem_t value, entry_t *next)
{
  entry_t *entry = malloc(sizeof(entry_t));
  entry->key = key;
  entry->value = value;
  entry->next = next;

  return entry;
}

// destroys the entry and returns its next pointer
static entry_t *entry_destroy(entry_t *entry)
{
  entry_t *next = entry->next;
  free(entry);
  return next;
}

static entry_t **find_entry_for_key(ioopm_hash_table_t *ht, elem_t key)
{
  // find bucket
  size_t bucket = ht->hash_fn(key) % BUCKET_COUNT;

  entry_t **current = &ht->buckets[bucket];

  while (*current != NULL)
  {
    // if we find a mathing key break
    if (ht->key_eq_fn(key, (*current)->key))
    {
      break;
    }
    current = &(*current)->next;
  }

  return current;
}
