/// ---------------------------------------
///
/// ゲームアクターの基盤クラス
/// ゲームアクター = プレイヤー、敵、ボス
/// 
/// ---------------------------------------
#include "GameActor.h"

using namespace UseEveryOne;
using namespace MyMath;

void GameActor::Initialize() {
	//行列の初期化
	wt_.Initialize();
	//Transform更新処理
	transform_ = wt_.UpdateTransform();

	//影の初期化
	shadow_ = std::make_unique<Shadow>();
	shadow_->Initialize();

	//リアクションクラス
	reaction_ = std::make_unique<Reaction>();

	actorState_ = std::make_unique<PerformanceState>();
}

void GameActor::Update() {
	//状態ステート(生存(active)、死亡(dead)、演出(performance))
	actorState_->Update(*this);
	actorState_->ChangeState(*this);
}

void GameActor::ShadowUpdate() {
	shadow_->Update(transform_);
}

void GameActor::HP_Initialize(uint32_t max) {
	maxHp_ = max;//最大値を設定
	hp_ = maxHp_;//体力を設定
}

Vector3 GameActor::GetWorldPosition() {
	Vector3 worldPos;

	worldPos.x = wt_.GetMatWorld().m[3][0];
	worldPos.y = wt_.GetMatWorld().m[3][1];
	worldPos.z = wt_.GetMatWorld().m[3][2];

	return worldPos;
}

void GameActor::ChangeStatePattern(std::unique_ptr<BaseActorState> actorState) {
	actorState_.reset();
	actorState_ = std::move(actorState);
}