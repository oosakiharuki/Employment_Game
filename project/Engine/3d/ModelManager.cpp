#include "ModelManager.h"

ModelManager* ModelManager::instance = nullptr;


ModelManager* ModelManager::GetInstance() {
	if (instance == nullptr) {
		instance = new ModelManager;
	}
	return instance;
}

void ModelManager::Finalize() {
	delete instance;
	instance = nullptr;
}

void ModelManager::Initialize(DirectXCommon* dxCommon) {
	modelCommon = new ModelCommon;
	modelCommon->Initialize(dxCommon);
}

void ModelManager::LoadModel(const std::string& filePath, const std::string& objType) {
	
	std::string fileName = filePath + objType;
	
	//同じ名前でもオブジェクトタイプ違ければ作成される
	if (objs.contains(fileName)) {
		return;
	}
	if (glTFs.contains(fileName)) {
		return;
	}

	if (objType == ".obj") {
		std::unique_ptr<Model_obj> model = std::make_unique<Model_obj>();
		model->Initialize(modelCommon, "resource", "Object/" + filePath, fileName);//model,file名,OBJ本体
		objs.insert(std::make_pair(fileName, std::move(model)));
	}
	else if (objType == ".gltf") {
		std::unique_ptr<Model_glTF> model = std::make_unique<Model_glTF>();
		model->Initialize(modelCommon, "resource", "Object/" + filePath + "/" + fileName);//model,file名,OBJ本体
		glTFs.insert(std::make_pair(fileName, std::move(model)));
	}
}

Model_glTF* ModelManager::FindModel_gltf(const std::string& filePath) {
	if(glTFs.contains(filePath)){
		return glTFs.at(filePath).get();
	}

	//ファイル一致なし
	return nullptr;
}

Model_obj* ModelManager::FindModel_obj(const std::string& filePath) {
	if (objs.contains(filePath)) {
		return objs.at(filePath).get();
	}

	//ファイル一致なし
	return nullptr;
}