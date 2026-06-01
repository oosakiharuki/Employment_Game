#pragma once
#include "Matrix4x4.h"
#include "Vector3.h"
#include "Quaternion.h"

namespace MyMath {

	/// <summary>
	///初期行列
	/// </summary>
	/// <returns>
	/// [1,0,0,0]
	/// [0,1,0,0]
	/// [0,0,1,0]
	/// [0,0,0,1]
	/// </returns>
	Matrix4x4 MakeIdentity4x4();

	/// <summary>
	/// スケール行列
	/// </summary>
	/// <param name="scale">大きさ</param>
	/// <returns>
	/// [x,0,0,0]
	/// [0,y,0,0]
	/// [0,0,z,0]
	/// [0,0,0,1]
	/// </returns>
	Matrix4x4 MakeScaleMatrix(const Vector3& scale);

	/// <summary>
	/// 回転行列_X軸
	/// </summary>
	/// <param name="radian">x軸の回転</param>
	/// <returns>
	/// [1,cos(radian),sin(radian),0]
	/// [0,-sin(radian),cos(radian),0]
	/// [0,0,1,0]
	/// [0,0,0,1]
	/// </returns>
	Matrix4x4 MakeRotateXMatrix(float radian);

	/// <summary>
	/// 回転行列_Y軸
	/// </summary>
	/// <param name="radian">y軸の回転</param>
	/// <returns>
	/// [cos(radian),0,-sin(radian),0]
	/// [0,1,0,0]
	/// [sin(radian),0,cos(radian),0]
	/// [0,0,0,1]
	/// </returns>
	Matrix4x4 MakeRotateYMatrix(float radian);

	/// <summary>
	/// 回転行列_Z軸
	/// </summary>
	/// <param name="radian">z軸の回転</param>
	/// <returns>
	/// [cos(radian),sin(radian),0,0]
	/// [-sin(radian),cons(radian),0,0]
	/// [0,0,1,0]
	/// [0,0,0,1]
	/// </returns>
	Matrix4x4 MakeRotateZMatrix(float radian);

	/// <summary>
	/// 座標行列
	/// </summary>
	/// <param name="translate">現在座標</param>
	/// <returns>
	/// [1,0,0,0]
	/// [0,1,0,0]
	/// [0,0,1,0]
	/// [x,y,z,1]
	/// </returns>
	Matrix4x4 MakeTranslateMatrix(const Vector3& translate);


	/// <summary>
	/// 行列の掛け算
	/// </summary>
	/// <param name="m1">行列1</param>
	/// <param name="m2">行列2</param>
	/// <returns>かけ合わせた行列</returns>
	Matrix4x4 Multiply(const Matrix4x4& m1, const Matrix4x4& m2);

	/// <summary>
	/// 演算子オーバーロード　「行列の掛け算」
	/// </summary>
	/// <param name="m1">行列1</param>
	/// <param name="m2">行列2</param>
	/// <returns>かけ合わせた行列</returns>
	Matrix4x4 operator*(const Matrix4x4& m1, const Matrix4x4& m2);


#pragma region Affine

	/// <summary>
	/// アフィン行列を作成
	/// </summary>
	/// <param name="scale">スケール</param>
	/// <param name="rotate">回転</param>
	/// <param name="translate">座標</param>
	/// <returns>作成されたアフィン行列</returns>
	Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& translate);

	//Quaternion.ver

	/// <summary>
	/// クオータニオンの回転行列
	/// </summary>
	/// <param name="quaternion">クオータニオン</param>
	/// <returns>回転行列</returns>
	Matrix4x4 MakeQuaternionRotateMatrix(const Quaternion& quaternion);
	/// <summary>
	/// アフィン行列を作成(クオータニオン版)
	/// </summary>
	/// <param name="scale">スケール</param>
	/// <param name="rotate">クオータニオンで生成した回転</param>
	/// <param name="translate">座標</param>
	/// <returns>作成されたアフィン行列</returns>
	Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Quaternion& rotate, const Vector3& translate);
#pragma endregion

#pragma region 逆数
	/// <summary>
	/// 逆数
	/// </summary>
	/// <param name="m">行列</param>
	/// <returns>逆数に変更された行列</returns>
	Matrix4x4 Inverse(const Matrix4x4& m);
#pragma endregion


	/// <summary>
	/// 反転
	/// </summary>
	/// <param name="m">変更する行列</param>
	/// <returns>反転した行列</returns>
	Matrix4x4 Transpose(const Matrix4x4& m);

	/// <summary>
	/// 透視投影行列
	/// </summary>
	/// <param name="fovY"></param>
	/// <param name="aspectRatio">アスペクト比</param>
	/// <param name="nearClip">近距離</param>
	/// <param name="farClip">遠距離</param>
	/// <returns></returns>
	Matrix4x4 MakePerspectiveFovMatrix(float fovY, float aspectRatio, float nearClip, float farClip);

	/// <summary>
	/// 生射投影行列
	/// </summary>
	/// <param name="left">左</param>
	/// <param name="top">上</param>
	/// <param name="right">右</param>
	/// <param name="bottom">下</param>
	/// <param name="nearClip">手前</param>
	/// <param name="farClip">奥</param>
	/// <returns></returns>
	Matrix4x4 MakeOrthographicMatrix(float left, float top, float right, float bottom, float nearClip, float farClip);


	/// <summary>
	/// 法線ありの座標
	/// </summary>
	/// <param name="v">現在の座標</param>
	/// <param name="m">使用する行列</param>
	/// <returns>法線の方向の座標</returns>
	Vector3 TransformNormal(const Vector3& v, const Matrix4x4& m);

}

