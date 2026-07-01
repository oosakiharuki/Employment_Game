#pragma once
#include "wrl.h"
#include "d3d12.h"
#include <externals/DirectXTex/DirectXTex.h>
#include <Vector4.h>
#include <memory>

namespace EngineLayer {
	/// <summary>
	/// DirectXのリソースマネージャー
	/// </summary>
	class D3D12CreateResourceManager {
	public:
		/// <summary>
		/// インスタンス生成
		/// </summary>
		/// <returns></returns>
		static D3D12CreateResourceManager& GetInstance();
		/// <summary>
		/// シェーダで使用するバッファ
		/// </summary>
		/// <param name="sizeInBytes">バイトサイズ</param>
		/// <returns>出来上がった</returns>
		Microsoft::WRL::ComPtr<ID3D12Resource> CreateBufferResource(size_t sizeInBytes);
		/// <summary>
		/// シェーダで使用するバッファ(RenderTextureResourceで使用)
		/// </summary>
		/// <param name="metadata">メタデータ</param>
		/// <returns>バッファ</returns>
		Microsoft::WRL::ComPtr<ID3D12Resource> CreateTextureResource(const DirectX::TexMetadata& metadata);
		/// <summary>
		/// テクスチャデータ
		/// </summary>
		/// <param name="texture">テクスチャ名</param>
		/// <param name="mipImages">ミップマップ</param>
		/// <returns>テクスチャデータリソースを作成</returns>
		Microsoft::WRL::ComPtr<ID3D12Resource> UploadTextureData(Microsoft::WRL::ComPtr<ID3D12Resource> texture, const DirectX::ScratchImage& mipImages);

		/// <summary>
		/// 深度ステンシルリソースを作成
		/// </summary>
		/// <returns></returns>
		Microsoft::WRL::ComPtr<ID3D12Resource> CreateDepthStencilResource();

		/// <summary>
		/// RTVのリソース作成(device,width,heightは省略)
		/// </summary>
		/// <param name="format">フォーマット</param>
		/// <param name="clearColor">clearした時の色</param>
		/// <returns>出来上がったRTVのリソース</returns>
		Microsoft::WRL::ComPtr<ID3D12Resource> CreateRenderTextureResource(DXGI_FORMAT format, const Vector4& clearColor);

		/// <summary>
		/// インスタンス解放キー
		/// </summary>
		class InstanceKey {
		private:
			InstanceKey() = default;
			friend class D3D12CreateResourceManager;
		};
		explicit D3D12CreateResourceManager(InstanceKey) {}

	private:

		static std::unique_ptr<D3D12CreateResourceManager> sInstance_;

	};
}