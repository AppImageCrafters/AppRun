#!/bin/bash

set -ex

APPDIR="$1"
if [ -z "$APPDIR" ]; then
  echo "Missing AppDir path"
  exit 1
fi

APPRUN="$2"
if [ -z "$APPRUN" ]; then
  echo "Missing AppRun path"
  exit 1
fi

APPRUN_HOOKS="$3"
if [ -z "$APPRUN_HOOKS" ]; then
  echo "Missing libapprun_hooks.so path"
  exit 1
fi

rm "$APPDIR" -rf || true
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
  ln -s $LINKER "${RUNTIME_DEFAULT_DIR}"/"${LINKER}"

  # deploy dependencies
  DEPENDENCIES=$(ldd "$BASH_BIN" | grep "=> " | cut -d' ' -f 3- | cut -d ' ' -f 1)
  LIBRARY_PATHS=""
  for DEP in $DEPENDENCIES; do
    LIBRARY_PATH=$(dirname "${RUNTIME_DEFAULT_DIR}"/"${DEP}")
    LIBRARY_PATHS="${LIBRARY_PATHS}:${LIBRARY_PATH}"
    mkdir -p "$LIBRARY_PATH"
    ln -s "$DEP" "$RUNTIME_DEFAULT_DIR/$DEP"
  done

  echo "$LIBRARY_PATHS"
}

RUNTIME_COMPAT_DIR="$APPDIR/runtime/compat"
RUNTIME_DEFAULT_DIR="$APPDIR/runtime/default"

# deploy bash
BASH_BIN=$(which bash)
mkdir -p "$(dirname "$APPDIR/$BASH_BIN")"
cp "$BASH_BIN" "$APPDIR/$BASH_BIN"

# make bash linker path relative
LINKER=$(patchelf --print-interpreter "$BASH_BIN")
patchelf --set-interpreter "${LINKER:1}" "$APPDIR/$BASH_BIN"

# deploy pwd
PWD_BIN=$(which pwd)
mkdir -p "$(dirname "$APPDIR/$PWD_BIN")"
cp "$PWD_BIN" "$APPDIR/$PWD_BIN"

# make pwd linker path relative
LINKER=$(patchelf --print-interpreter "$PWD_BIN")
patchelf --set-interpreter "${LINKER:1}" "$APPDIR/$PWD_BIN"

LD_PATHS=$(create_compat_runtime)
create_default_runtime

cp "$APPRUN_HOOKS" "$APPDIR"
LD_PATHS="$APPDIR:$LD_PATHS"

# deploy AppRun
cp "$APPRUN" "$APPDIR"
echo "APPDIR=$APPDIR/
APPIMAGE_UUID=khOEqNr
EXEC_PATH=\$APPDIR/$BASH_BIN
EXEC_ARGS=\$@
APPRUN_LD_PATHS=lib64/ld-linux-x86-64.so.2;
LIBC_LIBRARY_PATH=$LD_PATHS
LD_PRELOAD=libapprun_hooks.so
" >"$APPDIR/AppRun.env"
