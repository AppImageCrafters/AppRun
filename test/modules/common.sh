#!/bin/bash

function assert_success() {
  if $@; then
    echo " [PASS]"
  else
    echo " [FAIL]"
    exit 1
  fi
}

function assert_fail() {
  if $@; then
    echo " [FAIL]"
    exit 1
  else
    echo " [PASS]"
  fi
}