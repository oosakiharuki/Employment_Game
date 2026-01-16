#pragma once

#include<d3d12.h>
#include<dxgi1_6.h>
//ComPtr
#include <wrl.h>
#include "Logger.h"
#include "StringUtility.h"
#include "WinApp.h"

#include <array>
#include <dxcapi.h>

#include "externals/DirectXTex/DirectXTex.h"
#include <chrono>


#include "Vector4.h"
/// <summary>
/// DirectXの共有部分
/// </summary>
class DirectXCommon {
public:
	/// <summary>
	/// インスタンス生成
	/// </summary>
	/// <returns></returns>
	static std::shared_ptr<DirectXCommon> GetInstance();
	/// <summary>
	/// 初期化処理
	/// </summary>
	void Initialize();
	/// <summary>
	/// ドライブ作成
	/// </summary>
	void Device();
	/// <summary>
	/// コマンド作成
	/// </summary>
	void Command();
	/// <summary>
	/// スワップチェーン作成
	/// </summary>
	void SwapChain();
	/// <summary>
	/// 深度バッファ作成
	/// </summary>
	void ZBuffer();
	/// <summary>
	/// でスクリプターヒープ作成
	/// </summary>
	void DescriptorHeap();

	Microsoft::WRL::ComPtr <ID3D12DescriptorHeap> CreateDescriptorHeap(
		D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDesciptors, bool shaderVisible);
	/// <summary>
	/// レンダーターゲットビューの作成
	/// </summary>
	void RTV();

	
	/// <summary>
	/// 深度ステンシルビュー作成
	/// </summary>
	void DSV();
	/// <summary>
	/// フェンス作成
	/// </summary>
	void Fence();
	/// <summary>
	/// ビューポート作成
	/// </summary>
	void ViewPort();
	/// <summary>
	/// シザー作成
	/// </summary>
	void Scissor();
	/// <summary>
	/// DXC作成
	/// </summary>
	void DXC();
	/// <summary>
	/// setter_WinApp
	/// </summary>
	/// <param name="winApp"></param>
	void SetWinApp(WinApp* winApp) { winApp_ = winApp; }

	/// <summary>
	/// コンパイルシェーダ作成
	/// </summary>
	/// <param name="filePath"></param> HLSLの名前
	/// <param name="profile"></param> vs_6_0 or ps_6_0
	/// <returns></returns>
	Microsoft::WRL::ComPtr <IDxcBlob> CompileShader(const std::wstring& filePath, const wchar_t* profile);
	/// <summary>
	/// シェーダで使用するバッファ
	/// </summary>
	/// <param name="sizeInBytes"></param>
	/// <returns></returns>
	Microsoft::WRL::ComPtr<ID3D12Resource> CreateBufferResource(size_t sizeInBytes);
	/// <summary>
	/// シェーダで使用するバッファ
	/// </summary>
	/// <param name="metadata"></param>
	/// <returns></returns>
	Microsoft::WRL::ComPtr<ID3D12Resource> CreateTextureResource(const DirectX::TexMetadata& metadata);
	/// <summary>
	/// テクスチャデータ
	/// </summary>
	/// <param name="texture"></param>
	/// <param name="mipImages"></param>
	/// <returns></returns>
	Microsoft::WRL::ComPtr<ID3D12Resource> UploadTextureData(Microsoft::WRL::ComPtr<ID3D12Resource> texture, const DirectX::ScratchImage& mipImages);

	/// <summary>
	/// 描画バリア開始
	/// </summary>
	void PreDraw();
	/// <summary>
	/// 描画バリア解除
	/// </summary>
	void PostDraw();

	/// <summary>
	/// getter_Drive
	/// </summary>
	/// <returns></returns>
	ID3D12Device*  GetDevice()const { return device_.Get(); }
	/// <summary>
	/// getter_コマンドリスト
	/// </summary>
	/// <returns></returns>
	ID3D12GraphicsCommandList* GetCommandList() const { return commandList_.Get(); }
	/// <summary>
	/// getter_DSVのハンドル
	/// </summary>
	/// <returns></returns>
	D3D12_CPU_DESCRIPTOR_HANDLE GetDsvHandle() const { return dsvHandle_; }
	/// <summary>
	/// getter_fenceEvent
	/// </summary>
	/// <returns></returns>
	HANDLE GetFenceEvent() const { return fenceEvent_; }
	/// <summary>
	/// getter_rtvDesc
	/// </summary>
	/// <returns></returns>
	D3D12_RENDER_TARGET_VIEW_DESC GetRtvDesc() const { return rtvDesc_; }
	/// <summary>
	/// getter_デスクリプターヒープ
	/// </summary>
	/// <returns></returns>
	ID3D12DescriptorHeap* GetSrvDescriptorHeap() { return srvDescriptorHeap_.Get(); }
	/// <summary>
	/// スワップチェーンのリソースナンバー
	/// </summary>
	/// <returns></returns>
	size_t GetSwapChainResourceNum() const { return swapChainDesc_.BufferCount; }

	/// <summary>
	/// RTVのリソース作成(device,width,heightは省略)
	/// </summary>
	/// <param name="format"></param>
	/// <param name="clearColor"></param>
	/// <returns></returns>
	Microsoft::WRL::ComPtr<ID3D12Resource> CreateRenderTextureResource(DXGI_FORMAT format, const Vector4& clearColor);
	/// <summary>
	/// getter_RTVのリソース
	/// </summary>
	/// <returns></returns>
	ID3D12Resource* GetRenderTexture() { return renderTextureResource_.Get(); }
	/// <summary>
	/// getter_DSVのリソース
	/// </summary>
	/// <returns></returns>
	ID3D12Resource* GetOutlineResource() { return depthStencilResource_.Get(); }
	/// <summary>
	/// レンダーテクスチャ用の描画バリア開始
	/// </summary>
	void RenderTexturePreDraw();
	/// <summary>
	/// レンダーテクスチャ用の描画バリア解除
	/// </summary>
	void RenderTexturePostDraw();
	/// <summary>
	/// fade用
	/// </summary>
	void FadePreDraw();
	/// <summary>
	/// 解除処理
	/// </summary>
	void Finalize();

	//最大SRV数
	static const uint32_t kMaxSRVCount_;

private:

	/// <summary>
	/// ファクトリー生成
	/// </summary>
	void Factory();
	/// <summary>
	/// アダプタ生成
	/// </summary>
	void Adapter();
	/// <summary>
	/// ドライブ生成
	/// </summary>
	void CreateDevice();
	/// <summary>
	/// エラー処理でストップ
	/// </summary>
	void ErrorStop();

	Microsoft::WRL::ComPtr<IDXGIAdapter4> useAdapter = nullptr;

	//デバイス
	Microsoft::WRL::ComPtr<ID3D12Device> device_;
	Microsoft::WRL::ComPtr<IDXGIFactory7> dxgiFactory_;

	///コマンド-----------------

	//コマンドキュー
	Microsoft::WRL::ComPtr < ID3D12CommandQueue> commandQueue_ = nullptr;
	//コマンドアロケータ
	Microsoft::WRL::ComPtr < ID3D12CommandAllocator> commandAllocator_ = nullptr;
	//コマンドリスト
	Microsoft::WRL::ComPtr < ID3D12GraphicsCommandList> commandList_ = nullptr;


	//スワップチェーン
	Microsoft::WRL::ComPtr < IDXGISwapChain4> swapChain_ = nullptr;
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc_{};

	static const uint32_t kMaxResource_ = 2;
	std::array<Microsoft::WRL::ComPtr<ID3D12Resource>, 2> swapChainResources_;
	
	WinApp* winApp_ = nullptr;


	//深度バッファ
	Microsoft::WRL::ComPtr<ID3D12Resource> depthStencilResource_ = nullptr;


	//デスクリプタヒープ
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap_ = nullptr;
	D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc_{};

	uint32_t descriptorSizeSRV_;
	uint32_t descriptorSizeRTV_;
	uint32_t descriptorSizeDSV_;


	Microsoft::WRL::ComPtr < ID3D12DescriptorHeap> rtvDescriptorHeap_;
	Microsoft::WRL::ComPtr < ID3D12DescriptorHeap> srvDescriptorHeap_;
	Microsoft::WRL::ComPtr < ID3D12DescriptorHeap> dsvDescriptorHeap_;


	//RTV
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc_{};

	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles_[kMaxResource_];
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandlesRT_;

	//ビューポート
	D3D12_VIEWPORT viewport_;

	//シザリング短径(シザー)
	D3D12_RECT scissorRect_{};


	//DXC
	Microsoft::WRL::ComPtr <IDxcUtils> dxcUtils_ = nullptr;
	Microsoft::WRL::ComPtr <IDxcCompiler3> dxcCompiler_ = nullptr;
	Microsoft::WRL::ComPtr <IDxcIncludeHandler> includeHandler_ = nullptr;

	/// <summary>
	/// HLSLを読み取る
	/// </summary>
	/// <param name="filePath">HLSLパス</param>
	/// <param name="profile">プロファイル</param>
	void LoadHLSL(const std::wstring& filePath,const wchar_t* profile);
	/// <summary>
	/// 警告エラー
	/// </summary>
	void CompileError();
	/// <summary>
	/// コンパイルが完了した
	/// </summary>
	/// <param name="filePath">HSLSパス</param>
	/// <param name="profile">プロファイル</param>
	void CompileSuccess(const std::wstring& filePath, const wchar_t* profile);

	Microsoft::WRL::ComPtr<IDxcResult> shaderResult = nullptr;
	Microsoft::WRL::ComPtr<IDxcBlob> shaderBlob = nullptr;

	/// <summary>
	/// バリア設定
	/// </summary>
	/// <param name="resource">バリアを貼る対象のリソース</param>
	/// <param name="stateBefore">前のリソースステート</param>
	/// <param name="stateAfter">後のリソースステート</param>
	void SetBarrier(ID3D12Resource* resource,D3D12_RESOURCE_STATES stateBefore,D3D12_RESOURCE_STATES stateAfter);

	/// <summary>
	/// 共有描画処理
	/// </summary>
	/// <param name="handle">CPUハンドル</param>
	/// <param name="color">クリアカラー</param>
	void DrawCommon(D3D12_CPU_DESCRIPTOR_HANDLE handle, float color[]);

	//Update


	Microsoft::WRL::ComPtr <ID3D12Fence> fence_ = nullptr;
	uint64_t fenceValue_ = 0;
	HANDLE fenceEvent_;

	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle_;

	//TransitionBarrierの設定
	D3D12_RESOURCE_BARRIER barrier_{};

	//Fix = 固定
	void InitializeFixFPS();
	void UpdateFixFPS();
	//逆行しないタイマー
	std::chrono::steady_clock::time_point reference_;


	static std::shared_ptr<DirectXCommon> sInstance_;

	//書き込み可能なテクスチャ レンダーテクスチャ
	Microsoft::WRL::ComPtr<ID3D12Resource> renderTextureResource_;
	
	D3D12_CLEAR_VALUE clearValue_;
};