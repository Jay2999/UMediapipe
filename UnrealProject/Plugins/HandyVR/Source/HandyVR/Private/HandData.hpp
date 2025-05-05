#pragma once

#include "HandEnumLibrary.h"
#include "Types.h"

#define APPROX_DEPTH 1

struct HandData {
	FTransform transform = FTransform(FVector(40, 0, 0));
	Handedness handedness;
	HandGestures gesture;
	FVector fingersAngles[4];
	float thumbPitch;
	FVector thumbAngles;
};

static inline FVector toUSpace(const ump::Landmark& landmark) {
#if APPROX_DEPTH
#if WINDOWS
	float x = -2 * landmark.z();
	float y = 1 - landmark.x() * 2;
#else
	float x = 2 * landmark.z();
	float y = landmark.x() * 2 - 1;
#endif
	return FVector(x, y, 1 - 2 * landmark.y());
#else
#if WINDOWS
	float x = -2 * landmark.z();
	float y = -landmark.x() * 2;
#else
	float x = 2 * landmark.z();
	float y = landmark.x() * 2;
#endif
	return FVector(x, y, -2 * landmark.y());
#endif
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


static inline float getJointAngleDegrees(const ump::HandLandmarks& landmarks, const FVector& up, const FVector& fwd, int jointIndexStart, int jointIndexEnd) {
	auto v = toUSpace(landmarks[jointIndexEnd]) - toUSpace(landmarks[jointIndexStart]);
	v.Normalize();
	auto v_proj = FVector::VectorPlaneProject(v, up) * -FMath::Sign(FVector::DotProduct(v, fwd));
	v_proj.Normalize();
	float angle_rad = FMath::Acos(FVector::DotProduct(v, v_proj)) * FMath::Sign(FVector::DotProduct(v, up));
	float angle_deg = FMath::RadiansToDegrees(angle_rad);
	return angle_deg;
}

static HandData toHandData(const ump::HandDetectionResult& hand) {
	HandData handData;
	handData.handedness = hand.getHandedness() == ump::Handedness::LEFT ? Handedness::LEFT : Handedness::RIGHT;
	handData.gesture = (HandGestures)hand.Gesture();

	auto l0 = toUSpace(hand.Landmarks(0));
	auto location = l0;
#if APPROX_DEPTH
	float depth = approxDepth(hand.Landmarks());
	location *= (depth / 2 + 3) / 3;
	location.X = depth;
#else
	location.X += 3;
#endif
#if ANDROID
	location.Z += 0.5;
#endif
	location *= 50;

	auto v1 = toUSpace(hand.Landmarks(5)) - l0;
	auto v2 = toUSpace(hand.Landmarks(17)) - l0;
	if (hand.getHandedness() == ump::Handedness::RIGHT) {
		std::swap(v1, v2);
	}
	auto u = -FVector::CrossProduct(v1, v2);
	u.Normalize();

	auto f = toUSpace(hand.Landmarks(9)) + toUSpace(hand.Landmarks(13)) - 2 * l0 + v1 + v2;
	f /= 4;
	f -= FVector::DotProduct(f, u) * u;
	f.Normalize();
	f = -f;

	auto r = FVector::CrossProduct(f, u);
	handData.transform = FTransform(f, r, u, location);

	for (int i = 5; i < 21; ++i) {
		if ((i & 0b11) == 0) continue;
		const int fingerIndex = (i - 1) / 4 - 1;
		const int angleIndex = (i - 1) & 0b11;
		auto& angle = handData.fingersAngles[fingerIndex][angleIndex];

		angle = getJointAngleDegrees(hand.Landmarks(), u, f, i, i + 1);
		for (int j = angleIndex - 1; j >= 0; --j) {
			angle -= handData.fingersAngles[fingerIndex][j];
		}
		angle = FMath::ClampAngle(angle, -90, 90);
	}

	handData.thumbPitch = FMath::ClampAngle(getJointAngleDegrees(hand.Landmarks(), u, f, 0, 4), -40, 40);
	for (int i = 0; i < 3; ++i) {
		float angle = getJointAngleDegrees(hand.Landmarks(), r, f, i + 1, i + 2);
		if (handData.handedness == Handedness::RIGHT) {
			angle *= -1;
		}
		for (int j = i - 1; j >= 0; --j) {
			angle -= handData.thumbAngles[j];
		}
		handData.thumbAngles[i] = FMath::ClampAngle(angle, -40, 40);
	}

	return handData;
}