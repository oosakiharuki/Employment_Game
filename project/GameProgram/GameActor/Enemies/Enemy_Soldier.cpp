#include "Enemy_Soldier.h"
#include "Input.h"

#include "ImGuiManager.h"
#include "Object3dCommon.h"
#include "ParticleManager.h"

using namespace MyMath;
using namespace UseEveryOne;

Enemy_Soldier::~Enemy_Soldier() {
	fireCommand_->BulletReset();
}

void Enemy_Soldier::Initialize() {
	//敵の共通初期化処理
	BaseEnemy::Enemy_InitializeCommon("soldier.gltf");

	//体力の初期化
	GameActor::HP_Initialize(kHp_);

	//見える範囲初期化
	eyeReach_ = kEyeReach_;

	fireCommand_ = std::make_unique<EnemyFireCommand>();
	//最大弾丸数
	fireCommand_->SetRapidCountMax(kRapidCountMax_);
	fireCommand_->InitAudio();

	//攻撃(発泡)
	particles_[fireCommand_->GetParticleFireName()] = EngineLayer::ParticleManager::GetInstance().InitParticle(fireCommand_->GetParticleFireName());

	speed_.x = kMoveX_;
}

void Enemy_Soldier::Move() {
	move_ += speed_ * TimeScale::GetInstance().GetTimeScaleFacto();//移動ポイント
	transform_.translate += speed_ * TimeScale::GetInstance().GetTimeScaleFacto();

	//方向転換
	//敵が右向き
	if (move_.x <= routePointLeft_.x) {
		speed_.x = kMoveX_;//右に進む
		eyeReach_.x = kEyeReach_.x;
		transform_.rotate.y = kDirectionRight_;//右が正面
	}
	//左向き
	else if (move_.x >= routePointRight_.x) {
		speed_.x = -kMoveX_;//左に進む
		eyeReach_.x = -kEyeReach_.x;
		transform_.rotate.y = kDirectionLeft_;//左が正面
	}
}

void Enemy_Soldier::DirectionMove() {
	if (transform_.rotate.y == kDirectionRight_) {
		speed_.x = kMoveX_;//右に進む
		eyeReach_.x = kEyeReach_.x;
	}
	else if (transform_.rotate.y == kDirectionLeft_) {
		speed_.x = -kMoveX_;//左に進む
		eyeReach_.x = -kEyeReach_.x;
	}
}


void Enemy_Soldier::Active() {
	//敵のステートパターンの更新処理
	StatePatternUpdate();

	//重力
	GravityUpdate(transform_.translate.y, true);
	
	PlayerTarget();
	//捜索範囲更新
	SearchRange();
	//弾丸の更新
	fireCommand_->BulletUpdate();

	isGround_ = false;
}

void Enemy_Soldier::SearchCommand() {
	//移動する
	Move();
	fireCommand_->FireStart();
}

void Enemy_Soldier::AttackCommand() {
	
	if (fireCommand_->IsFire()) {
		//見つけたリアクション
		FoundReaction();

		//発砲処理
		fireCommand_->Fire(*this);
	}

	if (!fireCommand_->IsFire() && !enemyEye_->IsFound()) {
		attackSwitch_ = false;
	}
	else {
		fireCommand_->FireStart();
	}

	//マークの更新
	MarkUpdate();

	//コーンが上向きなので
	particles_[fireCommand_->GetParticleFireName()]->SetRotate({ 0.0f,0.0f,-transform_.rotate.y });
}

void Enemy_Soldier::Dead() {
	//死んだリアクション
	DeadReaction();
	//弾丸の更新
	fireCommand_->BulletUpdate();
	//発射準備SEの停止
	fireCommand_->DeleteAudio();
}

void Enemy_Soldier::Performance() {}

void Enemy_Soldier::UpdateImGui() {

#ifdef USE_IMGUI

	ImGui::Begin("Enemy_soldier");

	ImGui::Text("translate : %f,%f,%f", transform_.translate.x, transform_.translate.y, transform_.translate.z);
	ImGui::Text("translate : %f,%f,%f", transform_.rotate.x, transform_.rotate.y, transform_.rotate.z);

	ImGui::Text("routePointLeft : %f,%f,%f", routePointLeft_.x, routePointLeft_.y, routePointLeft_.z);
	ImGui::Text("routePointRight : %f,%f,%f", routePointRight_.x, routePointRight_.y, routePointRight_.z);

	ImGui::End();

#endif // USE_IMGUI
}

void Enemy_Soldier::Draw() {
	if (!isDeleteEnemy_ && !isPerformance_) {
		object_->Draw();
	}
	fireCommand_->BulletDraw();
}

void Enemy_Soldier::FireBullet() {
	
	Vector3 enemyPosition;
	enemyPosition = { wt_.GetMatWorld().m[3][0],wt_.GetMatWorld().m[3][1],wt_.GetMatWorld().m[3][2]};

	//プレイヤーの方向に向かう(最初に打つ弾にそって進む)
	if (fireCommand_->GetRapidCount() == 0) {

		const float kSpeed = 0.4f;
		//プレイヤーの座標
		Vector3 playerPosition = player_->GetWorldPosition();
		//敵とプレイヤーの距離
		Vector3 distance = playerPosition - enemyPosition;
		//
		Vector3 normal = Normalize(distance);

		//スピードを合わせる
		normal *= kSpeed;
		velocity_ = normal;
	}
	particles_[fireCommand_->GetParticleFireName()]->SetTranslate(enemyPosition);
	particles_[fireCommand_->GetParticleFireName()]->SetParticleBorn(EngineLayer::ParticleBorn::MomentMode);//パーティクルが出てくる

	//弾丸を生み出す
	fireCommand_->AddBullet(enemyPosition, velocity_);
}

