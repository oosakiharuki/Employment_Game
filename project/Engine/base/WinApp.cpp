#include "WinApp.h"

#ifdef USE_IMGUI
#include"externals/imgui/imgui_impl_win32.h"
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
#endif // USE_IMGUI

#pragma comment(lib,"winmm.lib")

void WinApp::Initialize() {
	HRESULT hr = CoInitializeEx(0, COINIT_MULTITHREADED);
	//システムタイマー
	timeBeginPeriod(1);

	//ウィンドウプロシージャ
	wc_.lpfnWndProc = WindowProc;
	//ウィンドウクラス名
	wc_.lpszClassName = L"C62WindowClass";
	//インスタンスハンドル
	wc_.hInstance = GetModuleHandle(nullptr);
	//カーソル
	wc_.hCursor = LoadCursor(nullptr, IDC_ARROW);

	//ウィンドウクラスの登録
	RegisterClass(&wc_);

	//　ウィンドウサイズを表す構造体にクライアント領域を入れる
	RECT wrc = { 0, 0,kClientWidth_,kClientHeight_ };
	//クライアント領域をもとに実際のサイズにwrcを変更してもらう
	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);

	hwnd_ = CreateWindow(
		wc_.lpszClassName,                         //利用するクラス名
		L"LE3B_05_オオサキ_ハルキ_パラソルジャー", //バーの名前
		WS_OVERLAPPEDWINDOW,                       //ウィンドウスタイル
		CW_USEDEFAULT,                             //表示するX座標
		CW_USEDEFAULT,                             //表示するY座標
		wrc.right - wrc.left,                      //ウィンドウ横幅
		wrc.bottom - wrc.top,                      //ウィンドウ縦幅
		nullptr,
		nullptr,
		wc_.hInstance,                             //インスタンスハンドル
		nullptr);

	ShowWindow(hwnd_, SW_SHOW);
}


LRESULT CALLBACK WinApp::WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {

#ifdef USE_IMGUI
	if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam)) {
		return true;
	}
#endif // USE_IMGUI

	//メッセージに応じてゲーム固有の処理を行う
	switch (msg) {
		//ウィンドウが破壊された
	case WM_DESTROY:
		//OSに対して、アプリの終了を伝える
		PostQuitMessage(0);
		return 0;
	}


	//標準のメッセージ処理を行う
	return DefWindowProc(hwnd, msg, wParam, lParam);
}

bool WinApp::ProcessMessage() {

	MSG msg{};

	if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	if (msg.message == WM_QUIT) {
		return true;
	}


	return false;
}

void WinApp::Finalize(){
	CloseWindow(hwnd_);
	CoUninitialize();
}