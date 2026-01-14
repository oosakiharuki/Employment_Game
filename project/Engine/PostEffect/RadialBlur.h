#pragma once
#include "BasePostEffect.h"
/// <summary>
/// ポストエフェクト_RadialBlur(IPostEffectの派生クラス)
/// </summary>
class  RadialBlur : public BasePostEffect {
public:	
	void Finalize() override;
	void Command() override;
private:
	/// <summary>
	/// ルートシグネチャ
	/// </summary>
	void RootSignature() override;

	/// <summary>
	/// PixelShaderを作成(ポストエフェクトはこれ以外は共通)
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