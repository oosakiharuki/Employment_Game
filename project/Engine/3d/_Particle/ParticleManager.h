#pragma once
#include <map>
#include <string>
#include <memory>
#include "Particle.h"
#include "SrvManager.h"
#include "ParticleEmitter.h"
#include "Primitive.h"
/// <summary>
/// パーティクルマネージャ
/// </summary>
class ParticleManager {
public:
	/// <summary>
	/// インスタンス生成
	/// </summary>
	/// <returns></returns>
	static ParticleManager* GetInstance();
	/// <summary>
	/// 解放処理
	/// </summary>
	void Finalize();
	/// <summary>
	/// 初期化処理
	/// </summary>
	/// <param name="dxCommon"></param>
	/// <param name="srvManager"></param>
	void Initialize(DirectXCommon* dxCommon, SrvManager* srvManager);

	void CreateParticleGroup(const std::string name, const std::string textureFilePath, PrimitiveType primitiveType);

	D3D12_GPU_DESCRIPTOR_HANDLE  GetSrvHandleGPU(const std::string filePath);
	ModelData GetModelData(const std::string filePath);
	std::string GetTextureHandle(const std::string filePath);
	Microsoft::WRL::ComPtr<ID3D12Resource> GetResource(const std::string filePath);
	std::list<Particles> GetParticle(const std::string filePath);
	
	const uint32_t& GetNum(const std::string& filePath);

	void Update(const std::string filePath, ParticleForGPU* wvpData);

	void Emit(const std::string filePath,const Emitter& emitter);

	void SetCamera(Camera* camera_) { camera = camera_; }

	void ResetNum(const std::string& filePath);
	/// <summary>
	/// パーティクルを一度消す
	/// </summary>
	/// <param name="filePath"></param>パーティクルグループ名を選ぶ
	void ResetParticle(const std::string& filePath);

private:
	static ParticleManager* sInstance;

	ParticleManager() = default;
	~ParticleManager() = default;
	ParticleManager(ParticleManager&) = delete;
	ParticleManager& operator=(ParticleManager&) = delete;

	ParticleCommon* particleCommon = nullptr;

	SrvManager* srvManager = nullptr;
	static uint32_t sSRVIndexTop;

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

		uint32_t numInstance = 0;

		bool Updated = false;
	};

	const float kDeltaTime = 1.0f / 60.0f;
	std::unordered_map<std::string, ParticleGroup> particleGroups;

	Camera* camera = nullptr;
	static const uint32_t kNumMaxInstance = 100;

};