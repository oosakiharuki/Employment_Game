#pragma once
#include "BaseScene.h"
#include "SpitOutLevelEditor.h"

/// <summary>
/// セレクト(ステージ選択)シーン(BaseSceneの派生クラス)
/// </summary>
class SelectScene : public BaseScene{
public:
	void Initialize() override;
	void Update() override;
	void Draw() override;
	void Finalize() override;
private:

	//入力処理
	Input* input_ = Input::GetInstance().get();
	//ゲームパット用の入力変数
	XINPUT_STATE state_, preState_;

	//カメラ
	std::unique_ptr<Camera> camera_ = nullptr;
	Vector3 cameraRotate_ = { 0.0f,0.0f,0.0f };//回転
	Vector3 cameraTranslate_ = { 0.0f,0.0f,0.0f };///座標
	//カメラ移動範囲
	Vector3 cameraPointLeft_;//左端
	Vector3 cameraPointRight_;//右端
	//カメラのコントロール
	std::unique_ptr<CameraControl> cameraControl_;

	//ズームするときのプレイヤーと少し離れた位置
	const Vector3 kPlayerAwayPos_ = { 0, 2, -15.0f };

	//レベルエディタ(オブジェクトの配置を.jsonでできる)
	Levelediter levelediter_;
	SpitOutLevelEditor spitOut_;

	//プレイヤー
	std::unique_ptr<Player> player_ = nullptr;
	//ステージオブジェクトたち
	std::list<std::shared_ptr<IStageObject>> stageObjects_;

	//ステージ全体のオブジェクト
	std::unique_ptr<Object3d> stageobj_;
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
	/// <param name="leveleditor_file">指定したい場合は名前を入れることも可能</param>
	void LevelEditorObjectSetting(const std::string& leveleditor_file = "");

	/// <summary>
	/// 全シーンに共有できる当たり判定
	/// </summary>
	void CollisionCommon();

	/// <summary>
	/// ワープして次のシーンに進む処理
	/// </summary>
	void WarpNextScene(const std::string& nextScene);

	/// <summary>
	/// カメラがプレイヤーにズームする
	/// </summary>
	void CameraZoomPlayer();



	/// <summary>
	/// 共通の更新
	/// </summary>
	void UpdateGuide();

	/// <summary>
	/// 共通の描画
	/// </summary>
	void DrawGuide();


	/// <summary>
	/// 構造体_操作説明(ガイド)の設定
	/// </summary>
	struct Guide {
		std::string name;       //ガイドの名前(テクスチャ名)
		float lookPointX_left;  //見れる範囲_左端
		float lookPointX_right; //見れる範囲_右端
	};

	/// <summary>
	/// 操作説明スプライトを作る
	/// </summary>
	/// <param name="guide">名前や座標が入った構造体</param>
	void CreateGuide(const Guide& guide);

	std::vector<Guide> guides_;

	//説明ガイドの初期座標と大きさ
	const Vector2 kSpriteSize_ = { 128,64 };
	const Vector2 kSpriteTranslate_ = { 300,20 };


	std::unordered_map<std::string, std::unique_ptr<Sprite>> spriteGuides_;

	//変更する場所(ジャンプ説明前は移動の説明)
	Guide kGuideMove_ = { "guide_move", 0.0f,0.0f };   //移動の説明
	Guide kGuideWarp_ = { "guide_warp", 0.0f, 0.0f };   //滑空の説明


	const Vector2 kSpriteTranslateMove_ = { 300,20 };
	const Vector2 kSpriteTranslateEkey_ = { 600,20 };
};