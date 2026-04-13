#include "UseEveryOne.h"

namespace UseEveryOne {

	float SecondDecimalPoint(float value) {
		const float kSecondDecimalPoint = 100.0f;
		return std::floor(value * kSecondDecimalPoint) / kSecondDecimalPoint;
	}

}