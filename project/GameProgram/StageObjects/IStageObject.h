#pragma once
#include "Object3d.h"
#include "MyMath.h"
#include <LevelEditor.h>

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
	/// <returns>現在の座標位置</returns>
	const Vector3& GetPosition() const { return wt_.translation_; }
	/// <summary>
	/// setter_座標位置
	/// </summary>
	/// <param name="position">代入する座標位置</param>
	void SetPosition(const Vector3& position) { wt_.translation_ = position; }

	/// <summary>
	/// getter_回転
	/// </summary>
	/// <returns>現在の回転角度</returns>
	const Vector3& GetRotation() const { return wt_.rotation_; }

	/// <summary>
	/// setter_回転
	/// </summary>
	/// <param name="rotation">代入する各回転角度</param>
	void SetRotation(const Vector3& rotation) { wt_.rotation_ = rotation; }

	/// <summary>
	/// getter_大きさ
	/// </summary>
	/// <returns>現在の大きさ</returns>
	const Vector3& GetScale() const { return wt_.scale_; }

	/// <summary>
	/// setter_大きさ
	/// </summary>
	/// <param name="scale">代入する大きさ</param>
	void SetScale(const Vector3& scale) { wt_.scale_ = scale; }

	/// <summary>
	/// getter_当たり判定AABB
	/// </summary>
	/// <returns>座標位置が真ん中のAABB</returns>
	AABB GetAABB();

	/// <summary>
	/// setter_当たり判定AABB
	/// </summary>
	/// <param name="aabb">代入するAABB</param>
	void SetAABB(const AABB& aabb) { aabb_ = aabb; }

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
	AABB aabb_;

	std::string objectName_;
};