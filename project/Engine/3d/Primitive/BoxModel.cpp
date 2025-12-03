#include "BoxModel.h"
#include <TextureManager.h>

using namespace MyMath;
using namespace Primitive;

void BoxModel::Initialize(const std::string& textureFile) {
	this->cubemap_ = Cubemap::GetInstance().get();

	modelData_ = CreateBox();
	modelData_.material.textureFilePath = textureFile;

	vertexResource_ = cubemap_->GetDirectXCommon()->CreateBufferResource(sizeof(VertexData) * modelData_.vertices.size());

	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	vertexBufferView_.SizeInBytes = UINT(sizeof(VertexData) * modelData_.vertices.size());
	vertexBufferView_.StrideInBytes = sizeof(VertexData);

	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));
	std::memcpy(vertexData_, modelData_.vertices.data(), sizeof(VertexData) * modelData_.vertices.size());

	//Model用マテリアル
	//マテリアル用のリソース
	materialResource_ = cubemap_->GetDirectXCommon()->CreateBufferResource(sizeof(Material));
	//書き込むためのアドレス
	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));
	//色の設定
	materialData_->color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	materialData_->enableLighting = false;
	materialData_->uvTransform = MakeIdentity4x4();
	materialData_->shininess = 70;

	//テクスチャ読み込み
	TextureManager::GetInstance()->LoadTexture(modelData_.material.textureFilePath);
	modelData_.material.textureIndex = TextureManager::GetInstance()->GetSrvIndex(modelData_.material.textureFilePath);

	camera_ = cubemap_->GetDefaultCamera();
	wvpResource_ = cubemap_->GetDirectXCommon()->CreateBufferResource(sizeof(TransformationMatrix));
	wvpResource_->Map(0, nullptr, reinterpret_cast<void**>(&wvpData_));

	wvpData_->World = MakeIdentity4x4();
	wvpData_->WVP = MakeIdentity4x4();
}

void BoxModel::Update(const Matrix4x4& matworld) {

	Matrix4x4 WorldViewProjectionMatrix;
	if (camera_) {
		Matrix4x4 projectionMatrix = camera_->GetViewProjectionMatrix();
		WorldViewProjectionMatrix = matworld * projectionMatrix;
	}
	else {
		WorldViewProjectionMatrix = matworld;
	}

	wvpData_->World = matworld;
	//wvpData->World = worldMatrix;
	wvpData_->WVP = WorldViewProjectionMatrix;

	materialData_->color = color_;

}

void BoxModel::Draw() {
	cubemap_->GetDirectXCommon()->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView_);
	cubemap_->GetDirectXCommon()->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress()); //rootParameterの配列の0番目 [0]
	cubemap_->GetDirectXCommon()->GetCommandList()->SetGraphicsRootConstantBufferView(1, wvpResource_->GetGPUVirtualAddress());
	cubemap_->GetDirectXCommon()->GetCommandList()->SetGraphicsRootDescriptorTable(2, TextureManager::GetInstance()->GetSrvHandleGPU(modelData_.material.textureFilePath));
	//cubemap_->GetDirectXCommon()->GetCommandList()->SetGraphicsRootConstantBufferView(4, cameraResource->GetGPUVirtualAddress());
	cubemap_->GetDirectXCommon()->GetCommandList()->DrawInstanced(UINT(modelData_.vertices.size()), 1, 0, 0);
}
