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

	void GrabityUpdate();
	
	Vector3 GetTranslate() { return wt.translation_; }
	void SetTranslate(Vector3 translate) { wt.translation_ = translate;}

	Vector3 GetRotate() { return wt.rotation_; }
	void SetRotate(Vector3 rotate) { wt.rotation_ = rotate;}

	void SetInit_Position(Vector3 translate, Vector3 rotate) {
		init_point = translate;
		init_rotate = rotate;
	}

	AABB GetAABB();
	void SetAABB(AABB aabb) { enemyAABB = aabb; }
	bool IsDead() { return isDead; }

	std::list<EnemyBullet*> GetBullets() { return bullets_; }


	void SetPlayer(Player* player) { player_ = player; }

	bool GetDeleteEnemy() { return deleteEnemy; }
	virtual void RespownEnemy() = 0;
	void RespownEnemy_All();

	void SetRoutePoint1(Vector3 point1) { route_point1 = point1; }
	void SetRoutePoint2(Vector3 point2) { route_point2 = point2; }


	//角度で向きを変更
	void DirectionDegree();

	//重力
	void IsGround(bool result) { isGround = result; }

	bool GetIsGround() { return isGround; }
	void GrabityZero() { grabity = 0.0f; }

	void SetStages(std::vector<AABB> aabb) { stages = aabb; }

	void ScaleUpdate(bool* mosionOn,Vector3 scale, const float maxTime);

	//影
	void SetShadowPosition(Vector3 position) { shadow_->SetTranslate(position); }
	AABB GetShadowAABB() { return shadow_->GetAABB(); }
	void ShadowUpdate();

protected:
	Object3d* object;
	WorldTransform wt;
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

	//初期位置保管
	Vector3 init_point;
	Vector3 init_rotate;

	//動く範囲
	Vector3 route_point1;
	Vector3 route_point2;


	bool isGround = false;

	float grabity = 0.0f;
	const float deltaTime = 1.0f / 60.0f;
	
	///影
	Shadow* shadow_;

	float scaleTimer = 0.0f;
	Vector3 defaultScale = { 1,1,1 };//元の大きさ

private:
	std::vector<AABB> stages;
};

