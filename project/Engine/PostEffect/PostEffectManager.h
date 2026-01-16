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
	/// <param name="dxCommon"></param>
	void Initialize(DirectXCommon* dxCommon);
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
	/// <returns></returns>
	static std::shared_ptr<PostEffectManager> GetInstance();

private:
	/// <summary>
	/// ポストエフェクト変更
	/// </summary>
	/// <param name="prev"></param>前
	/// <param name="current"></param>次
	void Change(int prev, int current);

	//最大数
	static const uint32_t kEffectNum_ = Max;

	std::unique_ptr<BasePostEffect> effectArr_[kEffectNum_];

	int currentNo_;//現在シーン
	int prevNo_;//前シーン

	static std::shared_ptr<PostEffectManager> sInstance_;

	DirectXCommon* dxCommon_;
};