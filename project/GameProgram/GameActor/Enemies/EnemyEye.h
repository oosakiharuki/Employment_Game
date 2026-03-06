#pragma once
#include "MyMath.h"
#include "CollisionSource.h"

/// <summary>
/// 敵の捜索範囲
/// </summary>
class EnemyEye : public CollisionSource {
public:
	/// <summary>
	/// 更新処理
	/// </summary>
	/// <param name="enemyPos">敵の座標位置</param>
	/// <param name="foundReach">範囲</param>
	void Update(const Vector3& enemyPos,const Vector3& foundReach);
	/// <summary>
	/// プレイヤーを見つけたか
	/// </summary>
	/// <returns>見つけたフラグ</returns>
	bool IsFound() { return isFound_; }
	/// <summary>
	/// 当たり判定コマンド
	/// </summary>
	/// <param name="collision">相手側の当たり判定ソース</param>
	void OnCollision(CollisionSource* collision) override;

private:

	//発見フラグ
	bool isFound_ = false;

	bool isWall_ = false;

	Vector3 enemyPosition_{};
	Vector3 playerPosition_{};
};

