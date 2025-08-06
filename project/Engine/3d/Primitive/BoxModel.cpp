#include "BoxModel.h"
#include <TextureManager.h>

using namespace MyMath;

void BoxModel::Initialize(std::string textureFile) {
	this->cubemap = Cubemap::GetInstance();

	CreateBox();
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

void BoxModel::CreateBox() {
	VertexData vertexs[24];

	//右面
	vertexs[0].position = { 1.0f,1.0f,1.0f,1.0f };
	vertexs[1].position = { 1.0f,1.0f,-1.0f,1.0f };
	vertexs[2].position = { 1.0f,-1.0f,1.0f,1.0f };
	vertexs[3].position = { 1.0f,-1.0f,-1.0f,1.0f };

	modelData.vertices.push_back(vertexs[0]);
	modelData.vertices.push_back(vertexs[1]);
	modelData.vertices.push_back(vertexs[2]);

	modelData.vertices.push_back(vertexs[2]);
	modelData.vertices.push_back(vertexs[1]);
	modelData.vertices.push_back(vertexs[3]);
	

	//左面
	vertexs[4].position = { -1.0f,1.0f,-1.0f,1.0f };
	vertexs[5].position = { -1.0f,1.0f,1.0f,1.0f };
	vertexs[6].position = { -1.0f,-1.0f,-1.0f,1.0f };
	vertexs[7].position = { -1.0f,-1.0f,1.0f,1.0f };

	modelData.vertices.push_back(vertexs[4]);
	modelData.vertices.push_back(vertexs[5]);
	modelData.vertices.push_back(vertexs[6]);

	modelData.vertices.push_back(vertexs[6]);
	modelData.vertices.push_back(vertexs[5]);
	modelData.vertices.push_back(vertexs[7]);

	//前面
	vertexs[8].position = { -1.0f,1.0f,1.0f,1.0f };
	vertexs[9].position = { 1.0f,1.0f,1.0f,1.0f };
	vertexs[10].position = { -1.0f,-1.0f,1.0f,1.0f };
	vertexs[11].position = { 1.0f,-1.0f,1.0f,1.0f };

	modelData.vertices.push_back(vertexs[8]);
	modelData.vertices.push_back(vertexs[9]);
	modelData.vertices.push_back(vertexs[10]);

	modelData.vertices.push_back(vertexs[10]);
	modelData.vertices.push_back(vertexs[9]);
	modelData.vertices.push_back(vertexs[11]);

	//後ろ面
	vertexs[12].position = { -1.0f,1.0f,-1.0f,1.0f };
	vertexs[13].position = { 1.0f,1.0f,-1.0f,1.0f };
	vertexs[14].position = { -1.0f,-1.0f,-1.0f,1.0f };
	vertexs[15].position = { 1.0f,-1.0f,-1.0f,1.0f };

	modelData.vertices.push_back(vertexs[12]);
	modelData.vertices.push_back(vertexs[13]);
	modelData.vertices.push_back(vertexs[14]);

	modelData.vertices.push_back(vertexs[14]);
	modelData.vertices.push_back(vertexs[13]);
	modelData.vertices.push_back(vertexs[15]);

	//上面
	vertexs[16].position = { -1.0f,1.0f,-1.0f,1.0f };
	vertexs[17].position = { -1.0f,1.0f,1.0f,1.0f };
	vertexs[18].position = { 1.0f,1.0f,-1.0f,1.0f };
	vertexs[19].position = { 1.0f,1.0f,1.0f,1.0f };

	modelData.vertices.push_back(vertexs[16]);
	modelData.vertices.push_back(vertexs[17]);
	modelData.vertices.push_back(vertexs[18]);

	modelData.vertices.push_back(vertexs[18]);
	modelData.vertices.push_back(vertexs[17]);
	modelData.vertices.push_back(vertexs[19]);

	//下面
	vertexs[20].position = { -1.0f,-1.0f,-1.0f,1.0f };
	vertexs[21].position = { -1.0f,-1.0f,1.0f,1.0f };
	vertexs[22].position = { 1.0f,-1.0f,-1.0f,1.0f };
	vertexs[23].position = { 1.0f,-1.0f,1.0f,1.0f };

	modelData.vertices.push_back(vertexs[20]);
	modelData.vertices.push_back(vertexs[21]);
	modelData.vertices.push_back(vertexs[22]);

	modelData.vertices.push_back(vertexs[22]);
	modelData.vertices.push_back(vertexs[21]);
	modelData.vertices.push_back(vertexs[23]);

}
