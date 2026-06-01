#pragma once
#include "MyMath.h"
#include "Sprite.h"
#include "Player.h"

/// <summary>
/// 操作ガイド
/// </summary>
class UIManager
{
public:

	/// <summary>
	/// インスタンス生成
	/// </summary>
	/// <returns>インスタンス</returns>
	static UIManager& GetInstance();

	/// <summary>
	/// フレーム更新型スプライトを作成
	/// </summary>
	/// <param name="sprite">スプライトを読み込む</param>
	void FrameSprite(EngineLayer::Sprite* sprite);
	/// <summary>
	/// 一度だけ読み取る型スプライトを作成
	/// </summary>
	/// <param name="sprite">スプライトを読み込む</param>
	void FixedSprite(EngineLayer::Sprite* sprite);

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw();

	/// <summary>
	/// 解放処理
	/// </summary>
	void Finalize();

private:
	//インスタン
	static std::unique_ptr<UIManager> sInstance_;
	//default_deleteを設定(解放処理を行える)
	friend struct std::default_delete<UIManager>;
	//フレーム更新スプライトたち
	std::vector<EngineLayer::Sprite*> frameSprites_;
	//一度読み取りの保存されるスプライトたち
	std::vector<EngineLayer::Sprite*> fixedSprites_;
};

