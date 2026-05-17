#pragma once
#include "DirectXCommon.h"
#include "Pipeline.h"
#include "Camera.h"

/// <summary>
/// エンジン層
/// </summary>
namespace EngineLayer {
	/// <summary>
	/// .gltf版のオブジェクト共有部分
	/// </summary>
	class GLTFCommon : public Pipeline {
	public:
		/// <summary>
		/// インスタンス生成
		/// </summary>
		/// <returns>インスタンス</returns>
		static GLTFCommon& GetInstance();
		/// <summary>
		/// 解放処理
		/// </summary>
		void Finalize();
		/// <summary>
		/// 初期化処理
		/// </summary>
		void Initialize() override;
		/// <summary>
		/// 描画コマンド
		/// </summary>
		void Command();

		/// <summary>
		/// setter_デフォルトカメラ
		/// </summary>
		/// <returns>シーンで使用しているカメラ</returns>
		void SetDefaultCamera(Camera* camera) { this->defaultCamera_ = camera; }
		/// <summary>
		/// getter_デフォルトカメラ
		/// </summary>
		/// <returns>現在使用しているカメラ</returns>
		Camera* GetDefaultCamera() const { return defaultCamera_; }

	private:
		/// <summary>
		/// ルートシグネチャ
		/// </summary>
		void RootSignature() override;

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
		/// PixelShaderを作成
		/// </summary>
		void CreatePixelShader() override;

		/// <summary>
		/// DepthStencilの作成
		/// </summary>
		void CreateDepthStencil() override;

		//RootSignature
		D3D12_DESCRIPTOR_RANGE descriptorRangeIBL_[1] = {};//iamge_based_lighting

		Camera* defaultCamera_ = nullptr;
		//インスタンス
		static std::unique_ptr<GLTFCommon> sInstance_;
		//default_deleteを設定(解放処理を行える)
		friend struct std::default_delete<GLTFCommon>;
	};
}