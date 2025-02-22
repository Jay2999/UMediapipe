#!/bin/bash
set -e
cd /c/msys64/home/josko/HandTracker

bazel build -c opt --define MEDIAPIPE_DISABLE_GPU=1 mediapipe/UMediapipe:UMediapipe \
  --action_env PYTHON_BIN_PATH="C://PROGRA~1//python3//python.exe" \
  --action_env PATH="$PATH:$HOME/python3:$HOME/python3/Scripts"

cp -f bazel-out/x64_windows-opt/bin/mediapipe/UMediapipe/UMediapipe.dll UnrealProject/Plugins/HandyVR/Binaries/Win64/UMediapipe.dll
cp -f bazel-out/x64_windows-opt/bin/mediapipe/UMediapipe/UMediapipe.if.lib UnrealProject/Plugins/HandyVR/Binaries/Win64/UMediapipe.lib
cp -rf mediapipe/UMediapipe/include/*.h UnrealProject/Plugins/HandyVR/Source/ThirdParty/libHandyVR/include/