#!/usr/bin/env bash

export EXEC_WRAPPER=$1

export APPIMAGE_ORIGINAL_TEST=ORIGINAL_VAL

export TEST=APPIMAGE_ONLY_VAL
export APPIMAGE_STARTUP_TEST=$TEST

export LD_PRELOAD=$EXEC_WRAPPER

function assert_not_empty() {
  if [ -z "$1" ]; then
    exit 1;
  fi
}

export APPDIR="/"
OUTPUT=$(/bin/bash -c '/usr/bin/env | grep -e ^TEST=APPIMAGE_ONLY_VAL')
assert_not_empty "$OUTPUT"


export APPDIR="$PWD"
OUTPUT=$(/bin/bash -c '/usr/bin/env | grep -e ^TEST=ORIGINAL_VAL')
assert_not_empty "$OUTPUT"