#pragma once
#include "MyMath.h"
#include "ModelCommon.h"
#include <assimp/scene.h>


class Model_glTF{
public:

	void Initialize(ModelCommon* modelCommon,const std::string& directorypath,const std::string& fileName);

	void Draw();
	void Draw(const std::string& textureFilePath);

	static MaterialData LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename);
	//static ModelData_glTF LoadObjFile(const std::string& directoryPath, const std::string& filename);
	//gltf用
	static ModelData_glTF LoadModelFile(const std::string& directoryPath, const std::string& filename);
	static Animation LoadAnimationFile(const std::string& directoryPath, const std::string& filename);

	void LightOn(bool Light) { materialData->enableLighting = Light; }
	void SetEnvironment(const std::string mapFile);

	static Node ReadNode(aiNode* node);

	ModelData_glTF GetModelData() { return modelData; }
	Animation GetAnimationData() { return animation; }

	Skeleton GetSkeleton() { return skeleton; }
	SkinCluster GetSkinCluster() { return skinCluster; }

	SkinCluster CreateSkinCluster(const Skeleton& skeleton, const ModelData_glTF& modelData);

	Material* GetMaterial() { return materialData; }
private:
	ModelCommon* modelCommon = nullptr;

	ModelData_glTF modelData;

	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource;
	Microsoft::WRL::ComPtr<ID3D12Resource> indexResource; //index
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource;


	VertexData* vertexData = nullptr;
	uint32_t* mappedIndex = nullptr;
	Material* materialData = nullptr;

	D3D12_VERTEX_BUFFER_VIEW vertexBufferView;
	D3D12_INDEX_BUFFER_VIEW indexBufferView; //index

	ModelData_glTF InitialData;
	
	//アニメーション
	Animation animation;

	Skeleton skeleton;

	SkinCluster skinCluster;

	D3D12_VERTEX_BUFFER_VIEW vbvs[2];
	
	std::string EnvironmentFile;
};