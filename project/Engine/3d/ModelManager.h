#pragma once
#include <map>
#include <string>
#include <memory>
#include "Model_obj.h"
#include "Model_glTF.h"

class ModelManager{
public:
	/// <summary>
	/// インスタンス生成
	/// </summary>
	/// <returns></returns>
	static std::shared_ptr<ModelManager> GetInstance();
	/// <summary>
	/// 解放処理
	/// </summary>
	void Finalize();
	/// <summary>
	/// 初期化処理
	/// </summary>
	/// <param name="dxCommon"></param>
	void Initialize(DirectXCommon* dxCommon);
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
	static std::shared_ptr<ModelManager> sInstance_;

	std::unique_ptr<ModelCommon> modelCommon_ = nullptr;

	//モデル別のコンテナ
	//.obj用のコンテナ
	std::map<std::string, std::unique_ptr<Model_obj>> objs_;
	//.gltf用のコンテナ
	std::map<std::string, std::unique_ptr<Model_glTF>> glTFs_;

};