#pragma once
#include "ModelManager.h"

/// <summary>
/// モデルのロードをいっぺんにやる場所
/// </summary>
class LoadingModels {
public:
	/// <summary>
	/// インスタンス生成
	/// </summary>
	/// <returns></returns>
	static LoadingModels& GetInstance();
	
	/// <summary>
	/// ロード
	/// </summary>
	void LoadObjects();
	/// <summary>
	/// 削除処理
	/// </summary>
	void Finalize();
private:
	//インスタンス
	static std::unique_ptr<LoadingModels> sInstance_;
	//default_deleteを設定(解放処理を行える)
	friend struct std::default_delete<LoadingModels>;

	/// <summary>
	/// .objを読み取る
	/// </summary>
	void LoadObj();
	/// <summary>
	/// .gltfを読み取る
	/// </summary>
	void LoadGltf();

};