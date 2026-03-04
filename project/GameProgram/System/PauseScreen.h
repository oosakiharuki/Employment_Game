#pragma once
#include "Sprite.h"
#include "MyMath.h"
#include "Input.h"
#include "BasePauseState.h"
#include "BaseScene.h"

/// <summary>
/// ポ－ズ画面
/// </summary>
class PauseScreen {
public:

	static PauseScreen& GetInstance();

	void PauseFlag(bool a) { isPause_ = a; }

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
	/// <summary>
	/// 解放処理
	/// </summary>
	void Finalize();

	/// <summary>
	/// ポーズ中であるか
	/// </summary>
	/// <returns></returns>
	bool IsPause() { return isPause_; }

	/// <summary>
	/// ポーズのUIの初期化
	/// </summary>
	void ResetPauseSprite();

	/// <summary>
	/// シーン移動の変更時、使うテクスチャの指定
	/// </summary>
	/// <param name="textureName">テクスチャ名</param>
	/// <param name="changeScene">移動するシーン名</param>
	void BeforeChangeScene(const std::string& textureName, std::unique_ptr<BaseScene> changeScene);

	/// <summary>
	/// 動く選択UI
	/// </summary>
	void MoveSprite();

	/// <summary>
	/// 操作ガイド更新処理
	/// </summary>
	void UpdateGuide();

	/// <summary>
	/// 操作ガイド用の描画処理
	/// </summary>
	void DrawGuide();

	/// <summary>
	/// 選択UI用の描画処理
	/// </summary>
	void DrawSelectMode();

	/// <summary>
	/// getter_次のシーン
	/// </summary>
	/// <returns>あらかじめ指定したシーン名</returns>
	std::unique_ptr<BaseScene> GetNextScene() { return std::move(nextScene_); }

	/// <summary>
	/// setter_ポーズステート(状態遷移)
	/// </summary>
	/// <param name="nextState">変更先のステート</param>
	void SetPauseState(std::unique_ptr<BasePauseState> nextState) { 
		pauseState_.reset();//一度リセット
		pauseState_ = std::move(nextState); //変更する
	}
	/// <summary>
	/// 項目から選択
	/// </summary>
	void SelectMode();

private:

	static std::unique_ptr<PauseScreen> sInstance_;
	//default_deleteを設定(解放処理を行える)
	friend struct std::default_delete<PauseScreen>;
	/// <summary>
	/// 選んだ後の処理
	/// </summary>
	void SelectResult();

	std::unique_ptr<Sprite> backScreen_ = nullptr;

	std::unique_ptr<Sprite> selectPoint_ = nullptr;

	std::unique_ptr<Sprite> spriteSelectSceneChange_ = nullptr;
	std::unique_ptr<Sprite> spriteSelectGuide_ = nullptr;
	std::unique_ptr<Sprite> spriteSelectReturn_ = nullptr;

	std::unique_ptr<Sprite> spriteGuide_ = nullptr;


	Vector2 select_;

	Vector2 moveSelectReturn_;
	Vector2 moveSelectGuide_;
	Vector2 moveSelectSceneChange_;

	const Vector2 kSelectReturnStartPosition_ = { -320,160 };
	const Vector2 kSelectGuideStartPosition_ = { -320,368 };
	const Vector2 kSelectSceneChangeStartPosition_ = { -320,576 };

	const Vector2 kSelectReturnEndPosition_ = { 64,160 };
	const Vector2 kSelectGuideEndPosition_ = { 120,368 };
	const Vector2 kSelectSceneChangeEndPosition_ = { 176,576 };

	float interpolation_ = 0.0f;


	uint32_t selectNumber = 0;

	bool isSelect_ = false;

	bool isPause_ = false;

	std::unique_ptr<BasePauseState> pauseState_ = nullptr;

	std::unique_ptr<BaseScene> nextScene_;
};
