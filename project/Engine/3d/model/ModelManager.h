#pragma once
#include <map>
#include <string>
#include <memory>

#include "BaseModel.h"

#include "Model_obj.h"
#include "Model_glTF.h"
/// <summary>
/// モデルマネージャ
/// </summary>
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
	/// <param name="filePath">オブジェクトの名前</param>
	/// <param name="objType">オブジェクトの拡張子(obj/gltf)</param>
	/// <param name="isAnimation">アニメーションがあるか</param>
	/// <param name="isSkinning">スキニングがあるか</param>
	void LoadModel(const std::string& filePath, const std::string& objType, bool isAnimation = false, bool isSkinning = false);
	/// <summary>
	/// .gltfのモデルを取り出す
	/// </summary>
	/// <param name="filePath">.gltfのモデルパス名</param>
	/// <returns>ファイルパスで指定した.gltf型のモデル</returns>
	Model_glTF* FindModel_gltf(const std::string& filePath);
	/// <summary>
	/// .objのモデルを取り出す
	/// </summary>
	/// <param name="filePath">.objのモデルパス名</param>
	/// <returns>ファイルパスで指定した.obj型のモデル</returns>
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