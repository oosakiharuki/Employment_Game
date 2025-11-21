#pragma once
#include "DirectXCommon.h"

/// <summary>
/// モデルの共有処理
/// </summary>
class ModelCommon {
public:
	/// <summary>
	/// 初期化処理
	/// </summary>
	/// <param name="dxCommon"></param>
	void Initialize(DirectXCommon* dxCommon);
	/// <summary>
	/// getter_directXCommon
	/// </summary>
	/// <returns></returns>
	DirectXCommon* GetDxCommon() const{ return dxCommon_; }


private:
	DirectXCommon* dxCommon_;
};