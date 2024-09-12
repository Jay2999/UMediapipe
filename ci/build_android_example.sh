#!/bin/bash
set -e
source ~/.profile
cd /mnt/c/msys64/home/josko/HandTracker

bazel build -c opt --config=android_arm64 mediapipe/examples/android/src/java/com/google/mediapipe/apps/handtrackinggpu:handtrackinggpu
adb install bazel-bin/mediapipe/examples/android/src/java/com/google/mediapipe/apps/handtrackinggpu/handtrackinggpu.apk
adb shell am start -n com.google.mediapipe.apps.handtrackinggpu/.MainActivity
