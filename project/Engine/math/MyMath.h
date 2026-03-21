#pragma once

#include"Vector2.h"
#include"Vector3.h"
#include"Vector4.h"
#include"Matrix4x4.h"
#include"Transform.h"

#define _USE_MATH_DEFINES
#include <math.h>

#include <vector>
#include <string>
#include <map>

#include <optional>
#include <span>
#include <array>

#include <wrl/client.h>
#include <d3d12.h>

#include <numbers>
#include <cassert>
#include <cmath>

/// <summary>
/// 構造体_ModelData
/// </summary>
struct MaterialData {
	std::string textureFilePath;
	uint32_t textureIndex;
	Matrix4x4 uvTransform;
	Vector4 materialColor;
};

/// <summary>
/// 構造体‗VertexData
/// </summary>
struct VertexData {
	Vector4 position;
	Vector2 texcoord;
	Vector3 normal;
};

/// <summary>
/// 構造体_クオータニオン
/// </summary>
struct Quaternion {
	float x;
	float y;
	float z;
	float w;
};

struct EulerTransform {
	Vector3 scale;
	Vector3 rotate;//Eulerでの回転
	Vector3 translate;
};

struct QuaternionTransform {
	Vector3 scale;
	Quaternion rotate;
	Vector3 translate;
};

struct Node {
	QuaternionTransform transform;
	Matrix4x4 localMatrix;
	std::string name;
	std::vector<Node> children;
};

struct Joint {
	QuaternionTransform transform; //transform
	Matrix4x4 localMatrix;//localMatrix
	Matrix4x4 skeletonSpaceMatrix;//スケルトン
	std::string name;//名前
	std::vector<int32_t> children;//子JointのIndexリスト
	int32_t index;//自身のIndex
	std::optional<int32_t> parent;//親JointのIndexリスト
};

struct VertexWeightData {
	float weight;
	uint32_t vertexIndex;
};

struct JointWeightData {
	Matrix4x4 inverseBindPoseMatrix;
	std::vector<VertexWeightData> vertexWeights;
};

const uint32_t kNumMaxInfluence = 4;
struct VertexInfluence {
	std::array<float, kNumMaxInfluence> weights;
	std::array<int32_t, kNumMaxInfluence> jointIndices;
};

struct ModelData {
	std::vector<VertexData> vertices;
	std::vector<uint32_t> indices;
	MaterialData materialData;
};

struct ModelDataMulti {
	std::vector<ModelData> Data;
	std::map<std::string, JointWeightData> skinClusterData;
	Node rootNode;
};

struct Material {
	Vector4 color;
	bool enableLighting;
	float padding[3];
	Matrix4x4 uvTransform;
	float shininess;
	float environmentCoefficient;
};

template<typename tValue>
struct keyframe {
	tValue value; //キーフレームの値(Vector3/Quaternion)
	float time;   //時刻(秒)
};

using keyframeVector3 = keyframe<Vector3>;      //トランスフォームとスケーリング用
using keyframeQuatarnion = keyframe<Quaternion>;//回転用

template<typename tValue>
struct AnimationCurve {
	std::vector<keyframe<tValue>> keyframes;
};

struct NodeAnimation {
	AnimationCurve<Vector3> translate;
	AnimationCurve<Quaternion> rotate;
	AnimationCurve<Vector3> scale;
};

struct Animation {
	float duration; //アニメーション全体の尺
	std::map<std::string, NodeAnimation> nodeAnimations;
};

struct DirectionalLight {
	Vector4 color;
	Vector3 direction;
	float intensity;
};

struct CameraForGPU {
	Vector3 worldPosition;
};

struct PointLight {
	Vector4 color;
	Vector3 position;
	float intensity;
	float radius;
	float decay;
	float padding[2];
};

struct SpotLight {
	Vector4 color;
	Vector3 position;
	float intensity;
	Vector3 direction;
	float distance;
	float decay;
	float cosAngle;
	float cosFalloffStart;
	float padding[2];
};

struct TransformationMatrix {
	Matrix4x4 WVP;
	Matrix4x4 World;
};

struct AABB {
	Vector3 min;
	Vector3 max;
};

struct Segment {
	Vector3 origin;
	Vector3 diff;
};

namespace MyMath {

	Vector2 operator+(const Vector2& v1, const Vector2& v2);
	Vector2 operator-(const Vector2& v1, const Vector2& v2);
	Vector2 operator*(const Vector2& v1, const Vector2& v2);
	Vector2 operator/(const Vector2& v1, const Vector2& v2);

	Vector2 operator*(const Vector2& v, float f);
	Vector2 operator*(float f, const Vector2& v);

	Vector2 operator/(const Vector2& v, float f);
	Vector2 operator/(float f, const Vector2& v);

	Vector2& operator+=(Vector2& v1, const Vector2& v2);
	Vector2& operator-=(Vector2& v1, const Vector2& v2);
	Vector2& operator*=(Vector2& v1, const Vector2& v2);
	Vector2& operator/=(Vector2& v1, const Vector2& v2);


	Vector3 operator+(const Vector3& v1, const Vector3& v2);
	Vector3 operator-(const Vector3& v1, const Vector3& v2);
	Vector3 operator*(const Vector3& v1, const Vector3& v2);
	Vector3 operator/(const Vector3& v1, const Vector3& v2);

	Vector3 operator+(const Vector3& v, float f);
	Vector3 operator+(float f, const Vector3& v);
	
	Vector3 operator-(const Vector3& v, float f);
	Vector3 operator-(float f, const Vector3& v);

	Vector3 operator*(const Vector3& v, float f);
	Vector3 operator*(float f, const Vector3& v);

	Vector3 operator/(const Vector3& v, float f);
	Vector3 operator/(float f, const Vector3& v);

	Vector3& operator+=(Vector3& v1, const Vector3& v2);
	Vector3& operator-=(Vector3& v1, const Vector3& v2);
	Vector3& operator*=(Vector3& v1, const Vector3& v2);
	Vector3& operator/=(Vector3& v1, const Vector3& v2);

	Vector3& operator+=(Vector3& v, float f);
	Vector3& operator-=(Vector3& v, float f);
	Vector3& operator*=(Vector3& v, float f);
	Vector3& operator/=(Vector3& v, float f);

	bool operator==(const Vector3& v1, const Vector3& v2);
	bool operator!=(const Vector3& v1, const Vector3& v2);
	bool operator<=(const Vector3& v1, const Vector3& v2);
	bool operator>=(const Vector3& v1,const Vector3& v2);

	bool operator<(const Vector3& v1, const Vector3& v2);
	bool operator>(const Vector3& v1, const Vector3& v2);

	
	Vector3 operator-(const Vector3& v);


	bool operator==(const Transform& a, const Transform& b);
	bool operator!=(const Transform& a, const Transform& b);

	Matrix4x4 operator*(const Matrix4x4& m1, const Matrix4x4& m2);

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
	/// 法線(normal)
	/// </summary>
	/// <param name="v">Vector3</param>
	/// <returns>現在の法線</returns>
	Vector3 Normalize(const Vector3& v);

	/// <summary>
	/// AABBの衝突判定
	/// </summary>
	/// <param name="aabb1">当たり判定1</param>
	/// <param name="aabb2">当たり判定2</param>
	/// <returns>当たっている場合true</returns>
	bool IsCollisionAABB(const AABB& aabb1, const AABB& aabb2);

	/// <summary>
	/// AABBの衝突判定(内側) 
	/// </summary>
	/// <param name="aabb1">範囲から出られなくするターゲットの当たり判定</param>
	/// <param name="aabb2">当たり判定(範囲の大きさ)</param>
	/// <returns>外側の場合true(外側に出ようとしている)</returns>
	bool IsCollisionAABB_outSide(const AABB& aabb1, const AABB& aabb2);

	/// <summary>
	/// 当たり判定の重なり(ステージの当たり判定で使う)
	/// </summary>
	/// <param name="aabb1">当たり判定1</param>
	/// <param name="aabb2">当たり判定2</param>
	/// <returns>重なりの小さいほう</returns>
	Vector3 OverAABB(const AABB& aabb1, const AABB& aabb2);

	/// <summary>
	/// 法線ありの座標
	/// </summary>
	/// <param name="v">現在の座標</param>
	/// <param name="m">使用する行列</param>
	/// <returns>法線の方向の座標</returns>
	Vector3 TransformNormal(const Vector3& v, const Matrix4x4& m);

	/// <summary>
	/// イーズイン_float
	/// </summary>
	/// <param name="startPoint">スタート位置</param>
	/// <param name="endPoint">目的(移動させたい)位置</param>
	/// <param name="t">補間(0~1)</param>
	/// <returns></returns>
	float EaseIn(float startPoint, float endPoint, float t);
	/// <summary>
	/// イーズイン_Vector2
	/// </summary>
	/// <param name="startPoint">スタート位置</param>
	/// <param name="endPoint">目的(移動させたい)位置</param>
	/// <param name="t">補間(0~1)</param>
	/// <returns></returns>
	Vector2 EaseIn(const Vector2& startPoint, const Vector2& endPoint, float t);
	/// <summary>
	/// イーズイン_Vector3
	/// </summary>
	/// <param name="startPoint">スタート位置</param>
	/// <param name="endPoint">目的(移動させたい)位置</param>
	/// <param name="t">補間(0~1)</param>
	/// <returns></returns>
	Vector3 EaseIn(const Vector3& startPoint, const Vector3& endPoint, float t);

	/// <summary>
	/// イーズアウト_float
	/// </summary>
	/// <param name="startPoint">スタート位置</param>
	/// <param name="endPoint">目的(移動させたい)位置</param>
	/// <param name="t">補間(0~1)</param>
	/// <returns></returns>
	float EaseOut(float startPoint, float endPoint, float t);
	/// <summary>
	/// イーズアウト_Vector2
	/// </summary>
	/// <param name="startPoint">スタート位置</param>
	/// <param name="endPoint">目的(移動させたい)位置</param>
	/// <param name="t">補間(0~1)</param>
	/// <returns></returns>
	Vector2 EaseOut(const Vector2& startPoint, const Vector2& endPoint, float t);	
	/// <summary>
	/// イーズアウト_Vector3
	/// </summary>
	/// <param name="startPoint">スタート位置</param>
	/// <param name="endPoint">目的(移動させたい)位置</param>
	/// <param name="t">補間(0~1)</param>
	/// <returns></returns>
	Vector3 EaseOut(const Vector3& startPoint, const Vector3& endPoint, float t);

	/// <summary>
	/// 長さ
	/// </summary>
	/// <param name="start">現在位置</param>
	/// <param name="target">ターゲット</param>
	/// <returns>スタートとターゲットの長さ</returns>
	float Length(float start, float target);
	/// <summary>
	/// 長さ_Vector3版
	/// </summary>
	/// <param name="start">現在位置</param>
	/// <param name="target">ターゲット</param>
	/// <returns>スタートとターゲットの長さ</returns>
	Vector3 Length(const Vector3& start, const Vector3& target);
	
	/// <summary>
	/// 距離の補間
	/// </summary>
	/// <param name="segment">セグメント</param>
	/// <returns>現在位置(origin)と目的地(diff)の距離、マイナスも含む</returns>
	Vector3 GoDestination(const Segment& segment);
	/// <summary>
	/// 距離の補間(分離版)
	/// </summary>
	/// <param name="origin">現在地</param>
	/// <param name="diff">目的地</param>
	/// <returns>現在位置(origin)と目的地(diff)の距離、マイナスも含む</returns>
	Vector3 GoDestination(const Vector3& origin, const Vector3& diff);

#pragma region Affine

	/// <summary>
	/// 行列の掛け算
	/// </summary>
	/// <param name="m1">行列1</param>
	/// <param name="m2">行列2</param>
	/// <returns>かけ合わせた行列</returns>
	Matrix4x4 Multiply(const Matrix4x4& m1, const Matrix4x4& m2);

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

	Matrix4x4 MakePerspectiveFovMatrix(float fovY, float aspectRatio, float nearClip, float farClip);

	Matrix4x4 MakeOrthographicMatrix(float left, float top, float right, float bottom, float nearClip, float farClip);


	Vector3 CalculateValue(const AnimationCurve<Vector3>& keyframes, float time);
	Quaternion CalculateValueQuaternion(const AnimationCurve<Quaternion>& keyframes, float time);

	Vector3 InterpolationValue(const AnimationCurve<Vector3>& key1, const AnimationCurve<Vector3>& key2, float time);
	Quaternion InterpolationValueQuaternion(const AnimationCurve<Quaternion>& key1, const AnimationCurve<Quaternion>& key2, float time);

	/// <summary>
	/// 補間
	/// </summary>
	/// <param name="p0">ポイント1(始点)</param>
	/// <param name="p1">ポイント2(終点)</param>
	/// <param name="t">補間単位</param>
	/// <returns>保管した座標</returns>
	Vector3 Lerp(const Vector3& p0, const Vector3& p1, float t);

	Quaternion operator-(const Quaternion& q);
	Quaternion operator*(float f, const Quaternion& q);
	Quaternion operator+(const Quaternion& q0, const Quaternion& q1);

	float MultiplyQuaternion(const Quaternion& q1, const Quaternion& q2);
	/// <summary>
	/// 補間_SLerp
	/// </summary>
	/// <param name="q0">制御点0</param>
	/// <param name="q1">制御点1</param>
	/// <param name="t">割合</param>
	/// <returns>現在の位置</returns>
	Quaternion SLerp(const Quaternion& q0, const Quaternion& q1, float t);

	/// <summary>
	/// 反転
	/// </summary>
	/// <param name="m">変更する行列</param>
	/// <returns>反転した行列</returns>
	Matrix4x4 Transpose(const Matrix4x4& m);

	/// <summary>
	/// 目線の当たり判定(ステージを貫通し、見えた判定にならないように)
	/// </summary>
	/// <param name="aabb">当たり判定</param>
	/// <param name="segment">自分の位置(origin)とターゲットの位置(diff)</param>
	/// <returns>セグメント間にAABBがいないか(true/false)</returns>
	bool IsCollisionAABB_Segment(const AABB& aabb, const Segment& segment);

	/// <summary>
	/// 線に当たったか
	/// </summary>
	/// <param name="a1">現在の位置</param>
	/// <param name="a2">ターゲットの位置</param>
	/// <param name="b1"></param>
	/// <param name="b2"></param>
	/// <returns></returns>
	bool Intersect(const Vector2& a1, const Vector2& a2,const Vector2& b1, const Vector2& b2);
}
