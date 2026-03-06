#pragma once
#include "IStageObject.h"

/// <summary>
/// チェックポイント(IStageObjectの派生クラス)
/// </summary>
class CheckPoint : public IStageObject {
public:
	/// <summary>
	/// デストラクタ
	/// </summary>
	~CheckPoint() override;
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

