#! /bin/bash

export PATH="$PATH:$(pwd)/xtensa-lx106-elf/bin"
export IDF_PATH="$(pwd)/ESP8266_RTOS_SDK"
export IDF_PY=$IDF_PATH/tools/idf.py
export PRJ_DIR=./prj
export TEST_DIR=./test

alias clean="$IDF_PY -C $PRJ_DIR fullclean && $IDF_PY -C $TEST_DIR fullclean"
alias test="$IDF_PY -C $TEST_DIR $@ flash monitor"
alias menuconfig="$IDF_PY -C $PRJ_DIR menuconfig"
alias build="$IDF_PY -C $PRJ_DIR all"
alias run="$IDF_PY -C $PRJ_DIR flash monitor -p"
alias idf="$IDF_PY"
