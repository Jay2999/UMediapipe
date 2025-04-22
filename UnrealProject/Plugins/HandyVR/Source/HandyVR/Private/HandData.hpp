#pragma once

#include "HandEnumLibrary.h"
#include "Types.h"

#define APPROX_DEPTH 1

struct HandData {
	FTransform transform;
	Handedness handedness;
};

static inline FVector toUSpace(const ump::Landmark& landmark) {
#if WINDOWS
	float y = 1 - landmark.x() * 2;
	float x = 1 - 2 * landmark.z();
#else
	float y = landmark.x() * 2 - 1;
	float x = 2 * landmark.z() - 1;
#endif
	return FVector(x, y, 1 - 2 * landmark.y());
}

static float approxDepth(const ump::HandLandmarks& landmarks) {
	auto l0 = toUSpace(landmarks[0]);
	auto v1 = toUSpace(landmarks[5]) - l0;
	auto v2 = toUSpace(landmarks[17]) - l0;
	float a = v1.Size() + v2.Size();
#if ANDROID
	a = 2.3 - a;
#endif
	return a * 2;
}

static HandData toHandData(const ump::HandDetectionResult& hand) {
	HandData handData;
	handData.handedness = hand.getHandedness() == ump::Handedness::LEFT ? Handedness::LEFT : Handedness::RIGHT;

	auto l0 = toUSpace(hand.Landmarks(0));
	handData.transform.SetLocation(l0);
#if APPROX_DEPTH
	float depth = approxDepth(hand.Landmarks());
	handData.transform.SetLocation(handData.transform.GetLocation() * (depth / 2 + 3) / 4);
	l0 = handData.transform.GetLocation();
	l0.X = depth;
	handData.transform.SetLocation(l0);
#endif

	auto v1 = toUSpace(hand.Landmarks(5)) - l0;
	auto v2 = toUSpace(hand.Landmarks(17)) - l0;
	if (hand.getHandedness() == ump::Handedness::RIGHT) {
		std::swap(v1, v2);
	}
	auto u = FVector::CrossProduct(v1, v2);
	u.Normalize();

	auto f = toUSpace(hand.Landmarks(9)) + toUSpace(hand.Landmarks(13)) - 2 * l0 + v1 + v2;
	f /= 4;
	f -= FVector::DotProduct(f, u) * u;
	f.Normalize();

	auto r = FVector::CrossProduct(f, u);

	FMatrix rotMatrix(f, r, u, FPlane(0, 0, 0, 0));
	handData.transform.SetRotation(FQuat(rotMatrix));

	return handData;
}