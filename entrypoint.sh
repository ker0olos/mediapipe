#!/bin/sh

cd "${GITHUB_WORKSPACE}" || exit 1

sudo cp /mediapipe/bazel-bin/mediapipe/lib/mediagraph/libmediagraph /usr/lib/libmediagraph.so
sudo cp /mediapipe/mediapipe/lib/mediagraph/mediagraph.h /usr/include/mediagraph.h

sudo cp /mediapipe/bazel-bin/mediapipe/lib/mediagraph/libmediagraph /usr/lib/x86_64-linux-gnu/libmediagraph.so
sudo cp /mediapipe/mediapipe/lib/mediagraph/mediagraph.h /usr/include/x86_64-linux-gnu/mediagraph.h
