#!/bin/bash

SCRIPT_PATH=$(realpath "$0")
SCRIPT_DIR=$(dirname "$SCRIPT_PATH")

while IFS= read -r -d '' dockerfile; do
  TARGET_ARCH=$(basename "$dockerfile" | cut -f 2 -d .)
  TARGET_IMAGE_NAME="appimagecrafters/apprun-build-env-$TARGET_ARCH"

  echo "Building $TARGET_IMAGE_NAME"
  docker build "$SCRIPT_DIR" -f "$dockerfile" -t "$TARGET_IMAGE_NAME"

  if [[ -n "$UPLOAD_IMAGES" ]]; then
    docker push "$TARGET_IMAGE_NAME"
  fi
done < <(find "$SCRIPT_DIR" -name 'Dockerfile.*' -print0)
