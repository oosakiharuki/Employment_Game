#include "SphereModel.h"
#include <TextureManager.h>

using namespace MyMath;

void SphereModel::Initialize() {
	this->debugWireframes = DebugWireframes::GetInstance();

	CreateSphere();

	vertexResource = debugWireframes->GetDirectXCommon()->CreateBufferResource(sizeof(VertexData) * modelData.vertices.size());
	
	vertexBufferView.BufferLocation = vertexResource->GetGPUVirtualAddress();
	vertexBufferView.SizeInBytes = UINT(sizeof(VertexData) * modelData.vertices.size());
	vertexBufferView.StrideInBytes = sizeof(VertexData);

	vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));
	std::memcpy(vertexData, modelData.vertices.data(), sizeof(VertexData) * modelData.vertices.size());

	//Model用マテリアル
	//マテリアル用のリソース
	materialResource = debugWireframes->GetDirectXCommon()->CreateBufferResource(sizeof(Material));
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




	this->camera = debugWireframes->GetDefaultCamera();
	wvpResource = debugWireframes->GetDirectXCommon()->CreateBufferResource(sizeof(TransformationMatrix));
	wvpResource->Map(0, nullptr, reinterpret_cast<void**>(&wvpData));

	wvpData->World = MakeIdentity4x4();
	wvpData->WVP = MakeIdentity4x4();

	//Phong Reflection Model
	cameraResource = debugWireframes->GetDirectXCommon()->CreateBufferResource(sizeof(CameraForGPU));
	cameraResource->Map(0, nullptr, reinterpret_cast<void**>(&cameraData));

	cameraData->worldPosition = { 0,0,0 };

}

void SphereModel::Update(Matrix4x4 matworld) {

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

void SphereModel::Draw() {
	debugWireframes->GetDirectXCommon()->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView);
	debugWireframes->GetDirectXCommon()->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResource->GetGPUVirtualAddress()); //rootParameterの配列の0番目 [0]
	debugWireframes->GetDirectXCommon()->GetCommandList()->SetGraphicsRootConstantBufferView(1, wvpResource->GetGPUVirtualAddress());
	debugWireframes->GetDirectXCommon()->GetCommandList()->SetGraphicsRootDescriptorTable(2, TextureManager::GetInstance()->GetSrvHandleGPU(modelData.material.textureFilePath));	
	debugWireframes->GetDirectXCommon()->GetCommandList()->SetGraphicsRootConstantBufferView(4, cameraResource->GetGPUVirtualAddress());
	debugWireframes->GetDirectXCommon()->GetCommandList()->DrawInstanced(UINT(modelData.vertices.size()), 1, 0, 0);
}

void SphereModel::CreateSphere() {

	const uint32_t kSubdivision = 16;

	float pi = float(M_PI);

	const float kLonEvery = pi * 2.0f / float(kSubdivision);
	const float kLatEvery = pi / float(kSubdivision);


	VertexData vertexDataBkaraA[kSubdivision]{};

	VertexData vertexDataCkaraA[kSubdivision]{};

	VertexData vertexDataDkaraA[kSubdivision][kSubdivision]{};

	VertexData vertexDataDkaraC[kSubdivision]{};
	VertexData vertexDataDkaraB[kSubdivision]{};


	for (uint32_t latIndex = 0; latIndex < kSubdivision; ++latIndex) {
		float lat = -pi / 2.0f + kLatEvery * latIndex;//緯度 シ－タ

		for (uint32_t lonIndex = 0; lonIndex < kSubdivision; ++lonIndex) {

			uint32_t start = (latIndex * kSubdivision + lonIndex) * 6;
			float lon = lonIndex * kLonEvery;//経度　ファイ


			VertexData vertA{};
			vertA.position =
			{
				std::cos(lat) * std::cos(lon),
				std::sin(lat),
				std::cos(lat) * std::sin(lon),
				1.0f
			};
			vertA.texcoord =
			{
				float(lonIndex) / float(kSubdivision),
				1.0f - float(latIndex) / float(kSubdivision)
			};
			vertA.normal = {
				0.0f,0.0f,-1.0f
			};


			VertexData vertB{};
			vertB.position =
			{
				std::cos(lat + kLatEvery) * std::cos(lon),
				std::sin(lat + kLatEvery),
				std::cos(lat + kLatEvery) * std::sin(lon)
				,1.0f
			};
			vertB.texcoord =
			{
				float(lonIndex) / float(kSubdivision),
				1.0f - float(latIndex + 1) / float(kSubdivision)
			};
			vertB.normal = {
				0.0f,0.0f,-1.0f
			};


			VertexData vertC{};
			vertC.position =
			{
				std::cos(lat) * std::cos(lon + kLonEvery),
				std::sin(lat),
				std::cos(lat) * std::sin(lon + kLonEvery),
				1.0f
			};
			vertC.texcoord =
			{
				float(lonIndex + 1) / float(kSubdivision),
				1.0f - float(latIndex) / float(kSubdivision)
			};
			vertC.normal = {
				0.0f,0.0f,-1.0f
			};


			VertexData vertD{};
			vertD.position =
			{
				std::cos(lat + kLatEvery) * std::cos(lon + kLonEvery),
				std::sin(lat + kLatEvery),
				std::cos(lat + kLatEvery) * std::sin(lon + kLonEvery),
				1.0f
			};
			vertD.texcoord =
			{
				float(lonIndex + 1) / float(kSubdivision),
				1.0f - float(latIndex + 1) / float(kSubdivision)
			};
			vertD.normal = {
				0.0f,0.0f,-1.0f
			};




			//最初点
			modelData.vertices.push_back(vertA);
			modelData.vertices.push_back(vertB);
			modelData.vertices.push_back(vertC);

			modelData.vertices.push_back(vertC);
			modelData.vertices.push_back(vertB);
			modelData.vertices.push_back(vertD);

		}

	}


	for (uint32_t index = 0; index < kSubdivision * kSubdivision * 6; index++) {
		modelData.vertices[index].normal.x = modelData.vertices[index].position.x;
		modelData.vertices[index].normal.y = modelData.vertices[index].position.y;
		modelData.vertices[index].normal.z = modelData.vertices[index].position.z;
	}


	modelData.material.textureFilePath = "resource/Sprite/white.png";

	//テクスチャ読み込み
	TextureManager::GetInstance()->LoadTexture(modelData.material.textureFilePath);
	modelData.material.textureIndex = TextureManager::GetInstance()->GetSrvIndex(modelData.material.textureFilePath);
}
