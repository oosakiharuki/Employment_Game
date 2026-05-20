#pragma once

/// <summary>
/// 構造体_Vector2(x,y)
/// </summary>
struct Vector2 {
	float x;
	float y;
};

namespace MyMath {
	
	//inline = 多重定義を防ぐ

	inline Vector2 operator+(const Vector2& v1, const Vector2& v2) {
		Vector2 result{};
		result.x = v1.x + v2.x;
		result.y = v1.y + v2.y;
		return result;
	}

	inline Vector2 operator-(const Vector2& v1, const Vector2& v2) {
		Vector2 result{};
		result.x = v1.x - v2.x;
		result.y = v1.y - v2.y;
		return result;
	}

	inline Vector2 operator*(const Vector2& v1, const Vector2& v2) {
		Vector2 result{};
		result.x = v1.x * v2.x;
		result.y = v1.y * v2.y;
		return result;
	}

	inline Vector2 operator/(const Vector2& v1, const Vector2& v2) {
		Vector2 result{};
		result.x = v1.x / v2.x;
		result.y = v1.y / v2.y;
		return result;
	}

	inline Vector2 operator*(const Vector2& v, float f) {
		Vector2 result{};
		result.x = v.x * f;
		result.y = v.y * f;
		return result;
	}

	inline Vector2 operator*(float f, const Vector2& v) {
		return v * f;
	}


	inline Vector2 operator/(const Vector2& v, float f) {
		Vector2 result{};
		result.x = v.x / f;
		result.y = v.y / f;
		return result;
	}

	inline Vector2 operator/(float f, const Vector2& v) {
		return v / f;
	}


	inline Vector2& operator+=(Vector2& v1, const Vector2& v2) {
		v1.x += v2.x;
		v1.y += v2.y;
		return v1;
	}

	inline Vector2& operator-=(Vector2& v1, const Vector2& v2) {
		v1.x -= v2.x;
		v1.y -= v2.y;
		return v1;
	}

	inline Vector2& operator*=(Vector2& v1, const Vector2& v2) {
		v1.x *= v2.x;
		v1.y *= v2.y;
		return v1;
	}

	inline Vector2& operator/=(Vector2& v1, const Vector2& v2) {
		v1.x /= v2.x;
		v1.y /= v2.y;
		return v1;
	}
}