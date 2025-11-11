#include "Particle.h"
#include "TextureManager.h"
#include "ParticleManager.h"
#include "Camera.h"
#include <fstream>
#include <sstream>
#include <mutex>

#include <numbers>
#include "ModelManager.h"

#include <string>
#include "ParticleNumber.h"

using namespace MyMath;
using namespace Primitive;

Particle::~Particle() {
	// リソースのアンマップ
	if (vertexResource && vertexData) {
		vertexResource->Unmap(0, nullptr);
		vertexData = nullptr;
	}
	if (materialResource && materialData) {
		materialResource->Unmap(0, nullptr);
		materialData = nullptr;
	}
	if (wvpResource && wvpData) {
		wvpResource->Unmap(0, nullptr);
		wvpData = nullptr;
	}
	if (directionalLightSphereResource && directionalLightSphereData) {
		directionalLightSphereResource->Unmap(0, nullptr);
		directionalLightSphereData = nullptr;
	}
}

void Particle::Initialize(const std::string& particleName, std::string textureFile , PrimitiveType type) {
	this->particleCommon = ParticleCommon::GetInstance();
	this->camera = particleCommon->GetDefaultCamera();

	//particleの設定
	ParticleManager::GetInstance()->CreateParticleGroup(particleName, textureFile, type);

	this->fileName = particleName;
	this->textureFile = textureFile;

	modelData = ParticleManager::GetInstance()->GetModelData(fileName);
	
	vertexResource = particleCommon->GetDxCommon()->CreateBufferResource(sizeof(VertexData) * modelData.vertices.size());

	vertexBufferView.BufferLocation = vertexResource->GetGPUVirtualAddress();
	vertexBufferView.SizeInBytes = UINT(sizeof(VertexData) * modelData.vertices.size());
	vertexBufferView.StrideInBytes = sizeof(VertexData);


	wvpResource = ParticleManager::GetInstance()->GetResource(fileName);
	wvpResource->Map(0, nullptr, reinterpret_cast<void**>(&wvpData));
	
	for (uint32_t index = 0; index < kNumMaxInstance; ++index) {
		wvpData[index].World = MakeIdentity4x4();
		wvpData[index].WVP = MakeIdentity4x4();
		wvpData[index].color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	}


	vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));
	std::memcpy(vertexData, modelData.vertices.data(), sizeof(VertexData) * modelData.vertices.size());


	//Particle用マテリアル
	//マテリアル用のリソース
	materialResource = particleCommon->GetDxCommon()->CreateBufferResource(sizeof(Material));
	//書き込むためのアドレス
	materialResource->Map(0, nullptr, reinterpret_cast<void**>(&materialData));
	//色の設定
	materialData->color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	materialData->enableLighting = true;
	materialData->uvTransform = MakeIdentity4x4();

	//テクスチャ読み込み
	TextureManager::GetInstance()->LoadTexture(modelData.material.textureFilePath);
	modelData.material.textureIndex = TextureManager::GetInstance()->GetSrvIndex(modelData.material.textureFilePath);


	//ライト用のリソース
	directionalLightSphereResource = particleCommon->GetDxCommon()->CreateBufferResource(sizeof(DirectionalLight));
	//書き込むためのアドレス
	directionalLightSphereResource->Map(0, nullptr, reinterpret_cast<void**>(&directionalLightSphereData));
	//色の設定
	directionalLightSphereData->color = { 1.0f,1.0f,1.0f,1.0f };
	directionalLightSphereData->direction = { 0.0f,-1.0f,0.0f };
	directionalLightSphereData->intensity = 1.0f;

	//エミッター
	emitter.transform.translate = { 0.0f,0.0f,-3.0f };
	emitter.transform.rotate = { 0.0f,0.0f,0.0f };
	emitter.transform.scale = { 1.0f,1.0f,1.0f };
	emitter.count = 3;
	emitter.frequency = 0.5f;
	emitter.frequencyTime = 0.0f;

	//場
	accelerationField.acceleration = { 0.0f,15.0f,0.0f };
	accelerationField.area.min = { -1.0f,-1.0f,-1.0f };
	accelerationField.area.max = { 1.0f,1.0f,1.0f };

	//パーティクル発生
	bornP = BornParticle::Stop;

	ParticleManager::GetInstance()->SetCamera(camera);
}

void Particle::Update() {

	const float kDeltaTime = 1.0f / 60.0f;

	switch (bornP)
	{
	case BornParticle::TimerMode:

		emitter.frequencyTime += kDeltaTime;

		if (emitter.frequency <= emitter.frequencyTime) {
			//発生処理
			ParticleManager::GetInstance()->Emit(fileName, emitter, particleMosion);
			emitter.frequencyTime -= emitter.frequency;
		}
		break;
	case BornParticle::MomentMode:
		//発生処理
		ParticleManager::GetInstance()->Emit(fileName, emitter, particleMosion);
		bornP = BornParticle::Stop;
		break;
	case BornParticle::Stop:
		break;
	}

	ParticleManager::GetInstance()->Update(fileName, wvpData,particleMosion);

	numInstance = ParticleManager::GetInstance()->GetNum(fileName);

	// directionalLightSphereDataのnullチェック
	if (directionalLightSphereData) {
		directionalLightSphereData->direction = Normalize(directionalLightSphereData->direction);
	}

}

void Particle::Draw() {
	//射影行列
	Matrix4x4 WorldViewProjectionMatrix;

	for (uint32_t i = 0; i < ParticleManager::GetInstance()->GetNum(fileName); i++) {
		if (camera) {
			Matrix4x4 projectionMatrix = camera->GetViewProjectionMatrix();
			WorldViewProjectionMatrix = wvpData[i].World * projectionMatrix;
		}
		else {
			WorldViewProjectionMatrix = wvpData[i].World;
		}

		wvpData[i].WVP = WorldViewProjectionMatrix;
	}

	//パーティクルが出ていないときはパス
	if (ParticleManager::GetInstance()->GetNum(fileName) > 0) {
		particleCommon->GetDxCommon()->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView);
		particleCommon->GetDxCommon()->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResource->GetGPUVirtualAddress()); //rootParameterの配列の0番目 [0]
		particleCommon->GetDxCommon()->GetCommandList()->SetGraphicsRootConstantBufferView(1, wvpResource->GetGPUVirtualAddress());
		particleCommon->GetDxCommon()->GetCommandList()->SetGraphicsRootDescriptorTable(2, TextureManager::GetInstance()->GetSrvHandleGPU(textureFile));
		particleCommon->GetDxCommon()->GetCommandList()->SetGraphicsRootConstantBufferView(3, directionalLightSphereResource->GetGPUVirtualAddress());

		//4のやつ particle専用
		particleCommon->GetDxCommon()->GetCommandList()->SetGraphicsRootDescriptorTable(4, ParticleManager::GetInstance()->GetSrvHandleGPU(fileName));

		particleCommon->GetDxCommon()->GetCommandList()->DrawInstanced(UINT(modelData.vertices.size()), ParticleManager::GetInstance()->GetNum(fileName), 0, 0);
	}
	numInstance = 0;

	ParticleManager::GetInstance()->ResetNum(fileName);
}

bool Particle::IsCollision(const AABB& aabb, const Vector3& point) {
	
	if ((aabb.min.x < point.x && aabb.max.x > point.x) &&
		(aabb.min.y < point.y && aabb.max.y > point.y) &&
		(aabb.min.z < point.z && aabb.max.z > point.z)) {
		return true;
	}

	return false;
}
