#pragma once
#include "Object_glTF.h"
#include "IStageObject.h"

/// <summary>
/// 壊せる箱[ギミック]
/// </summary>
class BrokenBox : public IStageObject
{
public:
	/// <summary>
	/// 初期化処理
	/// </summary>
	void Initialize();

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw();

private:

	/// <summary>
	/// 当たり判定コマンド
	/// </summary>
	/// <param name="collision">相手側の当たり判定ソース</param>
	void OnCollision(CollisionSource* collision) override;
	/// <summary>
	/// 当たり判定をとるタイプかをチェック
	/// </summary>
	/// <param name="collisionType">相手の当たり判定タイプ</param>
	/// <returns>該当するタイプがあるなら true</returns>
	bool TypeCheckUp(const CollisionTypes& collisionType) override;

	std::unique_ptr<Object_glTF> objectBox_ = nullptr;

	//壊したフラグ
	bool isBroken_ = false;
	bool isFinish_ = false;

	float timer = 0.0f;
	const float kAnimationTimeMax_ = 2.4f;
};

