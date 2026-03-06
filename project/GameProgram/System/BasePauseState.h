#pragma once

class PauseScreen;
/// <summary>
/// ポーズ選択処理(基盤)
/// </summary>
class BasePauseState {
public:
	/// <summary>
	/// 更新処理
	/// </summary>
	/// <param name="pauseScreen">ポーズスクリーンクラス</param>
	virtual void Update(PauseScreen& pauseScreen) = 0;
	/// <summary>
	/// 描画処理
	/// </summary>
	/// <param name="pauseScreen">ポーズスクリーンクラス</param>
	virtual void Draw(PauseScreen& pauseScreen) = 0;
};
/// <summary>
/// 選択中(通常)
/// </summary>
class PauseMove : public BasePauseState {
public:
	/// <summary>
	/// 更新処理
	/// </summary>
	/// <param name="pauseScreen">ポーズスクリーンクラス</param>
	void Update(PauseScreen& pauseScreen) override;
	/// <summary>
	/// 描画処理
	/// </summary>
	/// <param name="pauseScreen">ポーズスクリーンクラス</param>
	void Draw(PauseScreen& pauseScreen) override;
};
/// <summary>
/// 操作ガイドを選択
/// </summary>
class PauseSelectGuide : public BasePauseState {
public:
	/// <summary>
	/// 更新処理
	/// </summary>
	/// <param name="pauseScreen">ポーズスクリーンクラス</param>
	void Update(PauseScreen& pauseScreen) override;
	/// <summary>
	/// 描画処理
	/// </summary>
	/// <param name="pauseScreen">ポーズスクリーンクラス</param>
	void Draw(PauseScreen& pauseScreen) override;
};
/// <summary>
/// 戻るを選択(ゲームからセレクト、セレクトからタイトル)
/// </summary>
class PauseSelectSceneChange : public BasePauseState {
public:
	/// <summary>
	/// 更新処理
	/// </summary>
	/// <param name="pauseScreen">ポーズスクリーンクラス</param>
	void Update(PauseScreen& pauseScreen) override;
	/// <summary>
	/// 描画処理
	/// </summary>
	/// <param name="pauseScreen">ポーズスクリーンクラス</param>
	void Draw(PauseScreen& pauseScreen) override;
};