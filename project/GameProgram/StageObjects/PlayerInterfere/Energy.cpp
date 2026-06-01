/// -----------------------
///
/// エネルギーチャージ
/// 触れるとゲージが増える
/// 
/// -----------------------
#include "Energy.h"
#include "MyMath.h"
#include <ParticleManager.h>

using namespace MyMath;

void Energy::Initialize() {
	//モデル作成
	object_ = std::make_unique<EngineLayer::Object3d>();
	object_->Initialize();
	object_->SetModelFile("energy.obj");

	wt_.Initialize();
	collisionType_ = TypeEnergy;

	particles_[kCharge_] = EngineLayer::ParticleManager::GetInstance().InitParticle(kCharge_);
}

void Energy::Update() {
	for (auto& particle : particles_) {
		particle.second->Update();
	}

	wt_.UpdateMatrix(transform_);
	object_->Update(wt_);

	//当たり判定の設定
	collisionAABB_.min = transform_.translate - colliderSize_;
	collisionAABB_.max = transform_.translate + colliderSize_;
	center_ = transform_.translate;

	CollisionManager::GetInstance().FrameCollision(this);
}

void Energy::Draw() {
	object_->Draw();
}

void Energy::OnCollision(CollisionSource* collisionSource) {
	if (collisionSource->GetType() == CollisionTypes::TypePlayer) {
		particles_[kCharge_]->SetParticleBorn(EngineLayer::ParticleBorn::MomentMode);
		particles_[kCharge_]->SetTranslate(transform_.translate);
	}
}

bool Energy::TypeCheckUp(const CollisionTypes& collisionType) {
	if (collisionType == CollisionTypes::TypePlayer) {
		return true;
	}
	return false;
}


