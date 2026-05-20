#pragma once

/// <summary>
/// 構造体_Vector4(x,y,z,s)
/// </summary>
struct Vector4 {
	float x;
	float y;
	float z;
	float s;
};
namespace MyMath {
	//inline = 多重定義を防ぐ

	inline Vector4 operator+(const Vector4& v1, const Vector4& v2) {
		Vector4 result;
		result.x = v1.x + v2.x;
		result.y = v1.y + v2.y;
		result.z = v1.z + v2.z;
		result.s = v1.s + v2.s;
		return result;
	}

	inline Vector4 operator-(const Vector4& v1, const Vector4& v2) {
		Vector4 result;
		result.x = v1.x - v2.x;
		result.y = v1.y - v2.y;
		result.z = v1.z - v2.z;
		result.s = v1.s - v2.s;
		return result;
	}

	inline Vector4 operator*(const Vector4& v1, const Vector4& v2) {
		Vector4 result;
		result.x = v1.x * v2.x;
		result.y = v1.y * v2.y;
		result.z = v1.z * v2.z;
		result.s = v1.s * v2.s;
		return result;
	}

	inline Vector4 operator/(const Vector4& v1, const Vector4& v2) {
		Vector4 result;
		result.x = v1.x / v2.x;
		result.y = v1.y / v2.y;
		result.z = v1.z / v2.z;
		result.s = v1.s / v2.s;
		return result;
	}
}