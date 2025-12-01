#include "Enemy_Bomb.h"
#include "Object3dCommon.h"

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
}

void Enemy_Bomb::Update() {

	//敵の共有処理
	UpdateCommon();

	//体力が0の時
	if ((bombTimer_ >= kBombTimeMax_ || isDead_) && !isExplosion_) {
		Exprosion();
	}

	//死んだとき
	if (!isDead_) {
		//通常処理
		
		//それぞれの座標位置
		Vector3 enemyPosition = GetWorldPosition();
		Vector3 playerPosition = player_->GetWorldPosition();
		//プレイヤーとボムの距離
		distance_ = enemyPosition - playerPosition;
		//ノーマライズ
		distance_ = Normalize(distance_);

		if (!isStart_) {
			MoveEnemy();

			if (!isFoundTarget_) {
				SearchRange();
			}
		}
		else {
			TimeRimmit();
		}
	}

	//更新が終了
	UpdateBehind();
}

void Enemy_Bomb::Draw() {
	//死んだら移さない
	if (!isDead_) {
		object_->Draw();
		shadow_->Draw();
	}
}

void Enemy_Bomb::Attack() {
	//時限爆弾モードオン
	isStart_ = true;
}

void Enemy_Bomb::TimeRimmit() {
	//爆弾タイマー
	bombTimer_ += kDeltaTime_;

	//プレイヤーに追淳
	wt_.translation_ += distance_ * Vector3{ kSpeed_,0.0f,0.0f };

	//向きを合わせる
	if (distance_.x < 0) {
		wt_.rotation_.y = kDirectionRight_;
	}
	if (distance_.x >= 0) {
		wt_.rotation_.y = kDirectionLeft_;
	}

	//リアクション

	if (bombTimer_ >= kOnTheVerge) {
		//爆発寸前だと揺れが細かくなる
		ScaleUpdate(isStart_, bombScale_ * kScaleSpeedUp_, kScaleMax_ / kScaleSpeedUp_);
	}
	else {
		//爆発しそうな演出
		ScaleUpdate(isStart_, bombScale_, kScaleMax_);
	}
}

void Enemy_Bomb::RespawnEnemy() {
	RespawnEnemyCommon();
	
	//時限爆弾モードオフ
	isStart_ = false;
	//爆発してない
	isExplosion_ = false;
	bombTimer_ = 0.0f;
}

void Enemy_Bomb::Exprosion() {
	//爆発範囲AABB
	bombAABB_.min = wt_.translation_ - kExplosionRange_;
	bombAABB_.max = wt_.translation_ + kExplosionRange_;

	//パーティクルの設定
	particles_[particleDamage_.name]->SetTranslate(wt_.translation_);
	particles_[particleDamage_.name]->SetParticleBorn(ParticleBorn::MomentMode);

	//爆発したら死んでしまう
	isDead_ = true;
}