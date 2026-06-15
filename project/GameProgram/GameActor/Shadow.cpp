#include "Shadow.h"
#include "Object3dCommon.h"
#include "GLTFCommon.h"
#include "ImGuiManager.h"

using namespace MyMath;

Shadow::~Shadow() {}

void Shadow::Initialize() {
	collisionType_ = CollisionTypes::TypeShadow;
}


void Shadow::Update(const Transform& transform) {
	actorPosition_ = transform.translate;

	transform_.translate = actorPosition_;
	transform_.translate.y = minUnder_;

	collisionAABB_.min = actorPosition_ - Vector3{ kShadowWidth_, kShadowMinY_, kShadowWidth_ };
	collisionAABB_.max = actorPosition_ + Vector3{ kShadowWidth_, 0.0f, kShadowWidth_ };
	center_ = transform_.translate;
	
	//影の配置
	EngineLayer::ShadowManager::GetInstance().AddShadow(transform.translate, transform.scale, std::abs(actorPosition_.y - transform_.translate.y));
	
	CollisionManager::GetInstance().FrameCollision(this);
	minUnder_ = -kShadowMinY_;
}

void Shadow::OnCollision(CollisionSource* collision) {
	if (collision->GetType() == CollisionTypes::TypeStage || 
		collision->GetType() == CollisionTypes::TypeMoveGround) {
		CollisionUtility::GetInstance().UnderCollision(minUnder_,actorPosition_,collision->GetAABB());
	}
}

bool Shadow::TypeCheckUp(const CollisionTypes& collisionType) {
	if (collisionType == CollisionTypes::TypeStage || collisionType == CollisionTypes::TypeMoveGround) {
		return true;
	}
	return false;
}