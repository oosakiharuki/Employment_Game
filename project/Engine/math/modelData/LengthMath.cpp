#include "LengthMath.h"
#include "Segment.h"
#include <cmath>

namespace MyMath {
	float Length(float start, float target) {
		return std::abs(start - target);
	}

	Vector3 Length(const Vector3& start, const Vector3& target) {
		Vector3 result{};
		result.x = std::abs(start.x - target.x);
		result.y = std::abs(start.y - target.y);
		result.z = std::abs(start.z - target.z);

		return result;
	}

	Vector3 GoDestination(const Segment& segment) {
		Vector3 result = Length(segment.origin, segment.diff);
		//目的地(diff)が現在地(origin)より値が小さい場合、マイナス値にする
		if (segment.origin.x > segment.diff.x) {
			result.x = -result.x;
		}
		if (segment.origin.y > segment.diff.y) {
			result.y = -result.y;
		}
		if (segment.origin.z > segment.diff.z) {
			result.z = -result.z;
		}
		return result;
	}

	Vector3 GoDestination(const Vector3& origin, const Vector3& diff) {
		Segment segment = { origin,diff };
		Vector3 result = GoDestination(segment);
		return result;
	}
}