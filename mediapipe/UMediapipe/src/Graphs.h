#pragma once

#include "mediapipe/framework/port/parse_text_proto.h"
#include "mediapipe/framework/calculator_framework.h"

mediapipe::CalculatorGraphConfig getHandLandmarkGraphConfigCpu();
mediapipe::CalculatorGraphConfig getHandLandmarkGraphConfigGpu();