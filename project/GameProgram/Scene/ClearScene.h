#pragma once
#include "Sprite.h"
#include "Framework.h"
#include "IScene.h"

/// <summary>
/// クリアシーン(ISceneの派生クラス)
/// </summary>
class ClearScene : public IScene{
public:
	void Initialize() override;
	void Update() override;
	void Draw() override;
	void Finalize() override;
private:
	//スプライト描画
	std::unique_ptr<Sprite> sprite_;//ゲームクリアの文字
	std::unique_ptr<Sprite> spriteSpace_;//Spaceでもどるの文字

	//プレイヤー
	std::unique_ptr<Object_glTF> playerGltf_;	
	WorldTransform wt_;

	//演出用のステージ
	std::unique_ptr<Object_glTF> stageGltf_;

	//紙吹雪のパーティクルたち
	const uint32_t kParticleMaxNum_ = 3;
	std::vector<std::unique_ptr<Particle>> particle_fanfares_;
	const uint32_t kParticleFanfareCount_ = 10;
	const float kParticleFanfareFrequency_ = 1.5f;

	//クリアシーンのカメラ座標
	const Vector3 kCameraTranslate_ = { 0.0f,2.0f,-18.0f };

	//スプライト
	const Vector2 kSpritePositionGameClear_ = { 100, 100 };//[ゲームクリア]文字の座標
	const Vector2 kSpritePositionBotton_ = { 800,10 };//[ボタンで戻る]文字の座標
	const Vector2 kSpriteSizeBotton_ = { 256,64 };//[ボタンで戻る]文字のサイズ

	//プレイヤーを前に向かす
	const float kPlayerFrontRange_ = 180.0f;

	//紙吹雪パーティクルたちの座標をずらす
	const float kFanfareX_ = 3.0f;
	//紙吹雪パーティクルの[左側]の座標
	const Vector3 kFanfareInitTranslate_ = { -kFanfareX_,2,0 };
	//紙吹雪パーティクルの座標
	Vector3 fanfareTranslate_ = kFanfareInitTranslate_;

};