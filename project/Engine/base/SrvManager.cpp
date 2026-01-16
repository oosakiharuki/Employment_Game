#include "SrvManager.h"

const uint32_t SrvManager::sMaxSRVCount_ = 512;

std::shared_ptr<SrvManager> SrvManager::sInstance_ = nullptr;

std::shared_ptr<SrvManager> SrvManager::GetInstance() {
	if (sInstance_ == nullptr) {
		sInstance_ = std::make_unique<SrvManager>();
	}
	return sInstance_;
}
void SrvManager::Finalize() {
	sInstance_.reset();
	sInstance_ = nullptr;
}


void SrvManager::Initialize(DirectXCommon* dxCommon) {
	this->directXCommon_ = dxCommon;
	descriptorHeap_ = directXCommon_->CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, sMaxSRVCount_, true);
	descriptorSize_ = directXCommon_->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}
uint32_t SrvManager::Allocate() {
	assert(Max());
	//returnする番号
	int index = useIndex_;
	//次回のため番号を1進める [0] -> [1]
	useIndex_++;
	return index;
}

bool SrvManager::Max() {
	if (sMaxSRVCount_ < useIndex_) {
		return false;
	}
	else {
		return true;//上限に達して「ない」
	}
	return false;
}

D3D12_CPU_DESCRIPTOR_HANDLE SrvManager::GetCPUDescriptorHandle(uint32_t index) {
	D3D12_CPU_DESCRIPTOR_HANDLE handleCPU = descriptorHeap_->GetCPUDescriptorHandleForHeapStart();
	handleCPU.ptr += (descriptorSize_ * index);
	return handleCPU;
}

D3D12_GPU_DESCRIPTOR_HANDLE SrvManager::GetGPUDescriptorHandle(uint32_t index) {
	D3D12_GPU_DESCRIPTOR_HANDLE handleGPU = descriptorHeap_->GetGPUDescriptorHandleForHeapStart();
	handleGPU.ptr += (descriptorSize_ * index);
	return handleGPU;
}

void SrvManager::CreateSRVForTexture2D(uint32_t srvIndex, ID3D12Resource* pResource, DXGI_FORMAT Format, UINT MipLevels) {
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = Format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = MipLevels;
	directXCommon_->GetDevice()->CreateShaderResourceView(pResource, &srvDesc, GetCPUDescriptorHandle(srvIndex));
}

void SrvManager::CreateSRVForStructureBuffer(D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc, uint32_t srvIndex, ID3D12Resource* pResource, UINT numElements, UINT structureByteStride) {
	directXCommon_->GetDevice()->CreateShaderResourceView(pResource, &srvDesc, GetCPUDescriptorHandle(srvIndex));
}

void SrvManager::PreDraw() {
	ID3D12DescriptorHeap* descriptorHeaps[] = { descriptorHeap_.Get() };
	directXCommon_->GetCommandList()->SetDescriptorHeaps(1, descriptorHeaps);
}

void SrvManager::SetGraphicsRootDescriptorTable(UINT RootParameterIndex, uint32_t srvIndex) {
	directXCommon_->GetCommandList()->SetGraphicsRootDescriptorTable(RootParameterIndex, GetGPUDescriptorHandle(srvIndex));
}