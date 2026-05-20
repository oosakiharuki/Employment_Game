#pragma once
#include "Vector3.h"
#include "Matrix4x4.h"
#include "Quaternion.h"
#include "ModelData.h"

#include <vector>
#include <string>
#include <map>
#include <optional>
#include <span>
#include <array>

namespace MyMath {

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
		MyMath::QuaternionTransform transform;
		Matrix4x4 localMatrix;
		std::string name;
		std::vector<Node> children;
	};

	struct Joint {
		MyMath::QuaternionTransform transform; //transform
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


	struct ModelDataMulti {
		std::vector<ModelData> Data;
		std::map<std::string, JointWeightData> skinClusterData;
		Node rootNode;
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



	Vector3 CalculateValue(const AnimationCurve<Vector3>& keyframes, float time);
	Quaternion CalculateValueQuaternion(const AnimationCurve<Quaternion>& keyframes, float time);

	Vector3 InterpolationValue(const AnimationCurve<Vector3>& key1, const AnimationCurve<Vector3>& key2, float time);
	Quaternion InterpolationValueQuaternion(const AnimationCurve<Quaternion>& key1, const AnimationCurve<Quaternion>& key2, float time);


}