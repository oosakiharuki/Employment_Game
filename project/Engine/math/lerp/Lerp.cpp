#include "Lerp.h"
#include <cfloat>

namespace MyMath {

	Vector3 Lerp(const Vector3& p0, const Vector3& p1, float t) {
		Vector3 pointA = p0 * (1.0f - t);
		Vector3 pointB = p1 * t;
		return pointA + pointB;
	}

	Quaternion SLerp(const Quaternion& q0, const Quaternion& q1, float t) {

		Quaternion q3 = q0;

		float dot = MultiplyQuaternion(q0, q1);

		if (dot < 0) {
			q3 = -q0;
			dot = -dot;
		}

		if (dot >= 1.0f - FLT_EPSILON) {
			return (1.0f - t) * q3 + t * q1;
		}

		float theta = std::acos(dot);

		float scale0;

		scale0 = sin((1 - t) * theta) / sin(theta);

		float scale1;

		scale1 = sin(t * theta) / sin(theta);

		return scale0 * q3 + scale1 * q1;
	}
}
