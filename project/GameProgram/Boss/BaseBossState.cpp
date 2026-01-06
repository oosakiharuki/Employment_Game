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
		boss.SetMovePoint(boss.GetCenter() + kHazi_, kMoveFrame_);
	}
	else {
		//左位置設定
		boss.SetMovePoint(boss.GetCenter() - kHazi_, kMoveFrame_);
	}

	//目的地に着いたかどうか調べる
	boss.ArrivedSegmentDiff();

	//目的地に着いた
	if (boss.IsMoveSucces()) {
		if (addCount_ == 2) {
			//行動前モーションステートに変更		
			boss.ChangeStatePattern(std::make_unique<BossBeforeActionMosionState>());
		}
		addCount_++;
		boss.ResetMoveSucces();
	}
}

void BossAttackState::Update(Boss& boss) {
	boss.ResetStopFire();
	//発泡処理
	boss.Fire();
	//発砲を終了したら
	if (boss.IsStopFire()) {
		boss.ResetMoveSucces();
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
	if(boss.IsMoveSucces()) {
		boss.ResetMoveSucces();//移動完了フラグリセット(false)
		aroundMoveCount_++;    //カウント加算
	}

	//全てのポイントに移動できたら
	if (aroundMoveCount_ >= movePoints_.size()) {
		//移動ステートに変更
		boss.ChangeStatePattern(std::make_unique<BossMoveState>());
	}
	else {
		//
		boss.SetMovePoint(movePoints_[aroundMoveCount_].position, movePoints_[aroundMoveCount_].bunkatu);
		//目的地に着いたかどうか調べる
		boss.ArrivedSegmentDiff();
	}
}


void BossBeforeActionMosionState::Update(Boss& boss) {

	if (!boss.IsMosionFinish()) {	
		//回転して行動をわかりやすく
		boss.BeforeActionMosion();		
		return;
	}
	else if (moveCoolTimer_ < kMoveCoolTimeMax_) {
		moveCoolTimer_ += kDeltaTime_;
		return;
	}

	//移動カウント
	if (boss.ActionCount() >= kDeepAttackCountMax_) {
		boss.ResetMoveSucces();
		boss.ResetActionCount();
		boss.ChangeStatePattern(std::make_unique<BossFarAttackState>());
	}
	else if (boss.ActionCount() >= kMoveCountMax_) {
		boss.ResetMoveSucces();
		boss.AddActionCount();//移動カウント加算
		boss.ChangeStatePattern(std::make_unique<BossAroundMoveState>());
	}
	else {
		boss.AddActionCount();//移動カウント加算
		boss.ChangeStatePattern(std::make_unique<BossAttackState>());
	}

	boss.ResetMosionFinish();
}

void BossFarAttackState::Update(Boss& boss) {
	//最初のみ行う処理
	if (isStart) {
		//移動する前の位置を取得
		position_ = boss.GetTranslate();
		position_.x = -position_.x;
		//画面奥に移動
		boss.SetMovePoint(kFarPlace_, kBunkatu_);
		isStart = false;
	}

	
	//目的地に着いたかどうか調べる
	boss.ArrivedSegmentDiff();

	if(isAttackFinish_ && boss.IsMoveSucces()){
		//移動ステートに変更
		boss.ChangeStatePattern(std::make_unique<BossMoveState>());
	}
	else if (boss.IsMoveSucces()) {
		boss.ResetStopFire();
		//発泡処理
		boss.Fire(4.0f);
		//発砲を終了したら
		if (boss.IsStopFire()) {
			boss.ResetMoveSucces();
			boss.SetMovePoint(position_, kBunkatu_);
			isAttackFinish_ = true;
		}
	}
}
