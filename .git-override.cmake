# GitTag stays the upstream release: the update check and the online server compare it.
set(GENERALS_GIT_OVERRIDE_TAG "1.0.6")
# Shown in the main and options menus: the fork release tag on this commit, e.g. 1.0.6-bluejanis-2026-11-16
execute_process(COMMAND git describe --tags --exact-match --match "*-bluejanis-*"
    WORKING_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}"
    OUTPUT_VARIABLE GENERALS_GIT_OVERRIDE_DISPLAY_TAG OUTPUT_STRIP_TRAILING_WHITESPACE ERROR_QUIET)
if(GENERALS_GIT_OVERRIDE_DISPLAY_TAG STREQUAL "")
    set(GENERALS_GIT_OVERRIDE_DISPLAY_TAG "1.0.6-bluejanis-dev")
endif()
set(GENERALS_GIT_OVERRIDE_AUTHOR "Bluejanis")
