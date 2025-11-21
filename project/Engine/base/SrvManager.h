#pragma once
#include "DirectXCommon.h"
class SrvManager{
public:
	/// <summary>
	/// インスタンス生成
	/// </summary>
	/// <returns></returns>
	static SrvManager* GetInstance();
	/// <summary>
	/// 解放処理
	/// </summary>
	void Finalize();
	/// <summary>
	/// 初期化処理
	/// </summary>
	/// <param name="dxCommon"></param>
	void Initialize(DirectXCommon* dxCommon);
	/// <summary>
	/// カウント
	/// </summary>
	/// <returns></returns>
	uint32_t Allocate();
	/// <summary>
	/// sMaxSRVCount以上でないか
	/// </summary>
	/// <returns></returns>
	bool Max();
	/// <summary>
	/// CPUデスクリプターハンドル追加
	/// </summary>
	/// <param name="index"></param>
	/// <returns></returns>
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(uint32_t index);
	/// <summary>
	/// GPUデスクリプターハンドル追加
	/// </summary>
	/// <param name="index"></param>
	/// <returns></returns>
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(uint32_t index);
	
	/// <summary>
	/// Srv_テクスチャ
	/// </summary>
	void CreateSRVforTexture2D(uint32_t srvIndex, ID3D12Resource* pResource, DXGI_FORMAT Format, UINT MipLevels);
	/// <summary>
	/// Srv_オブジェクト
	/// </summary>
	void CreateSRVforStructureBuffer(D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc,uint32_t srvIndex, ID3D12Resource* pResource, UINT numElements, UINT structureByteStride);
	//描画コマンド
	void PreDraw();
	void SetGraphicsRootDescriptorTable(UINT RootParameterIndex, uint32_t srvIndex);
	/// <summary>
	/// getter_デスクリプターハンドル
	/// </summary>
	/// <returns></returns>
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> GetDescriptorHeap() { return descriptorHeap_; }

	//最大SRV数
	static const uint32_t sMaxSRVCount_;

private:
	DirectXCommon* directXCommon_ = nullptr;

	//
	uint32_t descriptorSize_;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap_;
	uint32_t useIndex_ = 0;


	static SrvManager* sInstance_;

	SrvManager() = default;
	~SrvManager() = default;
	SrvManager(SrvManager&) = default;
	SrvManager& operator=(SrvManager&) = default;

	static uint32_t sSRVIndexTop_;
};