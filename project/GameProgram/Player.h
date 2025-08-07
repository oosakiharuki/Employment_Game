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

	std::list<PlayerBullet*> GetBullets() { return bullets_; }
	
	Umbrella* GetUmbrella() { return umbrella; }
	bool GetIsShield() { return isShield; }

	void IsDamage();

private:
	Object3d* object;
	WorldTransform worldTransform;
	AABB playerAABB;

	bool isJump = false;

	/// stageの当たり判定で使う
	bool isWall = false;
	bool isGround = false;

	float grabity = 0.0f;

	/// 弾丸
	std::list<PlayerBullet*> bullets_;

	Direction direction = Direction::right;
	bool isChangeDirection = false;

	float coolTimer = 0.0f;
	float coolMax = 0.5f;
	uint32_t bulletCount = 3;

	/// 傘のシールド
	bool isShield = false;
	Umbrella* umbrella = nullptr;

	/// Hp
	uint32_t Hp = 10;
};