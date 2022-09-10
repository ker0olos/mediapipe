#!/bin/sh

echo "Installing libraries..."
cp /mediapipe/bazel-bin/mediapipe/lib/mediagraph/libmediagraph.so /usr/lib/libmediagraph.so || exit 1
cp /mediapipe/bazel-bin/mediapipe/lib/mediagraph/libmediagraph.so /usr/lib/x86_64-linux-gnu/libmediagraph.so || exit 1

echo "Installing header files..."
cp /mediapipe/mediapipe/lib/mediagraph/mediagraph.h /usr/include/mediagraph.h || exit 1
cp /mediapipe/mediapipe/lib/mediagraph/mediagraph.h /usr/include/x86_64-linux-gnu/mediagraph.h || exit 1
