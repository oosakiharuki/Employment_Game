#pragma once
#include "MyMath.h"
#include "ModelCommon.h"
#include <assimp/scene.h>


class Model_glTF{
public:

	void Initialize(ModelCommon* modelCommon,const std::string& directorypath,const std::string& fileName, bool isAnimation, bool isSkinning);

	void Draw();

	//gltf用
	static ModelData_glTF LoadModelFile(const std::string& directoryPath, const std::string& filename);
	static std::vector<Animation> LoadAnimationFile(const std::string& directoryPath, const std::string& filename,uint32_t Number);

	void LightOn(bool Light) { materialData->enableLighting = Light; }
	void SetEnvironment(const std::string mapFile);

	static Node ReadNode(aiNode* node);

	ModelData_glTF GetModelData() { return modelData; }
	std::vector<Animation> GetAnimationData() { return animation; }

	std::vector<Skeleton> GetSkeleton() { return skeletons; }
	std::vector<SkinCluster> GetSkinCluster() { return skinClusters; }

	SkinCluster CreateSkinCluster(const Skeleton& skeleton, const ModelData_glTF& modelData);

	Material* GetMaterial() { return materialData; }

	bool IsSkinning() { return isSkinning_; }
	bool IsAnimation() { return isAnimation_; }

	void ResetI() { i = 0; }

private:
	ModelCommon* modelCommon = nullptr;

	ModelData_glTF modelData;

	std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> vertexResource;
	std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> indexResource; //index
	std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> materialResources;


	VertexData* vertexData = nullptr;
	uint32_t* mappedIndex = nullptr;
	Material* materialData = nullptr;

	std::vector<D3D12_VERTEX_BUFFER_VIEW> vertexBufferView;
	std::vector<D3D12_INDEX_BUFFER_VIEW> indexBufferView; //index

	ModelData_glTF InitialData;
	
	//アニメーション
	std::vector<Animation> animation;

	std::vector<Skeleton> skeletons;

	std::vector<SkinCluster> skinClusters;

	D3D12_VERTEX_BUFFER_VIEW vbvs[2];
	
	std::string EnvironmentFile;

	bool isAnimation_ = false;
	bool isSkinning_ = false;

	uint32_t i = 0;

};