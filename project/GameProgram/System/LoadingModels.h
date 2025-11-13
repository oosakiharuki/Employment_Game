#pragma once
#include "ModelManager.h"

/// <summary>
/// モデルのロードをいっぺんにやる場所
/// </summary>
class LoadingModels {
public:
	static LoadingModels* GetInstance();
	
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

	static LoadingModels* sInstance;

	/// <summary>
	/// .objを読み取る
	/// </summary>
	void LoadObj();
	/// <summary>
	/// .gltfを読み取る
	/// </summary>
	void LoadGltf();


	LoadingModels() = default;
	~LoadingModels() = default;
	LoadingModels(LoadingModels&) = default;
	LoadingModels& operator=(LoadingModels&) = default;
};