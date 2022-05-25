#!/bin/bash
set -e

TARGET_SYSTEMS="
  archlinux:latest
  centos:6 centos:7 centos:8
  fedora:33 fedora:34 fedora:35 fedora:37
  opensuse/leap:latest opensuse/tumbleweed:latest
  debian:stable-slim debian:unstable-slim debian:testing-slim
  ubuntu:16.04 ubuntu:18.04 ubuntu:20.04 ubuntu:21.04 ubuntu:22.04
  "

if [ ! -f $PWD/src/check-runtime/check-libc ]; then
  echo "ERROR: Missing \"src/check-runtime/check-libc\", execute from the build dir."
  exit 1
fi

if [ ! -f $PWD/src/check-runtime/check-libstdc++ ]; then
  echo "ERROR: Missing \"src/check-runtime/check-libstdc++\", execute from the build dir."
  exit 1
fi

for TARGET in $TARGET_SYSTEMS; do
  echo $TARGET
  docker run --rm -v $PWD/src/check-runtime/:/check-runtime "$TARGET" /check-runtime/check-libc
  docker run --rm -v $PWD/src/check-runtime/:/check-runtime "$TARGET" /check-runtime/check-libstdc++
  echo ""
done
