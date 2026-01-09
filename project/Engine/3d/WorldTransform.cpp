#include "WorldTransform.h"
#include "MyMath.h"
#include "Object3dCommon.h"

using namespace MyMath;

WorldTransform::WorldTransform(){}
WorldTransform::~WorldTransform(){}

void WorldTransform::UpdateMatrix(Transform transform) {
	transform_ = transform;

	// スケール、回転、平行移動を合成して行列を計算する
	matWorld_ = MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.translate);

	//親子関係
	if (parent_) {
		matWorld_ = Multiply(matWorld_, parent_->matWorld_);
	}
}


void WorldTransform::Initialize() {
	transform_.scale = { 1.0f,1.0f,1.0f };
	transform_.rotate  = { 0.0f,0.0f,0.0f };
	transform_.translate = { 0.0f,0.0f,0.0f };

	matWorld_ = MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.translate);
}
