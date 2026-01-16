#include "ImGuiManager.h"

std::unique_ptr<ImGuiManager> ImGuiManager::sInstance_ = nullptr;

ImGuiManager& ImGuiManager::GetInstance() {
	if (sInstance_ == nullptr) {
		sInstance_ = std::make_unique<ImGuiManager>();
	}
	return *sInstance_;
}

void ImGuiManager::Initialize([[maybe_unused]]WinApp* winApp) {
#ifdef  USE_IMGUI

	uint32_t srvIndex = SrvManager::GetInstance().Allocate();
	srvHeap_ = SrvManager::GetInstance().GetDescriptorHeap();

	//ImGui初期化
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(winApp->GetHwnd());
	ImGui_ImplDX12_Init(
		DirectXCommon::GetInstance().GetDevice(),
		static_cast<int>(DirectXCommon::GetInstance().GetSwapChainResourceNum()),
		DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
		srvHeap_.Get(),
		SrvManager::GetInstance().GetCPUDescriptorHandle(srvIndex),
		SrvManager::GetInstance().GetGPUDescriptorHandle(srvIndex));

	//日本語表記
	JapaneseNotation();

#endif //  USE_IMGUI
}

void ImGuiManager::JapaneseNotation() {
#ifdef USE_IMGUI
	///日本語作成用
	ImGuiIO& io = ImGui::GetIO();
	static ImWchar const glyph_ranges[] = { 0x0020, 0xfffd,0, };
	ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\meiryo.ttc", 18.0f, NULL, glyph_ranges);
#endif // USE_IMGUI
}

void ImGuiManager::Begin() {
#ifdef  USE_IMGUI
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

#endif //  USE_IMGUI
}

void ImGuiManager::End() {
#ifdef  USE_IMGUI
	ImGui::Render();
#endif //  USE_IMGUI
}

void ImGuiManager::Draw(){
#ifdef  USE_IMGUI
	ID3D12GraphicsCommandList* commandList = DirectXCommon::GetInstance().GetCommandList();

	ID3D12DescriptorHeap* ppHeaps[] = { srvHeap_.Get() };
	commandList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), commandList);
#endif //  USE_IMGUI

}

void ImGuiManager::Finalize() {
#ifdef  USE_IMGUI

	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	sInstance_.reset();

#endif //  USE_IMGUI
}
