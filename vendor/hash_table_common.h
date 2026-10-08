#pragma once
#include "common.h"
typedef struct hash_table ioopm_hash_table_t;
typedef struct entry entry_t;

typedef bool ioopm_eq_function(elem_t a, elem_t b);
typedef size_t ioopm_hash_function(elem_t key);

#define BUCKET_COUNT 17

struct entry
{
  elem_t key;    // holds the key
  elem_t value;  // holds the value
  entry_t *next; // points to the next entry (possibly NULL)
};

struct hash_table
{
  // DODGE: hard-coding number of buckets as 17.
  // NOTE: addressing this dodge is optional.
  entry_t **buckets;
  ioopm_hash_function *hash_fn;
  ioopm_eq_function *key_eq_fn;
};
