#pragma once
#include <cstdint>
#include "DirectXCommon.h"
#include "Input.h"
#include "ImGuiManager.h"

/// <summary>
/// ポストエフェクトの種類
/// </summary>
enum EFFECT {
	Mode_Normal_Image,//None 何もなし
	Mode_Grayscale,
	Mode_Vignette,
	Mode_BoxFillter,
	Mode_GaussianFillter,
	Mode_LuminanceBasedOutline,
	Mode_DepthBasedOutline,
	Mode_RadialBlur,
	Mode_Dissolve,
	Mode_Random,
	Max,//最大
};

/// <summary>
/// ポストエフェクト(PostEffect)の基盤クラス
/// </summary>
class IPostEffects {
protected:
	//postEffect特融処理

	static int sEffectNo;

	DirectXCommon* dxCommon_;

	uint32_t srvIndex;
	D3D12_CPU_DESCRIPTOR_HANDLE srvHandleCPU;
	D3D12_GPU_DESCRIPTOR_HANDLE srvHandleGPU;

	///DepthOutlineで使う
	D3D12_CPU_DESCRIPTOR_HANDLE srvHandleCPU2;
	D3D12_GPU_DESCRIPTOR_HANDLE srvHandleGPU2;

	Microsoft::WRL::ComPtr < ID3D12RootSignature> rootSignature = nullptr;
	Microsoft::WRL::ComPtr < ID3D12PipelineState> graphicsPipelineState = nullptr;

	///RootSignature
	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
	//他はPixselShader等で配列が変わったりするため固有にする

public:
	/// <summary>
	/// 削除処理
	/// </summary>
	virtual void Finalize() = 0;
	/// <summary>
	/// 初期化処理
	/// </summary>
	/// <param name="dxCommon"></param>
	virtual void Initialize(DirectXCommon* dxCommon) = 0;

	/// <summary>
	/// 描画モード変更処理
	/// </summary>
	virtual void Command() = 0;
	
	int GetEffectNo() { return sEffectNo; }

	//PSO
	
	/// <summary>
	/// ルートシグネチャの作成
	/// </summary>
	virtual void RootSignature() = 0;
	/// <summary>
	/// パイプラインの作成
	/// </summary>
	virtual void GraphicsPipeline() = 0;
	/// <summary>
	/// imguiで値を動かせるようにする
	/// </summary>
	virtual void EffectUpdate() = 0;

	virtual ~IPostEffects();
	/// <summary>
	/// ポストエフェクト変更処理
	/// </summary>
	void ChangeNumber();

	D3D12_GPU_DESCRIPTOR_HANDLE GetEffect() { return srvHandleGPU; }
	//DirectXCommon* GetDirectXCommon()const { return dxCommon_; }
};