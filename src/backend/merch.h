#pragma once
#include "common.h"

///@file merch.h
///@brief interanl file to the backend that make it possbile for both store.c
/// and merch.c to
/// destroy merchandise without exposeing the function in store.h

///@brief destroy a merchandise by freeing the memory it owns
///@param merchandise to destroy
void merch_destroy(merch_t *merch);
