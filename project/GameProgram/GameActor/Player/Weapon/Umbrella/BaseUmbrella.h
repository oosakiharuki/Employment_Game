#pragma once
#include "MyMath.h"
#include "Reaction.h"
#include "CollisionManager.h"
#include "Audio.h"
#include "Object_glTF.h"
#include "PlayerBullet.h"

#include "Particle.h"
#include <memory>

class Player;

/// <summary>
/// 傘銃の基盤クラス
/// </summary>
class BaseUmbrella : public CollisionSource {
public:
	/// <summary>
	/// 初期化処理
	/// </summary>
	virtual void Initialize();
	/// <summary>
	/// 更新処理
	/// </summary>
	virtual void Update();
	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw();
		/// <summary>
	/// setter_プレイヤー
	/// </summary>
	/// <param name="player">プレイヤークラス</param>
	void SetPlayer(Player* player) { player_ = player; }

	/// <summary>
	/// getter_座標位置
	/// </summary>
	/// <returns>現在の座標位置</returns>
	const Vector3& GetTranslate() { return transform_.translate; }
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


	void FireCommand();


	/// <summary>
	/// シールド解除
	/// </summary>
	void OffShield();
	/// <summary>
	/// シールドモードのフラグ確認
	/// </summary>
	/// <returns>シールドモードフラグ</returns>
	bool GetShieldMode() { return isShield_; }

	/// <summary>
	/// 連続ヒットの場合タイマーをリセット
	/// </summary>
	void ResetScaleTimer() { scaleTimer_ = 0.0f; }
protected:

	/// <summary>
	/// 通常弾の生成方
	/// </summary>
	virtual void Fire() = 0;

	/// <summary>
	/// 強化弾の生成方
	/// </summary>
	virtual void PowerFire() = 0;

	/// <summary>
	/// 弾丸の生成
	/// </summary>
	/// <param name="translate">発生場所</param>
	/// <param name="velocity">方向</param>
	/// <param name="bulletPower">弾丸の攻撃力</param>
	void BornBullet(const Vector3& translate, const Vector3& velocity, uint32_t bulletPower);

	//オブジェクト設定
	std::unique_ptr<EngineLayer::Object_glTF> object_;
	Transform transform_;
	EngineLayer::WorldTransform wt_;

	//AABB
	AABB umbrellaAABB_;

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

	
	const float kVolume_ = 0.3f;//ボリューム

	Player* player_ = nullptr;

	//パーティクルのコンテナ
	std::unordered_map<std::string, std::unique_ptr<EngineLayer::Particle>> particles_;

	const std::string& kParticleParry_ = "player_parry";

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

	/// <summary>
	/// パリィ成功 = 連続弾も跳ね返す
	/// </summary>
	void ParrySuccess();

	const std::string kUmbrellaOpenSoundName_ = "resource/Sound/umbrellaOpen.mp3";//傘を開く
	const std::string kFireSoundName_ = "resource/Sound/fire.mp3";//発砲攻撃
	const std::string kParrySuccessSoundName_ = "resource/Sound/parry.mp3";//パリィに成功
	const std::string kParrySoundName_ = "resource/Sound/bane.mp3";//パリィで跳ね返す

	//傘より少し前に出す
	const Vector3 kParryParticleFrontPoint_ { 0.0f,0.0f,2.0f };
	const Vector3 kParryParticleRotate_ = { 90.0f,0.0f,0.0f };


	bool parryStart_ = true;
};

