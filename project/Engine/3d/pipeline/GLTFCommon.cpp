/// ---------------------------------------------
///
/// .gltfのオブジェクト読み込み用のパイプライン
/// 
/// ---------------------------------------------
#include "GLTFCommon.h"

using namespace Logger;

/// <summary>
/// エンジン層
/// </summary>
namespace EngineLayer {

	std::unique_ptr<GLTFCommon> GLTFCommon::sInstance_ = nullptr;

	GLTFCommon& GLTFCommon::GetInstance() {
		if (sInstance_ == nullptr) {
			sInstance_ = std::make_unique<GLTFCommon>();
		}
		return *sInstance_;
	}
	void GLTFCommon::Finalize() {
		sInstance_.reset();
	}

	void GLTFCommon::Initialize() {
		GraphicsPipeline();
	}

	void GLTFCommon::RootSignature() {
		//RootSignature
		descriptionRootSignature_.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

		CreateDescriptorRange(descriptorRange_, 0);
		CreateDescriptorRange(descriptorRangeIBL_, 1);

		//RootParameter作成__
		RootParameterCommon();

		CreateCBV(D3D12_SHADER_VISIBILITY_PIXEL, 1);//[3] ps b1
		CreateCBV(D3D12_SHADER_VISIBILITY_PIXEL, 2);//[4] ps b2
		CreateCBV(D3D12_SHADER_VISIBILITY_PIXEL, 3);//[5] ps b3
		CreateCBV(D3D12_SHADER_VISIBILITY_PIXEL, 4);//[6] ps b4
		CreateCBV(D3D12_SHADER_VISIBILITY_PIXEL, 5);//[7] ps b5

		//IBL t1
		CreateTABLE(D3D12_SHADER_VISIBILITY_PIXEL, descriptorRangeIBL_);//[8] ps t1
		rootParameters_[8].DescriptorTable.NumDescriptorRanges = _countof(descriptorRangeIBL_);

		//2でまとめる
		DefaultSampler(0);//s1

		IntroduceRootParameters();
		IntroduceSamplers();
	}

	void GLTFCommon::CreateInputLayout() {
		InputElementDescCommon();

		CreateInputElementDesc("NORMAL", DXGI_FORMAT_R32G32B32_FLOAT);
		CreateInputElementDesc("WORLDPOSITION", DXGI_FORMAT_R32G32B32_FLOAT);

		IntroduceInputElementDesc();
	}

	void GLTFCommon::CreateBlend() {
		blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
		blendDesc.RenderTarget[0].BlendEnable = TRUE;

		blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
		blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;

		blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
		blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	}

	void GLTFCommon::CreateRasterizer() {
		rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;//表裏表示
		rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;
	}

	void GLTFCommon::CreateVertexShader() {
		vertexShaderBlob = ShaderManager::GetInstance().CompileShader(L"resource/shaders/Object3d.VS.hlsl", L"vs_6_0");
		assert(vertexShaderBlob != nullptr);
	}

	void GLTFCommon::CreatePixelShader() {
		pixelShaderBlob = ShaderManager::GetInstance().CompileShader(L"resource/shaders/Object3d_glTF.PS.hlsl", L"ps_6_0");
		assert(pixelShaderBlob != nullptr);
	}

	void GLTFCommon::CreateDepthStencil() {
		depthStencilDesc.DepthEnable = true;
		depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
		depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
	}

	void GLTFCommon::Command() {
		DirectXCommon::GetInstance().GetCommandList()->SetGraphicsRootSignature(rootSignature_.Get());
		DirectXCommon::GetInstance().GetCommandList()->SetPipelineState(graphicsPipelineState_.Get());
		DirectXCommon::GetInstance().GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	}
}