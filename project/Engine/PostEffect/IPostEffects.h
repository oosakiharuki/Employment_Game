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
	Mode_Grayscale,//グレースケール
	Mode_Vignette,//ビネット
	Mode_BoxFillter,//ボックスフィルター
	Mode_GaussianFillter,//ガウシアンフィルター
	Mode_LuminanceBasedOutline,//アウトライン
	Mode_DepthBasedOutline,//ディープアウトライン
	Mode_RadialBlur,//ブラー
	Mode_Dissolve,//溶かす
	Mode_Random,//ランダム
	Max,//最大値(エフェクトではない)
};

/// <summary>
/// ポストエフェクト(PostEffect)の基盤クラス
/// </summary>
class IPostEffects {
protected:
	//postEffect特融処理

	static int sEffectNo_;

	DirectXCommon* dxCommon_;

	uint32_t srvIndex_;
	D3D12_CPU_DESCRIPTOR_HANDLE srvHandleCPU_;
	D3D12_GPU_DESCRIPTOR_HANDLE srvHandleGPU_;

	///DepthOutlineで使う
	D3D12_CPU_DESCRIPTOR_HANDLE srvHandleCPUDepth_;
	D3D12_GPU_DESCRIPTOR_HANDLE srvHandleGPUDepth_;

	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_ = nullptr;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineState_ = nullptr;

	///RootSignature
	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature_{};
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
	/// 描画コマンド
	/// </summary>
	virtual void Command() = 0;
	
	/// <summary>
	/// ポストエフェクト番号
	/// </summary>
	/// <returns></returns>
	int GetEffectNo() { return sEffectNo_; }

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

	D3D12_GPU_DESCRIPTOR_HANDLE GetEffect() { return srvHandleGPU_; }
};