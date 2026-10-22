# SPDX-License-Identifier: Apache-2.0
cmake_minimum_required(VERSION 3.20.0)

# Locate Zephyr (registered with the "west zephyr-export" command)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

project(bkit_blinky)

# Add the application source files
target_sources(app PRIVATE src/ex2.c)