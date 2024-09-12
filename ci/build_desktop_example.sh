#!/bin/bash
set -e
cd /c/msys64/home/josko/HandTracker
bazel build -c opt --define MEDIAPIPE_DISABLE_GPU=1 mediapipe/examples/desktop/hand_tracking:hand_tracking_cpu \
  --action_env PYTHON_BIN_PATH="C://PROGRA~1//python3//python.exe" \
  --action_env PATH="$PATH:$HOME/python3:$HOME/python3/Scripts"

GLOG_logtostderr=1 bazel-bin/mediapipe/examples/desktop/hand_tracking/hand_tracking_cpu --calculator_graph_config_file=mediapipe/graphs/hand_tracking/hand_tracking_desktop_live.pbtxt
