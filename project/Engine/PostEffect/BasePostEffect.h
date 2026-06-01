#pragma once
#include <cstdint>
#include "DirectXCommon.h"
#include "Input.h"
#include "ImGuiManager.h"
#include "Pipeline.h"


/// <summary>
/// エンジン層
/// </summary>
namespace EngineLayer {
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
	class BasePostEffect : public Pipeline {
	protected:
		//postEffect特融処理

		static int sEffectNo_;

		uint32_t srvIndex_;
		D3D12_CPU_DESCRIPTOR_HANDLE srvHandleCPU_;
		D3D12_GPU_DESCRIPTOR_HANDLE srvHandleGPU_;

		///DepthOutlineで使う
		D3D12_CPU_DESCRIPTOR_HANDLE srvHandleCPUDepth_;
		D3D12_GPU_DESCRIPTOR_HANDLE srvHandleGPUDepth_;

		D3D12_INPUT_ELEMENT_DESC inputElementDescs[2] = {};

	public:
		/// <summary>
		/// 初期化処理
		/// </summary>
		void Initialize() override;

		/// <summary>
		/// 削除処理
		/// </summary>
		virtual void Finalize() = 0;

		/// <summary>
		/// 描画コマンド
		/// </summary>
		virtual void Command() = 0;

		/// <summary>
		/// ポストエフェクト番号
		/// </summary>
		/// <returns></returns>
		int GetEffectNo() { return sEffectNo_; }

		/// <summary>
		/// エフェクト特有初期化処理
		/// </summary>
		virtual void EffectInit() = 0;

		/// <summary>
		/// imGuiで値を動かせるようにする
		/// </summary>
		virtual void EffectUpdate() = 0;
		/// <summary>
		/// デストラクタ
		/// </summary>
		virtual ~BasePostEffect();
		/// <summary>
		/// ポストエフェクト変更処理
		/// </summary>
		void ChangeNumber();
		/// <summary>
		/// エフェクトのGPU
		/// </summary>
		/// <returns>SRVHandleGPU</returns>
		D3D12_GPU_DESCRIPTOR_HANDLE GetEffect() { return srvHandleGPU_; }

		/// <summary>
		/// ポストエフェクトで共通して使われるルートシグネチャ内の処理
		/// </summary>
		void PostEffectRootSignatureCommon();

	private:

		/// <summary>
		/// InputLayoutを作成
		/// </summary>
		void CreateInputLayout() override;

		/// <summary>
		/// Blendを作成
		/// </summary>
		void CreateBlend() override;

		/// <summary>
		/// Rasterizerを作成
		/// </summary>
		void CreateRasterizer() override;

		/// <summary>
		/// VertexShaderを作成
		/// </summary>
		void CreateVertexShader() override;

		/// <summary>
		/// DepthStencilの作成
		/// </summary>
		void CreateDepthStencil() override;
	};
}