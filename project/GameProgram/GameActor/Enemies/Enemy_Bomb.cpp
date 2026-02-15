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

void Enemy_Bomb::Move() {
	move_ += speed_;//移動ポイント

	//方向転換
	//敵が右向き
	if (transform_.rotate.y == kDirectionRight_) {
		transform_.rotate.y = kDirectionLeft_;
		speed_.x = kMoveX_;//右に進む
	}
	//左向き
	else if (transform_.rotate.y == kDirectionLeft_) {
		transform_.rotate.y = kDirectionRight_;
		speed_.x = -kMoveX_;//左に進む
	}
}


void Enemy_Bomb::Active() {
	//敵のステートパターンの更新処理
	StatePatternUpdate();

	//重力
	GravityUpdate(transform_.translate.y);
}

void Enemy_Bomb::Dead() {
	if (isExplosion_ && deadTimer_ >= kDeadTimeMax_) {
		isDeleteEnemy_ = true;
		isExplosion_ = false;//爆発フラグオフ
	}
	else if (deadTimer_ < kDeltaTime_) {
		//強制爆発
		Explosion();
	}

	deadTimer_ += kDeltaTime_;
}

void Enemy_Bomb::Performance() {}

void Enemy_Bomb::SearchCommand() {
	//動く
	Move();

	//ボムとプレイヤーの距離
	DirectionPlayer();
}

void Enemy_Bomb::AttackCommand() {
	//制限時間がMaxを越した時
	if (bombTimer_ >= kBombTimeMax_) {
		//Hpが0になって爆発
		hp_ = 0;
	}

	//追尾モードオン
	isHomingStart_ = true;
	//!マーク表示時間
	markTimer_ += kDeltaTime_;

	//ボムとプレイヤーの距離
	DirectionPlayer();
	
	//爆弾までの制限時間カウント
	TimeLimit();

	//マークの更新
	MarkUpdate();
}

void Enemy_Bomb::UpdateImGui() {
#ifdef USE_IMGUI

	ImGui::Begin("Enemy_soldier");

	ImGui::Text("translate : %f,%f,%f", transform_.translate.x, transform_.translate.y, transform_.translate.z);
	ImGui::Text("translate : %f,%f,%f", transform_.rotate.x, transform_.rotate.y, transform_.rotate.z);

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

void Enemy_Bomb::TimeLimit() {
	//爆弾タイマー
	bombTimer_ += kDeltaTime_;

	//プレイヤーに追淳
	transform_.translate += distance_ * Vector3{ kSpeed_,0.0f,0.0f };

	//向きを合わせる
	if (distance_.x < 0) {
		transform_.rotate.y = kDirectionRight_;//右向き
	}
	if (distance_.x >= 0) {
		transform_.rotate.y = kDirectionLeft_;//左向き
	}

	//リアクション
	if (bombTimer_ >= kOnTheVerge) {
		//爆発寸前だと揺れが細かくなる
		reaction_->ScaleReaction(transform_.scale, isHomingStart_, bombScale_ * kScaleSpeedUp_, scaleTimer_, kScaleMax_ / kScaleSpeedUp_);
		colorTimeMax_ = kScaleMax_ / kScaleSpeedUp_;//点滅時間変更
	}
	else {
		//爆発しそうな演出
		reaction_->ScaleReaction(transform_.scale, isHomingStart_, bombScale_, scaleTimer_, kScaleMax_);
		colorTimeMax_ = kScaleMax_;//点滅時間
	}
	//赤い点滅
	RedBlinking();
}

void Enemy_Bomb::RedBlinking() {
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
	isHomingStart_ = false;
	//爆発してない
	isExplosion_ = false;
	bombTimer_ = 0.0f;
	//死亡タイマーリセット
	deadTimer_ = 0.0f;
	//点滅タイマーリセット
	colorTimer_ = 0.0f;
	//オブジェクト色変更
	color_ = { 1,1,1,1 };//default
	object_->SetColor(color_);
}

void Enemy_Bomb::Explosion() {
	//爆発範囲AABB
	bombAABB_.min = transform_.translate - kExplosionRange_;
	bombAABB_.max = transform_.translate + kExplosionRange_;

	//パーティクルの設定
	particles_[particleDamage_.name]->SetTranslate(transform_.translate);
	particles_[particleDamage_.name]->SetParticleBorn(ParticleBorn::MomentMode);

	//爆発フラグオン
	isExplosion_ = true;
}