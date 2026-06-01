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
	float padding[2];
	int32_t isEnable;
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
	
		void AddShadow(const Microsoft::WRL::ComPtr<ID3D12Resource>& resource);

		void Reset();

	private:

		static std::unique_ptr<ShadowManager> sInstance_;
		friend struct std::default_delete<ShadowManager>;


		std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> resources_;
	};
}
