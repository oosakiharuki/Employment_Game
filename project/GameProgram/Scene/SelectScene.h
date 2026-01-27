#pragma once
#include "BaseScene.h"
#include "SpitOutLevelEditor.h"

#include "UIManager.h"

#include "BackGroundObject.h"
#include "PauseScreen.h"

/// <summary>
/// セレクト(ステージ選択)シーン(BaseSceneの派生クラス)
/// </summary>
class SelectScene : public BaseScene{
public:
	void Initialize() override;
	void Update() override;
	void Draw() override;
	void Finalize() override;
	void SceneUpdate() override;
private:

	//カメラ
	std::unique_ptr<Camera> camera_ = nullptr;
	Vector3 cameraRotate_ = { 0.0f,0.0f,0.0f };//回転
	Vector3 cameraTranslate_ = { 0.0f,0.0f,0.0f };///座標
	//カメラ移動範囲
	Vector3 cameraPointLeft_;//左端
	Vector3 cameraPointRight_;//右端
	//カメラのコントロール
	std::unique_ptr<CameraControl> cameraControl_;

	//レベルエディタ(オブジェクトの配置を.jsonでできる)
	LevelEditor levelEditor_;
	SpitOutLevelEditor spitOut_;

	//プレイヤー
	std::unique_ptr<Player> player_ = nullptr;
	//ステージオブジェクトたち
	std::list<std::shared_ptr<IStageObject>> stageObjects_;

	//ステージ全体のオブジェクト
	std::unique_ptr<Object3d> stageObj_;
	//ステージ全体の当たり判定AABB
	std::vector<AABB> stagesAABB_;


	//前ステージデータ
	SceneSaveData sceneSaveData_ = {
		3,3,2,"stage_0" //初期設定
	};

	//ステージの.jsonファイル名
	std::string stageFileName_;

	/// <summary>
	/// レベルエディタで配置処理
	/// </summary>
	/// <param name="levelEditor_file">指定したい場合は名前を入れることも可能</param>
	void LevelEditorObjectSetting(const std::string& levelEditor_file = "");
	/// <summary>
	/// ゲームオブジェクトの配置処理をまとめた
	/// </summary>
	void SpitOutGameObject();
	
	/// <summary>
	/// 全シーンに共有できる当たり判定
	/// </summary>
	void CollisionCommon();

	//説明ガイドの初期座標と大きさ
	const Vector2 kSpriteSize_ = { 128,64 };

	const Vector2 kSpriteTranslateMove_ = { 300,20 };
	const Vector2 kSpriteTranslateKeyE_ = { 600,20 };

	//変更する場所(ジャンプ説明前は移動の説明)
	Guide kGuideMove_ = { "guide_move","guide_move", kSpriteTranslateMove_, kSpriteSize_, -100.0f, 100.0f };  //移動の説明
	Guide kGuideWarp_ = { "guide_warp","guide_warp",kSpriteTranslateKeyE_, kSpriteSize_, -100.0f, 100.0f };  //ワープに入る説明

	//ポーズ画面
	std::unique_ptr<PauseScreen> pauseScreen_ = nullptr;
	bool isPause_ = false;

	bool isNextGameScene = false;

	//背景
	std::unique_ptr<BackGroundObject> backGround = nullptr;
};