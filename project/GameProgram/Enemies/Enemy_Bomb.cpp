#include "Enemy_Bomb.h"
#include "Object3dCommon.h"

using namespace MyMath;

Enemy_Bomb::~Enemy_Bomb() {}

void Enemy_Bomb::Initialize() {

	Enemy_InitializeCommon();
	//モデル作成
	object_->SetModelFile("enemy_bomb.obj");

	//体力の初期化
	HP_Initialize(1);

	//見える範囲初期化
	eyeReach_ = { 15, 2, 1 };
}

void Enemy_Bomb::Update() {

	//敵の共有処理
	UpdateCommon();

	//体力が0の時
	if ((bombTimer_ >= kBombTimeMax_ || isDead_) && !isExplosion_) {
		Exprosion();
	}

	//死んだとき
	if (isDead_) {
		//爆発するフラグ
		isExplosion_ = true;
	}
	else {
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

	ParticleCommon::GetInstance()->Command();
	//ダメージのパーティクル
	particleDamage_->Draw();

	Object3dCommon::GetInstance()->Command();
}

void Enemy_Bomb::Attack() {
	//時限爆弾モードオン
	isStart_ = true;
}

void Enemy_Bomb::TimeRimmit() {
	//爆弾タイマー
	bombTimer_ += kDeltaTime_;

	//プレイヤーに追淳
	wt_.translation_ += distance_ * Vector3{ -0.03f,0,0 } *3;

	//向きを合わせる
	if (distance_.x < 0) {
		wt_.rotation_.y = 90.0f;
	}
	if (distance_.x >= 0) {
		wt_.rotation_.y = -90.0f;
	}

	//リアクション
	if (bombTimer_ >= kBombTimeMax_ / 1.5f) {
		//爆発寸前だと揺れが細かくなる
		ScaleUpdate(&isStart_, bombScale_ * 2, 0.2f / 2);
	}
	else {
		//爆発しそうな演出
		ScaleUpdate(&isStart_, bombScale_, 0.2f);
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
	particleDamage_->SetTranslate(wt_.translation_);
	particleDamage_->SetParticleBorn(ParticleBorn::MomentMode);

	//爆発したら死んでしまう
	isDead_ = true;
}