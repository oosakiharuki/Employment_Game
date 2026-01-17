#pragma once
#include "Sprite.h"
#include "Framework.h"
#include "BaseScene.h"

#include "LevelEditor.h"
#include "SpitOutLevelEditor.h"
#include "CameraControl.h"
/// <summary>
/// クリアシーン(BaseSceneの派生クラス)
/// </summary>
class ClearScene : public BaseScene{
public:
	void Initialize() override;
	void Update() override;
	void Draw() override;
	void Finalize() override;
	void SceneUpdate() override;
private:
	/// <summary>
	/// スプライト初期化処理
	/// </summary>
	void InitSprite();
	/// <summary>
	/// カメラ初期化処理
	/// </summary>
	void InitCamera();
	/// <summary>
	/// オブジェクト初期化処理
	/// </summary>
	void InitObject();
	/// <summary>
	/// オブジェクト初期化処理
	/// </summary>
	void InitParticle();
	
	//カメラ
	std::unique_ptr<Camera> camera_ = nullptr;
	//カメラのコントロール
	std::unique_ptr<CameraControl> cameraControl_;

	//レベルエディタ(オブジェクトの配置を.jsonでできる)
	LevelEditor levelEditor_;
	SpitOutLevelEditor spitOut_;
	//オブジェクト描画
	std::vector<std::shared_ptr<VisualActor>> visualActors;
	std::unordered_map<std::string, Transform> transforms_;//各々の変更用
	//プレイヤーを前に向かす
	const float kPlayerFrontRange_ = 180.0f;

	//スプライト描画
	std::unique_ptr<Sprite> sprite_;//ゲームクリアの文字
	std::unique_ptr<Sprite> spriteSpace_;//Spaceでもどるの文字

	//スプライト
	const Vector2 kSpritePositionGameClear_ = { 100, 100 };//[ゲームクリア]文字の座標
	const Vector2 kSpritePositionButton_ = { 800,10 };//[ボタンで戻る]文字の座標
	const Vector2 kSpriteSizeButton_ = { 256,64 };//[ボタンで戻る]文字のサイズ

	//パーティクル
	std::unordered_map<std::string, std::unique_ptr<Particle>> sceneParticles_;

	//紙吹雪のパーティクルたち
	const uint32_t kParticleMaxNum_ = 3;
	
	ParticleParameters particleFanfare_ = {
		"clear_fanfare", "resource/Sprite/white.png", Primitive::CreatePlane(),10,1.5f,{ 1,1,1 }
	};
	std::vector<std::unique_ptr<Particle>> particleFanfares_;

	//紙吹雪パーティクルたちの座標をずらす
	const float kFanfareX_ = 3.0f;
	//紙吹雪パーティクルの座標
	Vector3 fanfareTranslate_ = { -kFanfareX_,2,0 };
};