#pragma once
#include "BasePostEffect.h"
/// <summary>
/// ポストエフェクト_Random(IPostEffectの派生クラス)
/// </summary>
class Random : public BasePostEffect {
public:
	void Finalize() override;
	void Command() override;
private:
	/// <summary>
	/// ルートシグネチャ
	/// </summary>
	void RootSignature() override;

	/// <summary>
	/// PixelSharderを作成(ポストエフェクトはこれ以外は共通)
	/// </summary>
	void CreatePixelSharder() override;

	/// <summary>
	/// ポストエフェクトの初期化処理
	/// </summary>
	void EffectInit() override;

	/// <summary>
	/// ポストエフェクトの更新処理
	/// </summary>
	void EffectUpdate() override;

	struct RandomFunction {
		float randomeTimer;
	};

	RandomFunction* randomFunction_;

	Microsoft::WRL::ComPtr<ID3D12Resource> RandomResoure_;
};