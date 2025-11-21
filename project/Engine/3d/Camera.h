#pragma once
#include "MyMath.h"
#include "WorldTransform.h"
/// <summary>
/// カメラ
/// </summary>
class Camera {
public:
	Camera();
	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();

	/// <summary>
	/// setter_回転
	/// </summary>
	/// <param name="rotate"></param>
	void SetRotate(const Vector3& rotate) { worldTransform_.rotation_ = rotate; }
	/// <summary>
	/// setter_座標位置
	/// </summary>
	/// <param name="translate"></param>
	void SetTranslate(const Vector3& translate) { worldTransform_.translation_ = translate; }
	/// <summary>
	/// setter_親子関係
	/// </summary>
	/// <param name="parent"></param>親となるオブジェクト(カメラは子)
	void SetParent(const WorldTransform* parent) { worldTransform_.parent_ = parent; }

	/// <summary>
	/// setter_
	/// </summary>
	/// <param name="ForY"></param>
	void SetForY(const float& ForY) { forY_ = ForY; }
	/// <summary>
	/// setter_アスペクト比
	/// </summary>
	/// <param name="aspectRatio"></param>
	void SetAspectRatio(const float aspectRatio) { aspect_ = aspectRatio; }
	/// <summary>
	/// setter_描画範囲_near
	/// </summary>
	/// <param name="Clip"></param>
	void SetNearClip(const float Clip) { nearClip_ = Clip; }
	/// <summary>
	/// setter_描画範囲_far
	/// </summary>
	/// <param name="Clip"></param>
	void SetFarClip(const float Clip) { farClip_ = Clip; }
	/// <summary>
	/// getter_ワールド行列
	/// </summary>
	/// <returns></returns>
	const Matrix4x4& GetWorldMatrix() const { return worldMatrix_; }
	/// <summary>
	/// getter_ビュー行列
	/// </summary>
	/// <returns></returns>
	const Matrix4x4& GetViewMatrix() const { return viewMatrix_; }
	/// <summary>
	/// getter_プロジェクション行列
	/// </summary>
	/// <returns></returns>
	const Matrix4x4& GetProjectionMatrix() const { return projectionMatrix_; }
	/// <summary>
	/// getter_ビュープロジェクション行列
	/// </summary>
	/// <returns></returns>
	const Matrix4x4& GetViewProjectionMatrix() const { return viewProjectionMatrix_; }
	/// <summary>
	/// getter_回転
	/// </summary>
	/// <returns></returns>
	const Vector3& GetRotate() const { return worldTransform_.rotation_; }
	/// <summary>
	/// getter_座標位置
	/// </summary>
	/// <returns></returns>
	const Vector3& GetTranslate() const { return worldTransform_.translation_; }

private:
	WorldTransform worldTransform_;
	Matrix4x4 worldMatrix_;
	Matrix4x4 viewMatrix_;
	Matrix4x4 projectionMatrix_;
	float forY_;
	float aspect_;
	float nearClip_;
	float farClip_;
	Matrix4x4 viewProjectionMatrix_;
};