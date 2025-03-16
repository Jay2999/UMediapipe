#!/bin/bash
set -e
source ~/.profile
cd /mnt/c/msys64/home/josko/HandTracker

bazel build -c opt --jobs 16 --config=android_arm64 mediapipe/UMediapipe:UMediapipe
cp -L -f ./bazel-out/arm64-v8a-opt/bin/mediapipe/UMediapipe/libUMediapipe.so \
  ./UnrealProject/Plugins/HandyVR/Source/ThirdParty/libHandyVR/lib/Android/arm64-v8a/libUMediapipe.so