#pragma once
#include "BasePostEffect.h"
/// <summary>
/// ポストエフェクト_LuminanceBasedOutline(IPostEffectの派生クラス)
/// </summary>
class LuminanceBasedOutline: public BasePostEffect {
public:
	/// <summary>
	/// 解放処理
	/// </summary>
	void Finalize() override;
	/// <summary>
	/// 処理コマンド(描画)
	/// </summary>
	void Command() override;
private:
	/// <summary>
	/// ルートシグネチャ
	/// </summary>
	void RootSignature() override;

	/// <summary>
	/// PixelShaderを作成
	/// </summary>
	void CreatePixelShader() override;

	/// <summary>
	/// ポストエフェクトの初期化処理
	/// </summary>
	void EffectInit() override;

	/// <summary>
	/// ポストエフェクトの更新処理
	/// </summary>
	void EffectUpdate() override;
};