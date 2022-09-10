#!/bin/sh

echo "Installing library..."
LIB_FILE="/mediapipe/libmediagraph.so"
cp "$LIB_FILE" /usr/lib/libmediagraph.so || exit 1
cp "$LIB_FILE" /usr/lib/x86_64-linux-gnu/libmediagraph.so || exit 1

echo "Installing header file..."
HEADER_FILE=" /mediapipe/mediagraph.h"
cp "$HEADER_FILE" /usr/include/mediagraph.h || exit 1
cp "$HEADER_FILE" /usr/include/x86_64-linux-gnu/mediagraph.h || exit 1
