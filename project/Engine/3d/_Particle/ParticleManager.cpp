#include "ParticleManager.h"
#include "ModelManager.h"
#include <cassert>
#include <numbers>

#include "Camera.h"

using namespace MyMath;
using namespace Primitive;

ParticleManager* ParticleManager::sInstance = nullptr;

uint32_t ParticleManager::sSRVIndexTop = 1;

ParticleManager* ParticleManager::GetInstance() {
	if (sInstance == nullptr) {
		sInstance = new ParticleManager();
	}
	return sInstance;
}

void ParticleManager::Initialize(DirectXCommon* dxCommon, SrvManager* srvManager) {
	particleCommon = ParticleCommon::GetInstance(); 
	this->srvManager = srvManager;
}

void ParticleManager::Finalize() {
	delete sInstance;
	sInstance = nullptr;
}


void ParticleManager::CreateParticleGroup(const std::string name, const std::string textureFilePath, PrimitiveType primitiveType) {

	assert(srvManager->Max());
	
	if (particleGroups.contains(name)) {
		return;
	}
	ParticleGroup& particleG = particleGroups[name];


	particleG.textureFile = textureFilePath;

	//モデル -プリミティブ	
	switch (primitiveType)
	{
	case box:
		particleG.modelData = CreateBox();
		break;
	case plane:
		particleG.modelData = CreatePlane();
		break;
	case ring:
		particleG.modelData = CreateRing();
		break;
	case cylineder:
		particleG.modelData = CreateCylinder();
		break;
	case sphere:
		particleG.modelData = CreateSphere();
		break;
	case cone:
		particleG.modelData = CreateCone();
		break;
	case beam:
		particleG.modelData = CreateBeam();
		break;
	default:
		break;
	}

	//テクスチャ読み込み
	particleG.modelData.material.textureFilePath = textureFilePath;
	particleG.resource = particleCommon->GetDxCommon()->CreateBufferResource(sizeof(ParticleForGPU) * particleG.kNumInstance);

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	srvDesc.Buffer.FirstElement = 0;
	srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
	srvDesc.Buffer.NumElements = particleG.kNumInstance;
	srvDesc.Buffer.StructureByteStride = sizeof(ParticleForGPU);

	particleG.srvIndex = srvManager->Allocate();
	particleG.srvHandleCPU = srvManager->GetCPUDescriptorHandle(particleG.srvIndex);
	particleG.srvHandleGPU = srvManager->GetGPUDescriptorHandle(particleG.srvIndex);
	

	//SRVの生成
	particleCommon->GetDxCommon()->GetDevice()->CreateShaderResourceView(particleG.resource.Get(), &srvDesc, particleG.srvHandleCPU);
	
	particleG.numInstance = 0;
}

D3D12_GPU_DESCRIPTOR_HANDLE  ParticleManager::GetSrvHandleGPU(const std::string filePath) {
	assert(srvManager->Max());

	ParticleGroup& particleG = particleGroups[filePath];
	return particleG.srvHandleGPU;
}

ModelData ParticleManager::GetModelData(const std::string filePath) {
	assert(srvManager->Max());

	ParticleGroup& particleG = particleGroups[filePath];
	return particleG.modelData;
}

std::string ParticleManager::GetTextureHandle(const std::string filePath) {
	assert(srvManager->Max());

	ParticleGroup& particleG = particleGroups[filePath];
	return particleG.textureFile;
}

Microsoft::WRL::ComPtr<ID3D12Resource> ParticleManager::GetResource(const std::string filePath) {
	assert(srvManager->Max());

	ParticleGroup& particleG = particleGroups[filePath];
	return particleG.resource;
}

std::list<ParticleData> ParticleManager::GetParticle(const std::string filePath) {
	assert(srvManager->Max());

	ParticleGroup& particleG = particleGroups[filePath];
	return particleG.particles;
}

void ParticleManager::Update(const std::string filePath, ParticleForGPU* wvpData) {

	assert(srvManager->Max());

	ParticleGroup& particleG = particleGroups[filePath];

	//複数ある場合、出ていない部分は描画されないように
	particleG.numInstance = 0;

	//filePath名ですでに更新している場合
	if (particleG.Updated) {
		return;
	}
	particleG.Updated = true;

	for (std::list<ParticleData>::iterator particleIterator = particleG.particles.begin();
		particleIterator != particleG.particles.end(); ) {

		//生存時間を過ぎた or スケール(x,y,z)のいずれかが0以下の場合
		if ((*particleIterator).lifeTime <= (*particleIterator).currentTime || 
			((*particleIterator).transform.scale.x < 0 || (*particleIterator).transform.scale.y < 0 || (*particleIterator).transform.scale.z < 0)) {
			particleIterator = particleG.particles.erase(particleIterator);
			continue;
		}

		float alpha = 1.0f - ((*particleIterator).currentTime / (*particleIterator).lifeTime);

		(*particleIterator).transform.translate += (*particleIterator).velocityTransform.translate * kDeltaTime;
		
		if (filePath == "clear_fanfare") {
			(*particleIterator).velocityTransform.translate.y -= 0.1f;
		}
		
		(*particleIterator).transform.rotate += (*particleIterator).velocityTransform.rotate;

		if ((*particleIterator).transform.scale.x > 0) {
			(*particleIterator).transform.scale.x += (*particleIterator).velocityTransform.scale.x * kDeltaTime;
		}
		if ((*particleIterator).transform.scale.y > 0) {
			(*particleIterator).transform.scale.y += (*particleIterator).velocityTransform.scale.y * kDeltaTime;
		}
		if ((*particleIterator).transform.scale.z > 0) {
			(*particleIterator).transform.scale.z += (*particleIterator).velocityTransform.scale.z * kDeltaTime;
		}


		(*particleIterator).currentTime += kDeltaTime;

		Matrix4x4 scaleMatrix = MakeScaleMatrix((*particleIterator).transform.scale);
		Matrix4x4 translateMatrix = MakeTranslateMatrix((*particleIterator).transform.translate);

		//回転行列
		Matrix4x4 rotateX = MakeRotateXMatrix((*particleIterator).transform.rotate.x * (float(M_PI) / 180.0f));
		Matrix4x4 rotateY = MakeRotateYMatrix((*particleIterator).transform.rotate.y * (float(M_PI) / 180.0f));
		Matrix4x4 rotateZ = MakeRotateZMatrix((*particleIterator).transform.rotate.z * (float(M_PI) / 180.0f));
		//全てまとめた
		Matrix4x4 rotateXYZ = Multiply(Multiply(rotateX, rotateY), rotateZ);

		//ビルボード
		Matrix4x4 backToFrontMatrix = MakeRotateYMatrix(std::numbers::pi_v<float>);

		Matrix4x4 billboardMatrix = Multiply(Multiply(backToFrontMatrix, rotateXYZ), camera->GetWorldMatrix());
		billboardMatrix.m[3][0] = 0.0f;
		billboardMatrix.m[3][1] = 0.0f;
		billboardMatrix.m[3][2] = 0.0f;


		//ビルボード
		//worldMatrix = Multiply(scaleMatrix, Multiply(billboardMatrix, translateMatrix));
		//通常
		Matrix4x4 worldMatrix = MakeAffineMatrix((*particleIterator).transform.scale, (*particleIterator).transform.rotate, (*particleIterator).transform.translate);

		// wvpDataのnullチェック
		if (wvpData) {
			wvpData[particleG.numInstance].World = worldMatrix;

			wvpData[particleG.numInstance].color = (*particleIterator).color;
			wvpData[particleG.numInstance].color.s = alpha;

			//パーティクルカウンター
			if (particleG.numInstance < kNumMaxInstance) {
				++particleG.numInstance;
			}
		}
		++particleIterator;
	}
	
}

void ParticleManager::Emit(const std::string filePath, const Emitter& emitter) {

	assert(srvManager->Max());

	ParticleGroup& particleG = particleGroups[filePath];
	//
	std::random_device seedGenerator;
	std::mt19937 randomEngine(seedGenerator());

	particleG.particles.splice(particleG.particles.end(), ParticleEmitter::GetInstance()->MakeEmit(filePath ,emitter, randomEngine));
}

const uint32_t& ParticleManager::GetNum(const std::string& filePath) {
	assert(srvManager->Max());

	ParticleGroup& particleG = particleGroups[filePath];
	return particleG.numInstance;
}

void ParticleManager::ResetNum(const std::string& filePath) {
	assert(srvManager->Max());

	ParticleGroup& particleG = particleGroups[filePath];
	particleG.Updated = false;
}

void ParticleManager::ResetParticle(const std::string& filePath) {
	assert(srvManager->Max());

	ParticleGroup& particleG = particleGroups[filePath];
	particleG.particles.clear();
}

std::unique_ptr<Particle> ParticleManager::InitParticle(const ParticleParametars& parametars) {
	//使用するパーティクルを選ぶ
	std::unique_ptr<Particle>& particle = particles_[parametars.name];
	//パーティクルを設定
	particle = std::make_unique<Particle>();
	//初期化(名前,テクスチャファイル,モデルの形)
	particle->Initialize(parametars.name, parametars.textureFile, parametars.primitive);
	particle->SetParticleCount(parametars.count); //生成数を設定
	particle->SetFrequency(parametars.frequency); //頻度 / 生存時間を設定
	particle->SetScale(parametars.basicSize);     //基本サイズを設定

	return std::move(particle);
}