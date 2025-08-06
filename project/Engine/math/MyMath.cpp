#include "MyMath.h"
#include <cassert>
#include <cmath>
#include <numbers>

namespace MyMath {

	Vector3 operator+(const Vector3& v1, const Vector3& v2) {
		Vector3 result;
		result.x = v1.x + v2.x;
		result.y = v1.y + v2.y;
		result.z = v1.z + v2.z;
		return result;
	}

	Vector3 operator-(const Vector3& v1, const Vector3& v2) {
		Vector3 result;
		result.x = v1.x - v2.x;
		result.y = v1.y - v2.y;
		result.z = v1.z - v2.z;
		return result;
	}

	Vector3 operator*(const Vector3& v1, const Vector3& v2) {
		Vector3 result;
		result.x = v1.x * v2.x;
		result.y = v1.y * v2.y;
		result.z = v1.z * v2.z;
		return result;
	}

	Vector3 operator/(const Vector3& v1, const Vector3& v2) {
		Vector3 result;
		result.x = v1.x / v2.x;
		result.y = v1.y / v2.y;
		result.z = v1.z / v2.z;
		return result;
	}

	Vector3 operator*(const Vector3& v, const float f) {
		Vector3 result;
		result.x = v.x * f;
		result.y = v.y * f;
		result.z = v.z * f;
		return result;
	}

	Vector3 operator*(const float f, const Vector3& v) {
		return v * f;
	}

	Vector3 operator/(const Vector3& v, const float f) {
		Vector3 result;
		result.x = v.x / f;
		result.y = v.y / f;
		result.z = v.z / f;
		return result;
	}

	Vector3 operator/(const float f, const Vector3& v) {
		return v / f;
	}


	Vector3& operator+=(Vector3& v1, const Vector3& v2) {
		v1.x += v2.x;
		v1.y += v2.y;
		v1.z += v2.z;
		return v1;
	}

	Vector3& operator-=(Vector3& v1, const Vector3& v2) {
		v1.x -= v2.x;
		v1.y -= v2.y;
		v1.z -= v2.z;
		return v1;
	}

	Vector3& operator*=(Vector3& v1, const Vector3& v2) {
		v1.x *= v2.x;
		v1.y *= v2.y;
		v1.z *= v2.z;
		return v1;
	}

	Vector3& operator/=(Vector3& v1, const Vector3& v2) {
		v1.x /= v2.x;
		v1.y /= v2.y;
		v1.z /= v2.z;
		return v1;
	}


	Matrix4x4 operator*(const Matrix4x4& m1, const Matrix4x4& m2){
		Matrix4x4 result;
		result = Multiply(m1, m2);
		return result;
	}

	Matrix4x4 MakeIdentity4x4() {
		Matrix4x4 result{};

		result.m[0][0] = 1.0f;
		result.m[0][1] = 0.0f;
		result.m[0][2] = 0.0f;
		result.m[0][3] = 0.0f;
		result.m[1][0] = 0.0f;
		result.m[1][1] = 1.0f;
		result.m[1][2] = 0.0f;
		result.m[1][3] = 0.0f;
		result.m[2][0] = 0.0f;
		result.m[2][1] = 0.0f;
		result.m[2][2] = 1.0f;
		result.m[2][3] = 0.0f;
		result.m[3][0] = 0.0f;
		result.m[3][1] = 0.0f;
		result.m[3][2] = 0.0f;
		result.m[3][3] = 1.0f;

		return result;
	}

	Matrix4x4 MakeScaleMatrix(Vector3 scale) {
		Matrix4x4 result{};
		result.m[0][0] = scale.x;
		result.m[0][1] = 0.0f;
		result.m[0][2] = 0.0f;
		result.m[0][3] = 0.0f;
		result.m[1][0] = 0.0f;
		result.m[1][1] = scale.y;
		result.m[1][2] = 0.0f;
		result.m[1][3] = 0.0f;
		result.m[2][0] = 0.0f;
		result.m[2][1] = 0.0f;
		result.m[2][2] = scale.z;
		result.m[2][3] = 0.0f;
		result.m[3][0] = 0.0f;
		result.m[3][1] = 0.0f;
		result.m[3][2] = 0.0f;
		result.m[3][3] = 1.0f;

		return result;
	}
	Matrix4x4 MakeRotateXMatrix(float radian) {
		Matrix4x4 result{};
		result.m[0][0] = 1.0f;
		result.m[0][1] = 0.0f;
		result.m[0][2] = 0.0f;
		result.m[0][3] = 0.0f;
		result.m[1][0] = 0.0f;
		result.m[1][1] = cosf(radian);
		result.m[1][2] = sinf(radian);
		result.m[1][3] = 0.0f;
		result.m[2][0] = 0.0f;
		result.m[2][1] = -(sinf(radian));
		result.m[2][2] = cosf(radian);
		result.m[2][3] = 0.0f;
		result.m[3][0] = 0.0f;
		result.m[3][1] = 0.0f;
		result.m[3][2] = 0.0f;
		result.m[3][3] = 1.0f;

		return result;
	}	
	Matrix4x4 MakeRotateYMatrix(float radian) {
		Matrix4x4 result{};
		result.m[0][0] = cosf(radian);
		result.m[0][1] = 0.0f;
		result.m[0][2] = -(sinf(radian));
		result.m[0][3] = 0.0f;
		result.m[1][0] = 0.0f;
		result.m[1][1] = 1.0f;
		result.m[1][2] = 0.0f;
		result.m[1][3] = 0.0f;
		result.m[2][0] = sinf(radian);
		result.m[2][1] = 0.0f;
		result.m[2][2] = cosf(radian);
		result.m[2][3] = 0.0f;
		result.m[3][0] = 0.0f;
		result.m[3][1] = 0.0f;
		result.m[3][2] = 0.0f;
		result.m[3][3] = 1.0f;

		return result;
	}	
	Matrix4x4 MakeRotateZMatrix(float radian) {
		Matrix4x4 result{};
		result.m[0][0] = cosf(radian);
		result.m[0][1] = sinf(radian);
		result.m[0][2] = 0.0f;
		result.m[0][3] = 0.0f;
		result.m[1][0] = -(sinf(radian));
		result.m[1][1] = cosf(radian);
		result.m[1][2] = 0.0f;
		result.m[1][3] = 0.0f;
		result.m[2][0] = 0.0f;
		result.m[2][1] = 0.0f;
		result.m[2][2] = 1.0f;
		result.m[2][3] = 0.0f;
		result.m[3][0] = 0.0f;
		result.m[3][1] = 0.0f;
		result.m[3][2] = 0.0f;
		result.m[3][3] = 1.0f;

		return result;
	}
	Matrix4x4 MakeTranslateMatrix(Vector3 translate) {
		Matrix4x4 result{};

		result.m[0][0] = 1.0f;
		result.m[0][1] = 0.0f;
		result.m[0][2] = 0.0f;
		result.m[0][3] = 0.0f;
		result.m[1][0] = 0.0f;
		result.m[1][1] = 1.0f;
		result.m[1][2] = 0.0f;
		result.m[1][3] = 0.0f;
		result.m[2][0] = 0.0f;
		result.m[2][1] = 0.0f;
		result.m[2][2] = 1.0f;
		result.m[2][3] = 0.0f;
		result.m[3][0] = translate.x;
		result.m[3][1] = translate.y;
		result.m[3][2] = translate.z;
		result.m[3][3] = 1.0f;

		return result;
	}


	Vector3 Normalize(const Vector3& v) {
		Vector3 result;
		result.x = v.x / (float)sqrt((v.x * v.x) + (v.y * v.y) + (v.z * v.z));
		result.y = v.y / (float)sqrt((v.x * v.x) + (v.y * v.y) + (v.z * v.z));
		result.z = v.z / (float)sqrt((v.x * v.x) + (v.y * v.y) + (v.z * v.z));
		return result;
	}


	// AABBの衝突判定
	bool IsCollisionAABB(const AABB& aabb1, const AABB& aabb2) {
		if ((aabb1.min.x <= aabb2.max.x && aabb1.max.x >= aabb2.min.x) && //x軸
			(aabb1.min.y <= aabb2.max.y && aabb1.max.y >= aabb2.min.y) && //y軸
			(aabb1.min.z <= aabb2.max.z && aabb1.max.z >= aabb2.min.z)) { //z軸
			return true;
		}
		return false;
	}


	Vector3 OverAABB(const AABB& aabb1, const AABB& aabb2) {
		Vector3 result;
		float overX1 = aabb2.max.x - aabb1.min.x;
		float overX2 = aabb1.max.x - aabb2.min.x;

		//重なっているのは値が小さいほう
		if (overX1 < overX2) {
			result.x = overX1;
		}
		else {
			result.x = overX2;
		}

		float overY1 = aabb2.max.y - aabb1.min.y;
		float overY2 = aabb1.max.y - aabb2.min.y;

		if (overY1 < overY2) {
			result.y = overY1;
		}
		else {
			result.y = overY2;
		}

		float overZ1 = aabb2.max.z - aabb1.min.z;
		float overZ2 = aabb1.max.z - aabb2.min.z;

		if (overZ1 < overZ2) {
			result.z = overZ1;
		}
		else {
			result.z = overZ2;
		}

		return result;
	}

	void ReturnBack(const AABB& aabb1, const AABB& aabb2, Vector3 position) {

		Vector3 overlap = OverAABB(aabb1, aabb2);


		// 重なりが最小の軸で押し戻しを行う
		if (overlap.x < overlap.y && overlap.x < overlap.z) {
			position.x -= overlap.x;
		}
		else if (overlap.y < overlap.x && overlap.y < overlap.z) {
			position.y -= overlap.y;
			//// 上向きの押し戻しなら着地判定を立てる
			//if (push > 0.0f) {
			//	velocityY = 0.0f;
			//	onGround = true;
			//}
		}
		else if (overlap.z < overlap.x && overlap.z < overlap.y) {
			position.z -= overlap.z;
		}


		//position -= overlap;
	}


#pragma region Affine

	Matrix4x4 Multiply(Matrix4x4 m1, Matrix4x4 m2) {
		Matrix4x4 result{};
		result.m[0][0] = m1.m[0][0] * m2.m[0][0] + m1.m[0][1] * m2.m[1][0] + m1.m[0][2] * m2.m[2][0] + m1.m[0][3] * m2.m[3][0];
		result.m[0][1] = m1.m[0][0] * m2.m[0][1] + m1.m[0][1] * m2.m[1][1] + m1.m[0][2] * m2.m[2][1] + m1.m[0][3] * m2.m[3][1];
		result.m[0][2] = m1.m[0][0] * m2.m[0][2] + m1.m[0][1] * m2.m[1][2] + m1.m[0][2] * m2.m[2][2] + m1.m[0][3] * m2.m[3][2];
		result.m[0][3] = m1.m[0][0] * m2.m[0][3] + m1.m[0][1] * m2.m[1][3] + m1.m[0][2] * m2.m[2][3] + m1.m[0][3] * m2.m[3][3];

		result.m[1][0] = m1.m[1][0] * m2.m[0][0] + m1.m[1][1] * m2.m[1][0] + m1.m[1][2] * m2.m[2][0] + m1.m[1][3] * m2.m[3][0];
		result.m[1][1] = m1.m[1][0] * m2.m[0][1] + m1.m[1][1] * m2.m[1][1] + m1.m[1][2] * m2.m[2][1] + m1.m[1][3] * m2.m[3][1];
		result.m[1][2] = m1.m[1][0] * m2.m[0][2] + m1.m[1][1] * m2.m[1][2] + m1.m[1][2] * m2.m[2][2] + m1.m[1][3] * m2.m[3][2];
		result.m[1][3] = m1.m[1][0] * m2.m[0][3] + m1.m[1][1] * m2.m[1][3] + m1.m[1][2] * m2.m[2][3] + m1.m[1][3] * m2.m[3][3];

		result.m[2][0] = m1.m[2][0] * m2.m[0][0] + m1.m[2][1] * m2.m[1][0] + m1.m[2][2] * m2.m[2][0] + m1.m[2][3] * m2.m[3][0];
		result.m[2][1] = m1.m[2][0] * m2.m[0][1] + m1.m[2][1] * m2.m[1][1] + m1.m[2][2] * m2.m[2][1] + m1.m[2][3] * m2.m[3][1];
		result.m[2][2] = m1.m[2][0] * m2.m[0][2] + m1.m[2][1] * m2.m[1][2] + m1.m[2][2] * m2.m[2][2] + m1.m[2][3] * m2.m[3][2];
		result.m[2][3] = m1.m[2][0] * m2.m[0][3] + m1.m[2][1] * m2.m[1][3] + m1.m[2][2] * m2.m[2][3] + m1.m[2][3] * m2.m[3][3];

		result.m[3][0] = m1.m[3][0] * m2.m[0][0] + m1.m[3][1] * m2.m[1][0] + m1.m[3][2] * m2.m[2][0] + m1.m[3][3] * m2.m[3][0];
		result.m[3][1] = m1.m[3][0] * m2.m[0][1] + m1.m[3][1] * m2.m[1][1] + m1.m[3][2] * m2.m[2][1] + m1.m[3][3] * m2.m[3][1];
		result.m[3][2] = m1.m[3][0] * m2.m[0][2] + m1.m[3][1] * m2.m[1][2] + m1.m[3][2] * m2.m[2][2] + m1.m[3][3] * m2.m[3][2];
		result.m[3][3] = m1.m[3][0] * m2.m[0][3] + m1.m[3][1] * m2.m[1][3] + m1.m[3][2] * m2.m[2][3] + m1.m[3][3] * m2.m[3][3];
		return result;
	}

	Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& translate) {

		//角度に合わせる
		Matrix4x4 resultX = MakeRotateXMatrix(rotate.x * -(float(M_PI) / 180.0f));
		Matrix4x4 resultY = MakeRotateYMatrix(rotate.y * -(float(M_PI) / 180.0f));
		Matrix4x4 resultZ = MakeRotateZMatrix(rotate.z * -(float(M_PI) / 180.0f));

		Matrix4x4 rotateXYZ = Multiply(resultX, Multiply(resultY, resultZ));



		Matrix4x4 result;

		result.m[0][0] = scale.x * rotateXYZ.m[0][0];
		result.m[0][1] = scale.x * rotateXYZ.m[0][1];
		result.m[0][2] = scale.x * rotateXYZ.m[0][2];
		result.m[0][3] = 0.0f;
		result.m[1][0] = scale.y * rotateXYZ.m[1][0];
		result.m[1][1] = scale.y * rotateXYZ.m[1][1];
		result.m[1][2] = scale.y * rotateXYZ.m[1][2];
		result.m[1][3] = 0.0f;
		result.m[2][0] = scale.z * rotateXYZ.m[2][0];
		result.m[2][1] = scale.z * rotateXYZ.m[2][1];
		result.m[2][2] = scale.z * rotateXYZ.m[2][2];
		result.m[2][3] = 0.0f;
		result.m[3][0] = translate.x;
		result.m[3][1] = translate.y;
		result.m[3][2] = translate.z;
		result.m[3][3] = 1.0f;

		return result;
	}


	Matrix4x4 MakeQuaternionRotateMatrix(const Quaternion& quaternion) {
		Matrix4x4 r{};

		r.m[0][0] = (quaternion.w * quaternion.w) + (quaternion.x * quaternion.x) -
			(quaternion.y * quaternion.y) - (quaternion.z * quaternion.z);
		r.m[0][1] = 2 * (quaternion.x * quaternion.y + quaternion.w * quaternion.z);
		r.m[0][2] = 2 * (quaternion.x * quaternion.z - quaternion.w * quaternion.y);
		r.m[0][3] = 0;


		r.m[1][0] = 2 * (quaternion.x * quaternion.y - quaternion.w * quaternion.z);
		r.m[1][1] = (quaternion.w * quaternion.w) - (quaternion.x * quaternion.x) +
			(quaternion.y * quaternion.y) - (quaternion.z * quaternion.z);
		r.m[1][2] = 2 * (quaternion.y * quaternion.z + quaternion.w * quaternion.x);
		r.m[1][3] = 0;


		r.m[2][0] = 2 * (quaternion.x * quaternion.z + quaternion.w * quaternion.y);
		r.m[2][1] = 2 * (quaternion.y * quaternion.z - quaternion.w * quaternion.x);
		r.m[2][2] = (quaternion.w * quaternion.w) - (quaternion.x * quaternion.x) -
			(quaternion.y * quaternion.y) + (quaternion.z * quaternion.z);
		r.m[2][3] = 0;

		r.m[0][3] = 0;
		r.m[1][3] = 0;
		r.m[2][3] = 0;
		r.m[3][3] = 1;

		return r;
	}

	//Quaternionバージョン
	Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Quaternion& rotate, const Vector3& translate) {

		Matrix4x4 rotateXYZ = MakeQuaternionRotateMatrix(rotate);


		Matrix4x4 result;

		result.m[0][0] = scale.x * rotateXYZ.m[0][0];
		result.m[0][1] = scale.x * rotateXYZ.m[0][1];
		result.m[0][2] = scale.x * rotateXYZ.m[0][2];
		result.m[0][3] = 0.0f;
		result.m[1][0] = scale.y * rotateXYZ.m[1][0];
		result.m[1][1] = scale.y * rotateXYZ.m[1][1];
		result.m[1][2] = scale.y * rotateXYZ.m[1][2];
		result.m[1][3] = 0.0f;
		result.m[2][0] = scale.z * rotateXYZ.m[2][0];
		result.m[2][1] = scale.z * rotateXYZ.m[2][1];
		result.m[2][2] = scale.z * rotateXYZ.m[2][2];
		result.m[2][3] = 0.0f;
		result.m[3][0] = translate.x;
		result.m[3][1] = translate.y;
		result.m[3][2] = translate.z;
		result.m[3][3] = 1.0f;

		return result;
	}
#pragma endregion

#pragma region 逆数
	Matrix4x4 Inverse(const Matrix4x4& m) {
		float A = m.m[0][0] * m.m[1][1] * m.m[2][2] * m.m[3][3]
			+ m.m[0][0] * m.m[1][2] * m.m[2][3] * m.m[3][1]
			+ m.m[0][0] * m.m[1][3] * m.m[2][1] * m.m[3][2]
			- m.m[0][0] * m.m[1][3] * m.m[2][2] * m.m[3][1]
			- m.m[0][0] * m.m[1][2] * m.m[2][1] * m.m[3][3]
			- m.m[0][0] * m.m[1][1] * m.m[2][3] * m.m[3][2]
			- m.m[0][1] * m.m[1][0] * m.m[2][2] * m.m[3][3]
			- m.m[0][2] * m.m[1][0] * m.m[2][3] * m.m[3][1]
			- m.m[0][3] * m.m[1][0] * m.m[2][1] * m.m[3][2]
			+ m.m[0][3] * m.m[1][0] * m.m[2][2] * m.m[3][1]
			+ m.m[0][2] * m.m[1][0] * m.m[2][1] * m.m[3][3]
			+ m.m[0][1] * m.m[1][0] * m.m[2][3] * m.m[3][2]
			+ m.m[0][1] * m.m[1][2] * m.m[2][0] * m.m[3][3]
			+ m.m[0][2] * m.m[1][3] * m.m[2][0] * m.m[3][1]
			+ m.m[0][3] * m.m[1][1] * m.m[2][0] * m.m[3][2]
			- m.m[0][3] * m.m[1][2] * m.m[2][0] * m.m[3][1]
			- m.m[0][2] * m.m[1][1] * m.m[2][0] * m.m[3][3]
			- m.m[0][1] * m.m[1][3] * m.m[2][0] * m.m[3][2]
			- m.m[0][1] * m.m[1][2] * m.m[2][3] * m.m[3][0]
			- m.m[0][2] * m.m[1][3] * m.m[2][1] * m.m[3][0]
			- m.m[0][3] * m.m[1][1] * m.m[2][2] * m.m[3][0]
			+ m.m[0][3] * m.m[1][2] * m.m[2][1] * m.m[3][0]
			+ m.m[0][2] * m.m[1][1] * m.m[2][3] * m.m[3][0]
			+ m.m[0][1] * m.m[1][3] * m.m[2][2] * m.m[3][0];


		Matrix4x4 result{};
		result.m[0][0] = (m.m[1][1] * m.m[2][2] * m.m[3][3]
			+ m.m[1][2] * m.m[2][3] * m.m[3][1]
			+ m.m[1][3] * m.m[2][1] * m.m[3][2]
			- m.m[1][3] * m.m[2][2] * m.m[3][1]
			- m.m[1][2] * m.m[2][1] * m.m[3][3]
			- m.m[1][1] * m.m[2][3] * m.m[3][2]) / A;

		result.m[0][1] = (-m.m[0][1] * m.m[2][2] * m.m[3][3]
			- m.m[0][2] * m.m[2][3] * m.m[3][1]
			- m.m[0][3] * m.m[2][1] * m.m[3][2]
			+ m.m[0][3] * m.m[2][2] * m.m[3][1]
			+ m.m[0][2] * m.m[2][1] * m.m[3][3]
			+ m.m[0][1] * m.m[2][3] * m.m[3][2]) / A;

		result.m[0][2] = (m.m[0][1] * m.m[1][2] * m.m[3][3]
			+ m.m[0][2] * m.m[1][3] * m.m[3][1]
			+ m.m[0][3] * m.m[1][1] * m.m[3][2]
			- m.m[0][3] * m.m[1][2] * m.m[3][1]
			- m.m[0][2] * m.m[1][1] * m.m[3][3]
			- m.m[0][1] * m.m[1][3] * m.m[3][2]) / A;

		result.m[0][3] = (-m.m[0][1] * m.m[1][2] * m.m[2][3]
			- m.m[0][2] * m.m[1][3] * m.m[2][1]
			- m.m[0][3] * m.m[1][1] * m.m[2][2]
			+ m.m[0][3] * m.m[1][2] * m.m[2][1]
			+ m.m[0][2] * m.m[1][1] * m.m[2][3]
			+ m.m[0][1] * m.m[1][3] * m.m[2][2]) / A;


		result.m[1][0] = (-m.m[1][0] * m.m[2][2] * m.m[3][3]
			- m.m[1][2] * m.m[2][3] * m.m[3][0]
			- m.m[1][3] * m.m[2][0] * m.m[3][2]
			+ m.m[1][3] * m.m[2][2] * m.m[3][0]
			+ m.m[1][2] * m.m[2][0] * m.m[3][3]
			+ m.m[1][0] * m.m[2][3] * m.m[3][2]) / A;

		result.m[1][1] = (m.m[0][0] * m.m[2][2] * m.m[3][3]
			+ m.m[0][2] * m.m[2][3] * m.m[3][0]
			+ m.m[0][3] * m.m[2][0] * m.m[3][2]
			- m.m[0][3] * m.m[2][2] * m.m[3][0]
			- m.m[0][2] * m.m[2][0] * m.m[3][3]
			- m.m[0][0] * m.m[2][3] * m.m[3][2]) / A;

		result.m[1][2] = (-m.m[0][0] * m.m[1][2] * m.m[3][3]
			- m.m[0][2] * m.m[1][3] * m.m[3][0]
			- m.m[0][3] * m.m[1][0] * m.m[3][2]
			+ m.m[0][3] * m.m[1][2] * m.m[3][0]
			+ m.m[0][2] * m.m[1][0] * m.m[3][3]
			+ m.m[0][0] * m.m[1][3] * m.m[3][2]) / A;

		result.m[1][3] = (m.m[0][0] * m.m[1][2] * m.m[2][3]
			+ m.m[0][2] * m.m[1][3] * m.m[2][0]
			+ m.m[0][3] * m.m[1][0] * m.m[2][2]
			- m.m[0][3] * m.m[1][2] * m.m[2][0]
			- m.m[0][2] * m.m[1][0] * m.m[2][3]
			- m.m[0][0] * m.m[1][3] * m.m[2][2]) / A;


		result.m[2][0] = (m.m[1][0] * m.m[2][1] * m.m[3][3]
			+ m.m[1][1] * m.m[2][3] * m.m[3][0]
			+ m.m[1][3] * m.m[2][0] * m.m[3][1]
			- m.m[1][3] * m.m[2][1] * m.m[3][0]
			- m.m[1][1] * m.m[2][0] * m.m[3][3]
			- m.m[1][0] * m.m[2][3] * m.m[3][1]) / A;

		result.m[2][1] = (-m.m[0][0] * m.m[2][1] * m.m[3][3]
			- m.m[0][1] * m.m[2][3] * m.m[3][0]
			- m.m[0][3] * m.m[2][0] * m.m[3][1]
			+ m.m[0][3] * m.m[2][1] * m.m[3][0]
			+ m.m[0][1] * m.m[2][0] * m.m[3][3]
			+ m.m[0][0] * m.m[2][3] * m.m[3][1]) / A;

		result.m[2][2] = (m.m[0][0] * m.m[1][1] * m.m[3][3]
			+ m.m[0][1] * m.m[1][3] * m.m[3][0]
			+ m.m[0][3] * m.m[1][0] * m.m[3][1]
			- m.m[0][3] * m.m[1][1] * m.m[3][0]
			- m.m[0][1] * m.m[1][0] * m.m[3][3]
			- m.m[0][0] * m.m[1][3] * m.m[3][1]) / A;

		result.m[2][3] = (-m.m[0][0] * m.m[1][1] * m.m[2][3]
			- m.m[0][1] * m.m[1][3] * m.m[2][0]
			- m.m[0][3] * m.m[1][0] * m.m[2][1]
			+ m.m[0][3] * m.m[1][1] * m.m[2][0]
			+ m.m[0][1] * m.m[1][0] * m.m[2][3]
			+ m.m[0][0] * m.m[1][3] * m.m[2][1]) / A;


		result.m[3][0] = (-m.m[1][0] * m.m[2][1] * m.m[3][2]
			- m.m[1][1] * m.m[2][2] * m.m[3][0]
			- m.m[1][2] * m.m[2][0] * m.m[3][1]
			+ m.m[1][2] * m.m[2][1] * m.m[3][0]
			+ m.m[1][1] * m.m[2][0] * m.m[3][2]
			+ m.m[1][0] * m.m[2][2] * m.m[3][1]) / A;

		result.m[3][1] = (m.m[0][0] * m.m[2][1] * m.m[3][2]
			+ m.m[0][1] * m.m[2][2] * m.m[3][0]
			+ m.m[0][2] * m.m[2][0] * m.m[3][1]
			- m.m[0][2] * m.m[2][1] * m.m[3][0]
			- m.m[0][1] * m.m[2][0] * m.m[3][2]
			- m.m[0][0] * m.m[2][2] * m.m[3][1]) / A;

		result.m[3][2] = (-m.m[0][0] * m.m[1][1] * m.m[3][2]
			- m.m[0][1] * m.m[1][2] * m.m[3][0]
			- m.m[0][2] * m.m[1][0] * m.m[3][1]
			+ m.m[0][2] * m.m[1][1] * m.m[3][0]
			+ m.m[0][1] * m.m[1][0] * m.m[3][2]
			+ m.m[0][0] * m.m[1][2] * m.m[3][1]) / A;

		result.m[3][3] = (m.m[0][0] * m.m[1][1] * m.m[2][2]
			+ m.m[0][1] * m.m[1][2] * m.m[2][0]
			+ m.m[0][2] * m.m[1][0] * m.m[2][1]
			- m.m[0][2] * m.m[1][1] * m.m[2][0]
			- m.m[0][1] * m.m[1][0] * m.m[2][2]
			- m.m[0][0] * m.m[1][2] * m.m[2][1]) / A;

		return result;
	}
#pragma endregion

	Matrix4x4 MakePerspectiveFovMatrix(float forY, float aspectRatio, float nearClip, float farClip) {
		Matrix4x4 result;
		float cot = 1 / tanf(forY / 2);

		result.m[0][0] = (1 / aspectRatio) * cot;
		result.m[1][1] = cot;
		result.m[2][2] = farClip / (farClip - nearClip);
		result.m[2][3] = 1.0f;
		result.m[3][2] = (-nearClip * farClip) / (farClip - nearClip);


		result.m[0][1] = 0.0f;
		result.m[0][2] = 0.0f;
		result.m[0][3] = 0.0f;
		result.m[1][0] = 0.0f;
		result.m[1][2] = 0.0f;
		result.m[1][3] = 0.0f;
		result.m[2][0] = 0.0f;
		result.m[2][1] = 0.0f;
		result.m[3][0] = 0.0f;
		result.m[3][1] = 0.0f;
		result.m[3][3] = 0.0f;


		return result;
	}


	Matrix4x4 MakeOrthographicMatrix(float left, float top, float right, float bottom, float nearClip, float farClip) {
		Matrix4x4 result;
		result.m[0][0] = 2 / (right - left);
		result.m[1][1] = 2 / (top - bottom);
		result.m[2][2] = 1 / (farClip - nearClip);

		result.m[3][0] = (left + right) / (left - right);
		result.m[3][1] = (top + bottom) / (bottom - top);
		result.m[3][2] = nearClip / (nearClip - farClip);
		result.m[3][3] = 1.0f;

		result.m[0][1] = 0.0f;
		result.m[0][2] = 0.0f;
		result.m[0][3] = 0.0f;
		result.m[1][0] = 0.0f;
		result.m[1][2] = 0.0f;
		result.m[1][3] = 0.0f;
		result.m[2][0] = 0.0f;
		result.m[2][1] = 0.0f;
		result.m[2][3] = 0.0f;



		return result;
	}

	Vector3 CalculateValue(const AnimationCurve<Vector3>& keyframes, float time) {
		assert(!keyframes.keyframes.empty());
		if (keyframes.keyframes.size() == 1 || time <= keyframes.keyframes[0].time) {
			return keyframes.keyframes[0].value; //アニメーションしない、止まっている時
		}
		for (size_t index = 0; index < keyframes.keyframes.size() - 1; ++index) {
			size_t nextIndex = index + 1;
			//jointが動いていたら
			if (keyframes.keyframes[index].time <= time && time <= keyframes.keyframes[nextIndex].time) {
				float t = (time - keyframes.keyframes[index].time) / (keyframes.keyframes[nextIndex].time - keyframes.keyframes[index].time);
				return Lerp(keyframes.keyframes[index].value, keyframes.keyframes[nextIndex].value, t);
			}
		}
		return (*keyframes.keyframes.rbegin()).value;
	}	

	Quaternion CalculateValueQuaternion(const AnimationCurve<Quaternion>& keyframes, float time) {
		assert(!keyframes.keyframes.empty());
		if (keyframes.keyframes.size() == 1 || time <= keyframes.keyframes[0].time) {
			return { keyframes.keyframes[0].value.x,keyframes.keyframes[0].value.y ,keyframes.keyframes[0].value.z,keyframes.keyframes[0].value.w };
		}
		for (size_t index = 0; index < keyframes.keyframes.size() - 1; ++index) {
			size_t nextIndex = index + 1;
			//jointが動いていたら
			if (keyframes.keyframes[index].time <= time && time <= keyframes.keyframes[nextIndex].time) {
				float t = (time - keyframes.keyframes[index].time) / (keyframes.keyframes[nextIndex].time - keyframes.keyframes[index].time);

				return Lerp(keyframes.keyframes[index].value, keyframes.keyframes[nextIndex].value, t);
			}
		}

		return (*keyframes.keyframes.rbegin()).value;
	}


	Vector3 InterpolationValue(const AnimationCurve<Vector3>& key1, const AnimationCurve<Vector3>& key2, float time) {
		assert(!key1.keyframes.empty() || !key2.keyframes.empty());
		if (key1.keyframes.size() == 1 || time <= key1.keyframes[0].time ||
			key2.keyframes.size() == 1 || time <= key2.keyframes[0].time) {
			return { key2.keyframes[0].value };
		}

		Vector3 animation1, animation2;

		for (size_t index = 0; index < key1.keyframes.size() - 1; ++index) {
			size_t nextIndex = index + 1;
			//jointが動いていたら
			if (key1.keyframes[index].time <= time && time <= key1.keyframes[nextIndex].time) {
				float t = (time - key1.keyframes[index].time) / (key1.keyframes[nextIndex].time - key1.keyframes[index].time);
				animation1 =  Lerp(key1.keyframes[index].value, key1.keyframes[nextIndex].value, t);
			}
		}


		for (size_t index = 0; index < key2.keyframes.size() - 1; ++index) {
			size_t nextIndex = index + 1;
			//jointが動いていたら
			if (key2.keyframes[index].time <= time && time <= key2.keyframes[nextIndex].time) {
				float t = (time - key2.keyframes[index].time) / (key2.keyframes[nextIndex].time - key2.keyframes[index].time);
				animation2 = Lerp(key2.keyframes[index].value, key2.keyframes[nextIndex].value, t);
			}
		}

		return Lerp(animation1 ,animation2, time);
	}

	Quaternion InterpolationValueQuaternion(const AnimationCurve<Quaternion>& key1, const AnimationCurve<Quaternion>& key2, float time) {
		assert(!key1.keyframes.empty() || !key2.keyframes.empty());
		if (key1.keyframes.size() == 1 || time <= key1.keyframes[0].time ||
			key2.keyframes.size() == 1 || time <= key2.keyframes[0].time) {
			return { key1.keyframes[0].value };
		}

		Quaternion animation1, animation2;

		for (size_t index = 0; index < key1.keyframes.size() - 1; ++index) {
			size_t nextIndex = index + 1;
			if (key1.keyframes[index].time <= time && time <= key1.keyframes[nextIndex].time) {
				float t = (time - key1.keyframes[index].time) / (key1.keyframes[nextIndex].time - key1.keyframes[index].time);
				animation1 = Lerp(key1.keyframes[index].value, key1.keyframes[nextIndex].value, t);
			}
		}


		for (size_t index = 0; index < key2.keyframes.size() - 1; ++index) {
			size_t nextIndex = index + 1;
			if (key2.keyframes[index].time <= time && time <= key2.keyframes[nextIndex].time) {
				float t = (time - key2.keyframes[index].time) / (key2.keyframes[nextIndex].time - key2.keyframes[index].time);
				animation2 = Lerp(key2.keyframes[index].value, key2.keyframes[nextIndex].value, t);
			}
		}

		return Slerp(animation1, animation2, time);
	}


	Vector3 Lerp(const Vector3& p0, const Vector3& p1, float t) {

		Vector3 a = { t * p0.x ,t * p0.y ,t * p0.z };
		Vector3 b = { (1.0f - t) * p1.x,(1.0f - t) * p1.y,(1.0f - t) * p1.z };
		Vector3	c = { a.x + b.x,a.y + b.y,a.z + b.z };

		return c;
	}

	Quaternion Lerp(const Quaternion& p0, const Quaternion& p1, float t) {

		Vector3 a = { t * p0.x ,t * p0.y ,t * p0.z };
		Vector3 b = { (1.0f - t) * p1.x,(1.0f - t) * p1.y,(1.0f - t) * p1.z };
		Vector3	c = { a.x + b.x,a.y + b.y,a.z + b.z };

		Quaternion result = { c.x,c.y,c.z,p0.w };

		return result;
	}

	Skeleton CreateSkeltion(const Node& rootNode) {
		Skeleton skeleton;
		skeleton.root = CreateJoint(rootNode, {},skeleton.joints);

		for (const Joint& joint : skeleton.joints) {
			skeleton.jointMap.emplace(joint.name, joint.index);
		}
		return skeleton;
	}

	int32_t CreateJoint(const Node& node, const std::optional<int32_t>& parent, std::vector<Joint>& joints) {
		Joint joint;
		joint.name = node.name;
		joint.localMatrix = node.localMatrix;
		joint.skeletonSpaceMatrix = MakeIdentity4x4();
		joint.transform = node.transform;
		joint.index = int32_t(joints.size());
		joint.parent = parent;
		joints.push_back(joint);

		//再帰関数
		for (const Node& child : node.children) {
			int32_t childIndex = CreateJoint(child, joint.index, joints);
			joints[joint.index].children.push_back(childIndex);
		}

		return joint.index;
	}

	Quaternion operator-(const Quaternion& q) {
		Quaternion result;
		result.x = -q.x;
		result.y = -q.y;
		result.z = -q.z;
		result.w = -q.w;
		return result;
	}

	Quaternion operator*(const float f, const Quaternion& q) {
		Quaternion result;
		result.x = f * q.x;
		result.y = f * q.y;
		result.z = f * q.z;
		result.w = f * q.w;
		return result;
	}

	Quaternion operator+(const Quaternion& q0, const Quaternion& q1) {
		Quaternion result;
		result.x = q0.x + q1.x;
		result.y = q0.y + q1.y;
		result.z = q0.z + q1.z;
		result.w = q0.w + q1.w;
		return result;
	}

	float MulctyQuaternion(const Quaternion& q1, const Quaternion& q2) {
		Quaternion r;

		r.x = q1.x * q2.x;
		r.y = q1.y * q2.y;
		r.z = q1.z * q2.z;
		r.w = q1.w * q2.w;

		float result;

		result = r.x + r.y + r.z + r.w;

		return result;
	}

	Quaternion Slerp(const Quaternion& q0, const Quaternion& q1, float t) {

		Quaternion q3 = q0;

		float dot = MulctyQuaternion(q0, q1);

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


	Matrix4x4 Transpose(Matrix4x4 m) {
		Matrix4x4 result{};

		result.m[0][0] = m.m[0][0];
		result.m[0][1] = m.m[1][0];
		result.m[0][2] = m.m[2][0];
		result.m[0][3] = m.m[3][0];
		result.m[1][0] = m.m[0][1];
		result.m[1][1] = m.m[1][1];
		result.m[1][2] = m.m[2][1];
		result.m[1][3] = m.m[3][1];
		result.m[2][0] = m.m[0][2];
		result.m[2][1] = m.m[1][2];
		result.m[2][2] = m.m[2][2];
		result.m[2][3] = m.m[3][2];
		result.m[3][0] = m.m[0][3];
		result.m[3][1] = m.m[1][3];
		result.m[3][2] = m.m[2][3];
		result.m[3][3] = m.m[3][3];

		return result;
	}

	VertexData AddVert(const VertexData& v1, const VertexData& v2) {
		VertexData result{};

		result.position.x = v1.position.x + v2.position.x;
		result.position.y = v1.position.y + v2.position.y;
		result.position.z = v1.position.z + v2.position.z;
		result.position.s = v1.position.s + v2.position.s;
		result.texcoord.x = v1.texcoord.x + v2.texcoord.x;
		result.texcoord.y = v1.texcoord.y + v2.texcoord.y;
		return result;
	}

}
