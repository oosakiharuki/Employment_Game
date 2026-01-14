#pragma once
#include "BaseScene.h"
#include "Boss.h"
#include "VisualActor.h"
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
	/// プレイヤーの配置
	/// </summary>
	/// <param name="player">プレイヤー</param>
	void SpitOutPlayer(std::unique_ptr<Player>& player);

	/// <summary>
	/// 敵の配置
	/// </summary>
	/// <param name="enemies">敵の配列</param>
	void SpitOutEnemies(std::vector<std::shared_ptr<BaseEnemy>>& enemies);

	/// <summary>
	/// ステージの当たり判定設定
	/// </summary>
	/// <param name="stageObj">オブジェクト化</param>
	/// <param name="stageFileName">ステージの名前(オブジェクト読み込みに使う)</param>
	/// <param name="stagesAABB">当たり判定</param>
	void SpitOutStage(std::unique_ptr<Object3d>& stageObj, const std::string& stageFileName, std::vector<AABB>& stagesAABB);

	/// <summary>
	/// ステージオブジェクトの配置
	/// </summary>
	/// <param name="stageObjects">ステージオブジェクトの配列</param>
	void SpitOutStageObject(std::list<std::shared_ptr<IStageObject>>& stageObjects);

	
	/// <summary>
	/// ステージオブジェクトの配置
	/// </summary>
	/// <param name="stageObjects">ステージオブジェクトの配列</param>
	void SettingStageObject(IStageObject& stageObject,LevelEditor::LevelData::StageObjectData data);


	/// <summary>
	/// イベントトリガーの配置
	/// </summary>
	/// <param name="eventTrigger">イベントトリガーの配列</param>
	void SpitOutEventTrigger(std::vector<std::shared_ptr<EventTrigger>>& eventTrigger);

	/// <summary>
	/// ボスの配置
	/// </summary>
	/// <param name="boss">ボス</param>
	void SpitOutBoss(std::unique_ptr<Boss>& boss);

	void SpitOutVisualActor(std::vector<std::shared_ptr<VisualActor>>& visualActors);

private:
	//使用するエディタ
	LevelEditor* levelEditor_;
};

