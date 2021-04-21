
#pragma once
#include <stdlib.h>

#define RESORE_VOLUME * (1 / t)

static class DistortionAlgorithms {
public:
	static float Brickwall(float v, float t) {
		// clamp between positive and negative threshold
		return max(min(v, t), -t) RESORE_VOLUME;
	}

	static float Mod(float v, float t) {
		// if value passes the threshold, threshold value is substracted
		return fmodf(v, t) RESORE_VOLUME;
	}

	static float Infinite(float v, float t) {
		const float t2 = t * 2;
		// if magnitude of v greater than the threshold...
		while (abs(v) > abs(t))
			// ...flip back to the other threshold
			v += v > 0 ? -t2 : t2;
		return v RESORE_VOLUME;
	}

	static float Mirror(float v, float t) {
		// TODO
		return 0;
	}
};