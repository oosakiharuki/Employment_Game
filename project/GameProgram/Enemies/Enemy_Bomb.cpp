#include "Enemy_Bomb.h"
#include "Object3dCommon.h"
#include "ImGuiManager.h"

using namespace UseEveryOne;
using namespace MyMath;

Enemy_Bomb::~Enemy_Bomb() {}

void Enemy_Bomb::Initialize() {
	//敵の共通初期化処理
	Enemy_InitializeCommon("enemy_bomb.obj");

	//体力の初期化
	HP_Initialize(kHp_);

	//見える範囲初期化
	eyeReach_ = kEyeReach_;

	//元色
	object_->SetColor(color_);
}

void Enemy_Bomb::UpdateNormal() {
	//動く
	MoveEnemy();

	//ボムとプレイヤーの距離
	DirectionPlayer();
}

void Enemy_Bomb::UpdateAttack() {
	//制限時間がMaxを越した時
	if (bombTimer_ >= kBombTimeMax_) {
		//Hpが0になって爆発
		hp_ = 0;
	}

	//ボムとプレイヤーの距離
	DirectionPlayer();
	
	//爆弾までの制限時間カウント
	TimeRimmit();
}

void Enemy_Bomb::UpdateDead() {
	//プレイヤーに倒された場合
	if (!isExplosion_) {
		//強制爆発
		Exprosion();
	}

	deadTimer_ += kDeltaTime_;

	if (isExplosion_ &&  deadTimer_ >= kDeadTimeMax_) {
		isDeleteEnemy_ = true;
	}
}

void Enemy_Bomb::UpdateImgui() {
#ifdef USE_IMGUI

	ImGui::Begin("Enemy_soldier");

	ImGui::Text("translate : %f,%f,%f", wt_.translation_.x, wt_.translation_.y, wt_.translation_.z);
	ImGui::Text("translate : %f,%f,%f", wt_.rotation_.x, wt_.rotation_.y, wt_.rotation_.z);

	ImGui::End();

#endif // USE_IMGUI
}

void Enemy_Bomb::Draw() {
	//死んだら移さない
	if (!isDead_) {
		object_->Draw();
		shadow_->Draw();
	}
}

void Enemy_Bomb::Attack() {
	//追尾モードオン
	isTuibiStart_ = true;
}

void Enemy_Bomb::TimeRimmit() {
	//爆弾タイマー
	bombTimer_ += kDeltaTime_;

	//プレイヤーに追淳
	wt_.translation_ += distance_ * Vector3{ kSpeed_,0.0f,0.0f };

	//向きを合わせる
	if (distance_.x < 0) {
		wt_.rotation_.y = kDirectionRight_;//右向き
	}
	if (distance_.x >= 0) {
		wt_.rotation_.y = kDirectionLeft_;//左向き
	}

	//リアクション

	if (bombTimer_ >= kOnTheVerge) {
		//爆発寸前だと揺れが細かくなる
		reaction_->ScaleReaction(wt_.scale_,isTuibiStart_, bombScale_ * kScaleSpeedUp_, scaleTimer_, kScaleMax_ / kScaleSpeedUp_);
		colorTimeMax_ = kScaleMax_ / kScaleSpeedUp_;//点滅時間変更
	}
	else {
		//爆発しそうな演出
		reaction_->ScaleReaction(wt_.scale_, isTuibiStart_, bombScale_, scaleTimer_, kScaleMax_);
		colorTimeMax_ = kScaleMax_;//点滅時間
	}

	//RGBの緑、青の変更(赤色にするため除外)
	if (colorTimer_ >= colorTimeMax_ / 2) {
		//色を足して元の色に
		color_.y += kColorChangePower_;//緑
		color_.z += kColorChangePower_;//青
	}
	else {
		//色を引いて赤に近づかせる
		color_.y -= kColorChangePower_;//緑
		color_.z -= kColorChangePower_;//青
	}
	//時間経過
	colorTimer_ += kDeltaTime_;
	//一定の時間を超えたらリセット
	if (colorTimer_ >= colorTimeMax_) {
		colorTimer_ = 0.0f;
	}

	//オブジェクト色変更
	object_->SetColor(color_);
}

void Enemy_Bomb::DirectionPlayer() {
	//- プレイヤー追尾処理 -
	//それぞれの座標位置
	Vector3 enemyPosition = GetWorldPosition();
	Vector3 playerPosition = player_->GetWorldPosition();
	//プレイヤーとボムの距離
	distance_ = enemyPosition - playerPosition;
	//ノーマライズ
	distance_ = Normalize(distance_);
}


void Enemy_Bomb::RespawnEnemy() {
	RespawnEnemyCommon();
	
	//追尾モードオフ
	isTuibiStart_ = false;
	//爆発してない
	isExplosion_ = false;
	bombTimer_ = 0.0f;
	//死亡タイマーリセット
	deadTimer_ = 0.0f;
}

void Enemy_Bomb::Exprosion() {
	//爆発範囲AABB
	bombAABB_.min = wt_.translation_ - kExplosionRange_;
	bombAABB_.max = wt_.translation_ + kExplosionRange_;

	//パーティクルの設定
	particles_[particleDamage_.name]->SetTranslate(wt_.translation_);
	particles_[particleDamage_.name]->SetParticleBorn(ParticleBorn::MomentMode);

}