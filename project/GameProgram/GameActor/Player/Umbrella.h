#pragma once
#include "Object_glTF.h"
#include "MyMath.h"

#include "Reaction.h"
#include "CollisionManager.h"

/// <summary>
/// 傘(発泡、守が使える)
/// </summary>
class Umbrella : public CollisionSource {
public:
	~Umbrella();
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
	/// getter_座標位置
	/// </summary>
	/// <returns>現在の座標位置</returns>
	Vector3 GetTranslate() const { return transform_.translate; }
	/// <summary>
	/// setter_座標位置
	/// </summary>
	/// <param name="translate">代入する座標位置</param>
	void SetTranslate(const Vector3& translate) { transform_.translate = translate; }

	/// <summary>
	/// getter_回転
	/// </summary>
	/// <returns>現在の回転</returns>
	Vector3 GetRotate() const { return transform_.rotate; }
	/// <summary>
	/// setter_回転
	/// </summary>
	/// <param name="rotate">代入する各回転角度</param>
	void SetRotate(const Vector3& rotate) { transform_.rotate = rotate; }

	/// <summary>
	/// getter_スケール
	/// </summary>
	/// <returns>現在の大きさ</returns>
	Vector3 GetScale() const { return transform_.scale; }
	/// <summary>
	/// setter_スケール
	/// </summary>
	/// <param name="translate">代入する大きさ</param>
	void SetScale(const Vector3& scale) { transform_.scale = scale; }
	
	/// <summary>
	/// setter_当たり判定AABB
	/// </summary>
	/// <param name="aabb">AABB</param>
	void SetAABB(const AABB& aabb) { umbrellaAABB_ = aabb; }

	/// <summary>
	/// シールドモード
	/// </summary>
	/// <param name="isShield">trueはシールドモードに変更</param>
	void ShieldMode(bool isShield) { isShieldMode_ = isShield; }

	/// <summary>
	/// 傘で防いだ時
	/// </summary>
	/// <param name="isShieldMode">リアクションフラグ</param>
	void HitReaction(bool& isShieldMode);

	/// <summary>
	/// 連続ヒットの場合タイマーをリセット
	/// </summary>
	void ResetScaleTimer() { scaleTimer_ = 0.0f; }

private:
	void OnCollision(CollisionSource* collision) override;

	//オブジェクト設定
	std::unique_ptr<Object_glTF> object_;
	Transform transform_;
	WorldTransform wt_;

	//AABB
	AABB umbrellaAABB_;
	//AABBのサイズ
	const Vector3 kAABBSize_ = { 1,2,1 };
	//シールドモード
	bool isShieldMode_ = false;


	std::unique_ptr<Reaction> reaction_;

	const float kReactionMaxTime_ = 0.15f;            //リアクションする時間
	const Vector3 kScalePower_ = { 0.1f, 0.1f, 0.1f };//大きくする力

	float scaleTimer_ = 0.0f;
};

