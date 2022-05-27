#!/bin/bash

set -e

TEST_DIR="$PWD"
CHECK_GLIBSTDCPP_BIN=$1
if [ -z "$CHECK_GLIBSTDCPP_BIN" ]; then
  echo "Missing CHECK_GLIBSTDCPP_BIN"
  exit 1
fi

function setup_test_case() {
  TARGET_DIR=$1
  REQUIRED_GLIBSTDCPP=$2
  mkdir -p "$TARGET_DIR"
  cp "$CHECK_GLIBSTDCPP_BIN" "$TARGET_DIR/check"
  cat >"$TARGET_DIR/config" <<EOF
version = "1.0";
check:
{
  required_glibstdcpp = "$REQUIRED_GLIBSTDCPP";
};
EOF
}

source $(dirname $0)/common.sh


###############
## TEST CASES #
###############

printf "Check on system glibstdc++ higher than required, expected ZERO as return value"
setup_test_case "$TEST_DIR/test-check-glibstdc++-with-lower-requirement" "1.0.0"
assert_success "$TEST_DIR/test-check-glibstdc++-with-lower-requirement/check"

printf "Check on system glibstdc++ lower than required, expected NON ZERO as return value"
setup_test_case "$TEST_DIR/test-check-glibstdc++-with-lower-requirement" "9999.0.0"
assert_fail "$TEST_DIR/test-check-glibstdc++-with-lower-requirement/check"
