#include "ParticleManager.h"
#include "ModelManager.h"
#include <cassert>
#include <numbers>

using namespace MyMath;

using namespace StringUtility;

ParticleManager* ParticleManager::instance = nullptr;

uint32_t ParticleManager::kSRVIndexTop = 1;

ParticleManager* ParticleManager::GetInstance() {
	if (instance == nullptr) {
		instance = new ParticleManager();
	}
	return instance;
}

void ParticleManager::Initialize(DirectXCommon* dxCommon, SrvManager* srvManager) {
	particleCommon = ParticleCommon::GetInstance(); 
	this->srvManager = srvManager;
	particleCommon->Initialize(dxCommon);
}

void ParticleManager::Finalize() {
	particleCommon->Finalize();
	delete instance;
	instance = nullptr;
}


void ParticleManager::CreateParticleGroup(const std::string name, const std::string textureFilePath) {
	//読み込み済み
	if (particleGroups.contains(name)) {
		return;
	}

	assert(srvManager->Max());

	ParticleGroup& particleG = particleGroups[name];


	particleG.textureFile = textureFilePath;

	//仮のモデル
	//particleG.modelData.vertices.push_back({ {1.0f,1.0f,0.0f,1.0f},{0.0f,0.0f},{0.0f,0.0f,1.0f} });
	//particleG.modelData.vertices.push_back({ {-1.0f,1.0f,0.0f,1.0f},{1.0f,0.0f},{0.0f,0.0f,1.0f} });
	//particleG.modelData.vertices.push_back({ {1.0f,-1.0f,0.0f,1.0f},{0.0f,1.0f},{0.0f,0.0f,1.0f} });
	//particleG.modelData.vertices.push_back({ {1.0f,-1.0f,0.0f,1.0f},{0.0f,1.0f},{0.0f,0.0f,1.0f} });
	//particleG.modelData.vertices.push_back({ {-1.0f,1.0f,0.0f,1.0f},{1.0f,0.0f},{0.0f,0.0f,1.0f} });
	//particleG.modelData.vertices.push_back({ {-1.0f,-1.0f,0.0f,1.0f},{1.0f,1.0f},{0.0f,0.0f,1.0f} });


	const uint32_t kCylinderDivide = 32;
	const float kTopRadius = 1.0f;
	const float kBottomRadius = 1.0f;
	const float kHeight = 3.0f;
	const float radianPreDevice = 2.0f * std::numbers::pi_v<float> / float(kCylinderDivide);

	for (uint32_t index = 0; index < kCylinderDivide; ++index) {
		float sin = std::sin(index * radianPreDevice);
		float cos = std::cos(index * radianPreDevice);

		float sinNext = std::sin((index + 1) * radianPreDevice); // ベジェ曲線のイメージ
		float cosNext = std::cos((index + 1) * radianPreDevice);

		float u = float(index) / float(kCylinderDivide);
		float uNext = float(index + 1) / float(kCylinderDivide);

		VertexData vertexData1, vertexData2, vertexData3, vertexData4, vertexData5, vertexData6;

		vertexData1.position = { -sin * kTopRadius,kHeight, cos * kTopRadius,1.0f };
		vertexData1.texcoord = { u,0.0f };
		vertexData1.normal = { -sin,0.0f,cos };

		vertexData2.position = { -sinNext * kTopRadius,kHeight, cosNext * kTopRadius,1.0f };
		vertexData2.texcoord = { uNext,0.0f };
		vertexData2.normal = { -sinNext,0.0f,cosNext };

		vertexData3.position = { -sin * kBottomRadius,0.0f, cos * kBottomRadius,1.0f };
		vertexData3.texcoord = { u,1.0f };
		vertexData3.normal = { -sin,0.0f,cos };

		vertexData4.position = { -sin * kBottomRadius,0.0f, cos * kBottomRadius,1.0f };
		vertexData4.texcoord = { u,1.0f };
		vertexData4.normal = { -sin,0.0f,cos };

		vertexData5.position = { -sinNext * kTopRadius,kHeight, cosNext * kTopRadius,1.0f };
		vertexData5.texcoord = { uNext,0.0f };
		vertexData5.normal = { -sinNext,0.0f,cosNext };
		
		vertexData6.position = { -sinNext * kBottomRadius,0.0f, cosNext * kBottomRadius,1.0f };
		vertexData6.texcoord = { uNext,1.0f };
		vertexData6.normal = { -sinNext,0.0f,cosNext };


		particleG.modelData.vertices.push_back(vertexData1);
		particleG.modelData.vertices.push_back(vertexData2);
		particleG.modelData.vertices.push_back(vertexData3);

		particleG.modelData.vertices.push_back(vertexData3);
		particleG.modelData.vertices.push_back(vertexData2);
		particleG.modelData.vertices.push_back(vertexData6);

	}

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


	//particleEmit->Emit();

	emitter.transform.rotate = { 0.0f,0.0f,0.0f };
	emitter.transform.scale = { 1.0f,1.0f,1.0f };
	emitter.frequency = 0.5f;
	emitter.frequencyTime = 0.0f;
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

std::list<Particles> ParticleManager::GetParticle(const std::string filePath) {
	assert(srvManager->Max());

	ParticleGroup& particleG = particleGroups[filePath];
	return particleG.particles;
}



void ParticleManager::Emit(const std::string name, const Vector3& position, uint32_t count, ParticleType type) {
	ParticleGroup& particleG = particleGroups[name];


	emitter.transform.translate = position;
	emitter.count = count;

	//
	std::random_device seedGenerator;
	std::mt19937 randomEngine(seedGenerator());

	particleG.particles = particleEmit.MakeEmit(emitter, randomEngine,type);
	//emitter.frequencyTime -= emitter.frequency;
	
}
