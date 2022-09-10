#!/bin/bash

set -e

HEADER_FILE="/mediapipe/lib/mediagraph/mediagraph.h"
LIB_FILE="/bazel-bin/mediapipe/lib/mediagraph/libmediagraph.so"

if [ -f "$HEADER_FILE" ]; then
    echo "Found \"$HEADER_FILE\""

    mkdir -p "/usr/include/" && cp "$HEADER_FILE" "/usr/include/mediagraph.h"
    mkdir -p "/usr/include/x86_64-linux-gnu" && cp "$HEADER_FILE" "/usr/include/x86_64-linux-gnu/mediagraph.h"
else 
    echo "$HEADER_FILE does not exist"
    exit 1
fi

if [ -f "$LIB_FILE" ]; then
    echo "Found \"$LIB_FILE\""

    mkdir -p "/usr/lib" && cp "$LIB_FILE" "/usr/lib/libmediagraph.so"
    mkdir -p "/usr/lib/x86_64-linux-gnu" && cp "$LIB_FILE" "/usr/lib/x86_64-linux-gnu/libmediagraph.so"
else 
    echo "$LIB_FILE does not exist"
    exit 1
fi