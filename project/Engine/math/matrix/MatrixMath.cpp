#include "MatrixMath.h"
#define _USE_MATH_DEFINES
#include <math.h>

namespace MyMath{

	Matrix4x4 operator*(const Matrix4x4& m1, const Matrix4x4& m2) {
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

	Matrix4x4 MakeScaleMatrix(const Vector3& scale) {
		Matrix4x4 result = MakeIdentity4x4();
		result.m[0][0] = scale.x;
		result.m[1][1] = scale.y;
		result.m[2][2] = scale.z;
		return result;
	}
	Matrix4x4 MakeRotateXMatrix(float radian) {
		Matrix4x4 result = MakeIdentity4x4();
		result.m[1][1] = cosf(radian);
		result.m[1][2] = sinf(radian);
		result.m[2][1] = -(sinf(radian));
		result.m[2][2] = cosf(radian);
		return result;
	}
	Matrix4x4 MakeRotateYMatrix(float radian) {
		Matrix4x4 result = MakeIdentity4x4();
		result.m[0][0] = cosf(radian);
		result.m[0][2] = -(sinf(radian));
		result.m[2][0] = sinf(radian);
		result.m[2][2] = cosf(radian);
		return result;
	}
	Matrix4x4 MakeRotateZMatrix(float radian) {
		Matrix4x4 result = MakeIdentity4x4();
		result.m[0][0] = cosf(radian);
		result.m[0][1] = sinf(radian);
		result.m[1][0] = -(sinf(radian));
		result.m[1][1] = cosf(radian);
		return result;
	}
	Matrix4x4 MakeTranslateMatrix(const Vector3& translate) {
		Matrix4x4 result = MakeIdentity4x4();
		result.m[3][0] = translate.x;
		result.m[3][1] = translate.y;
		result.m[3][2] = translate.z;
		return result;
	}



#pragma region Affine

	Matrix4x4 Multiply(const Matrix4x4& m1, const Matrix4x4& m2) {
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
		Matrix4x4 resultX = MakeRotateXMatrix(rotate.x * (float(M_PI) / 180.0f));
		Matrix4x4 resultY = MakeRotateYMatrix(rotate.y * (float(M_PI) / 180.0f));
		Matrix4x4 resultZ = MakeRotateZMatrix(rotate.z * (float(M_PI) / 180.0f));

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

	Matrix4x4 MakePerspectiveFovMatrix(float fovY, float aspectRatio, float nearClip, float farClip) {
		Matrix4x4 result;
		float cot = 1 / tanf(fovY / 2);

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

	Matrix4x4 Transpose(const Matrix4x4& m) {
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


	Vector3 TransformNormal(const Vector3& v, const Matrix4x4& m) {
		Vector3 result;
		result.x = v.x * m.m[0][0] + v.y * m.m[1][0] + v.z * m.m[2][0];
		result.y = v.x * m.m[0][1] + v.y * m.m[1][1] + v.z * m.m[2][1];
		result.z = v.x * m.m[0][2] + v.y * m.m[1][2] + v.z * m.m[2][2];
		return result;
	}
}
