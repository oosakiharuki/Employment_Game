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
	/// <summary>
	/// インスタンス生成
	/// </summary>
	/// <returns>インスタンス</returns>
	static PauseScreen& GetInstance();

	/// <summary>
	/// ポーズをオフに
	/// </summary>
	void OffPause() { isPause_ = false; }

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
	/// <returns>trueでポーズ中</returns>
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
	void BeforeChangeScene(const std::string& textureName, std::unique_ptr<EngineLayer::BaseScene> changeScene);

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
	std::unique_ptr<EngineLayer::BaseScene> GetNextScene() { return std::move(nextScene_); }
	
	/// <summary>
	/// ポーズステート(状態遷移)
	/// </summary>
	/// <param name="nextState">変更先のステート</param>
	void ChangePauseState(std::unique_ptr<BasePauseState> nextState);
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
	/// <summary>
	/// ゲームパッドで選択
	/// </summary>
	void SelectGamePad();
	/// <summary>
	/// キーボードで選択
	/// </summary>
	void SelectKeyBoard();

	//スプライトたち
	std::unique_ptr<EngineLayer::Sprite> backScreen_ = nullptr;
	std::unique_ptr<EngineLayer::Sprite> selectPoint_ = nullptr;
	std::unique_ptr<EngineLayer::Sprite> spriteSelectSceneChange_ = nullptr;
	std::unique_ptr<EngineLayer::Sprite> spriteSelectGuide_ = nullptr;
	std::unique_ptr<EngineLayer::Sprite> spriteSelectReturn_ = nullptr;
	std::unique_ptr<EngineLayer::Sprite> spriteGuide_ = nullptr;

	//プレイヤーの選択枠座標
	Vector2 select_;
	//項目の座標
	Vector2 moveSelectReturn_;
	Vector2 moveSelectGuide_;
	Vector2 moveSelectSceneChange_;
	//項目を移動する(始点)
	static constexpr Vector2 kSelectReturnStartPosition_ = { -320,160 };
	static constexpr Vector2 kSelectGuideStartPosition_ = { -320,368 };
	static constexpr Vector2 kSelectSceneChangeStartPosition_ = { -320,576 };
	//項目を移動する(終点)
	static constexpr Vector2 kSelectReturnEndPosition_ = { 64,160 };
	static constexpr Vector2 kSelectGuideEndPosition_ = { 120,368 };
	static constexpr Vector2 kSelectSceneChangeEndPosition_ = { 176,576 };
	//EaseOutで使用
	float interpolation_ = 0.0f;

	//項目の判別
	uint32_t selectNumber = 0;
	//選択したフラグ
	bool isSelect_ = false;
	//ポーズしているフラグ
	bool isPause_ = false;
	//ゲームパッドでスティックを傾けたかフラグ
	bool isMoveStick_ = false;
	//ポーズ選択後のステートパターン
	std::unique_ptr<BasePauseState> pauseState_ = nullptr;
	//シーンを変更する時に使うBaseScene
	std::unique_ptr<EngineLayer::BaseScene> nextScene_;
};
