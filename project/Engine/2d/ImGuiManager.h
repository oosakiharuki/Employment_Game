#pragma once

#include "wrl.h"
#include "d3d12.h"
#include "dxgi1_2.h"

#ifdef  USE_IMGUI
#include"externals/imgui/imgui.h"
#include"externals/imgui/imgui_impl_dx12.h"
#include"externals/imgui/imgui_impl_win32.h"
#endif //  USE_IMGUI

#include "WinApp.h"
#include "DirectXCommon.h"
#include "SrvManager.h"
/// <summary>
/// imugiマネージャ
/// </summary>
class ImGuiManager {
public:
	/// <summary>
	/// 初期化処理
	/// </summary>
	/// <param name="winApp"></param>
	void Initialize(WinApp* winApp, DirectXCommon* dxCommon, SrvManager* srvManager);
	
	/// <summary>
	/// インスタンス生成
	/// </summary>
	/// <returns></returns>
	static std::shared_ptr<ImGuiManager> GetInstance();

	/// <summary>
	/// imgui 更新始め
	/// </summary>
	void Begin();

	/// <summary>
	/// imgui 更新終わり
	/// </summary>
	void End();

	/// <summary>
	/// 解放処理
	/// </summary>
	void Finalize();

	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw();

private:

	/// <summary>
	/// 日本語表記
	/// </summary>
	void JapaneseNotation();

	static std::shared_ptr<ImGuiManager> sInstance_;

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> srvHeap_;

	DirectXCommon* dxCommon_ = nullptr;	
	SrvManager* srvManager_ = nullptr;
};