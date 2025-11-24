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
		isFading_ = true;
		fadeType_ = type;
	}


	/// <summary>
	/// フェードの最中か
	/// </summary>
	/// <returns></returns>フェード中ならtrue
	bool GetIsFadeing() const{ return isFading_; }
	
	void SetMaskTexture(const std::string textureFile) { dissolveTexture_ = textureFile; }
	void SetBackGround(const std::string textureFile) { sprite_->SetTextureFile(textureFile); }

private:

	/// <summary>
	/// フェードイン(シーン移動画面)
	/// </summary>
	void FadeIn();
	/// <summary>
	/// フェードアウト(ゲーム画面)
	/// </summary>
	void FadeOut();

	static FadeScreen* sInstance;

	//使用するスプライト
	std::unique_ptr<Sprite> sprite_;

	//フェード中フラグ
	bool isFading_ = true;
	//フェードのタイプ
	FadeType fadeType_ = type_fadeOut;

	//Dissolve(ポストエフェクト)で溶け具合の変数
	float degress_ = 0.0f;
	//edgeサイズ
	const float kEdgeSize = 0.01f;

	const float kDegressMax = 1.0f;


	//使用するポストエフェクト
	//std::unique_ptr<IPostEffects> postEffect_ = nullptr;
	std::shared_ptr<Dissolve> dissolve_;
	std::string dissolveTexture_;

	FadeScreen() = default;
	~FadeScreen() = default;
	FadeScreen(FadeScreen&) = default;
	FadeScreen& operator=(FadeScreen&) = default;
};