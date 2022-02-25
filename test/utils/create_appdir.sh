#!/bin/bash

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

function create_compat_runtime() {
  mkdir -p "$RUNTIME_COMPAT_DIR"

  # deploy linker
  mkdir -p "$(dirname "${RUNTIME_COMPAT_DIR}"/"${LINKER}")"
  cp $LINKER "${RUNTIME_COMPAT_DIR}"/"${LINKER}"

  # deploy dependencies
  DEPENDENCIES=$(ldd "$BASH_BIN" | grep "=> " | cut -d' ' -f 3- | cut -d ' ' -f 1)
  LIBRARY_PATHS=""
  for DEP in $DEPENDENCIES; do
    LIBRARY_PATH=$(dirname "${RUNTIME_COMPAT_DIR}"/"${DEP}")
    LIBRARY_PATHS="${LIBRARY_PATHS}:${LIBRARY_PATH}"
    mkdir -p "$LIBRARY_PATH"
    cp "$DEP" "${RUNTIME_COMPAT_DIR}/$DEP"
  done

  echo "$LIBRARY_PATHS"
}

function create_default_runtime() {
  mkdir -p "$RUNTIME_DEFAULT_DIR"

  # deploy linker
  mkdir -p "$(dirname "${RUNTIME_DEFAULT_DIR}"/"${LINKER}")"
  ln -sf $LINKER "${RUNTIME_DEFAULT_DIR}"/"${LINKER}"

  # deploy dependencies
  DEPENDENCIES=$(ldd "$BASH_BIN" | grep "=> " | cut -d' ' -f 3- | cut -d ' ' -f 1)
  for DEP in $DEPENDENCIES; do
    LIBRARY_PATH=$(dirname "${RUNTIME_DEFAULT_DIR}"/"${DEP}")
    mkdir -p "$LIBRARY_PATH"
    ln -sf "$DEP" "$RUNTIME_DEFAULT_DIR/$DEP"
  done
}

RUNTIME_COMPAT_DIR="$APPDIR/runtime/compat"
RUNTIME_DEFAULT_DIR="$APPDIR/runtime/default"

# deploy binaries
mkdir -p "$APPDIR/bin/" "$APPDIR/usr/bin/" "$APPDIR/lib/"

DEPLOYED_BIN_PATH="$APPDIR/usr/bin/app"
DEPLOYED_BIN_SYMLINK_PATH="$APPDIR/bin/app"

BASH_BIN=$(which bash)
cp "$TARGET_BIN" "$DEPLOYED_BIN_PATH"
ln -sf "../usr/bin/app" "$DEPLOYED_BIN_SYMLINK_PATH"

# read linker path from bin
LINKER=$(patchelf --print-interpreter "$DEPLOYED_BIN_PATH")
patchelf --set-interpreter "${LINKER:1}" "$DEPLOYED_BIN_PATH"

LD_PATHS=$(create_compat_runtime)
create_default_runtime

cp "$APPRUN_HOOKS" "$APPDIR/lib/"
LD_PATHS="$APPDIR/lib:$LD_PATHS"
LD_PATHS="${LD_PATHS/$APPDIR/\$APPDIR}"

# deploy AppRun
cp "$APPRUN" "$APPDIR"
echo "APPDIR=\$ORIGIN
LD_PRELOAD=libapprun_hooks.so
EXEC_PATH=\$APPDIR/usr/bin/app
EXEC_ARGS=\$@
APPRUN_LD_PATHS=lib64/ld-linux-x86-64.so.2;
LIBC_LIBRARY_PATH=$LD_PATHS
" >"$APPDIR/AppRun.env"
