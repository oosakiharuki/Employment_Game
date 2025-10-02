#pragma once
#include <map>
#include <string>
#include <memory>
#include "Particle.h"
#include "SrvManager.h"
#include "ParticleEmitter.h"
#include "Primitive.h"

class ParticleManager {
public:
	static ParticleManager* GetInstance();
	void Finalize();
	void Initialize(DirectXCommon* dxCommon, SrvManager* srvManager);

	void CreateParticleGroup(const std::string name, const std::string textureFilePath, PrimitiveType primitiveType);

	D3D12_GPU_DESCRIPTOR_HANDLE  GetSrvHandleGPU(const std::string filePath);
	ModelData GetModelData(const std::string filePath);
	std::string GetTextureHandle(const std::string filePath);
	Microsoft::WRL::ComPtr<ID3D12Resource> GetResource(const std::string filePath);
	std::list<Particles> GetParticle(const std::string filePath);

private:
	static ParticleManager* instance;

	ParticleManager() = default;
	~ParticleManager() = default;
	ParticleManager(ParticleManager&) = delete;
	ParticleManager& operator=(ParticleManager&) = delete;

	ParticleCommon* particleCommon = nullptr;

	SrvManager* srvManager = nullptr;
	static uint32_t kSRVIndexTop;

	struct ParticleGroup {
		std::string textureFile;
		ModelData modelData;

		std::list<Particles> particles;
		uint32_t srvIndex;
		Microsoft::WRL::ComPtr<ID3D12Resource> resource;
		uint32_t kNumInstance = 100;
		DirectX::TexMetadata metadata; //width,height


		D3D12_CPU_DESCRIPTOR_HANDLE srvHandleCPU;
		D3D12_GPU_DESCRIPTOR_HANDLE srvHandleGPU;
	};

	const float kDeltaTime = 1.0f / 60.0f;
	std::unordered_map<std::string, ParticleGroup> particleGroups;

};