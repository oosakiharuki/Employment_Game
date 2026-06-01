#pragma once
#include "DirectXCommon.h"
#include "Pipeline.h"
#include "Camera.h"

/// <summary>
/// エンジン層
/// </summary>
namespace EngineLayer {
	/// <summary>
	/// ,objのオブジェクト共有部分
	/// </summary>
	class Object3dCommon : public Pipeline {
	public:
		/// <summary>
		/// インスタンス生成
		/// </summary>
		/// <returns></returns>
		static Object3dCommon& GetInstance();
		/// <summary>
		/// 解放処理
		/// </summary>
		void Finalize();
		/// <summary>
		/// 初期化処理
		/// </summary>
		/// <param name="dxCommon"></param>
		void Initialize() override;
		/// <summary>
		/// 描画コマンド
		/// </summary>
		void Command();
		/// <summary>
		/// setter_デフォルトカメラ
		/// </summary>
		/// <param name="camera"></param> 現在使用しているカメラ
		void SetDefaultCamera(Camera* camera) { this->defaultCamera_ = camera; }
		/// <summary>
		/// getter_デフォルトカメラ
		/// </summary>
		/// <returns></returns>
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

		Camera* defaultCamera_ = nullptr;
		//インスタンス
		static std::unique_ptr<Object3dCommon> sInstance_;
		//default_deleteを設定(解放処理を行える)
		friend struct std::default_delete<Object3dCommon>;
	};
}