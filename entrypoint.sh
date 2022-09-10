#!/bin/bash

set -e

HEADER_FILE="/mediapipe/lib/mediagraph/mediagraph.h"
LIB_FILE="/bazel-bin/mediapipe/lib/mediagraph/libmediagraph.so"

# github only allows changes to the github workspace directory
# through setting up a docker volume
# can't install the header and library to the system directories  
cd "$GITHUB_WORKSPACE"

if [ -f "$HEADER_FILE" ]; then
    echo "Found \"$HEADER_FILE\""
    cp "$HEADER_FILE" "mediagraph.h"
    echo "Copied mediagraph.h to \"$GITHUB_WORKSPACE/mediagraph.h\""
else 
    echo "$HEADER_FILE does not exist"
    exit 1
fi

if [ -f "$LIB_FILE" ]; then
    echo "Found \"$LIB_FILE\""
    cp "$LIB_FILE" "libmediagraph.so"
    echo "Copied libmediagraph.so to \"$GITHUB_WORKSPACE/libmediagraph.so\""

else 
    echo "$LIB_FILE does not exist"
    exit 1
fi