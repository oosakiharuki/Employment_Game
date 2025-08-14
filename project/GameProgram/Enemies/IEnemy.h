#pragma once
#include "MyMath.h"
#include "Object3d.h"
#include "EnemyBullet.h"
#include "Player.h"

class IEnemy {
public:

	IEnemy();
	virtual ~IEnemy();
	virtual void Initialize() = 0;
	virtual void Update() = 0;
	virtual void Draw() = 0;
	virtual void IsDamage() = 0;
	virtual void Attack() = 0;


	void SetTranslate(Vector3 translate) { 
		worldTransform.translation_ = translate;
		init_point= translate;
	}
	void SetRotate(Vector3 rotate) { worldTransform.rotation_ = rotate; }

	AABB GetAABB();
	void SetAABB(AABB aabb) { enemyAABB = aabb; }
	bool IsDead() { return isDead; }

	std::list<EnemyBullet*> GetBullets() { return bullets_; }


	void SetPlayer(Player* player) { player_ = player; }

	bool GetDeleteEnemy() { return deleteEnemy; }
	void RespownEnemy();

	void SetRoutePoint1(Vector3 point1) { route_point1 = point1; }
	void SetRoutePoint2(Vector3 point2) { route_point2 = point2; }


	//角度で向きを変更
	void DirectionDegree();

protected:
	Object3d* object;
	WorldTransform worldTransform;
	AABB enemyAABB;

	std::list<EnemyBullet*> bullets_;

	//
	Player* player_ = nullptr;
	
	//playerを見つけたとき
	void PlayerTarget();
	AABB eyeAABB;//見える範囲
	bool isFoundTarget = false;

	//敵を倒した時のUpdate関数
	void DeadUpdate();


	///向き左右
	enum Direction {
		right,
		left
	};
	Direction direction;

	uint32_t maxHp;
	uint32_t hp;

	bool isDead = false;
	bool deleteEnemy = false;

	Vector3 init_point;
	Vector3 route_point1;
	Vector3 route_point2;
};

