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

function join_by {
  local IFS="$1"
  shift
  echo "$*"
}

function remove_duplicated_paths() {
  local items=("${@}")
  IFS=":" read -r -a items <<<"$(tr ':' '\n' <<<"${items[@]}" | sort -u | tr '\n' ':')"
  join_by ":" "${items[*]}"
}

function create_compat_runtime() {
  mkdir -p "$APPDIR_COMPAT_RUNTIME"

  # deploy linker
  mkdir -p "$(dirname "${APPDIR_COMPAT_RUNTIME}"/"${APPDIR_LIBC_LINKER_PATH}")"
  cp "/$APPDIR_LIBC_LINKER_PATH" "${APPDIR_COMPAT_RUNTIME}"/"${APPDIR_LIBC_LINKER_PATH}"

  # deploy app as interpreter
  mkdir -p "$(dirname "$APPDIR_COMPAT_RUNTIME/$REL_BIN_PATH")"
  ln -sf "../../../../$REL_BIN_PATH" "$APPDIR_COMPAT_RUNTIME/$REL_BIN_PATH"

  # deploy dependencies
  DEPENDENCIES=$(ldd "$TARGET_BIN" | grep "=> " | cut -d' ' -f 3- | cut -d ' ' -f 1)
  LIBRARY_PATHS=""
  for DEP in $DEPENDENCIES; do
    LIBRARY_PATH=$(dirname "${APPDIR_COMPAT_RUNTIME}"/"${DEP}")
    LIBRARY_PATHS="${LIBRARY_PATHS}:${LIBRARY_PATH}"
    mkdir -p "$LIBRARY_PATH"
    cp "$DEP" "${APPDIR_COMPAT_RUNTIME}/$DEP"
  done

  APPDIR_LIBC_LIBRARY_PATH="$(remove_duplicated_paths "${LIBRARY_PATHS:1}")"
  APPDIR_LIBC_VERSION=$(ldd --version | grep GLIBC | rev | cut -d" " -f 1 | rev)

  export APPDIR_LIBC_LIBRARY_PATH
  export APPDIR_LIBC_VERSION
}

function create_default_runtime() {
  mkdir -p "$APPDIR_DEFAULT_RUNTIME"

  # deploy linker
  mkdir -p "$(dirname "${APPDIR_DEFAULT_RUNTIME}"/"${APPDIR_LIBC_LINKER_PATH}")"
  ln -sf "/$APPDIR_LIBC_LINKER_PATH" "$APPDIR_DEFAULT_RUNTIME/$APPDIR_LIBC_LINKER_PATH"

  # deploy app as interpreter
  mkdir -p "$(dirname "$APPDIR_DEFAULT_RUNTIME/$REL_BIN_PATH")"
  ln -sf "../../../../$REL_BIN_PATH" "$APPDIR_DEFAULT_RUNTIME/$REL_BIN_PATH"
}

APPDIR_COMPAT_RUNTIME="$APPDIR/runtime/compat"
APPDIR_DEFAULT_RUNTIME="$APPDIR/runtime/default"

# deploy binaries
mkdir -p "$APPDIR/bin/" "$APPDIR/usr/bin/" "$APPDIR/lib/"

REL_BIN_PATH="usr/bin/app"
DEPLOYED_BIN_PATH="$APPDIR/$REL_BIN_PATH"
DEPLOYED_BIN_SYMLINK_PATH="$APPDIR/bin/app"

cp "$TARGET_BIN" "$DEPLOYED_BIN_PATH"
ln -sf "../usr/bin/app" "$DEPLOYED_BIN_SYMLINK_PATH"

echo "#! $REL_BIN_PATH" > "$APPDIR/usr/bin/script"
chmod +x "$APPDIR/usr/bin/script"


# read linker path from bin
APPDIR_LIBC_LINKER_PATH=$(patchelf --print-interpreter "$DEPLOYED_BIN_PATH")
APPDIR_LIBC_LINKER_PATH="${APPDIR_LIBC_LINKER_PATH:1}"

# patch bin with a relative linker path
patchelf --set-interpreter "$APPDIR_LIBC_LINKER_PATH" "$DEPLOYED_BIN_PATH"

create_compat_runtime
create_default_runtime

APPDIR_LIBRARY_PATH="$APPDIR/lib/"
cp "$APPRUN_HOOKS" "$APPDIR/lib/"

# deploy AppRun
cp "$APPRUN" "$APPDIR"

echo "APPDIR=\$ORIGIN
APPDIR_EXEC_PATH=\$APPDIR/usr/bin/script
APPDIR_EXEC_ARGS=\$@
APPDIR_LIBC_VERSION=$APPDIR_LIBC_VERSION
APPDIR_LIBC_LINKER_PATH=$APPDIR_LIBC_LINKER_PATH
APPDIR_LIBC_LIBRARY_PATH=$APPDIR_LIBC_LIBRARY_PATH
APPDIR_LIBRARY_PATH=$APPDIR_LIBRARY_PATH" >"$APPDIR/AppRun.env"

sed -i "s|$APPDIR|\$APPDIR|g" "$APPDIR/AppRun.env"
