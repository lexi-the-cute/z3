/*++
Copyright (c) 2025 Microsoft Corporation

Module Name:

    debug.cpp

Abstract:

    Test debug utilities.

Author:

    Daily Test Coverage Improver 2025-01-16.

Revision History:

--*/
#include <iostream>
#include <sstream>
#include <stdexcept>
#include "util/debug.h"

// Test assertion enabling/disabling
static void tst_assertions() {
    // Test initial state
    bool initial_state = assertions_enabled();

    // Test disabling assertions
    enable_assertions(false);
    ENSURE(!assertions_enabled());

    // Test enabling assertions
    enable_assertions(true);
    ENSURE(assertions_enabled());

    // Restore initial state
    enable_assertions(initial_state);
}

// Test debug tag management
static void tst_debug_tags() {
    // Initially no tags should be enabled
    ENSURE(!is_debug_enabled("test_tag"));
    ENSURE(!is_debug_enabled("another_tag"));

    // Enable a debug tag
    enable_debug("test_tag");
    ENSURE(is_debug_enabled("test_tag"));
    ENSURE(!is_debug_enabled("another_tag"));

    // Enable another debug tag
    enable_debug("another_tag");
    ENSURE(is_debug_enabled("test_tag"));
    ENSURE(is_debug_enabled("another_tag"));

    // Disable a debug tag
    disable_debug("test_tag");
    ENSURE(!is_debug_enabled("test_tag"));
    ENSURE(is_debug_enabled("another_tag"));

    // Disable the remaining tag
    disable_debug("another_tag");
    ENSURE(!is_debug_enabled("test_tag"));
    ENSURE(!is_debug_enabled("another_tag"));

    // Test disabling non-existent tag (should not crash)
    disable_debug("non_existent_tag");
    ENSURE(!is_debug_enabled("non_existent_tag"));
}

// Test exit action management
static void tst_exit_action() {
    // Save current state
    exit_action original = get_default_exit_action();

    // Test setting throw_exception action
    set_default_exit_action(exit_action::throw_exception);
    ENSURE(get_default_exit_action() == exit_action::throw_exception);

    // Test setting exit action
    set_default_exit_action(exit_action::exit);
    ENSURE(get_default_exit_action() == exit_action::exit);

    // Restore original state
    set_default_exit_action(original);
}

// Test debug action management
static void tst_debug_action() {
    // Save current state
    debug_action original = get_default_debug_action();

    // Test setting different debug actions
    set_default_debug_action(debug_action::cont);
    ENSURE(get_default_debug_action() == debug_action::cont);

    set_default_debug_action(debug_action::abort);
    ENSURE(get_default_debug_action() == debug_action::abort);

    set_default_debug_action(debug_action::stop);
    ENSURE(get_default_debug_action() == debug_action::stop);

    set_default_debug_action(debug_action::throw_exception);
    ENSURE(get_default_debug_action() == debug_action::throw_exception);

    set_default_debug_action(debug_action::invoke_gdb);
    ENSURE(get_default_debug_action() == debug_action::invoke_gdb);

    set_default_debug_action(debug_action::invoke_lldb);
    ENSURE(get_default_debug_action() == debug_action::invoke_lldb);

    set_default_debug_action(debug_action::ask);
    ENSURE(get_default_debug_action() == debug_action::ask);

    // Restore original state
    set_default_debug_action(original);
}

// Test enum values for debug_action to ensure they're accessible
static void tst_debug_action_enum() {
    // Test that all enum values are accessible
    debug_action actions[] = {
        debug_action::ask,
        debug_action::cont,
        debug_action::abort,
        debug_action::stop,
        debug_action::throw_exception,
        debug_action::invoke_gdb,
        debug_action::invoke_lldb
    };

    // Test that we can assign and compare enum values
    debug_action test_action = debug_action::cont;
    ENSURE(test_action == debug_action::cont);
    ENSURE(test_action != debug_action::abort);

    // Test setting and getting each action
    for (auto action : actions) {
        set_default_debug_action(action);
        ENSURE(get_default_debug_action() == action);
    }
}

// Test invoke_exit_action with exception throwing
static void tst_invoke_exit_action() {
    // Save current state
    exit_action original = get_default_exit_action();

    // Set to throw exceptions
    set_default_exit_action(exit_action::throw_exception);

    // Test different error codes
    bool exception_thrown = false;
    try {
        invoke_exit_action(ERR_INTERNAL_FATAL);
    } catch (const std::exception&) {
        exception_thrown = true;
    }
    ENSURE(exception_thrown);

    exception_thrown = false;
    try {
        invoke_exit_action(ERR_UNREACHABLE);
    } catch (const std::exception&) {
        exception_thrown = true;
    }
    ENSURE(exception_thrown);

    exception_thrown = false;
    try {
        invoke_exit_action(ERR_NOT_IMPLEMENTED_YET);
    } catch (const std::exception&) {
        exception_thrown = true;
    }
    ENSURE(exception_thrown);

    // Test unknown error code
    exception_thrown = false;
    try {
        invoke_exit_action(99999); // Unknown error code
    } catch (const std::exception&) {
        exception_thrown = true;
    }
    ENSURE(exception_thrown);

    // Restore original state
    set_default_exit_action(original);
}

// Test multiple debug tags simultaneously
static void tst_multiple_debug_tags() {
    // Clean up any existing tags
    disable_debug("tag1");
    disable_debug("tag2");
    disable_debug("tag3");

    // Enable multiple tags
    enable_debug("tag1");
    enable_debug("tag2");
    enable_debug("tag3");

    ENSURE(is_debug_enabled("tag1"));
    ENSURE(is_debug_enabled("tag2"));
    ENSURE(is_debug_enabled("tag3"));

    // Selectively disable tags
    disable_debug("tag2");

    ENSURE(is_debug_enabled("tag1"));
    ENSURE(!is_debug_enabled("tag2"));
    ENSURE(is_debug_enabled("tag3"));

    // Clean up
    disable_debug("tag1");
    disable_debug("tag3");

    ENSURE(!is_debug_enabled("tag1"));
    ENSURE(!is_debug_enabled("tag2"));
    ENSURE(!is_debug_enabled("tag3"));
}

// Test finalize_debug function
static void tst_finalize_debug() {
    // Enable some debug tags
    enable_debug("cleanup_test");
    ENSURE(is_debug_enabled("cleanup_test"));

    // Call finalize_debug to clean up
    finalize_debug();

    // After finalization, tags should still work (new table created)
    enable_debug("after_cleanup");
    ENSURE(is_debug_enabled("after_cleanup"));

    // Clean up
    disable_debug("after_cleanup");
}

void tst_debug() {
    tst_assertions();
    tst_debug_tags();
    tst_exit_action();
    tst_debug_action();
    tst_debug_action_enum();
    tst_invoke_exit_action();
    tst_multiple_debug_tags();
    tst_finalize_debug();
}