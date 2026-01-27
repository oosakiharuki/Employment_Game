#pragma once
#include "Sprite.h"
#include "MyMath.h"
#include "Input.h"
#include "BasePauseState.h"

/// <summary>
/// ポ－ズ画面
/// </summary>
class PauseScreen {
public:

	void PauseFlag(bool a) { isPause_ = a; }

	/// <summary>
	/// 初期化処理
	/// </summary>
	void Initialize(const std::string& textureName, const std::string& changeScene);
	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();
	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw();


	bool IsPauseFinish() { return isPause_; }

	void ResetPauseSprite();


	void MoveSprite();

	/// <summary>
	/// 操作ガイド更新処理
	/// </summary>
	void UpdateGuide();

	/// <summary>
	/// 操作ガイド描画処理
	/// </summary>
	void DrawGuide();

	void DrawSelectMode();

	const std::string& GetNextSceneName() { return nextSceneName_; }

	void SetPauseState(std::unique_ptr<BasePauseState> nextState) { 
		pauseState_.reset();
		pauseState_ = std::move(nextState); 
	}
	/// <summary>
	/// 項目から選択
	/// </summary>
	void SelectMode();

private:
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

	std::string nextSceneName_;
};
