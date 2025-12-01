#pragma once
#include "ModelManager.h"

/// <summary>
/// モデルのロードをいっぺんにやる場所
/// </summary>
class LoadingModels {
public:
	static std::shared_ptr<LoadingModels> GetInstance();
	
	/// <summary>
	/// ロード
	/// </summary>
	void LoadObjects();
	/// <summary>
	/// 削除処理
	/// </summary>
	void Finalize();
private:

	ModelManager* modelManager;

	static std::shared_ptr<LoadingModels> sInstance;

	/// <summary>
	/// .objを読み取る
	/// </summary>
	void LoadObj();
	/// <summary>
	/// .gltfを読み取る
	/// </summary>
	void LoadGltf();

};