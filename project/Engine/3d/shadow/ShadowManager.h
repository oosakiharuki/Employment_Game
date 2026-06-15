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

constexpr uint32_t kMaxShadow = 60;

struct ShadowFactory {		
	ShadowData shadowData[kMaxShadow]; // 60 * kMaxShadow
};

/// <summary>
/// エンジン層
/// </summary>
namespace EngineLayer {

	class ShadowManager {
	public:
		/// <summary>
		/// インスタンス生成
		/// </summary>
		/// <returns>インスタンスを生成する</returns>
		static ShadowManager& GetInstance();
		/// <summary>
		/// 解放処理
		/// </summary>
		void Finalize();
		/// <summary>
		/// 初期化処理
		/// </summary>
		void Initialize();
		/// <summary>
		/// 更新処理
		/// </summary>
		void Update();
		/// <summary>
		/// 描画処理
		/// </summary>
		void Draw();
		/// <summary>
		/// 影(ライティング減算)の追加
		/// </summary>
		/// <param name="shadowPosition">影の始まる位置</param>
		/// <param name="shadowSize">対象の大きさ</param>
		/// <param name="distance">影を映し出す距離</param>
		void AddShadow(const Vector3& shadowPosition, const Vector3& shadowSize,float distance);
		/// <summary>
		/// リセット
		/// </summary>
		void Reset();

	private:

		static std::unique_ptr<ShadowManager> sInstance_;
		friend struct std::default_delete<ShadowManager>;


		Microsoft::WRL::ComPtr<ID3D12Resource> resource_;

		ShadowFactory* shadowFactory_ = nullptr;
		
		//影データ
		std::vector<ShadowData> shadowDatas_;

		const Vector3 kDirection_ = { 0.0f,-1.0f,0.0f };//ベクトル向き
		
		const float kMaxDistance_ = 9.0f;//最高飛距離
		const float kMinDistance_ = 1.0f;//最低でも必要な長さ

		const float kDecay_ = 0.1f;//減衰度
		const float kAngle_ = 12.0f;//角度
	};
}
