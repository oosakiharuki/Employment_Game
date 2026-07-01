#pragma once
#include "DirectXCommon.h"

/// <summary>
/// エンジン層
/// </summary>
namespace EngineLayer {
	/// <summary>
	/// SRVマネージャ
	/// </summary>
	class SrvManager {
	public:
		/// <summary>
		/// インスタンス生成
		/// </summary>
		/// <returns>インスタンス</returns>
		static SrvManager& GetInstance();
		/// <summary>
		/// 解放処理
		/// </summary>
		void Finalize();
		/// <summary>
		/// 初期化処理
		/// </summary>
		void Initialize();
		/// <summary>
		/// カウント
		/// </summary>
		/// <returns>SRVの数</returns>
		uint32_t Allocate();
		/// <summary>
		/// sMaxSRVCount以上でないか
		/// </summary>
		/// <returns>trueで上限に達した、falseで上限に達していない</returns>
		bool Max();
		/// <summary>
		/// CPUデスクリプターハンドル追加
		/// </summary>
		/// <param name="index">SRVIndex</param>
		/// <returns>CPUのデスクリプターハンドル</returns>
		D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(uint32_t index);
		/// <summary>
		/// GPUデスクリプターハンドル追加
		/// </summary>
		/// <param name="index">SRVIndex</param>
		/// <returns>GPUのデスクリプターハンドル</returns>
		D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(uint32_t index);

		/// <summary>
		/// SRV_テクスチャ
		/// </summary>
		/// <param name="srvIndex">SRVの番号</param>
		/// <param name="pResource">テクスチャのリソース</param>
		/// <param name="Format">フォーマット</param>
		/// <param name="MipLevels">ミットマップ</param>
		void CreateSRVForTexture2D(uint32_t srvIndex, ID3D12Resource* pResource, DXGI_FORMAT Format, UINT MipLevels);
		/// <summary>
		/// SRV_オブジェクト
		/// </summary>
		/// <param name="srvDesc"></param>
		/// <param name="srvIndex">SRVの番号</param>
		/// <param name="pResource">テクスチャのリソース</param>
		/// <param name="numElements"></param>
		/// <param name="structureByteStride"></param>
		void CreateSRVForStructureBuffer(D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc, uint32_t srvIndex, ID3D12Resource* pResource, UINT numElements, UINT structureByteStride);
		/// <summary>
		/// 描画コマンド
		/// </summary>
		void PreDraw();
		/// <summary>
		/// getter_デスクリプターハンドル
		/// </summary>
		/// <returns>デスクリプターハンドル</returns>
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> GetDescriptorHeap() { return descriptorHeap_; }

		//最大SRV数
		static const uint32_t sMaxSRVCount_;

	private:

		//
		uint32_t descriptorSize_;
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap_;
		uint32_t useIndex_ = 0;

		//インスタンス
		static std::unique_ptr<SrvManager> sInstance_;
		//default_deleteを設定(解放処理を行える)
		friend struct std::default_delete<SrvManager>;
	};
}