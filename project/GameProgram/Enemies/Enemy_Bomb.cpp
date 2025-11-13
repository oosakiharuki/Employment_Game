#include "Enemy_Bomb.h"
#include "Object3dCommon.h"

using namespace MyMath;

Enemy_Bomb::~Enemy_Bomb() {}

void Enemy_Bomb::Initialize() {

	Enemy_InitializeCommon();
	//モデル作成
	object->SetModelFile("enemy_bomb.obj");

	//体力の初期化
	HP_Initialize(1);

	//見える範囲初期化
	eyeReach = { 15, 2, 1 };
}

void Enemy_Bomb::Update() {

	//敵の共有処理
	UpdateCommon();

	//体力が0の時
	if ((bombTimer >= kBombTimeMax || isDead) && !isExplosion) {
		Exprosion();
	}

	//死んだとき
	if (isDead) {
		//爆発するフラグ
		isExplosion = true;
	}
	else {
		//通常処理
		
		//それぞれの座標位置
		Vector3 enemyPosition = GetWorldPosition();
		Vector3 playerPosition = player_->GetWorldPosition();
		//プレイヤーとボムの距離
		distance = enemyPosition - playerPosition;
		//ノーマライズ
		distance = Normalize(distance);

		if (!isStart) {
			MoveEnemy();

			if (!isFoundTarget) {
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
	if (!isDead) {
		object->Draw();
		shadow_->Draw();
	}

	ParticleCommon::GetInstance()->Command();
	//ダメージのパーティクル
	particle_damage->Draw();

	Object3dCommon::GetInstance()->Command();
}

void Enemy_Bomb::Attack() {
	//時限爆弾モードオン
	isStart = true;
}

void Enemy_Bomb::TimeRimmit() {
	//爆弾タイマー
	bombTimer += kDeltaTime;

	//プレイヤーに追淳
	wt.translation_ += distance * Vector3{ -0.03f,0,0 } *3;

	//向きを合わせる
	if (distance.x < 0) {
		wt.rotation_.y = 90.0f;
	}
	if (distance.x >= 0) {
		wt.rotation_.y = -90.0f;
	}

	//リアクション
	if (bombTimer >= kBombTimeMax / 1.5f) {
		//爆発寸前だと揺れが細かくなる
		ScaleUpdate(&isStart, bombScale * 2, 0.2f / 2);
	}
	else {
		//爆発しそうな演出
		ScaleUpdate(&isStart, bombScale, 0.2f);
	}
}

void Enemy_Bomb::RespawnEnemy() {
	RespawnEnemyCommon();
	
	//時限爆弾モードオフ
	isStart = false;
	//爆発してない
	isExplosion = false;
	bombTimer = 0.0f;
}

void Enemy_Bomb::Exprosion() {
	//爆発範囲AABB
	bombAABB.min = wt.translation_ - kExplosionRange;
	bombAABB.max = wt.translation_ + kExplosionRange;

	//パーティクルの設定
	particle_damage->SetTranslate(wt.translation_);
	particle_damage->ChangeMode(BornParticle::MomentMode);

	//爆発したら死んでしまう
	isDead = true;
}