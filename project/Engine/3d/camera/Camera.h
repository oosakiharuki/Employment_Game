#pragma once
#include "MyMath.h"
#include "WorldTransform.h"

/// <summary>
/// エンジン層
/// </summary>
namespace EngineLayer {
	/// <summary>
	/// カメラ
	/// </summary>
	class Camera {
	public:
		/// <summary>
		/// コンストラクタ
		/// </summary>
		Camera();
		/// <summary>
		/// 更新処理
		/// </summary>
		void Update();

		/// <summary>
		/// setter_回転
		/// </summary>
		/// <param name="rotate">指定する回転</param>
		void SetRotate(const Vector3& rotate) { transform_.rotate = rotate; }
		/// <summary>
		/// setter_座標位置
		/// </summary>
		/// <param name="translate">指定する座標位置</param>
		void SetTranslate(const Vector3& translate) { transform_.translate = translate; }
		/// <summary>
		/// 親子関係追加
		/// </summary>
		/// <param name="parent">親となるオブジェクト(カメラは子)</param>
		void SetParent(const WorldTransform* parent) { worldTransform_.SetParent(*parent); }

		/// <summary>
		/// setter_視野角
		/// </summary>
		/// <param name="ForY">視野角の設定</param>
		void SetForY(float ForY) { forY_ = ForY; }
		/// <summary>
		/// setter_アスペクト比
		/// </summary>
		/// <param name="aspectRatio">アスペクト比の設定</param>
		void SetAspectRatio(float aspectRatio) { aspect_ = aspectRatio; }
		/// <summary>
		/// setter_描画範囲_near
		/// </summary>
		/// <param name="Clip">描画範囲設定</param>
		void SetNearClip(float Clip) { nearClip_ = Clip; }
		/// <summary>
		/// setter_描画範囲_far
		/// </summary>
		/// <param name="Clip">描画範囲設定</param>
		void SetFarClip(float Clip) { farClip_ = Clip; }
		/// <summary>
		/// getter_ワールド行列
		/// </summary>
		/// <returns>カメラのワールド行列</returns>
		const Matrix4x4& GetWorldMatrix() const { return worldMatrix_; }
		/// <summary>
		/// getter_ビュー行列
		/// </summary>
		/// <returns>カメラのビュー行列</returns>
		const Matrix4x4& GetViewMatrix() const { return viewMatrix_; }
		/// <summary>
		/// getter_プロジェクション行列
		/// </summary>
		/// <returns>カメラのプロジェクション座標</returns>
		const Matrix4x4& GetProjectionMatrix() const { return projectionMatrix_; }
		/// <summary>
		/// getter_ビュープロジェクション行列
		/// </summary>
		/// <returns></returns>
		const Matrix4x4& GetViewProjectionMatrix() const { return viewProjectionMatrix_; }
		/// <summary>
		/// getter_回転
		/// </summary>
		/// <returns>回転角度</returns>
		const Vector3& GetRotate() const { return transform_.rotate; }
		/// <summary>
		/// getter_座標位置
		/// </summary>
		/// <returns>座標位置</returns>
		const Vector3& GetTranslate() const { return transform_.translate; }

	private:
		WorldTransform worldTransform_;
		Transform transform_{};


		Matrix4x4 worldMatrix_;
		Matrix4x4 viewMatrix_;
		Matrix4x4 projectionMatrix_;
		float forY_;
		float aspect_;
		float nearClip_;
		float farClip_;
		Matrix4x4 viewProjectionMatrix_;
	};
}