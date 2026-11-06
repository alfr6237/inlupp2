#pragma once

#include "../backend/store.h"

///@file tui.h
///@brief Text-based user interface for the store.

/// @brief Starts the interactive store interface.
///
/// Displays the action menu and processes user commands
/// until the user chooses to quit.
///
/// @param store The store operated on by the interface.
///              Must not be NULL.
///
/// @note The caller retains ownership of the store.
///
void ioopm_event_loop(ioopm_store_t *store);
