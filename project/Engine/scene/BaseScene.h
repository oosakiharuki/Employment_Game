#pragma once
#include <cstdint>

#include "MyMath.h"
#include "Framework.h"

#include "LevelEditor.h"
#include "Player.h"
#include "Enemy_Soldier.h"
#include "Enemy_Turret.h"
#include "Enemy_Bomb.h"
#include "CheckPoint.h"
#include "WarpGate.h"
#include "Goal.h"

#include "BoxModel.h"
#include "FadeScreen.h"

#include "EventTrigger.h"
#include "CameraControl.h"

#include "CollisionManager.h"
#include "NextStageSave.h"
/// <summary>
/// シーン共有処理
/// </summary>
class BaseScene {
protected:
	//end
	bool isGameEnd_ = false;

public:
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
	/// 解放処理
	/// </summary>
	virtual void Finalize() = 0;

	virtual ~BaseScene();

	/// <summary>
	/// シーン更新処理(状態遷移)
	/// </summary>
	virtual void SceneUpdate() = 0;

	/// <summary>
	/// ゲーム終了処理
	/// </summary>
	/// <returns></returns>trueで終了
	bool GetIsGameEnd() { return isGameEnd_; }

};