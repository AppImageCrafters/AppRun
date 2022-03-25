#!/bin/bash

set -e

APPDIR="$1"
if [ -z "$APPDIR" ]; then
  echo "Missing AppDir path"
  exit 1
fi
BUILD_DIR="$2"
if [ -z "$BUILD_DIR" ]; then
  echo "Missing BUILD_DIR path"
  exit 1
fi

APPRUN=$(find "$BUILD_DIR/src" -name AppRun | head -n 1)
APPRUN_HOOKS=$(find "$BUILD_DIR/src" -name libapprun_hooks.so | head -n 1)
TARGET_BIN=$(find "$BUILD_DIR/test" -name hooks_inner_target_test | head -n 1)

if [ -z "$APPRUN" ]; then
  echo "Missing AppRun path"
  exit 1
fi

if [ -z "$APPRUN_HOOKS" ]; then
  echo "Missing libapprun_hooks.so path"
  exit 1
fi

if [ -z "$TARGET_BIN" ]; then
  echo "Missing TARGET_BIN path"
  exit 1
fi

mkdir -p "$APPDIR"

function join_by { local IFS="$1"; shift; echo "$*"; }

function remove_duplicated_paths() {
  local items=("${@}")
  IFS=":" read -r -a items <<<"$(tr ':' '\n' <<<"${items[@]}" | sort -u | tr '\n' ':')"
  join_by ":" "${items[*]}"
}

function deploy_libc() {
  mkdir -p "$APPDIR_LIBC_PREFIX"

  # deploy linker
  mkdir -p "$(dirname "${APPDIR_LIBC_PREFIX}"/"${APPDIR_LIBC_LINKER_PATH}")"
  cp "/$APPDIR_LIBC_LINKER_PATH" "${APPDIR_LIBC_PREFIX}"/"${APPDIR_LIBC_LINKER_PATH}"

  # deploy dependencies
  DEPENDENCIES=$(ldd "$BASH_BIN" | grep "=> " | cut -d' ' -f 3- | cut -d ' ' -f 1)
  LIBRARY_PATHS=""
  for DEP in $DEPENDENCIES; do
    LIBRARY_PATH=$(dirname "${APPDIR_LIBC_PREFIX}"/"${DEP}")
    LIBRARY_PATHS="${LIBRARY_PATHS}:${LIBRARY_PATH}"
    mkdir -p "$LIBRARY_PATH"
    cp "$DEP" "${APPDIR_LIBC_PREFIX}/$DEP"
  done

  APPDIR_LIBC_LIBRARY_PATH="$(remove_duplicated_paths "${LIBRARY_PATHS:1}")"
  export APPDIR_LIBC_LIBRARY_PATH
}

APPDIR_LIBC_PREFIX="$APPDIR/opt/libc"

# deploy binaries
mkdir -p "$APPDIR/bin/" "$APPDIR/usr/bin/" "$APPDIR/lib/"

DEPLOYED_BIN_PATH="$APPDIR/usr/bin/app"
DEPLOYED_BIN_SYMLINK_PATH="$APPDIR/bin/app"

BASH_BIN=$(which bash)
cp "$TARGET_BIN" "$DEPLOYED_BIN_PATH"
ln -sf "../usr/bin/app" "$DEPLOYED_BIN_SYMLINK_PATH"

# read linker path from bin
APPDIR_LIBC_LINKER_PATH=$(patchelf --print-interpreter "$DEPLOYED_BIN_PATH")
APPDIR_LIBC_LINKER_PATH="${APPDIR_LIBC_LINKER_PATH:1}"
patchelf --set-interpreter "$APPDIR_LIBC_LINKER_PATH" "$DEPLOYED_BIN_PATH"

deploy_libc
APPDIR_LIBC_VERSION=$(ldd --version | grep GLIBC | rev | cut -d" " -f 1 | rev)

APPDIR_LIBRARY_PATH="$APPDIR/lib/"
cp "$APPRUN_HOOKS" "$APPDIR/lib/"

# deploy AppRun
cp "$APPRUN" "$APPDIR"

echo "APPDIR=\$ORIGIN
APPDIR_EXEC_PATH=\$APPDIR/usr/bin/app
APPDIR_EXEC_ARGS=\$@
APPDIR_LIBC_PREFIX=$APPDIR_LIBC_PREFIX
APPDIR_LIBC_VERSION=$APPDIR_LIBC_VERSION
APPDIR_LIBC_LINKER_PATH=$APPDIR_LIBC_LINKER_PATH
APPDIR_LIBC_LIBRARY_PATH=$APPDIR_LIBC_LIBRARY_PATH
APPDIR_LIBRARY_PATH=$APPDIR_LIBRARY_PATH
" >"$APPDIR/AppRun.env"

sed -i "s|$APPDIR|\$APPDIR|g" "$APPDIR/AppRun.env"