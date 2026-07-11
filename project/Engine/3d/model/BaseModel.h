#pragma once
#include "MyMath.h"
#include <memory>

/// <summary>
/// エンジン層
/// </summary>
namespace EngineLayer {
	/// <summary>
	/// Model(基盤クラス)
	/// </summary>
	class BaseModel
	{
	public:
		/// <summary>
		/// 初期化処理
		/// </summary>
		/// <param name="directoryPath">リソースファイル名(resource)</param>
		/// <param name="fileName">オブジェクト名</param>
		virtual void Initialize(const std::string& directoryPath, const std::string& fileName) = 0;

		/// <summary>
		/// 描画処理
		/// </summary>
		virtual void Draw() = 0;

		/// <summary>
		/// マテリアルの色を変更
		/// </summary>
		/// <param name="color">変更カラー</param>
		void SetColor(const Vector4& color);

		/// <summary>
		/// ライト設定
		/// </summary>
		/// <param name="Light">trueでオン / falseでオフ</param>
		void LightOn(bool Light);

	protected:

		/// <summary>
		/// VertexResource作成(初期化)
		/// </summary>
		/// <param name="modelData">モデルデータ</param>
		virtual void InitVertexResource(ModelData modelData) = 0;

		/// <summary>
		/// MaterialResource作成(初期化)
		/// </summary>
		/// <param name="modelData">モデルデータ</param>
		virtual void InitMaterialResource(ModelData modelData) = 0;

		/// <summary>
		/// IndexResource作成(初期化)
		/// </summary>
		/// <param name="modelData">モデルデータ</param>
		virtual void InitIndexResource(ModelData modelData) = 0;

		/// <summary>
		/// カメラ更新処理
		/// </summary>
		void CameraUpdate();

		std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> vertexResource_;
		std::vector<D3D12_VERTEX_BUFFER_VIEW> vertexBufferView_;
		VertexData* vertexData_ = nullptr;

		std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> materialResources_;
		Material* materialData_ = nullptr;

		MyMath::ModelDataMulti modelData_;

		//マテリアルデータ初期値
		const Vector4 kInitColor_ = { 1.0f,1.0f,1.0f,1.0f };
		const float kShininess_ = 70.0f;
	};
}