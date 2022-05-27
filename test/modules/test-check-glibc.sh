#!/bin/bash

set -e

TEST_DIR="$PWD"
CHECK_GLIBC_BIN=$1
if [ -z "$CHECK_GLIBC_BIN" ]; then
  echo "Missing CHECK_GLIBC_BIN"
  exit 1
fi

function setup_test_case() {
  TARGET_DIR=$1
  REQUIRED_GLIBC=$2
  mkdir -p "$TARGET_DIR"
  cp "$CHECK_GLIBC_BIN" "$TARGET_DIR/check"
  cat >"$TARGET_DIR/config" <<EOF
version = "1.0";
check:
{
  required_glibc = "$REQUIRED_GLIBC";
};
EOF
}

###############
## TEST CASES #
###############

printf "Check on system glibc higher than required, expected ZERO as return value"
setup_test_case "$TEST_DIR/test-check-glibc-with-lower-requirement" "1.0.0"
"$TEST_DIR/test-check-glibc-with-lower-requirement/check" && echo " PASS"

printf "Check on system glibc lower than required, expected NON ZERO as return value"
setup_test_case "$TEST_DIR/test-check-glibc-with-lower-requirement" "9999.0.0"
! "$TEST_DIR/test-check-glibc-with-lower-requirement/check" && echo " PASS"
