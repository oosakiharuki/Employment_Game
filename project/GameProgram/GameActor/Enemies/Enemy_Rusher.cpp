#include "Enemy_Rusher.h"
#include "TimeScale.h"
#include "MyMath.h"
#include "UseEveryOne.h"

using namespace MyMath;
using namespace UseEveryOne;

Enemy_Rusher::~Enemy_Rusher() {}

void Enemy_Rusher::Initialize() {

	BaseEnemy::Enemy_InitializeCommon("rusher.gltf");
	//体力の初期化
	GameActor::HP_Initialize(kHp_);

	//見える範囲初期化
	eyeReach_ = kEyeReach_;

	//体自体にダメージがある敵
	collisionType_ = CollisionTypes::TypeEnemyDamageBody;

}

void Enemy_Rusher::UpdateImGui() {

}

void Enemy_Rusher::Draw() {
	if (hp_ != 0 && !isPerformance_) {
		object_->Draw();
	}
}

void Enemy_Rusher::Active() {
	//敵のステートパターンの更新処理
	StatePatternUpdate();

	if (transform_.rotate.y == kDirectionRight_) {
		eyeReach_.x = kEyeReach_.x;
	}
	//左向き
	else if (transform_.rotate.y == kDirectionLeft_) {
		eyeReach_.x = -kEyeReach_.x;
	}
	//ターゲット発見
	PlayerTarget();
	//捜索範囲更新
	SearchRange();

}

void Enemy_Rusher::Dead() {
	//死んだリアクション
	DeadReaction();
}

void Enemy_Rusher::Performance() {}

void Enemy_Rusher::SearchCommand() {
	//マークの更新
	MarkUpdate();
}

void Enemy_Rusher::AttackCommand() {
	//マークの更新
	MarkUpdate();

	object_->SetAnimationTime(TimeScale::GetInstance().GetTimeScale());

	//攻撃処理が終了した場合
	if (!attackSwitch_) {
		return;
	}
	//予備動作
	Charge();
	//突撃処理
	RushStreet();
	//元の定位置に戻る
	ReturnPosition();
}

void Enemy_Rusher::Charge() {
	//時間になってないなら
	if (ChargeMax()) return;

	if (chargeTimer_ == 0.0f) {
		prevPosition_ = transform_.translate;
	}

	chargeTimer_ += TimeScale::GetInstance().GetTimeScale();
	

	object_->SetAnimationTime(TimeScale::GetInstance().GetTimeScale() * 20.0f * chargeTimer_);


	transform_.translate -= TransformNormal(kRushSpeed_ / kStepBackSpeed_, wt_.GetMatWorld()) * TimeScale::GetInstance().GetTimeScaleFacto() ;
	transform_.scale.z = 1.0f - (kScaleBack_ * chargeTimer_);
}

void Enemy_Rusher::RushStreet() {
	//時間がたった
	if (!ChargeMax() || isReturnPosition_) return;

	//前に突撃する
	transform_.translate += TransformNormal(kRushSpeed_, wt_.GetMatWorld()) * TimeScale::GetInstance().GetTimeScaleFacto();
	
	//壁に衝突した場合
	if (collisionOverlap.isWall) {
		isReturnPosition_ = true;
		hitWallPosition_ = transform_.translate;

	}
	object_->SetAnimationTime(TimeScale::GetInstance().GetTimeScale() * 20.0f * chargeTimer_);
	

	transform_.scale = kDefaultScale_;
}

void Enemy_Rusher::ReturnPosition() {
	//定位置に戻るフラグでないなら
	if (!isReturnPosition_) return;

	transform_.rotate.x = 0.0f;

	//元の位置に戻す(突撃の逆方向に+スピード半減)
	transform_.translate -= TransformNormal(kRushSpeed_, wt_.GetMatWorld()) * TimeScale::GetInstance().GetTimeScaleFacto() * kDivideByTwo_;
		
	lengthMax_ = {
		std::abs(transform_.translate.x - prevPosition_.x),
		std::abs(transform_.translate.y - prevPosition_.y),
		std::abs(transform_.translate.z - prevPosition_.z),	
	};

	if (lengthMax_ < Vector3{0.1f,0.1f,0.1f}) {
		isReturnPosition_ = false;
		attackSwitch_ = false;
		chargeTimer_ = 0.0f;
		transform_.translate = prevPosition_;
	}

}

bool Enemy_Rusher::ChargeMax() {
	return chargeTimer_ >= kChargeTimeMax_;
}
