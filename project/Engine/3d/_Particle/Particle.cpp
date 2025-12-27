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

using namespace MyMath;
using namespace Primitive;

Particle::~Particle() {
	// リソースのアンマップ
	if (vertexResource_ && vertexData_) {
		vertexResource_->Unmap(0, nullptr);
		vertexData_ = nullptr;
	}
	if (materialResource_ && materialData_) {
		materialResource_->Unmap(0, nullptr);
		materialData_ = nullptr;
	}
	if (wvpResource_ && wvpData_) {
		wvpResource_->Unmap(0, nullptr);
		wvpData_ = nullptr;
	}
	if (directionalLightSphereResource_ && directionalLightSphereData_) {
		directionalLightSphereResource_->Unmap(0, nullptr);
		directionalLightSphereData_ = nullptr;
	}
}

void Particle::Initialize(const std::string& particleName, const std::string& textureFile , PrimitiveType type) {
	this->particleCommon_ = ParticleCommon::GetInstance().get();
	this->camera_ = particleCommon_->GetDefaultCamera();

	//particleの設定
	ParticleManager::GetInstance()->CreateParticleGroup(particleName, textureFile, type);

	//
	this->fileName_ = particleName;
	this->textureFile_ = textureFile;

	modelData_ = ParticleManager::GetInstance()->GetModelData(fileName_);
	
	vertexResource_ = particleCommon_->GetDxCommon()->CreateBufferResource(sizeof(VertexData) * modelData_.vertices.size());

	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	vertexBufferView_.SizeInBytes = UINT(sizeof(VertexData) * modelData_.vertices.size());
	vertexBufferView_.StrideInBytes = sizeof(VertexData);


	wvpResource_ = ParticleManager::GetInstance()->GetResource(fileName_);
	wvpResource_->Map(0, nullptr, reinterpret_cast<void**>(&wvpData_));
	
	for (uint32_t index = 0; index < kNumMaxInstance_; ++index) {
		wvpData_[index].World = MakeIdentity4x4();
		wvpData_[index].WVP = MakeIdentity4x4();
		wvpData_[index].color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	}


	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));
	std::memcpy(vertexData_, modelData_.vertices.data(), sizeof(VertexData) * modelData_.vertices.size());


	//Particle用マテリアル
	//マテリアル用のリソース
	materialResource_ = particleCommon_->GetDxCommon()->CreateBufferResource(sizeof(Material));
	//書き込むためのアドレス
	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));
	//色の設定
	materialData_->color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	materialData_->enableLighting = true;
	materialData_->uvTransform = MakeIdentity4x4();

	//テクスチャ読み込み
	TextureManager::GetInstance()->LoadTexture(modelData_.materialData.textureFilePath);
	modelData_.materialData.textureIndex = TextureManager::GetInstance()->GetSrvIndex(modelData_.materialData.textureFilePath);


	//ライト用のリソース
	directionalLightSphereResource_ = particleCommon_->GetDxCommon()->CreateBufferResource(sizeof(DirectionalLight));
	//書き込むためのアドレス
	directionalLightSphereResource_->Map(0, nullptr, reinterpret_cast<void**>(&directionalLightSphereData_));
	//色の設定
	directionalLightSphereData_->color = { 1.0f,1.0f,1.0f,1.0f };
	directionalLightSphereData_->direction = { 0.0f,-1.0f,0.0f };
	directionalLightSphereData_->intensity = 1.0f;

	//エミッター
	emitter_.transform.translate = { 0.0f,0.0f,-3.0f };
	emitter_.transform.rotate = { 0.0f,0.0f,0.0f };
	emitter_.transform.scale = { 1.0f,1.0f,1.0f };
	emitter_.count = 3;
	emitter_.frequency = 0.5f;
	emitter_.frequencyTime = 0.0f;

	//パーティクル発生
	particleBorn_ = ParticleBorn::Stop;

	ParticleManager::GetInstance()->SetCamera(camera_);
}

void Particle::Update() {

	const float kDeltaTime = 1.0f / 60.0f;

	switch (particleBorn_)
	{
	case ParticleBorn::TimerMode:
		//時間を経過させる
		emitter_.frequencyTime += kDeltaTime;
		//時間が特定数を上回ったら
		if (emitter_.frequency <= emitter_.frequencyTime) {
			//発生処理
			ParticleManager::GetInstance()->Emit(fileName_, emitter_);
			emitter_.frequencyTime -= emitter_.frequency;//時間を元に戻す
		}
		break;
	case ParticleBorn::MomentMode:
		//発生処理
		ParticleManager::GetInstance()->Emit(fileName_, emitter_);
		particleBorn_ = ParticleBorn::Stop;//すぐに止める
		break;
	case ParticleBorn::Stop:
		break;
	}

	//出てきたパーティクルの更新処理
	ParticleManager::GetInstance()->Update(fileName_, wvpData_);
	//出ているパーティクルの数(インスタンス)をコピー
	numInstance_ = ParticleManager::GetInstance()->GetNum(fileName_);

	// directionalLightSphereDataのnullチェック
	if (directionalLightSphereData_) {
		directionalLightSphereData_->direction = Normalize(directionalLightSphereData_->direction);
	}

}

void Particle::Draw() {
	//射影行列
	Matrix4x4 WorldViewProjectionMatrix;

	for (uint32_t i = 0; i < numInstance_; i++) {
		if (camera_) {
			Matrix4x4 projectionMatrix = camera_->GetViewProjectionMatrix();
			WorldViewProjectionMatrix = wvpData_[i].World * projectionMatrix;
		}
		else {
			WorldViewProjectionMatrix = wvpData_[i].World;
		}

		wvpData_[i].WVP = WorldViewProjectionMatrix;
	}

	//パーティクルが出ていないときはパス
	if (numInstance_ > 0) {
		particleCommon_->GetDxCommon()->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView_);
		particleCommon_->GetDxCommon()->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress()); //rootParameterの配列の0番目 [0]
		particleCommon_->GetDxCommon()->GetCommandList()->SetGraphicsRootConstantBufferView(1, wvpResource_->GetGPUVirtualAddress());
		particleCommon_->GetDxCommon()->GetCommandList()->SetGraphicsRootDescriptorTable(2, TextureManager::GetInstance()->GetSrvHandleGPU(textureFile_));
		particleCommon_->GetDxCommon()->GetCommandList()->SetGraphicsRootConstantBufferView(3, directionalLightSphereResource_->GetGPUVirtualAddress());

		//4のやつ particle専用
		particleCommon_->GetDxCommon()->GetCommandList()->SetGraphicsRootDescriptorTable(4, ParticleManager::GetInstance()->GetSrvHandleGPU(fileName_));

		particleCommon_->GetDxCommon()->GetCommandList()->DrawInstanced(UINT(modelData_.vertices.size()), numInstance_, 0, 0);
	}
	numInstance_ = 0;

	ParticleManager::GetInstance()->ResetNum(fileName_);
}
