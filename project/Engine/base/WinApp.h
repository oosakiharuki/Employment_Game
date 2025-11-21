#include<Windows.h>
#include <cstdint>

#pragma once
/// <summary>
/// ウィンドウ作成
/// </summary>
class WinApp
{	
public://静寂メンバ関数
	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

public://メンバ関数
	/// <summary>
	/// 初期化処理
	/// </summary>
	void Initialize();
	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();
	/// <summary>
	/// 解放処理
	/// </summary>
	void Finalize(); //API終了処理　(delete)


	//クライアント領域のサイズ　横　縦
	static const int32_t kClientWidth_ = 1280;
	static const int32_t kClientHeight_ = 720;

	HWND GetHwnd()const { return hwnd_; }
	HINSTANCE GetHInstance() const { return wc_.hInstance; }
	bool ProcessMessage();

private:
	WNDCLASS wc_{};
	HWND hwnd_ = nullptr;
};