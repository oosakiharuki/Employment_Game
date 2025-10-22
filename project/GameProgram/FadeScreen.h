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

	std::unique_ptr<Sprite> sprite;
	Vector4 color = {1,1,1,1};

	const float deltaTime = 1.0f / 60.0f;

	bool isFading = false;
	float degress = 0.0f;

	//使用するポストエフェクト
	std::unique_ptr<IPostEffects> postEffect_ = nullptr;
	Dissolve* dissolve;

	FadeScreen() = default;
	~FadeScreen() = default;
	FadeScreen(FadeScreen&) = default;
	FadeScreen& operator=(FadeScreen&) = default;
};