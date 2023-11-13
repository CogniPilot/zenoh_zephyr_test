#-------------------------------------------------------------------------------
# Zephyr Cerebri Application
#
# Copyright (c) 2023 CogniPilot Foundation
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.13.1)

set(MODULE_EXT_ROOT ${CMAKE_CURRENT_SOURCE_DIR})

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

project(zenoh_test LANGUAGES C)

target_compile_options(app PRIVATE -Wall -Wextra -Werror -Wno-unused-parameter -Wno-type-limits)

target_link_libraries(app PRIVATE zenoh-pico)

set(SOURCE_FILES
  src/z_sub.c
  )

message(STATUS "${SOURCE_FILES}")

target_sources(app PRIVATE ${SOURCE_FILES})
