#pragma once
#include "Lighting.h"
#include <wrl.h>
#include <d3d12.h>
#include <vector>
#include <memory>

struct ShadowData {
	Vector4 color;
	Vector3 position;
	float intensity;
	Vector3 direction;
	float distance;
	float decay;
	float cosAngle;
	float cosFalloffStart;
	float padding[17];
}; // 60バイト

constexpr uint32_t kMaxShadow = 30;

struct ShadowFactory {		
	ShadowData shadowData[kMaxShadow]; // 60 * kMaxShadow
};

/// <summary>
/// エンジン層
/// </summary>
namespace EngineLayer {

	class ShadowManager {
	public:

		static ShadowManager& GetInstance();

		void Finalize();

		/// <summary>
		/// 
		/// </summary>
		void Initialize();
		/// <summary>
		/// 
		/// </summary>
		void Update();
		/// <summary>
		/// 
		/// </summary>
		void Draw();
	
		void AddShadow(const Vector3& shadowPosition);

		void Reset();

	private:

		static std::unique_ptr<ShadowManager> sInstance_;
		friend struct std::default_delete<ShadowManager>;


		Microsoft::WRL::ComPtr<ID3D12Resource> resource_;

		ShadowFactory* shadowFactory_ = nullptr;

		std::vector<ShadowData> shadowDatas_;
	};
}
