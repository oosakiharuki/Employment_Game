#pragma once
#include "MyMath.h"
#include "Object3d.h"
#include "PlayerBullet.h"
#include "Umbrella.h"
#include "Audio.h"
#include "Particle.h"

class Player {
public:
	Player();
	~Player();
	void Initialize();
	void Update();
	void Draw();
	void DrawP();

	Vector3 GetTranslate() { return worldTransform.translation_; }
	void SetTranslate(Vector3 translate) { worldTransform.translation_ = translate; }
	void SetRotate(Vector3 rotate) { worldTransform.rotation_ = rotate; }

	void SetAABB(AABB aabb) { playerAABB = aabb; }
	AABB GetAABB();

	void SetModelFile(std::string filename);

	void IsGround(bool result) { isGround = result; }

	bool GetIsGround() { return isGround; }
	void GrabityZero() { grabity = 0.0f; }

	const WorldTransform& GetWorldTransform() { return worldTransform; }

	void ShootBullet();

	enum Direction {
		right,
		left
	};

	///射程範囲 8方向(順番は上から時計回り)
	enum Range {
		Up,
		UpRight,
		Right,
		DownRight,
		Down,
		DownLeft,
		Left,
		UpLeft,
	};

	std::list<PlayerBullet*> GetBullets() { return bullets_; }
	
	Umbrella* GetUmbrella() { return umbrella; }
	bool GetIsShield() { return isShield; }

	void IsDamage();
	void IsFall();//落ちたときの処理

	void KnockBackPlayer(const Vector3 Power,const float TimerMax);


	Vector3 GetWorldPosition();

	void SetRespownPosition(Vector3 translate) { respownPosition = translate; }
	
	//死んだときの処理
	void DeadPlayer();
	bool GetIsPlayerDown() { return isPlayerDown; }
	//復活
	bool GetIsRespown() { return isRespown; }
	//全ての敵が初期地に戻った時
	void AllRespownEnd() { 
		isRespown = false;
		isPlayerDown = false;
	}

private:
	Object3d* object;
	WorldTransform worldTransform;
	AABB playerAABB;

	float speed = 0.1f;

	bool isJump = false;

	/// stageの当たり判定で使う
	bool isGround = false;


	float grabity = 0.0f;
	const float deltaTime = 1.0f / 60.0f;

	/// 弾丸
	std::list<PlayerBullet*> bullets_;

	float coolTimer = 0.0f;
	float coolMax = 0.5f;
	uint32_t bulletCount = 3;
	WorldTransform wtGun;
	//プレイヤーの向き
	Direction direction = Direction::right;
	Range range = Range::Right;

	bool isChangeDirection = false;

	/// 傘のシールド
	bool isShield = false;
	Umbrella* umbrella = nullptr;

	/// ノックバック
	bool isKnockback = false;
	Vector3 backPower = { 0,0,0 };

	float KnockBackTimer = 0.0f;
	float KnockBackTimeMax = 0.5f;

	///ブリンク
	float brinkTimer = 0.0f;
	const float brinkTimeMax = 0.5f;

	bool isUmbrellaFall = false;

	/// Hp
	uint32_t MaxHp = 3;
	uint32_t Hp = MaxHp;
	//ダメージを食らった後の無敵時間
	float infinityTimer = 0.0f;
	const float infinityTimeMax = 1.0f;


	float deadTimer = 0.0f;
	float deadTimeMax = 2.0f;

	Vector3 respownPosition;
	bool isPlayerDown = false;

	bool isRespown = false;

	//サウンド
	SoundData hitSound;

	//パーティクル
	Particle* particle_walk;
	Particle* particle_fire;
	Particle* particle_brink;

	Vector3 PrePosition;
};