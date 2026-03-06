#include<Windows.h>
#include <cstdint>

#pragma once
/// <summary>
/// ウィンドウ作成
/// </summary>
class WinApp{	
public:
	/// <summary>
	/// 初期化処理
	/// </summary>
	void Initialize();
	/// <summary>
	/// 解放処理
	/// </summary>
	void Finalize(); //API終了処理

	//クライアント領域のサイズ　横　縦
	static const int32_t kClientWidth_ = 1280;
	static const int32_t kClientHeight_ = 720;

	/// <summary>
	/// getter_HWND
	/// </summary>
	/// <returns></returns>
	HWND GetHwnd()const { return hwnd_; }
	/// <summary>
	/// getter_
	/// </summary>
	/// <returns></returns>
	HINSTANCE GetHInstance() const { return wc_.hInstance; }
	/// <summary>
	/// メッセージを出す
	/// </summary>
	/// <returns>メッセージがある場合true</returns>
	bool ProcessMessage();

private:	
	
	//静寂メンバ関数
	
	/// <summary>
	/// ウィンドウプロシージャ
	/// </summary>
	/// <param name="hwnd">ウィンドウズハンドル</param>
	/// <param name="msg">メッセージ</param>
	/// <param name="wParam">メッセージ処理で使用</param>
	/// <param name="lParam">メッセージ処理で使用</param>
	/// <returns>ウィンドウプロシージャ</returns>
	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	WNDCLASS wc_{};
	HWND hwnd_ = nullptr;
};