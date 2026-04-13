#pragma once
#include "Object_glTF.h"
#include "MyMath.h"

#include "Reaction.h"
#include "CollisionManager.h"

class Player;

/// <summary>
/// 傘(発泡、守が使える)
/// </summary>
class Umbrella : public CollisionSource {
public:
	/// <summary>
	/// デストラクタ
	/// </summary>
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
	const Vector3& GetTranslate()  { return transform_.translate; }
	/// <summary>
	/// setter_座標位置
	/// </summary>
	/// <param name="translate">代入する座標位置</param>
	void SetTranslate(const Vector3& translate) { transform_.translate = translate; }

	/// <summary>
	/// getter_回転
	/// </summary>
	/// <returns>現在の回転</returns>
	const Vector3& GetRotate() { return transform_.rotate; }
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
	void ShieldMode();

	/// <summary>
	/// シールド解除
	/// </summary>
	void OffShield() { isShield_ = false; }
	/// <summary>
	/// シールドモードのフラグ確認
	/// </summary>
	/// <returns>シールドモードフラグ</returns>
	bool GetShieldMode() { return isShield_; }

	/// <summary>
	/// 連続ヒットの場合タイマーをリセット
	/// </summary>
	void ResetScaleTimer() { scaleTimer_ = 0.0f; }
	/// <summary>
	/// setter_プレイヤー
	/// </summary>
	/// <param name="player">プレイヤークラス</param>
	void SetPlayer(Player* player) { player_ = player; }

private:
	/// <summary>
	/// 当たり判定コマンド
	/// </summary>
	/// <param name="collision">相手側の当たり判定ソース</param>
	void OnCollision(CollisionSource* collision) override;
	/// <summary>
	/// 当たり判定をとるタイプかをチェック
	/// </summary>
	/// <param name="collisionType">相手の当たり判定タイプ</param>
	/// <returns>該当するタイプがあるなら true</returns>
	bool TypeCheckUp(const CollisionTypes& collisionType) override;

	/// <summary>
	/// パリィの更新処理
	/// </summary>
	void ParryUpdate();

	//オブジェクト設定
	std::unique_ptr<Object_glTF> object_;
	Transform transform_;
	WorldTransform wt_;

	//AABB
	AABB umbrellaAABB_;
	//AABBのサイズ
	const Vector3 kAABBSize_ = { 1,2,1 };

	std::unique_ptr<Reaction> reaction_;

	const float kReactionMaxTime_ = 0.15f;            //リアクションする時間
	const Vector3 kScalePower_ = { 0.1f, 0.1f, 0.1f };//大きくする力

	float scaleTimer_ = 0.0f;

	//傘に当たったフラグ
	bool isHit_ = false;

	//傘のシールドフラグ
	bool isShield_ = false;

	//パリィ
	bool isParry_ = false;
	const float kParryTimeMax_ = 0.5f;//パリィする時間//ちょっと簡単に
	float parryTime_ = kParryTimeMax_;

	//傘のノックバックの値
	const Vector3 kUmbrellaKnockBackPower_ = { 0.0f,0.0f,0.3f };
	const float kUmbrellaKnockBackTime_ = 0.0f;

	Player* player_ = nullptr;
};

