#include "ModelManager.h"

std::unique_ptr<ModelManager> ModelManager::sInstance_ = nullptr;


ModelManager& ModelManager::GetInstance() {
	if (sInstance_ == nullptr) {
		sInstance_ = std::make_unique<ModelManager>();
	}
	return *sInstance_;
}

void ModelManager::Finalize() {
	sInstance_.reset();
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
		model->Initialize("resource", filePath);//model,file名,OBJ本体
		objs_.insert(std::make_pair(fileName, std::move(model)));
	}
	else if (objType == ".gltf") {
		std::unique_ptr<Model_glTF> model = std::make_unique<Model_glTF>();
		model->Initialize("resource", filePath + "/" + fileName);//model,file名,OBJ本体
		model->InitAnimation("resource", filePath + "/" + fileName, isAnimation, isSkinning);//アニメーションが必要な場合
		glTFs_.insert(std::make_pair(fileName, std::move(model)));
	}
}

Model_glTF* ModelManager::FindModel_gltf(const std::string& filePath) {
	if (glTFs_.contains(filePath)) {
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