#include "BoxModel.h"
#include <TextureManager.h>

using namespace MyMath;
using namespace Primitive;

void BoxModel::Initialize(std::string textureFile) {
	this->cubemap = Cubemap::GetInstance();

	modelData = CreateBox();
	modelData.material.textureFilePath = textureFile;

	vertexResource = cubemap->GetDirectXCommon()->CreateBufferResource(sizeof(VertexData) * modelData.vertices.size());

	vertexBufferView.BufferLocation = vertexResource->GetGPUVirtualAddress();
	vertexBufferView.SizeInBytes = UINT(sizeof(VertexData) * modelData.vertices.size());
	vertexBufferView.StrideInBytes = sizeof(VertexData);

	vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));
	std::memcpy(vertexData, modelData.vertices.data(), sizeof(VertexData) * modelData.vertices.size());

	//Model用マテリアル
	//マテリアル用のリソース
	materialResource = cubemap->GetDirectXCommon()->CreateBufferResource(sizeof(Material));
	//書き込むためのアドレス
	materialResource->Map(0, nullptr, reinterpret_cast<void**>(&materialData));
	//色の設定
	materialData->color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	materialData->enableLighting = false;
	materialData->uvTransform = MakeIdentity4x4();
	materialData->shininess = 70;

	//テクスチャ読み込み
	TextureManager::GetInstance()->LoadTexture(modelData.material.textureFilePath);
	modelData.material.textureIndex = TextureManager::GetInstance()->GetSrvIndex(modelData.material.textureFilePath);




	this->camera = cubemap->GetDefaultCamera();
	wvpResource = cubemap->GetDirectXCommon()->CreateBufferResource(sizeof(TransformationMatrix));
	wvpResource->Map(0, nullptr, reinterpret_cast<void**>(&wvpData));

	wvpData->World = MakeIdentity4x4();
	wvpData->WVP = MakeIdentity4x4();

	////Phong Reflection Model
	//cameraResource = cubemap->GetDirectXCommon()->CreateBufferResource(sizeof(CameraForGPU));
	//cameraResource->Map(0, nullptr, reinterpret_cast<void**>(&cameraData));

	//cameraData->worldPosition = { 0,0,0 };

}

void BoxModel::Update(Matrix4x4 matworld) {

	Matrix4x4 WorldViewProjectionMatrix;
	if (camera) {
		Matrix4x4 projectionMatrix = camera->GetViewProjectionMatrix();
		WorldViewProjectionMatrix = matworld * projectionMatrix;
	}
	else {
		WorldViewProjectionMatrix = matworld;
	}

	wvpData->World = matworld;
	//wvpData->World = worldMatrix;
	wvpData->WVP = WorldViewProjectionMatrix;

	materialData->color = color_;

}

void BoxModel::Draw() {
	cubemap->GetDirectXCommon()->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView);
	cubemap->GetDirectXCommon()->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResource->GetGPUVirtualAddress()); //rootParameterの配列の0番目 [0]
	cubemap->GetDirectXCommon()->GetCommandList()->SetGraphicsRootConstantBufferView(1, wvpResource->GetGPUVirtualAddress());
	cubemap->GetDirectXCommon()->GetCommandList()->SetGraphicsRootDescriptorTable(2, TextureManager::GetInstance()->GetSrvHandleGPU(modelData.material.textureFilePath));
	//cubemap->GetDirectXCommon()->GetCommandList()->SetGraphicsRootConstantBufferView(4, cameraResource->GetGPUVirtualAddress());
	cubemap->GetDirectXCommon()->GetCommandList()->DrawInstanced(UINT(modelData.vertices.size()), 1, 0, 0);
}
