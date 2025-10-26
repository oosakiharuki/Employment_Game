#pragma once
#include "Sprite.h"
#include "Dissolve.h"

enum FadeType {
	type_fadeIn,
	type_fadeOut
};

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

	void FadeStart(const FadeType& type) {
		isFading = true;
		FadeType_ = type;
	}


	/// <summary>
	/// フェードの最中か
	/// </summary>
	/// <returns></returns>フェード中ならtrue
	bool GetIsFadeing() const{ return isFading; }
	
private:

	/// <summary>
	/// フェードイン(シーン移動画面)
	/// </summary>
	void FadeIn();
	/// <summary>
	/// フェードアウト(ゲーム画面)
	/// </summary>
	void FadeOut();

	static FadeScreen* instance;

	//使用するスプライト
	std::unique_ptr<Sprite> sprite;

	//秒数時間
	const float deltaTime = 1.0f / 60.0f;

	//フェード中フラグ
	bool isFading = true;
	//フェードのタイプ
	FadeType FadeType_ = type_fadeOut;

	//Dissolve(ポストエフェクト)で溶け具合の変数
	float degress = 0.0f;

	//使用するポストエフェクト
	//std::unique_ptr<IPostEffects> postEffect_ = nullptr;
	std::unique_ptr<Dissolve> dissolve;

	FadeScreen() = default;
	~FadeScreen() = default;
	FadeScreen(FadeScreen&) = default;
	FadeScreen& operator=(FadeScreen&) = default;
};