#pragma once
#include "MyMath.h"
#include "BaseEnemy.h"
/// <summary>
/// 敵の移動コマンド
/// </summary>
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

	virtual void DirectionMove() = 0;

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

/// <summary>
/// 弾丸を発砲できる敵
/// </summary>
class EnemyCanFireBullet {
public:
	/// <summary>
	/// 弾丸発射処理
	/// </summary>
	virtual void FireBullet() = 0;
};

/// <summary>
/// 敵の発砲コマンド
/// </summary>
class EnemyFireCommand {
public:

	/// <summary>
	/// 弾丸更新処理
	/// </summary>
	void BulletUpdate();
	/// <summary>
	/// 弾丸描画処理
	/// </summary>
	void BulletDraw();
	/// <summary>
	/// 弾丸削除処理
	/// </summary>
	void BulletReset();

	/// <summary>
	/// 発泡
	/// </summary>
	void Fire(EnemyCanFireBullet& enemyCanFireBullet);

	/// <summary>
	/// 攻撃フラグ
	/// </summary>
	/// <returns>true攻撃する / false攻撃しない</returns>
	bool IsFire() { return isFire_; }
	/// <summary>
	/// 攻撃処理
	/// </summary>
	void FireStart() { isFire_ = true; }
	/// <summary>
	/// getter_放った弾丸
	/// </summary>
	/// <returns>現在の放った弾丸数</returns>
	uint32_t GetRapidCount() { return rapidCount_; }
	/// <summary>
	/// setter_放つ弾丸Max
	/// </summary>
	/// <param name="rapidCountMax">放つ弾丸の最大数を設定</param>
	void SetRapidCountMax(uint32_t rapidCountMax) { rapidCountMax_ = rapidCountMax; }
	/// <summary>
	/// 弾丸を追加
	/// </summary>
	/// <param name="startPoint">弾丸が出る座標</param>
	/// <param name="velocity">速度</param>
	void AddBullet(const Vector3& startPoint, const Vector3& velocity);
	/// <summary>
	/// getter_パーティクル攻撃の名前
	/// </summary>
	/// <returns>パーティクル攻撃の名前部分を渡す</returns>
	const std::string& GetParticleFireName() { return particleFire_.name; }
	/// <summary>
	/// getter_パーティクル攻撃そのもの
	/// </summary>
	/// <returns>パーティクル攻撃を渡す</returns>
	const ParticleParameters& GetParticleFireParameter() { return particleFire_; }

private:
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
};
