#include "ModelManager.h"

ModelManager* ModelManager::sInstance_ = nullptr;


ModelManager* ModelManager::GetInstance() {
	if (sInstance_ == nullptr) {
		sInstance_ = new ModelManager;
	}
	return sInstance_;
}

void ModelManager::Finalize() {
	delete sInstance_;
	sInstance_ = nullptr;
}

void ModelManager::Initialize(DirectXCommon* dxCommon) {
	modelCommon_ = new ModelCommon;
	modelCommon_->Initialize(dxCommon);
}

void ModelManager::LoadModel(const std::string& filePath, const std::string& objType, bool isAnimation, bool isSkinning) {
	
	std::string fileName = filePath + objType;
	
	//同じ名前でもオブジェクトタイプ違ければ作成される
	if (objs_.contains(fileName)) {
		return;
	}
	if (glTFs_.contains(fileName)) {
		return;
	}

	if (objType == ".obj") {
		std::unique_ptr<Model_obj> model = std::make_unique<Model_obj>();
		model->Initialize(modelCommon_, "resource", "Object/" + filePath, fileName);//model,file名,OBJ本体
		objs_.insert(std::make_pair(fileName, std::move(model)));
	}
	else if (objType == ".gltf") {
		std::unique_ptr<Model_glTF> model = std::make_unique<Model_glTF>();
		model->Initialize(modelCommon_, "resource", "Object/" + filePath + "/" + fileName,isAnimation, isSkinning);//model,file名,OBJ本体
		glTFs_.insert(std::make_pair(fileName, std::move(model)));
	}
}

Model_glTF* ModelManager::FindModel_gltf(const std::string& filePath) {
	if(glTFs_.contains(filePath)){
		return glTFs_.at(filePath).get();
	}

	//ファイル一致なし
	return nullptr;
}

Model_obj* ModelManager::FindModel_obj(const std::string& filePath) {
	if (objs_.contains(filePath)) {
		return objs_.at(filePath).get();
	}

	//ファイル一致なし
	return nullptr;
}