#include "BoxModel.h"
#include <TextureManager.h>

using namespace MyMath;
using namespace Primitive;

void BoxModel::Initialize(const std::string& textureFile) {
	this->cubeMap_ = CubeMap::GetInstance().get();

	modelData_ = CreateBox();
	modelData_.materialData.textureFilePath = textureFile;

	vertexResource_ = cubeMap_->GetDirectXCommon()->CreateBufferResource(sizeof(VertexData) * modelData_.vertices.size());

	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	vertexBufferView_.SizeInBytes = UINT(sizeof(VertexData) * modelData_.vertices.size());
	vertexBufferView_.StrideInBytes = sizeof(VertexData);

	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));
	std::memcpy(vertexData_, modelData_.vertices.data(), sizeof(VertexData) * modelData_.vertices.size());

	//Model用マテリアル
	//マテリアル用のリソース
	materialResource_ = cubeMap_->GetDirectXCommon()->CreateBufferResource(sizeof(Material));
	//書き込むためのアドレス
	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));
	//色の設定
	materialData_->color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	materialData_->enableLighting = false;
	materialData_->uvTransform = MakeIdentity4x4();
	materialData_->shininess = 70;

	//テクスチャ読み込み
	TextureManager::GetInstance()->LoadTexture(modelData_.materialData.textureFilePath);
	modelData_.materialData.textureIndex = TextureManager::GetInstance()->GetSrvIndex(modelData_.materialData.textureFilePath);

	camera_ = cubeMap_->GetDefaultCamera();
	wvpResource_ = cubeMap_->GetDirectXCommon()->CreateBufferResource(sizeof(TransformationMatrix));
	wvpResource_->Map(0, nullptr, reinterpret_cast<void**>(&wvpData_));

	wvpData_->World = MakeIdentity4x4();
	wvpData_->WVP = MakeIdentity4x4();
}

void BoxModel::Update(const Matrix4x4& matWorld) {

	Matrix4x4 WorldViewProjectionMatrix;
	if (camera_) {
		Matrix4x4 projectionMatrix = camera_->GetViewProjectionMatrix();
		WorldViewProjectionMatrix = matWorld * projectionMatrix;
	}
	else {
		WorldViewProjectionMatrix = matWorld;
	}

	wvpData_->World = matWorld;
	//wvpData->World = worldMatrix;
	wvpData_->WVP = WorldViewProjectionMatrix;

	materialData_->color = color_;

}

void BoxModel::Draw() {
	cubeMap_->GetDirectXCommon()->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView_);
	cubeMap_->GetDirectXCommon()->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress()); //rootParameterの配列の0番目 [0]
	cubeMap_->GetDirectXCommon()->GetCommandList()->SetGraphicsRootConstantBufferView(1, wvpResource_->GetGPUVirtualAddress());
	cubeMap_->GetDirectXCommon()->GetCommandList()->SetGraphicsRootDescriptorTable(2, TextureManager::GetInstance()->GetSrvHandleGPU(modelData_.materialData.textureFilePath));
	cubeMap_->GetDirectXCommon()->GetCommandList()->DrawInstanced(UINT(modelData_.vertices.size()), 1, 0, 0);
}
