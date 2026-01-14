#include "BaseBossState.h"
#include "Boss.h"
#include "UseEveryOne.h"

using namespace MyMath;
using namespace UseEveryOne;

void BaseBossState::Update(Boss& boss) {}

void BossMoveState::Update(Boss& boss) {

	//偶数か奇数か
	if (std::fmod(boss.ActionCount() + addCount_, 2) == 0) {
		//右位置設定
		boss.SetMovePoint(boss.GetCenter() + kEdge_);
	}
	else {
		//左位置設定
		boss.SetMovePoint(boss.GetCenter() - kEdge_);
	}

	//目的地に着いたかどうか調べる
	boss.ArrivedSegmentDiff();

	//目的地に着いた
	if (boss.IsMoveSuccess()) {
		if (addCount_ == 2) {
			//行動前モーションステートに変更		
			boss.ChangeStatePattern(std::make_unique<BossBeforeActionMotionState>());
		}
		addCount_++;
		boss.ResetMoveSuccess();
	}
}

void BossAttackState::Update(Boss& boss) {
	boss.ResetStopFire();
	//発泡処理
	boss.Fire();
	//発砲を終了したら
	if (boss.IsStopFire()) {
		boss.ResetMoveSuccess();
		//移動ステートに変更
		boss.ChangeStatePattern(std::make_unique<BossMoveState>());
	}
}

void BossAroundMoveState::Update(Boss& boss) {

	if (movePoints_.empty()) {
		//移動を設定
		movePoints_.push_back({ Vector3(30, 4, 0),60.0f });
		movePoints_.push_back({ Vector3(-30, 4, 0), 180.0f });
		movePoints_.push_back({ Vector3(-20, 13, 0), 60.0f });
		movePoints_.push_back({ Vector3(20, 13, 0), 180.0f });
	}

	//目的地に着いたら
	if(boss.IsMoveSuccess()) {
		boss.ResetMoveSuccess();//移動完了フラグリセット(false)
		aroundMoveCount_++;    //カウント加算
	}

	//全てのポイントに移動できたら
	if (aroundMoveCount_ >= movePoints_.size()) {
		//移動ステートに変更
		boss.ChangeStatePattern(std::make_unique<BossMoveState>());
	}
	else {
		//
		boss.SetMovePoint(movePoints_[aroundMoveCount_].position, movePoints_[aroundMoveCount_].division);
		//目的地に着いたかどうか調べる
		boss.ArrivedSegmentDiff();
	}
}


void BossBeforeActionMotionState::Update(Boss& boss) {

	if (!boss.IsMotionFinish()) {	
		//回転して行動をわかりやすく
		boss.BeforeActionMotion();		
		return;
	}
	else if (moveCoolTimer_ < kMoveCoolTimeMax_) {
		moveCoolTimer_ += kDeltaTime_;
		return;
	}

	//移動カウント
	if (boss.ActionCount() >= kDeepAttackCountMax_) {
		boss.ResetMoveSuccess();
		boss.ResetActionCount();
		boss.ChangeStatePattern(std::make_unique<BossFarAttackState>());
	}
	else if (boss.ActionCount() >= kMoveCountMax_) {
		boss.ResetMoveSuccess();
		boss.AddActionCount();//行動カウント加算
		boss.ChangeStatePattern(std::make_unique<BossAroundMoveState>());
	}
	else {
		boss.AddActionCount();//行動カウント加算
		boss.ChangeStatePattern(std::make_unique<BossAttackState>());
	}

	boss.ResetMotionFinish();
}

void BossFarAttackState::Update(Boss& boss) {
	//最初のみ行う処理
	if (isStart) {
		//移動する前の位置を取得
		position_ = boss.GetTranslate();
		position_.x = -position_.x;
		//画面奥に移動
		boss.SetMovePoint(kFarPlace_);
		isStart = false;
	}

	//目的地に着いたかどうか調べる
	boss.ArrivedSegmentDiff();

	if(isAttackFinish_){
		boss.ResetMoveSuccess();
		//移動ステートに変更
		boss.ChangeStatePattern(std::make_unique<BossFarTackleState>());
	}
	else if (boss.IsMoveSuccess()) {
		boss.ResetStopFire();
		//発泡処理
		boss.Fire(4.0f);
		//発砲を終了したら
		if (boss.IsStopFire()) {
			isAttackFinish_ = true;
		}
	}
}

void BossFarTackleState::Update(Boss& boss) {
	if (isStart_) {
		playerPosition_ = boss.GetPlayer()->GetTranslate();
		isStart_ = false;
	}

	boss.SetMovePoint(playerPosition_,kMoveFrame_);

	if (boss.GetTranslate().z <= kNearEnd) {
		boss.ResetMoveSuccess();
		boss.ChangeStatePattern(std::make_unique<BossFallPlayerState>());
	}
}

void BossFallPlayerState::Update(Boss& boss) {
	//移動ポイントの設定
	MovePointSetting(boss);

	boss.SetMovePoint(movePoint_,moveFrame_);

	//目的地に着いたかどうか調べる
	boss.ArrivedSegmentDiff();

	if (boss.IsMoveSuccess()) {
		if (isFinish_) {
			//移動ステートに変更
			boss.ChangeStatePattern(std::make_unique<BossMoveState>());
		}
		//完了フラグリセット
		boss.ResetMoveSuccess();
		isMovingStartPoint = false;
	}
}

void BossFallPlayerState::MovePointSetting(Boss& boss) {
	if (fallTimer_ < kFallTimeMax_) {
		movePoint_ = boss.GetPlayer()->GetTranslate();
		movePoint_.y = kStartPoint_.y - fallTimer_;
		boss.SetTranslate(movePoint_);
		boss.SetOrigin(movePoint_);
	}
	else if (fallTimer_ >= kGoUpTime_) {
		movePoint_.y = kGoUpPointY_;
		isFinish_ = true;
	}
	else {
		movePoint_.y = kFallPointY_;
		moveFrame_ = kFallFrame_;
	}

	fallTimer_ += kDeltaTime_;
}

void BossDeadMotionState::Update(Boss& boss) {
	if (isStart_) {
		boss.DeadPosition();
		isStart_ = false;
	}

	boss.DeadMotion();
}
