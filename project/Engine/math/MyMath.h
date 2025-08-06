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

//model
struct MaterialData {
	std::string textureFilePath;
	uint32_t textureIndex;
};

struct VertexData {
	Vector4 position;
	Vector2 texcoord;
	Vector3 normal;
};

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

struct Skeleton {
	int32_t root;//RootJointのIndex
	std::map <std::string, int32_t>jointMap;//コンテナ
	std::vector<Joint> joints;//所属ジョイント
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

struct WellForGPU {
	Matrix4x4 skeletonSpaceMatrix; //位置用
	Matrix4x4 skeletonSpaceInverseTransposeMatrix; //法線用
};

//std::spanは「配列の上限がない」みたいなやつ（決めることもできる）

struct SkinCluster {
	std::vector<Matrix4x4> inverseBindPoseMatrices;
	Microsoft::WRL::ComPtr<ID3D12Resource> influenceResource;
	D3D12_VERTEX_BUFFER_VIEW influenceBufferView;
	std::span<VertexInfluence> mappedInfluence;
	Microsoft::WRL::ComPtr<ID3D12Resource> paletteResource;
	std::span<WellForGPU> mappedPalette;
	std::pair<D3D12_CPU_DESCRIPTOR_HANDLE, D3D12_GPU_DESCRIPTOR_HANDLE> paletteSrvHandle;
};

struct ModelData {
	std::vector<VertexData> vertices;
	MaterialData material;
};

struct ModelData_glTF {
	std::map<std::string, JointWeightData> skinClusterDeta;
	std::vector<VertexData> vertices;
	std::vector<uint32_t> indices;
	MaterialData material;
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
	tValue value; //キーフレームの値(Vector3/Quatanrion)
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

namespace MyMath {

	Vector3 operator+(const Vector3& v1, const Vector3& v2);
	Vector3 operator-(const Vector3& v1, const Vector3& v2);
	Vector3 operator*(const Vector3& v1, const Vector3& v2);
	Vector3 operator/(const Vector3& v1, const Vector3& v2);

	Vector3 operator*(const Vector3& v, const float f);
	Vector3 operator*(const float f, const Vector3& v);

	Vector3 operator/(const Vector3& v, const float f);
	Vector3 operator/(const float f, const Vector3& v);

	Vector3& operator+=(Vector3& v1, const Vector3& v2);
	Vector3& operator-=(Vector3& v1, const Vector3& v2);
	Vector3& operator*=(Vector3& v1, const Vector3& v2);
	Vector3& operator/=(Vector3& v1, const Vector3& v2);

	Matrix4x4 operator*(const Matrix4x4& m1, const Matrix4x4& m2);

	Matrix4x4 MakeIdentity4x4();
	Matrix4x4 MakeScaleMatrix(Vector3 scale);
	Matrix4x4 MakeRotateXMatrix(float radian);
	Matrix4x4 MakeRotateYMatrix(float radian);
	Matrix4x4 MakeRotateZMatrix(float radian);
	Matrix4x4 MakeTranslateMatrix(Vector3 translate);


	Vector3 Normalize(const Vector3& v);
	bool IsCollisionAABB(const AABB& aabb1, const AABB& aabb2);

	Vector3 OverAABB(const AABB& aabb1, const AABB& aabb2);
	void ReturnBack(const AABB& aabb1, const AABB& aabb2, Vector3 position);

#pragma region Affine

	Matrix4x4 Multiply(Matrix4x4 m1, Matrix4x4 m2);

	Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& translate);
	
	//Quaternion.ver
	Matrix4x4 MakeQuaternionRotateMatrix(const Quaternion& quaternion);
	Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Quaternion& rotate, const Vector3& translate);
#pragma endregion

#pragma region 逆数
	Matrix4x4 Inverse(const Matrix4x4& m);
#pragma endregion

	Matrix4x4 MakePerspectiveFovMatrix(float forY, float aspectRatio, float nearClip, float farClip);

	Matrix4x4 MakeOrthographicMatrix(float left, float top, float right, float bottom, float nearClip, float farClip);


	Vector3 CalculateValue(const AnimationCurve<Vector3>& keyframes, float time);
	Quaternion CalculateValueQuaternion(const AnimationCurve<Quaternion>& keyframes, float time);

	Vector3 InterpolationValue(const AnimationCurve<Vector3>& key1, const AnimationCurve<Vector3>& key2, float time);
	Quaternion InterpolationValueQuaternion(const AnimationCurve<Quaternion>& key1, const AnimationCurve<Quaternion>& key2, float time);

	Vector3 Lerp(const Vector3& p0, const Vector3& p1, float t);
	Quaternion Lerp(const Quaternion& p0, const Quaternion& p1, float t);

	Quaternion operator-(const Quaternion& q);
	Quaternion operator*(const float f, const Quaternion& q);
	Quaternion operator+(const Quaternion& q0, const Quaternion& q1);

	float MulctyQuaternion(const Quaternion& q1, const Quaternion& q2);
	Quaternion Slerp(const Quaternion& q0, const Quaternion& q1, float t);

	Skeleton CreateSkeltion(const Node& rootNode);
	int32_t CreateJoint(const Node& node, const std::optional<int32_t>& parent, std::vector<Joint>& joints);

	Matrix4x4 Transpose(Matrix4x4 m);

	VertexData AddVert(const VertexData& v1, const VertexData& v2);

}
