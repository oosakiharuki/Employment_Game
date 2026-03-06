#pragma once
#include "IStageObject.h"

/// <summary>
/// ゴール(IStageObjectの派生クラス)
/// </summary>
class Goal : public IStageObject {
public:
	/// <summary>
	/// デストラクタ
	/// </summary>
	~Goal() override;
	/// <summary>
	/// 初期化処理
	/// </summary>
	void Initialize() override;
	/// <summary>
	/// 更新処理
	/// </summary>
	void Update() override;
	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw() override;
private:
	/// <summary>
	/// 当たり判定コマンド
	/// </summary>
	/// <param name="collision">相手側の当たり判定ソース</param>
	void OnCollision(CollisionSource* collision) override;
};