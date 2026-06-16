#pragma once
#include "DirectXCommon.h"

namespace EngineLayer {
	/// <summary>
	/// RTV マネージャ
	/// </summary>
	class RtvManager {
	public:
		/// <summary>
		/// インスタンス生成
		/// </summary>
		/// <returns>インスタンス</returns>
		static RtvManager& GetInstance();

		/// <summary>
		/// 初期化処理
		/// </summary>
		void Initialize();

		/// <summary>
		/// 解放処理
		/// </summary>
		void Finalize();

		/// <summary>
		/// RTVを作成
		/// </summary>
		void CreateRTV(const Microsoft::WRL::ComPtr<ID3D12Resource>& resource);

		const D3D12_CPU_DESCRIPTOR_HANDLE& GetHandle(uint32_t number) { return rtvHandles_[number]; }

		const D3D12_RENDER_TARGET_VIEW_DESC& GetDesc() { return rtvDesc_; }

	private:

		static std::unique_ptr<RtvManager> sInstance_;
		friend struct std::default_delete<RtvManager>;

		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvDescriptorHeap_;
		uint32_t descriptorSizeRTV_;


		//RTV
		D3D12_RENDER_TARGET_VIEW_DESC rtvDesc_{};



		std::array<D3D12_CPU_DESCRIPTOR_HANDLE, 3> rtvHandles_;
		uint32_t useIndex_ = 0;
	};
}