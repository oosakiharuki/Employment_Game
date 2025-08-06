#pragma once
#include <map>
#include <string>
#include <memory>
#include "Model_obj.h"
#include "Model_glTF.h"

class ModelManager{
public:
	static ModelManager* GetInstance();
	void Finalize();
	std::map<std::string, std::unique_ptr<Model_obj>> objs;
	std::map<std::string, std::unique_ptr<Model_glTF>> glTFs;

	void Initialize(DirectXCommon* dxCommon);

	void LoadModel(const std::string& filePath, const std::string& objType);
	Model_glTF* FindModel_gltf(const std::string& filePath);
	Model_obj* FindModel_obj(const std::string& filePath);

private:
	static ModelManager* instance;

	ModelManager() = default;
	~ModelManager() = default;
	ModelManager(ModelManager&) = delete;
	ModelManager& operator=(ModelManager&) = delete;

	ModelCommon* modelCommon = nullptr;

};