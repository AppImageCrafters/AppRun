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
CHECK_LIBC=$(find "$BUILD_DIR/src" -name check-glibc | head -n 1)
CHECK_LIBSTDCPP=$(find "$BUILD_DIR/src" -name check-glibstdc++ | head -n 1)
TARGET_BIN=$(find "$BUILD_DIR/test" -name hooks_inner_target_test | head -n 1)

if [ -z "$APPRUN" ]; then
  echo "Missing AppRun path"
  exit 1
fi

if [ -z "$APPRUN_HOOKS" ]; then
  echo "Missing libapprun_hooks.so path"
  exit 1
fi

if [ -z "$CHECK_LIBC" ]; then
  echo "Missing check-glibc path"
  exit 1
fi

if [ -z "$CHECK_LIBSTDCPP" ]; then
  echo "Missing check-libstdc++ path"
  exit 1
fi

if [ -z "$TARGET_BIN" ]; then
  echo "Missing TARGET_BIN path"
  exit 1
fi

mkdir -p "$APPDIR"

function join_by() {
  local IFS="$1"
  shift
  echo "$*"
}

function remove_duplicated_paths() {
  local items=("${@}")
  IFS=":" read -r -a items <<<"$(tr ':' '\n' <<<"${items[@]}" | sort -u | tr '\n' ':')"
  join_by ":" "${items[*]}"
}

function patch_appdir_path_in_config() {
  sed -i "s|$APPDIR|\$APPDIR|g" $1
}

function create_libc_module() {
  LIBC_MODULE_PATH="$APPDIR/opt/libc"
  mkdir -p "$LIBC_MODULE_PATH"

  # deploy linker
  mkdir -p "$(dirname "${LIBC_MODULE_PATH}"/"${APPDIR_LIBC_LINKER_PATH}")"
  cp "/$APPDIR_LIBC_LINKER_PATH" "${LIBC_MODULE_PATH}"/"${APPDIR_LIBC_LINKER_PATH}"

  # deploy app as interpreter
  mkdir -p "$(dirname "$LIBC_MODULE_PATH/$REL_BIN_PATH")"
  ln -sf "../../../../$REL_BIN_PATH" "$LIBC_MODULE_PATH/$REL_BIN_PATH"

  # deploy dependencies
  DEPENDENCIES=$(ldd "$TARGET_BIN" | grep "=> " | cut -d' ' -f 3- | cut -d ' ' -f 1)
  LIBRARY_PATHS=""
  for DEP in $DEPENDENCIES; do
    LIBRARY_PATH=$(dirname "${LIBC_MODULE_PATH}"/"${DEP}")
    LIBRARY_PATHS="${LIBRARY_PATHS}:${LIBRARY_PATH}"
    mkdir -p "$LIBRARY_PATH"
    cp "$DEP" "${LIBC_MODULE_PATH}/$DEP"
  done

  APPDIR_LIBC_LIBRARY_PATH="$(remove_duplicated_paths "${LIBRARY_PATHS:1}")"
  APPDIR_LIBC_VERSION=$(ldd --version | grep GLIBC | rev | cut -d" " -f 1 | rev)

  # deploy runtime tools
  cp "$CHECK_LIBC" "$APPDIR/opt/libc/check"

  # deploy config file
  cat >$APPDIR/opt/libc/config <<EOF
version = "1.0";
check:
{
  required_glibc = "$APPDIR_LIBC_VERSION";
};
module:
{
  library_path = ( "${APPDIR_LIBC_LIBRARY_PATH}" );
};
EOF
  patch_appdir_path_in_config "$APPDIR/opt/libc/config"
}

function setup_default_linker() {
  # deploy linker
  mkdir -p "$(dirname "$APPDIR"/"${APPDIR_LIBC_LINKER_PATH}")"
  ln -sf "/$APPDIR_LIBC_LINKER_PATH" "$APPDIR/$APPDIR_LIBC_LINKER_PATH"
}

function create_libstdcpp_module() {
  libstdcpp_path=$(ldconfig -p | grep libstdc++ | head -1 | rev | cut -d' ' -f 1 | rev)
  real_libstdcpp_path=$(realpath $libstdcpp_path)
  target_dir=$(dirname "$APPDIR/opt/libstdc++/$real_libstdcpp_path")

  mkdir -p "$target_dir"

  cp "$real_libstdcpp_path" "$target_dir"

  link_file_name=$(basename $libstdcpp_path)
  ln -sf $(basename "$real_libstdcpp_path") "$target_dir/$link_file_name"

  APPDIR_LIBSTDCPP_VERSION=$(echo "$real_libstdcpp_path" | grep -oP "libstdc\+\+\.so\.\K(\d+\.\d+(\.\d+))")

  cp "$CHECK_LIBSTDCPP" "$APPDIR/opt/libstdc++/check"

  cat >$APPDIR/opt/libstdc++/config <<EOF
version = "1.0";
check:
{
  required_glibstdcpp = "${APPDIR_LIBSTDCPP_VERSION}";
};
module:
{
  library_path = ( "${target_dir}" );
};
EOF

  patch_appdir_path_in_config "$APPDIR/opt/libstdc++/config"
}

APPDIR_DEFAULT_RUNTIME="$APPDIR"

# deploy binaries
mkdir -p "$APPDIR/bin/" "$APPDIR/usr/bin/" "$APPDIR/lib/"

REL_BIN_PATH="usr/bin/app"
DEPLOYED_BIN_PATH="$APPDIR/$REL_BIN_PATH"
DEPLOYED_BIN_SYMLINK_PATH="$APPDIR/bin/app"

cp "$TARGET_BIN" "$DEPLOYED_BIN_PATH"
ln -sf "../usr/bin/app" "$DEPLOYED_BIN_SYMLINK_PATH"

echo "#! $REL_BIN_PATH" >"$APPDIR/usr/bin/script"
chmod +x "$APPDIR/usr/bin/script"

# read linker path from bin
APPDIR_LIBC_LINKER_PATH=$(patchelf --print-interpreter "$DEPLOYED_BIN_PATH")
APPDIR_LIBC_LINKER_PATH="${APPDIR_LIBC_LINKER_PATH:1}"

# patch bin with a relative linker path
patchelf --set-interpreter "$APPDIR_LIBC_LINKER_PATH" "$DEPLOYED_BIN_PATH"

# make libstdc++.so.6 a dependency
patchelf --add-needed "libstdc++.so.6" "$DEPLOYED_BIN_PATH"

setup_default_linker
create_libc_module
create_libstdcpp_module

APPDIR_LIBRARY_PATH="$APPDIR/lib/"
cp "$APPRUN_HOOKS" "$APPDIR/lib/"

# deploy AppRun
cp "$APPRUN" "$APPDIR"

cat >"$APPDIR/AppRun.config" <<EOF
version = "1.0";
runtime:
{
  exec = ( "\$APPDIR/usr/bin/script", "\$@" );
  linkers = ( "$APPDIR_LIBC_LINKER_PATH" );
  library_path = ( "${APPDIR_LIBRARY_PATH}" );
  path_mappings = ( ("/bin", "\$APPDIR/bin") );
};
EOF

patch_appdir_path_in_config "$APPDIR/AppRun.config"
