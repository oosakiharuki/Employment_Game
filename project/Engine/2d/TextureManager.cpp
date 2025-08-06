#include "TextureManager.h"

using namespace StringUtility;

TextureManager* TextureManager::instance = nullptr;

uint32_t TextureManager::kSRVIndexTop = 1;


TextureManager* TextureManager::GetInstance() {
	if (instance == nullptr) {
		instance = new TextureManager;
	}
	return instance;
}

void TextureManager::Initialize(DirectXCommon* dxCommon, SrvManager* srvManager) {
	dxCommon_ = dxCommon;
	this->srvManager = srvManager;
	textureDatas.reserve(SrvManager::kMaxSRVCount);
}


void TextureManager::Finalize() {
	delete instance;
	instance = nullptr;
}

void TextureManager::LoadTexture(const std::string& filePath) {

	//読み込み済みテクスチャを検索
	if (textureDatas.contains(filePath)) {
		return;
	}

	assert(srvManager->Max());

	//テクスチャファイル // byte関連
	DirectX::ScratchImage image{};
	std::wstring filePathW = ConvertString(filePath);
	HRESULT hr;
	if (filePathW.ends_with(L".dds")) {
		//sRGB搭載のためFLAGは立てない
		hr = DirectX::LoadFromDDSFile(filePathW.c_str(), DirectX::DDS_FLAGS_NONE, nullptr, image);
	}
	else {
		hr = DirectX::LoadFromWICFile(filePathW.c_str(), DirectX::WIC_FLAGS_FORCE_SRGB, nullptr, image);
	}
	assert(SUCCEEDED(hr));

	//ミップマップ　//拡大縮小で使う
	DirectX::ScratchImage mipImages{};
	if (DirectX::IsCompressed(image.GetMetadata().format)) {
		mipImages = std::move(image);
	}
	else {
		hr = DirectX::GenerateMipMaps(image.GetImages(), image.GetImageCount(), image.GetMetadata(), DirectX::TEX_FILTER_SRGB, 4, mipImages);
		assert(SUCCEEDED(hr));
	}

	const DirectX::TexMetadata& metadata = mipImages.GetMetadata();
	
	//最後尾を取得
	TextureData& textureData = textureDatas[filePath];

	textureData.metadata = metadata;
	textureData.resource = dxCommon_->CreateTextureResource(textureData.metadata);
	Microsoft::WRL::ComPtr<ID3D12Resource> val = dxCommon_->UploadTextureData(textureData.resource, mipImages);
	intermediateResources.push_back(val);

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = metadata.format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

	if (metadata.IsCubemap()) {
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
		srvDesc.TextureCube.MostDetailedMip = 0;
		srvDesc.TextureCube.MipLevels = UINT_MAX;
		srvDesc.TextureCube.ResourceMinLODClamp = 0.0f;
	}
	else {
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = UINT(metadata.mipLevels);
	}

	textureData.srvIndex = srvManager->Allocate();
	textureData.srvHandleCPU = srvManager->GetCPUDescriptorHandle(textureData.srvIndex);
	textureData.srvHandleGPU = srvManager->GetGPUDescriptorHandle(textureData.srvIndex);

	//SRVの生成
	srvManager->CreateSRVforStructureBuffer(srvDesc,textureData.srvIndex,textureData.resource.Get(), metadata.format, UINT(metadata.mipLevels));
}

uint32_t TextureManager::GetSrvIndex(const std::string filePath) {
	assert(srvManager->Max());

	TextureData& textureData = textureDatas[filePath];
	return textureData.srvIndex;
}

D3D12_GPU_DESCRIPTOR_HANDLE TextureManager::GetSrvHandleGPU(const std::string filePath) {
	assert(srvManager->Max());

	TextureData& textureData = textureDatas[filePath];
	return textureData.srvHandleGPU;
}

const DirectX::TexMetadata& TextureManager::GetMetaData(const std::string filePath) {
	assert(srvManager->Max());

	TextureData& textureData = textureDatas[filePath];
	return textureData.metadata;
}
