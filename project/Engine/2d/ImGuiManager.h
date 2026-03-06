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
/// imGuiマネージャ
/// </summary>
class ImGuiManager {
public:
	/// <summary>
	/// 初期化処理
	/// </summary>
	/// <param name="winApp">ウィンドウズの設定</param>
	void Initialize(WinApp* winApp);
	
	/// <summary>
	/// インスタンス生成
	/// </summary>
	/// <returns>インスタンス</returns>
	static ImGuiManager& GetInstance();

	/// <summary>
	/// imGui 更新始め
	/// </summary>
	void Begin();

	/// <summary>
	/// imGui 更新終わり
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
	//インスタンス
	static std::unique_ptr<ImGuiManager> sInstance_;
	//default_deleteを設定(解放処理を行える)
	friend struct std::default_delete<ImGuiManager>;

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> srvHeap_;
};