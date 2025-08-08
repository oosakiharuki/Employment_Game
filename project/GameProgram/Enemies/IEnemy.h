#pragma once
#include "MyMath.h"
#include "Object3d.h"
#include "EnemyBullet.h"

class IEnemy {
public:

	IEnemy();
	virtual ~IEnemy();
	virtual void Initialize() = 0;
	virtual void Update() = 0;
	virtual void Draw() = 0;
	virtual void IsDamage() = 0;
	virtual void Attack() = 0;


	void SetTranslate(Vector3 translate) { worldTransform.translation_ = translate; }
	void SetRotate(Vector3 rotate) { worldTransform.rotation_ = rotate; }

	AABB GetAABB();
	void SetAABB(AABB aabb) { enemyAABB = aabb; }
	bool IsDead() { return isDead; }

	std::list<EnemyBullet*> GetBullets() { return bullets_; }

protected:
	Object3d* object;
	WorldTransform worldTransform;
	AABB enemyAABB;
	bool isDead = false;

	std::list<EnemyBullet*> bullets_;
};

