#pragma once
#include <map>
#include <string>
#include <memory>

#include "BaseModel.h"

#include "Model_obj.h"
#include "Model_glTF.h"

class ModelManager{
public:
	/// <summary>
	/// インスタンス生成
	/// </summary>
	/// <returns></returns>
	static ModelManager& GetInstance();
	/// <summary>
	/// 解放処理
	/// </summary>
	void Finalize();
	/// <summary>
	/// モデル読み込み
	/// </summary>
	/// <param name="filePath"></param>オブジェクトの名前
	/// <param name="objType"></param>オブジェクトの拡張子(obj/gltf)
	/// <param name="isAnimation"></param>アニメーションがあるか
	/// <param name="isSkinning"></param>スキニングがあるか
	void LoadModel(const std::string& filePath, const std::string& objType, bool isAnimation = false, bool isSkinning = false);
	
	Model_glTF* FindModel_gltf(const std::string& filePath);
	Model_obj* FindModel_obj(const std::string& filePath);

private:
	//インスタンス
	static std::unique_ptr<ModelManager> sInstance_;
	//default_deleteを設定(解放処理を行える)
	friend struct std::default_delete<ModelManager>;

	//モデル別のコンテナ
	//.obj用のコンテナ
	std::map<std::string, std::unique_ptr<Model_obj>> objs_;
	//.gltf用のコンテナ
	std::map<std::string, std::unique_ptr<Model_glTF>> glTFs_;

};