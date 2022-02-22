#!/bin/bash

#set -ex

TARGET="$1"
if [ -z "$TARGET" ]; then
  echo "Missing TARGET path"
  exit 1
fi

LINKER=$(patchelf --print-interpreter "$TARGET")
patchelf --set-interpreter "${LINKER:1}" "$TARGET"
