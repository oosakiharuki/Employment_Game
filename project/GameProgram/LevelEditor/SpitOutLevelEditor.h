#pragma once
#include "BaseScene.h"
#include "BaseEnemy.h"
#include "Boss.h"
#include "IStageObject.h"
#include "EventTrigger.h"
#include "Guide.h"
#include "VisualActor.h"
#include "LevelEditor.h"

/// <summary>
/// レベルエディタの配置
/// </summary>
class SpitOutLevelEditor
{
public:
	/// <summary>
	/// 使用するレベルエディタをもらう
	/// </summary>
	/// <param name="levelEditor">使用するレベルエディタ</param>
	void SetLevelEditor(LevelEditor* levelEditor) { levelEditor_ = std::move(levelEditor); }
	
	/// <summary>
	/// カメラの配置、使用するカメラ設定
	/// </summary>
	/// <param name="cameraControl">カメラコントロール</param>
	void SpitOutCamera(std::unique_ptr<CameraControl>& cameraControl);

	/// <summary>
	/// プレイヤーから使用するカメラに
	/// </summary>
	void CameraStartPointPlayer(std::unique_ptr<CameraControl>& cameraControl);

	/// <summary>
	/// プレイヤーの配置
	/// </summary>
	/// <param name="player">プレイヤー</param>
	void SpitOutPlayer(std::unique_ptr<Player>& player);

	/// <summary>
	/// 敵の配置
	/// </summary>
	/// <param name="enemies">敵の配列</param>
	std::vector<std::unique_ptr<BaseEnemy>> SpitOutEnemies();

	/// <summary>
	/// ステージの当たり判定設定
	/// </summary>
	/// <param name="stageObj">オブジェクト化</param>
	/// <param name="stageFileName">ステージの名前(オブジェクト読み込みに使う)</param>
	/// <param name="stagesAABB">当たり判定</param>
	void SpitOutStage(std::unique_ptr<EngineLayer::Object3d>& stageObj, const std::string& stageFileName);

	/// <summary>
	/// ステージオブジェクトの配置
	/// </summary>
	/// <param name="stageObjects">ステージオブジェクトの配列</param>
	std::list<std::unique_ptr<IStageObject>> SpitOutStageObject();

	
	/// <summary>
	/// ステージオブジェクトの配置
	/// </summary>
	/// <param name="stageObjects">ステージオブジェクトの配列</param>
	void SettingStageObject(IStageObject& stageObject,LevelEditor::LevelData::StageObjectData data);


	/// <summary>
	/// イベントトリガーの配置
	/// </summary>
	/// <param name="eventTrigger">イベントトリガーの配列</param>
	std::list<std::unique_ptr<EventTrigger>> SpitOutEventTrigger();

	/// <summary>
	/// ボスの配置
	/// </summary>
	/// <param name="boss">ボス</param>
	void SpitOutBoss(std::unique_ptr<Boss>& boss);

	/// <summary>
	/// 操作ガイドの配置
	/// </summary>
	/// <returns>操作ガイド(配列)</returns>
	std::vector<std::unique_ptr<Guide>> SpitOutGuide();
	/// <summary>
	/// ビジュアルアクターの配置
	/// </summary>
	/// <returns>ビジュアルアクター(配列)</returns>
	std::vector<std::unique_ptr<VisualActor>> SpitOutVisualActor();

private:
	/// <summary>
	/// 敵の共通テンプレート処理
	/// </summary>
	/// <param name="enemy">敵の基盤/派生クラスを入れる</param>
	/// <param name="enemyData">LevelEditorからもらった敵データ</param>
	void EnemyTemplate(BaseEnemy& enemy, LevelEditor::LevelData::EnemySpawnData enemyData);
	/// <summary>
	/// 移動能力がある敵キャラ
	/// </summary>
	/// <param name="enemy">MoveCommandを持っている敵</param>
	/// <param name="enemyData">LevelEditorからもらった敵データ</param>
	void EnemyMoveRoute(EnemyMoveCommand& enemy,LevelEditor::LevelData::EnemySpawnData enemyData);

	//使用するエディタ
	LevelEditor* levelEditor_;
};

