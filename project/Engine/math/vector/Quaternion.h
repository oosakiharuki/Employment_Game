#pragma once
/// <summary>
/// 構造体_クオータニオン
/// </summary>
struct Quaternion {
	float x;
	float y;
	float z;
	float w;
};
namespace MyMath {
	//inline = 多重定義を防ぐ

	inline Quaternion operator-(const Quaternion& q) {
		Quaternion result;
		result.x = -q.x;
		result.y = -q.y;
		result.z = -q.z;
		result.w = -q.w;
		return result;
	}

	inline Quaternion operator*(float f, const Quaternion& q) {
		Quaternion result;
		result.x = f * q.x;
		result.y = f * q.y;
		result.z = f * q.z;
		result.w = f * q.w;
		return result;
	}

	inline Quaternion operator+(const Quaternion& q0, const Quaternion& q1) {
		Quaternion result;
		result.x = q0.x + q1.x;
		result.y = q0.y + q1.y;
		result.z = q0.z + q1.z;
		result.w = q0.w + q1.w;
		return result;
	}

	inline float MultiplyQuaternion(const Quaternion& q1, const Quaternion& q2) {
		Quaternion r;

		r.x = q1.x * q2.x;
		r.y = q1.y * q2.y;
		r.z = q1.z * q2.z;
		r.w = q1.w * q2.w;

		float result;

		result = r.x + r.y + r.z + r.w;

		return result;
	}
}