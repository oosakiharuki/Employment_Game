#pragma once
#include "Sprite.h"
#include "Dissolve.h"

/// <summary>
/// フェードスクリーン
/// </summary>
class FadeScreen {
public:
	static FadeScreen* GetInstance();

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
	/// 削除処理
	/// </summary>
	void Finalize();

	/// <summary>
	/// フェードイン(シーン移動画面)
	/// </summary>
	void FedeIn();
	/// <summary>
	/// フェードアウト(ゲーム画面)
	/// </summary>
	void FedeOut();

	/// <summary>
	/// フェードの最中か
	/// </summary>
	/// <returns></returns>フェード中ならtrue
	bool GetIsFadeing() const{ return isFading; }

private:

	static FadeScreen* instance;

	//使用するスプライト
	std::unique_ptr<Sprite> sprite;

	//秒数時間
	const float deltaTime = 1.0f / 60.0f;

	//フェード中フラグ
	bool isFading = false;
	//Dissolve(ポストエフェクト)で溶け具合の変数
	float degress = 0.0f;

	//使用するポストエフェクト
	std::unique_ptr<IPostEffects> postEffect_ = nullptr;
	Dissolve* dissolve;

	FadeScreen() = default;
	~FadeScreen() = default;
	FadeScreen(FadeScreen&) = default;
	FadeScreen& operator=(FadeScreen&) = default;
};