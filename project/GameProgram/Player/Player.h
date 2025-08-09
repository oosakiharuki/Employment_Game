#pragma once
#include "MyMath.h"
#include "Object3d.h"
#include "PlayerBullet.h"
#include "Umbrella.h"

class Player {
public:
	Player();
	~Player();
	void Initialize();
	void Update();
	void Draw();

	Vector3 GetTranslate() { return worldTransform.translation_; }
	void SetTranslate(Vector3 translate) { worldTransform.translation_ = translate; }
	void SetRotate(Vector3 rotate) { worldTransform.rotation_ = rotate; }

	void SetAABB(AABB aabb) { playerAABB = aabb; }
	AABB GetAABB();

	void SetModelFile(std::string filename);

	void IsWall(bool result) { isWall = result; }
	void IsGround(bool result) { isGround = result; }

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

	void KnockBackPlayer(const Vector3 Power,const float TimerMax);

private:
	Object3d* object;
	WorldTransform worldTransform;
	AABB playerAABB;

	float speed = 0.1f;

	bool isJump = false;

	/// stageの当たり判定で使う
	bool isWall = false;
	bool isGround = false;


	float grabity = 0.0f;
	const float deltaTime = 1.0f / 60.0f;

	/// 弾丸
	std::list<PlayerBullet*> bullets_;

	Direction direction = Direction::right;
	Range range = Range::Right;
	WorldTransform wtGun;

	bool isChangeDirection = false;

	float coolTimer = 0.0f;
	float coolMax = 0.5f;
	uint32_t bulletCount = 3;

	/// 傘のシールド
	bool isShield = false;
	Umbrella* umbrella = nullptr;

	/// ノックバック
	bool isKnockback = false;
	Vector3 backPower = { 0,0,0 };

	float KnockBackTimer = 0.0f;
	float KnockBackTimeMax = 0.5f;

	/// Hp
	uint32_t Hp = 10;
};