#include "StageObjectFunction.h"
#include "Player.h"
#include "BaseScene.h"

using namespace MyMath;

namespace StageObjectFunction {

	void WarpNextScene(Player& player,CameraControl* cameraControl_, bool& isNextScene) {

		//プレイヤーが演出判定でない
		//「!player_->GetPerformanceMode()」は何度もplayer_のGetTranslateを読み取ることで予定の速度より速くならないようにするため
		if (!player.GetPerformanceMode()) {
			//プレイヤーにカメラズーム
			cameraControl_->ZoomStart(player.GetTranslate() + kPlayerAwayPos_);
			player.IsPerformanceFlag(true);//演出モード
			player.BackDirection();//向きを前に(Z方向)
		}
		//カメラがズームし終わった
		if (cameraControl_->ZoomEnd()) {
			//次のステージに進む時Hpなどパラメータがリセットされないようにする
			NextStageSave::GetInstance().SetPlayerHp(player.GetHp()); //現在のプレイヤー体力を保存
			NextStageSave::GetInstance().SetPlayerRemain(player.GetRemain()); //現在のプレイヤー残機を保存
			//フェードインした後、次のシーンに
			isNextScene = true;
		}
	}

}