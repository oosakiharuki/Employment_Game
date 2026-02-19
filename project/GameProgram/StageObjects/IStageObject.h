#pragma once
#include "Object3d.h"
#include "MyMath.h"
#include <LevelEditor.h>
#include "CollisionManager.h"

/// <summary>
/// ステージオブジェクト(ギミック)の基盤クラス
/// </summary>
class IStageObject : public CollisionSource {
public:

	IStageObject();
	virtual ~IStageObject();
	/// <summary>
	/// 初期化処理
	/// </summary>
	virtual void Initialize() = 0;
	/// <summary>
	/// 更新処理
	/// </summary>
	virtual void Update() = 0;
	/// <summary>
	/// 描画処理
	/// </summary>
	virtual void Draw() = 0;

	/// <summary>
	/// getter_座標位置
	/// </summary>
	/// <returns>現在の座標位置</returns>
	const Vector3& GetPosition() const { return transform_.translate; }
	/// <summary>
	/// setter_座標位置
	/// </summary>
	/// <param name="position">代入する座標位置</param>
	void SetPosition(const Vector3& position) { transform_.translate = position; }

	/// <summary>
	/// getter_回転
	/// </summary>
	/// <returns>現在の回転角度</returns>
	const Vector3& GetRotation() const { return transform_.rotate; }

	/// <summary>
	/// setter_回転
	/// </summary>
	/// <param name="rotation">代入する各回転角度</param>
	void SetRotation(const Vector3& rotation) { transform_.rotate = rotation; }

	/// <summary>
	/// getter_大きさ
	/// </summary>
	/// <returns>現在の大きさ</returns>
	const Vector3& GetScale() const { return transform_.scale; }

	/// <summary>
	/// setter_大きさ
	/// </summary>
	/// <param name="scale">代入する大きさ</param>
	void SetScale(const Vector3& scale) { transform_.scale = scale; }

	/// <summary>
	/// setter_当たり判定の大きさ
	/// </summary>
	/// <param name="size">大きさ</param>
	void SetColliderSize(const Vector3& size) { colliderSize_ = size; }

	/// <summary>
	/// オブジェクトのクラスを知るための名前
	/// </summary>
	void SetObjectName(const std::string& name);

	const std::string& GetObjectName() { return objectName_; }

	///ワープゲートで使う

	/// <summary>
	/// setter_次のステージ
	/// </summary>
	/// <param name="fileName"></param>ステージ名(例:stage_01)
	virtual void SetNextStage(const std::string& fileName);

	/// <summary>
	/// getter_次のステージ
	/// </summary>
	/// <returns></returns>次のステージ
	virtual std::string GetNextStage();

protected:

	std::unique_ptr<Object3d> object_ = nullptr;
	WorldTransform wt_;
	Transform transform_{};

	Vector3 colliderSize_;

	std::string objectName_;
};