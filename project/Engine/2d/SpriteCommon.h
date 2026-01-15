#pragma once
#include "DirectXCommon.h"
#include "Pipeline.h" 

/// <summary>
/// スプライトの共有部分
/// </summary>
class SpriteCommon : public Pipeline{
public:
	/// <summary>
	/// インスタンス生成
	/// </summary>
	/// <returns></returns>
	static SpriteCommon& GetInstance();
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

	//インスタンス
	static std::unique_ptr<SpriteCommon> sInstance_;
	//default_deleteを設定(解放処理を行える)
	friend struct std::default_delete<SpriteCommon>;
};