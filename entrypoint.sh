#!/bin/bash

set -e

HEADER_FILE="/mediapipe/lib/mediagraph/mediagraph.h"
LIB_FILE="/bazel-bin/mediapipe/lib/mediagraph/libmediagraph.so"

echo "Installing header file..."

if [ -f "$HEADER_FILE" ]; then
    echo "Found \"$HEADER_FILE\""

    cp "$HEADER_FILE" "/usr/include/mediagraph.h"
    cp "$HEADER_FILE" "/usr/include/x86_64-linux-gnu/mediagraph.h"
    cp "$HEADER_FILE" "${GITHUB_WORKSPACE}/mediagraph.h"
else 
    echo "$HEADER_FILE does not exist"
    exit 1
fi

echo "Installing library..."

if [ -f "$LIB_FILE" ]; then
    echo "Found \"$LIB_FILE\""

    cp "$LIB_FILE" "/usr/lib/libmediagraph.so"
    cp "$LIB_FILE" "/usr/lib/x86_64-linux-gnu/libmediagraph.so"
    cp "$LIB_FILE" "$GITHUB_WORKSPACE/libmediagraph.so"
else 
    echo "$LIB_FILE does not exist"
    exit 1
fi