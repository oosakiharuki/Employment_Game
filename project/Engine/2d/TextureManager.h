#pragma once
#include <string>
#include "externals/DirectXTex/DirectXTex.h"
#include <wrl.h>
#include <d3d12.h>
#include <unordered_map>

#include "DirectXCommon.h"
#include "SrvManager.h"
/// <summary>
/// テクスチャマネージャ
/// </summary>
class TextureManager {
public:
	/// <summary>
	/// インスタンス生成
	/// </summary>
	/// <returns></returns>
	static std::shared_ptr<TextureManager> GetInstance();
	/// <summary>
	/// 初期化処理
	/// </summary>
	void Initialize(DirectXCommon* dxCommon, SrvManager* srvManager);
	/// <summary>
	/// 解放処理
	/// </summary>
	void Finalize();
	/// <summary>
	/// テクスチャを読み込む
	/// </summary>
	/// <param name="filePath"></param>
	void LoadTexture(const std::string& filePath);
	/// <summary>
	/// getter_srvIndex
	/// </summary>
	/// <param name="filePath"></param>
	/// <returns></returns>
	uint32_t GetSrvIndex(const std::string& filePath);
	/// <summary>
	/// getter_srvHandle
	/// </summary>
	/// <param name="filePath"></param>
	/// <returns></returns>
	D3D12_GPU_DESCRIPTOR_HANDLE GetSrvHandleGPU(const std::string& filePath);
	/// <summary>
	/// getter_メタデータ
	/// </summary>
	/// <param name="filePath"></param>
	/// <returns></returns>
	const DirectX::TexMetadata& GetMetaData(const std::string& filePath);
private:
	static std::shared_ptr<TextureManager> sInstance_;

	/// <summary>
	/// ミップマップ作成
	/// </summary>
	/// <param name="filePath">ファイルパス</param>
	void MipMap(const std::string filePath);

	/// <summary>
	/// 画像ロード方法設定
	/// </summary>
	/// <param name="filePath">ファイルパス</param>
	void Byte(const std::string filePath);

	struct TextureData {
		DirectX::TexMetadata metadata; //width,height
		Microsoft::WRL::ComPtr<ID3D12Resource>resource; // テクスチャリソース
		uint32_t srvIndex;
		D3D12_CPU_DESCRIPTOR_HANDLE srvHandleCPU;
		D3D12_GPU_DESCRIPTOR_HANDLE srvHandleGPU;
	};

	void CreateSRV(TextureData& textureData, const DirectX::TexMetadata metadata);

	std::unordered_map<std::string, TextureData> textureDatas_;

	DirectXCommon* dxCommon_ = nullptr;
	SrvManager* srvManager_ = nullptr;

	std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> intermediateResources_;

	//画像
	DirectX::ScratchImage image{};

	//ミップマップ
	DirectX::ScratchImage mipImages{};

};