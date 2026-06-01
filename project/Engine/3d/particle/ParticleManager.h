#pragma once
#include <map>
#include <string>
#include <memory>
#include "Particle.h"
#include "SrvManager.h"
#include "ParticleEmitter.h"
#include "Primitive.h"
#include "Camera.h"

/// <summary>
/// エンジン層
/// </summary>
namespace EngineLayer {
	/// <summary>
	/// パーティクルマネージャ
	/// </summary>
	class ParticleManager {
	public:
		/// <summary>
		/// インスタンス生成
		/// </summary>
		/// <returns></returns>
		static ParticleManager& GetInstance();
		/// <summary>
		/// 解放処理
		/// </summary>
		void Finalize();

		/// <summary>
		/// パーティクルグループ作成
		/// </summary>
		/// <param name="name">パーティクルの名前</param>
		/// <param name="textureFilePath">テクスチャファイルパス</param>
		/// <param name="modelData">パーティクルの形(PrimitiveクラスにCreate○○()のモデルデータ項目がある)</param>
		void CreateParticleGroup(const std::string& name, const std::string& textureFilePath, const ModelData& modelData);
		/// <summary>
		/// getter_GPU
		/// </summary>
		/// <param name="filePath">パーティクルの名前</param>
		/// <returns></returns>
		D3D12_GPU_DESCRIPTOR_HANDLE  GetSrvHandleGPU(const std::string& filePath);
		/// <summary>
		/// getter_ModelData
		/// </summary>
		/// <param name="filePath">パーティクルの名前</param>
		/// <returns></returns>
		ModelData GetModelData(const std::string& filePath);
		/// <summary>
		/// getter_texture名
		/// </summary>
		/// <param name="filePath">パーティクルの名前</param>
		/// <returns></returns>
		std::string GetTextureFile(const std::string& filePath);
		/// <summary>
		/// getter_Resource
		/// </summary>
		/// <param name="filePath">パーティクルの名前</param>
		/// <returns></returns>
		Microsoft::WRL::ComPtr<ID3D12Resource> GetResource(const std::string& filePath);

		/// <summary>
		/// getter_パーティクルデータ
		/// </summary>
		/// <param name="filePath">パーティクルの名前</param>
		/// <returns>選択したパーティクル</returns>
		Particle* GetParticle(const std::string& filePath);

		/// <summary>
		/// getter_パーティクルパラメータ
		/// </summary>
		/// <param name="filePath">パーティクルの名前</param>
		/// <returns>選択したパーティクルのパラメータ</returns>
		ParticleParameters GetParticleParameter(const std::string& filePath);

		/// <summary>
		/// getter_パーティクルの数
		/// </summary>
		/// <param name="filePath">パーティクルの名前</param>
		/// <returns>現在選択したパーティクルの総数</returns>
		uint32_t& GetNum(const std::string& filePath);

		/// <summary>
		/// 更新処理
		/// </summary>
		/// <param name="filePath">グループの名前を選択</param>
		/// <param name="wvpData">座標WVP</param>
		void Update(const std::string& filePath, ParticleForGPU* wvpData);

		/// <summary>
		/// エミッタ
		/// </summary>
		/// <param name="filePath">グループの名前</param>
		/// <param name="emitter">使用しているエミッタ</param>
		void Emit(const std::string& filePath, const Emitter& emitter);

		/// <summary>
		/// setter_カメラ
		/// </summary>
		/// <param name="camera_">カメラ</param>
		void SetCamera(Camera* camera_) { camera = camera_; }
		/// <summary>
		/// 数のリセット
		/// </summary>
		/// <param name="filePath">選択するパーティクル</param>
		void ResetNum(const std::string& filePath);
		/// <summary>
		/// パーティクルを一度消す
		/// </summary>
		/// <param name="filePath">パーティクルグループ名を選ぶ</param>
		void ResetParticle(const std::string& filePath);

		/// <summary>
		/// パーティクルパラメータJson読み込み
		/// </summary>
		void InitializeParameter();

		/// <summary>
		/// パーティクル初期化処理テンプレート
		/// </summary>
		/// <param name="parameters">パーティクルのパラメータをまとめたもの</param>
		std::unique_ptr<Particle> InitParticle(const std::string& name);

		/// <summary>
		/// パラメータをいじくる
		/// </summary>
		void ParameterImGui();

	private:
		//インスタンス
		static std::unique_ptr<ParticleManager> sInstance_;
		//default_deleteを設定(解放処理を行える)
		friend struct std::default_delete<ParticleManager>;

		struct ParticleGroup {
			std::string textureFile;
			ModelData modelData;

			std::list<ParticleData> particles;
			uint32_t srvIndex;
			Microsoft::WRL::ComPtr<ID3D12Resource> resource;
			uint32_t kNumInstance = 100;
			DirectX::TexMetadata metadata; //width,height


			D3D12_CPU_DESCRIPTOR_HANDLE srvHandleCPU;
			D3D12_GPU_DESCRIPTOR_HANDLE srvHandleGPU;

			uint32_t numInstance = 0;

			bool Updated = false;
		};

		const float kDeltaTime = 1.0f / 60.0f;
		std::unordered_map<std::string, ParticleGroup> particleGroups;

		//パーティクルコンテナ
		std::unordered_map<std::string, std::unique_ptr<Particle>> particles_;

		std::unordered_map<std::string, ParticleParameters> particleParameters_;

		EngineLayer::Camera* camera = nullptr;
		static const uint32_t kNumMaxInstance = 100;

		//生存時間
		float alpha = 0.0f;

		//パラメータ名選択用
		std::string imGuiName = "";

		/// <summary>
		/// パーティクルの動き
		/// </summary>
		/// <param name="particleData">パラメータデータ</param>
		void VelocityMove(ParticleData& particleData);
		/// <summary>
		/// ワールド行列に変更
		/// </summary>
		/// <param name="particleData">パラメータデータ</param>
		/// <returns>ワールドに変換した行列</returns>
		Matrix4x4 CreateWorldMatrix(ParticleData& particleData);
		/// <summary>
		/// ビルボード行列
		/// </summary>
		/// <param name="scaleMatrix">大きさの行列</param>
		/// <param name="rotateMatrix">回転行列</param>
		/// <param name="translateMatrix">座標行列</param>
		/// <returns>ビルボードに変換した行列</returns>
		Matrix4x4 CreateBillBoardMatrix(const Matrix4x4& scaleMatrix, const Matrix4x4& rotateMatrix, const Matrix4x4& translateMatrix);
		/// <summary>
		/// パーティクルの生存タイマー
		/// </summary>
		/// <param name="particleData">パラメータデータ</param>
		void Timer(ParticleData& particleData);

		/// <summary>
		/// プリミティブオブジェクト作成
		/// </summary>
		/// <param name="primitiveName">形の選択</param>
		/// <returns>プリミティブオブジェクトのモデルデータ</returns>
		ModelData LoadObject(const std::string& primitiveName);

		/// <summary>
		/// 小数点切り捨て
		/// </summary>
		/// <param name="value"></param>
		/// <returns></returns>
		double DecimalPointCut(float value);


		//デバッグで変更する文字列
		char objectName[1024 * 160] = "";
		char textureFileName[1024 * 160] = "";

	};
}