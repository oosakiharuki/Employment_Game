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
	/// <returns>インスタンス</returns>
	static TextureManager& GetInstance();
	/// <summary>
	/// 初期化処理
	/// </summary>
	void Initialize();
	/// <summary>
	/// 解放処理
	/// </summary>
	void Finalize();
	/// <summary>
	/// テクスチャを読み込む
	/// </summary>
	/// <param name="filePath">読み込むテクスチャパス</param>
	void LoadTexture(const std::string& filePath);
	/// <summary>
	/// getter_srvIndex
	/// </summary>
	/// <param name="filePath">テクスチャパス</param>
	/// <returns>そのテクスチャのsrvIndex</returns>
	uint32_t GetSrvIndex(const std::string& filePath);
	/// <summary>
	/// getter_srvHandle
	/// </summary>
	/// <param name="filePath">テクスチャパス</param>
	/// <returns>そのテクスチャのsrvHandle</returns>
	D3D12_GPU_DESCRIPTOR_HANDLE GetSrvHandleGPU(const std::string& filePath);
	/// <summary>
	/// getter_メタデータ
	/// </summary>
	/// <param name="filePath">テクスチャパス</param>
	/// <returns>そのテクスチャのメタデータ</returns>
	const DirectX::TexMetadata& GetMetaData(const std::string& filePath);
private:
	//インスタンス
	static std::unique_ptr<TextureManager> sInstance_;
	//default_deleteを設定(解放処理を行える)
	friend struct std::default_delete<TextureManager>;

	/// <summary>
	/// ミップマップ作成
	/// </summary>
	/// <param name="filePath">ファイルパス</param>
	void MipMap(const std::string& filePath);

	/// <summary>
	/// 画像ロード方法設定
	/// </summary>
	/// <param name="filePath">ファイルパス</param>
	void Byte(const std::string& filePath);

	/// <summary>
	/// 構造体_テクスチャのデータ
	/// </summary>
	struct TextureData {
		DirectX::TexMetadata metadata; //width,height
		Microsoft::WRL::ComPtr<ID3D12Resource>resource; // テクスチャリソース
		uint32_t srvIndex;
		D3D12_CPU_DESCRIPTOR_HANDLE srvHandleCPU;
		D3D12_GPU_DESCRIPTOR_HANDLE srvHandleGPU;
	};

	/// <summary>
	/// ShaderResourceViewを作成
	/// </summary>
	/// <param name="textureData">テクスチャデータ</param>
	/// <param name="metadata">メタデータ</param>
	void CreateSRV(TextureData& textureData, const DirectX::TexMetadata metadata);
	//テクスチャデータのコンテナ
	std::unordered_map<std::string, TextureData> textureDatas_;
	
	std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> intermediateResources_;

	//画像
	DirectX::ScratchImage image{};

	//ミップマップ
	DirectX::ScratchImage mipImages{};

};