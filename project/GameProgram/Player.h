#pragma once
#include "MyMath.h"
#include "Object3d.h"
#include "PlayerBullet.h"

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


private:
	Object3d* object;
	WorldTransform worldTransform;
	AABB playerAABB;

	bool isJump = false;

	//stageの当たり判定で使う
	bool isWall = false;
	bool isGround = false;

	float grabity = 0.0f;

	std::list<PlayerBullet*> bullets_;

	Direction direction = Direction::right;
	bool isChangeDirection = false;
};