#pragma once
#include "Object3d.h"
#include "MyMath.h"

/// <summary>
/// ステージオブジェクト(ギミック)の基盤クラス
/// </summary>
class IStageObject {
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
	/// <returns></returns>現在の座標位置
	const Vector3& GetPosition() { return wt.translation_; }
	/// <summary>
	/// setter_座標位置
	/// </summary>
	/// <param name="position"></param>代入する座標位置
	void SetPosition(const Vector3& position) { wt.translation_ = position; }

	/// <summary>
	/// getter_回転
	/// </summary>
	/// <returns></returns>現在の回転角度
	const Vector3& GetRotation() { return wt.rotation_; }

	/// <summary>
	/// setter_回転
	/// </summary>
	/// <param name="rotation"></param>代入する各回転角度
	void SetRotation(const Vector3& rotation) { wt.rotation_ = rotation; }

	/// <summary>
	/// getter_当たり判定AABB
	/// </summary>
	/// <returns></returns>座標位置が真ん中のAABB
	AABB GetAABB();

	/// <summary>
	/// setter_当たり判定AABB
	/// </summary>
	/// <param name="aabb"></param>代入するAABB
	void SetAABB(AABB aabb) { aabb_ = aabb; }

	/// <summary>
	/// オブジェクトのクラスを知るための名前
	/// </summary>
	void SetObjectName(const std::string& name);

	const std::string& GetObjectName() { return objectName; }

	///ワープゲートで使う

	/// <summary>
	/// setter_次のステージ
	/// </summary>
	/// <param name="fileName"></param>ステージ名(例:stage_01)
	virtual void SetNextStage(std::string fileName);

	/// <summary>
	/// getter_次のステージ
	/// </summary>
	/// <returns></returns>次のステージ
	virtual std::string GetNextStage();

protected:

	std::unique_ptr<Object3d> object_ = nullptr;
	WorldTransform wt;
	AABB aabb_;

	std::string objectName;
};