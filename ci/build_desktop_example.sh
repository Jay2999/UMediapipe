#!/bin/bash
set -e
cd /c/msys64/home/josko/HandTracker

bazel build -c opt --define MEDIAPIPE_DISABLE_GPU=1 mediapipe/UMediapipe:UMediapipe_shared \
  --action_env PYTHON_BIN_PATH="C://PROGRA~1//python3//python.exe" \
  --action_env PATH="$PATH:$HOME/python3:$HOME/python3/Scripts" \
  --experimental_cc_shared_library

bazel build -c opt --define MEDIAPIPE_DISABLE_GPU=1 mediapipe/UMediapipe/example/desktop:ump_desktop_example \
  --action_env PYTHON_BIN_PATH="C://PROGRA~1//python3//python.exe" \
  --action_env PATH="$PATH:$HOME/python3:$HOME/python3/Scripts" \
  --experimental_cc_shared_library

GLOG_logtostderr=1 bazel-bin/mediapipe/UMediapipe/example/desktop/ump_desktop_example
