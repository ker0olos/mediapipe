#!/bin/sh

echo "Installing library..."
LIB_FILE="/mediapipe/bazel-bin/mediapipe/lib/mediagraph/libmediagraph.so"
if [ -f "$LIB_FILE" ]; then
  cp "$LIB_FILE" "/usr/lib/libmediagraph.so"
  cp "$LIB_FILE" "/usr/lib/x86_64-linux-gnu/libmediagraph.so"
else 
    echo "Could not find library file"
    exit 1
fi

echo "Installing header file..."
HEADER_FILE="/mediapipe/mediapipe/lib/mediagraph/mediagraph.h"
if [ -f "$HEADER_FILE" ]; then
  cp "$HEADER_FILE" "/usr/include/mediagraph.h"
  cp "$HEADER_FILE" "/usr/include/x86_64-linux-gnu/mediagraph.h"
else 
    echo "Could not find header file"
    exit 1
fi

