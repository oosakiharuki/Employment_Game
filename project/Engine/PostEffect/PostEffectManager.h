#pragma once
#include "BasePostEffect.h"
#include "RadialBlur.h"
#include "Vignette.h"
#include "BoxFilter.h"
#include "GaussianFilter.h"
#include "LuminanceBasedOutline.h"
#include "Grayscale.h"
#include "Normal_Image.h"
#include "DepthBasedOutline.h"
#include "Dissolve.h"
#include "Random.h"
#include "random"

/// <summary>
/// ポストエフェクトマネージャ
/// </summary>
class PostEffectManager {
public:
	/// <summary>
	/// 初期化処理
	/// </summary>
	void Initialize();
	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();
	/// <summary>
	/// 描画コマンド
	/// </summary>
	void Command();
	/// <summary>
	/// 解放処理
	/// </summary>
	void Finalize();
	/// <summary>
	/// インスタンス生成
	/// </summary>
	/// <returns>インスタンス</returns>
	static PostEffectManager& GetInstance();

private:
	/// <summary>
	/// ポストエフェクト変更
	/// </summary>
	/// <param name="prev">前のポストエフェクト</param>
	/// <param name="current">次のポストエフェクト</param>
	void Change(int prev, int current);

	//最大数
	static const uint32_t kEffectNum_ = Max;

	std::unique_ptr<BasePostEffect> effectArr_[kEffectNum_];

	int currentNo_;//現在シーン
	int prevNo_;//前シーン

	//インスタンス
	static std::unique_ptr<PostEffectManager> sInstance_;
	//default_deleteを設定(解放処理を行える)
	friend struct std::default_delete<PostEffectManager>;
};