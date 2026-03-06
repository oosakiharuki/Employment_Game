#pragma once
#include "BasePostEffect.h"
#include "Vector3.h"
/// <summary>
/// ポストエフェクト_Dissolve(IPostEffectの派生クラス)
/// </summary>
class Dissolve : public BasePostEffect {
public:
	/// <summary>
	/// 解放処理
	/// </summary>
	void Finalize() override;
	/// <summary>
	/// 処理コマンド(描画)
	/// </summary>
	void Command() override;
	/// <summary>
	/// 背景
	/// </summary>
	/// <param name="gpu">GPUハンドル</param>
	/// <param name="textureFile">テクスチャファイルパス</param>
	void SetBackGround(D3D12_GPU_DESCRIPTOR_HANDLE gpu, const std::string& textureFile);
	/// <summary>
	/// 溶かし度合
	/// </summary>
	/// <param name="value">度合の数値</param>
	void Degree(float value);
	/// <summary>
	/// Edgeの範囲
	/// </summary>
	/// <param name="value">範囲の数値</param>
	void EdgeSize(float value);

	struct Threshold {
		float degree;
		float edgeSize;
	};

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

	//RootSignature
	D3D12_DESCRIPTOR_RANGE descriptorRangeDissolve_[1] = {};

	std::string textureFileName_;

	Microsoft::WRL::ComPtr<ID3D12Resource> dissolveResource_;
	
	Microsoft::WRL::ComPtr<ID3D12Resource> backGroundResource_;

	D3D12_CPU_DESCRIPTOR_HANDLE TextureCPU_;
	D3D12_GPU_DESCRIPTOR_HANDLE TextureGPU_;
	bool isFade_ = false;


	Threshold* threshold_;
	bool isChangeMask_ = false;
};