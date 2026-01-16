#include "TextureManager.h"

using namespace StringUtility;

std::shared_ptr<TextureManager> TextureManager::sInstance_ = nullptr;

std::shared_ptr<TextureManager> TextureManager::GetInstance() {
	if (sInstance_ == nullptr) {
		sInstance_ = std::make_unique<TextureManager>();
	}
	return sInstance_;
}

void TextureManager::Initialize(DirectXCommon* dxCommon, SrvManager* srvManager) {
	dxCommon_ = dxCommon;
	srvManager_ = srvManager;
	textureDatas_.reserve(SrvManager::sMaxSRVCount_);
}


void TextureManager::Finalize() {
	sInstance_.reset();
	sInstance_ = nullptr;
}

void TextureManager::LoadTexture(const std::string& filePath) {

	//読み込み済みテクスチャを検索
	if (textureDatas_.contains(filePath)) {
		return;
	}

	assert(srvManager_->Max());

	//ミップマップ作成
	MipMap(filePath);

	const DirectX::TexMetadata& metadata = mipImages.GetMetadata();
	
	//filePath名義のテクスチャデータを設定
	TextureData& textureData = textureDatas_[filePath];
	textureData.metadata = metadata;
	textureData.resource = dxCommon_->CreateTextureResource(textureData.metadata);
	Microsoft::WRL::ComPtr<ID3D12Resource> val = dxCommon_->UploadTextureData(textureData.resource, mipImages);
	intermediateResources_.push_back(val);

	//SRVの設定
	CreateSRV(textureData, metadata);
}


void TextureManager::Byte(const std::string& filePath) {
	//テクスチャファイル // byte関連
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
}

void TextureManager::MipMap(const std::string& filePath) {
	
	//画像をロード
	Byte(filePath);
	
	HRESULT hr;
	//ミップマップ　//拡大縮小で使う
	if (DirectX::IsCompressed(image.GetMetadata().format)) {
		mipImages = std::move(image);
	}
	else {
		hr = DirectX::GenerateMipMaps(image.GetImages(), image.GetImageCount(), image.GetMetadata(), DirectX::TEX_FILTER_SRGB, 4, mipImages);
		assert(SUCCEEDED(hr));
	}
}

void TextureManager::CreateSRV(TextureData& textureData, const DirectX::TexMetadata metadata){
	//SRVDesc作成
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

	textureData.srvIndex = srvManager_->Allocate();
	textureData.srvHandleCPU = srvManager_->GetCPUDescriptorHandle(textureData.srvIndex);
	textureData.srvHandleGPU = srvManager_->GetGPUDescriptorHandle(textureData.srvIndex);

	//SRVの生成
	srvManager_->CreateSRVForStructureBuffer(srvDesc, textureData.srvIndex, textureData.resource.Get(), metadata.format, UINT(metadata.mipLevels));
}


uint32_t TextureManager::GetSrvIndex(const std::string& filePath) {
	assert(srvManager_->Max());

	TextureData& textureData = textureDatas_[filePath];
	return textureData.srvIndex;
}

D3D12_GPU_DESCRIPTOR_HANDLE TextureManager::GetSrvHandleGPU(const std::string& filePath) {
	assert(srvManager_->Max());

	TextureData& textureData = textureDatas_[filePath];
	return textureData.srvHandleGPU;
}

const DirectX::TexMetadata& TextureManager::GetMetaData(const std::string& filePath) {
	assert(srvManager_->Max());

	TextureData& textureData = textureDatas_[filePath];
	return textureData.metadata;
}
