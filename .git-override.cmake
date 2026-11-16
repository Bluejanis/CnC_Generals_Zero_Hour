# GitTag stays the upstream release: the update check and the online server compare it
set(GENERALS_GIT_OVERRIDE_TAG "1.0.7")
# Release name for the menus: the newest fork release tag on this commit
set(_fork_git "${GIT_EXECUTABLE}")
if(NOT _fork_git)
    set(_fork_git git)
endif()
execute_process(COMMAND "${_fork_git}" tag --points-at HEAD --list "*-bluejanis-*" --sort=-v:refname
    WORKING_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}"
    OUTPUT_VARIABLE GENERALS_GIT_OVERRIDE_RELEASE_NAME OUTPUT_STRIP_TRAILING_WHITESPACE ERROR_QUIET)
string(REGEX REPLACE "\n.*" "" GENERALS_GIT_OVERRIDE_RELEASE_NAME "${GENERALS_GIT_OVERRIDE_RELEASE_NAME}")
if(GENERALS_GIT_OVERRIDE_RELEASE_NAME STREQUAL "")
    set(GENERALS_GIT_OVERRIDE_RELEASE_NAME "1.0.7-bluejanis-dev")
endif()
set(GENERALS_GIT_OVERRIDE_AUTHOR "Bluejanis")
