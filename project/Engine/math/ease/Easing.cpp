#include "Easing.h"
#include <math.h>

namespace MyMath {

#pragma region EaseIn
	float EaseIn(float startPoint, float endPoint, float t) {
		float easeT = t * t;
		return  startPoint * (1 - easeT) + easeT * endPoint;
	}

	Vector2 EaseIn(const Vector2& startPoint, const Vector2& endPoint, float t) {
		Vector2 result{};

		result.x = EaseIn(startPoint.x, endPoint.x, t);
		result.y = EaseIn(startPoint.y, endPoint.y, t);

		return result;
	}

	Vector3 EaseIn(const Vector3& startPoint, const Vector3& endPoint, float t) {
		Vector3 result{};

		result.x = EaseIn(startPoint.x, endPoint.x, t);
		result.y = EaseIn(startPoint.y, endPoint.y, t);
		result.x = EaseIn(startPoint.z, endPoint.z, t);

		return result;
	}
#pragma endregion

#pragma region EaseOut
	float EaseOut(float startPoint, float endPoint, float t) {

		float easeT = powf(1.0f - t, 2.0f);

		return (1.0f - easeT) * startPoint + easeT * endPoint;
	}

	Vector2 EaseOut(const Vector2& startPoint, const Vector2& endPoint, float t) {
		Vector2 result{};
		result.x = EaseOut(startPoint.x, endPoint.x, t);
		result.y = EaseOut(startPoint.y, endPoint.y, t);
		return result;
	}

	Vector3 EaseOut(const Vector3& startPoint, const Vector3& endPoint, float t) {
		Vector3 result{};
		result.x = EaseOut(startPoint.x, endPoint.x, t);
		result.y = EaseOut(startPoint.y, endPoint.y, t);
		result.z = EaseOut(startPoint.z, endPoint.z, t);
		return result;
	}
#pragma endregion

#pragma region EaseInOut
	float EaseInOut(float startPoint, float endPoint, float t) {

		float easeT = powf(1.0f - (t * t), 2.0f);

		return (1.0f - easeT) * startPoint + easeT * endPoint;
	}

	Vector2 EaseInOut(const Vector2& startPoint, const Vector2& endPoint, float t) {
		Vector2 result{};
		result.x = EaseInOut(startPoint.x, endPoint.x, t);
		result.y = EaseInOut(startPoint.y, endPoint.y, t);
		return result;
	}

	Vector3 EaseInOut(const Vector3& startPoint, const Vector3& endPoint, float t) {
		Vector3 result{};
		result.x = EaseInOut(startPoint.x, endPoint.x, t);
		result.y = EaseInOut(startPoint.y, endPoint.y, t);
		result.z = EaseInOut(startPoint.z, endPoint.z, t);
		return result;
	}
#pragma endregion

}
