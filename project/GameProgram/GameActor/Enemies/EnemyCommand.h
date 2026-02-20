#pragma once
#include "MyMath.h"
#include "BaseEnemy.h"

class EnemyMoveCommand {
public:

	/// <summary>
	/// 移動ルート中心の場合(Eventで使う)
	/// </summary>
	/// <param name="init">真ん中にする場所</param>
	void SetMoveInit(const Vector3& init) { move_ = init; }

	/// <summary>
	/// 移動ルートのポイント設定:左(現在2つ)
	/// </summary>
	/// <param name="leftPoint">左端</param>
	void SetRouteLeftPoint(const Vector3& leftPoint) { routePointLeft_ = leftPoint; }
	
	/// <summary>
	/// 移動ルートのポイント設定:右(現在2つ)
	/// </summary>
	/// <param name="leftPoint">右端</param>
	void SetRouteRightPoint(const Vector3& rightPoint) { routePointRight_ = rightPoint; }

	/// <summary>
	/// 動く敵
	/// </summary>
	virtual void Move() = 0;

protected:

	//動く範囲
	Vector3 routePointRight_;
	Vector3 routePointLeft_;

	//移動するとき
	Vector3 speed_ = { 0,0,0 };
	//移動
	Vector3 move_ = { 0,0,0 };
	const float kMoveX_ = 0.03f;
};

class EnemyFireCommand {
public:

	/// <summary>
	/// 銃弾の処理
	/// </summary>
	/// <returns>銃弾リスト</returns>
	std::list<EnemyBullet*> GetBullets() { 
		std::list<EnemyBullet*> bullets;
		for (auto& bullet : bullets_) {
			bullets.push_back(bullet.get());
		}
		return bullets;
	}

protected:
	//弾丸リスト
	std::list<std::unique_ptr<EnemyBullet>> bullets_;

	//クールタイム
	float coolTime_ = 0.0f;
	const float kCoolTimeMax_ = 2.0f;
	//弾丸の出す間の時間
	float rapidFireTime_ = 0.0f;
	const float kRapidFireTimeMax_ = 0.1f;
	//発射カウント
	uint32_t rapidCount_ = 0;
	//最大弾丸数
	uint32_t rapidCountMax_ = 3;

	//攻撃するとき
	ParticleParameters particleFire_ = {
		"enemy_fire", "resource/Sprite/cone.dds", Primitive::CreateCone(), 1, 0.1f, {1 ,1 ,1}
	};

	bool isFire_ = true;

	/// <summary>
	/// 弾丸更新処理
	/// </summary>
	void BulletUpdate();

	/// <summary>
	/// 発泡
	/// </summary>
	void Fire();

	/// <summary>
	/// 発泡方法
	/// </summary>
	virtual void FireBullet() = 0;
};

class EnemyExplosionCommand {
public:

	/// <summary>
	/// getter‗爆発範囲AABB
	/// </summary>
	/// <returns>bombAABB</returns>
	virtual AABB GetBombAABB() = 0;

	/// <summary>
	/// getter_距離
	/// </summary>
	/// <returns>プレイヤーからボムの距離</returns>
	virtual Vector3 GetDistance() = 0;

	/// <summary>
	/// 爆発しているか
	/// </summary>
	/// <returns>爆発フラグ</returns>
	virtual bool IsExplosion() = 0;

protected:

	//追尾開始
	bool isHomingStart_ = false;
	//爆発
	bool isExplosion_ = false;

	//爆発するまでのタイマー
	const float kBombTimeMax_ = 5.0f;//max
	float bombTimer_ = 0.0f;

	//爆発範囲AABB
	AABB bombAABB_;
	const Vector3 kExplosionRange_ = { 5,5,5 };//大きさ

	//リアクション(拡大縮小)
	Vector3 bombScale_ = { 0.05f, 0.05f, 0.05f };
	const float kScaleMax_ = 0.2f;//スケール最大値
	const float kOnTheVerge = 3.5f;//爆発寸前のタイマー
	const float kScaleSpeedUp_ = 2.0f;
	//リアクション(色)
	Vector4 color_ = { 1,1,1,1 };
	float colorTimer_ = 0.0f;//時間
	float colorTimeMax_ = 0.2f;//色変化の最大時間
	const float kColorChangePower_ = 0.1f;//足し引きするパワー

	float deadTimer_ = 0.0f;
	const float kDeadTimeMax_ = 0.5f;//爆発する間の時間

	/// <summary>
	/// 爆発する
	/// </summary>
	virtual void Explosion() = 0;

	/// <summary>
	/// タイムリミット
	/// </summary>
	virtual void TimeLimit() = 0;

	/// <summary>
	/// 赤の点滅
	/// </summary>
	virtual void RedBlinking() = 0;
};
