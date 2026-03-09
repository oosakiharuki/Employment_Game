#include "WarpGate.h"
#include "UseEveryOne.h"
#include "CollisionManager.h"
#include <Input.h>
#include <NextStageSave.h>

using namespace MyMath;
using namespace UseEveryOne;

WarpGate::~WarpGate() {}


void WarpGate::Initialize() {
	wt_.Initialize();
	object_ = std::make_unique<Object3d>();
	object_->Initialize();
	object_->SetModelFile("warpGate.obj");
	//Transform更新処理
	transform_ = wt_.UpdateTransform();

	collisionType_ = CollisionTypes::TypeStageObject;
}

void WarpGate::Update() {
	//プレイヤーに触れたワープゲートの処理
	TouchWarpGate();

	object_->Update(wt_);
	wt_.UpdateMatrix(transform_);

	if (warpExitMode_) {
		transform_.translate = position_;
		minUnder_ += kLittleUp_;//影と重ならないように
		transform_.translate.y = minUnder_;

		//当たり判定設定(ワープ出口版)
		collisionAABB_.min = transform_.translate - Vector3{ 0, minUnder_,0 };
		collisionAABB_.max = transform_.translate;
	}
	else {
		//当たり判定設定(通常)
		collisionAABB_.min = transform_.translate - colliderSize_;
		collisionAABB_.max = transform_.translate + colliderSize_;
	}

	center_ = transform_.translate;

	CollisionManager::GetInstance().AddCollisions(this);
	minUnder_ = 1000.0f;
}

void WarpGate::Update(Player* player) {
	//通常更新処理

	Update();
	if (!player->GetPerformanceMode()) {
		Vanish();//出てきた後消えるようにする
	}
}

void WarpGate::Draw() {
	//完全に小さくなったら映さない
	if (transform_.scale.x <= 0 && transform_.scale.y <= 0 && transform_.scale.z <= 0) {
		return;
	}
	
	object_->Draw();
}

void WarpGate::Vanish() {
	//小さくて見えないためすぐに返す
	if (smallingTimer_ < 1.0f) {

		if (largeFlag_) {
			largeTimer_ += kDeltaTime_;
			transform_.scale = EaseOut(kLargeMax_, kDefaultScale_, largeTimer_);
		}
		else {
			smallingTimer_ += kDeltaTime_;
			transform_.scale = EaseIn(kLargeMax_, { 0,0,0 }, smallingTimer_);
		}
		//補間がMaxに達した時
		if (largeTimer_ >= 1.0f) {
			largeFlag_ = false;//拡大を終了
		}
	}
}

void WarpGate::WarpExit(const Vector3& translate) {
	warpExitMode_ = true;
	position_ = translate;
}

void WarpGate::TouchWarpGate() {
	//ワープ出口モードの時はしない
	if (warpExitMode_) return;
	//触れたら拡大、離れたら縮小
	(scaleFlag_) ? scaleTimer_ += kDeltaTime_ * 2.0f : scaleTimer_ -= kDeltaTime_ * 2.0f;
	transform_.scale = EaseOut(kDefaultScale_ * 1.5f, kDefaultScale_, scaleTimer_);
	
	scaleTimer_ = std::clamp(scaleTimer_, 0.0f, 1.0f);
	scaleFlag_ = false;//フラグリセット
}



void WarpGate::OnCollision(CollisionSource* collision) {
	if (collision->GetType() == CollisionTypes::TypePlayer && !warpExitMode_) {
		scaleFlag_ = true;
		if (Input::GetInstance().TriggerKey(DIK_E) || Input::GetInstance().TriggerButton(XINPUT_GAMEPAD_Y)) {
			CollisionManager::GetInstance().SuccessWarp(center_);
			NextStageSave::GetInstance().SetNextStageFile(fileName_);//次のステージの名前を導入
		}
	}

	if (collision->GetType() == CollisionTypes::TypeStage && warpExitMode_) {
		CollisionManager::GetInstance().UnderCollision(minUnder_, position_, collision->GetAABB());
	}
}
