#pragma once
#include <cmath>

/// <summary>
/// 構造体_Vector3(x,y,z)
/// </summary>
struct Vector3 {
	float x;
	float y;
	float z;
};

namespace MyMath {
	
	//inline = 多重定義を防ぐ

	/// <summary>
	/// 法線(normal)
	/// </summary>
	/// <param name="v">Vector3</param>
	/// <returns>現在の法線</returns>
	inline Vector3 Normalize(const Vector3& v) {
		Vector3 result;
		result.x = v.x / (float)sqrt((v.x * v.x) + (v.y * v.y) + (v.z * v.z));
		result.y = v.y / (float)sqrt((v.x * v.x) + (v.y * v.y) + (v.z * v.z));
		result.z = v.z / (float)sqrt((v.x * v.x) + (v.y * v.y) + (v.z * v.z));
		return result;
	}

	inline Vector3 operator+(const Vector3& v1, const Vector3& v2) {
		Vector3 result;
		result.x = v1.x + v2.x;
		result.y = v1.y + v2.y;
		result.z = v1.z + v2.z;
		return result;
	}

	inline Vector3 operator-(const Vector3& v1, const Vector3& v2) {
		Vector3 result;
		result.x = v1.x - v2.x;
		result.y = v1.y - v2.y;
		result.z = v1.z - v2.z;
		return result;
	}

	inline Vector3 operator*(const Vector3& v1, const Vector3& v2) {
		Vector3 result;
		result.x = v1.x * v2.x;
		result.y = v1.y * v2.y;
		result.z = v1.z * v2.z;
		return result;
	}

	inline Vector3 operator/(const Vector3& v1, const Vector3& v2) {
		Vector3 result;
		result.x = v1.x / v2.x;
		result.y = v1.y / v2.y;
		result.z = v1.z / v2.z;
		return result;
	}

	inline Vector3 operator+(const Vector3& v, float f) {
		Vector3 result;
		result.x = v.x + f;
		result.y = v.y + f;
		result.z = v.z + f;
		return result;
	}

	inline Vector3 operator+(float f, const Vector3& v) {
		return v + f;
	}

	inline Vector3 operator-(const Vector3& v, float f) {
		Vector3 result;
		result.x = v.x - f;
		result.y = v.y - f;
		result.z = v.z - f;
		return result;
	}

	inline Vector3 operator-(float f, const Vector3& v) {
		return v - f;
	}

	inline Vector3 operator*(const Vector3& v, float f) {
		Vector3 result;
		result.x = v.x * f;
		result.y = v.y * f;
		result.z = v.z * f;
		return result;
	}

	inline Vector3 operator*(float f, const Vector3& v) {
		return v * f;
	}

	inline Vector3 operator/(const Vector3& v, float f) {
		Vector3 result;
		result.x = v.x / f;
		result.y = v.y / f;
		result.z = v.z / f;
		return result;
	}

	inline Vector3 operator/(float f, const Vector3& v) {
		return v / f;
	}


	inline Vector3& operator+=(Vector3& v1, const Vector3& v2) {
		v1.x += v2.x;
		v1.y += v2.y;
		v1.z += v2.z;
		return v1;
	}

	inline Vector3& operator-=(Vector3& v1, const Vector3& v2) {
		v1.x -= v2.x;
		v1.y -= v2.y;
		v1.z -= v2.z;
		return v1;
	}

	inline Vector3& operator*=(Vector3& v1, const Vector3& v2) {
		v1.x *= v2.x;
		v1.y *= v2.y;
		v1.z *= v2.z;
		return v1;
	}

	inline Vector3& operator/=(Vector3& v1, const Vector3& v2) {
		v1.x /= v2.x;
		v1.y /= v2.y;
		v1.z /= v2.z;
		return v1;
	}

	inline Vector3 operator-(const Vector3& v) {
		Vector3 result;
		result.x = -v.x;
		result.y = -v.y;
		result.z = -v.z;
		return result;
	}


	inline Vector3& operator+=(Vector3& v, float f) {
		v.x += f;
		v.y += f;
		v.z += f;
		return v;
	}

	inline Vector3& operator-=(Vector3& v, float f) {
		v.x -= f;
		v.y -= f;
		v.z -= f;
		return v;
	}

	inline Vector3& operator*=(Vector3& v, float f) {
		v.x *= f;
		v.y *= f;
		v.z *= f;
		return v;
	}

	inline Vector3& operator/=(Vector3& v, float f) {
		v.x /= f;
		v.y /= f;
		v.z /= f;
		return v;
	}

	inline bool operator==(const Vector3& v1, const Vector3& v2) {
		if (v1.x == v2.x && v1.y == v2.y && v1.z == v2.z) {
			return true;
		}
		return false;
	}

	inline bool operator!=(const Vector3& v1, const Vector3& v2) {
		if (v1 == v2) {
			return false;
		}
		return true;
	}

	inline bool operator<=(const Vector3& v1, const Vector3& v2) {
		if (v1.x <= v2.x && v1.y <= v2.y && v1.z <= v2.z) {
			return true;
		}
		return false;
	}

	inline bool operator>=(const Vector3& v1, const Vector3& v2) {
		if (v1.x >= v2.x && v1.y >= v2.y && v1.z >= v2.z) {
			return true;
		}
		return false;
	}


	inline bool operator<(const Vector3& v1, const Vector3& v2) {
		if (v1.x < v2.x && v1.y < v2.y && v1.z < v2.z) {
			return true;
		}
		return false;
	}

	inline bool operator>(const Vector3& v1, const Vector3& v2) {
		if (v1.x > v2.x && v1.y > v2.y && v1.z > v2.z) {
			return true;
		}
		return false;
	}
}
