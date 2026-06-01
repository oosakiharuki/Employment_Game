#include "Boss.h"
#include "ImGuiManager.h"

#include "TimeScale.h"

using namespace MyMath;
using namespace UseEveryOne;

void Boss::Initialize() {
	GameActor::Initialize();

	object_ = std::make_unique<EngineLayer::Object_glTF>();
	object_->Initialize();
	object_->SetModelFile("Boss.gltf");

	wt_.Initialize();
	//Transform更新処理
	transform_ = wt_.UpdateTransform();

	bossState_ = std::make_unique<BossMoveState>();

	collisionType_ = CollisionTypes::TypeBoss;

	hp_ = kMaxHp_;
	maxHp_ = kMaxHp_;


	hpSprite_ = std::make_unique<EngineLayer::Sprite>();
	hpSprite_->Initialize("bossHp.png");
	hpSprite_->SetSize(kHpSpriteSize_);

	underBarSprite_ = std::make_unique<EngineLayer::Sprite>();
	underBarSprite_->Initialize("bossHpBar.png");
	underBarSprite_->SetSize(kHpSpriteSize_);

	EngineLayer::Audio::GetInstance().LoadWave(kFireSoundName_);
	EngineLayer::Audio::GetInstance().LoadWave(kFireBeforeSoundName_);
}

void Boss::Update() {

	GameActor::Update();//ステートパターンが入っている

	bullets_.remove_if([](auto& bullet) {
		return bullet->IsDead();
	});

	for (auto& bullet : bullets_) {
		bullet->Update();
	}

	ImGuiUpdate();

	wt_.UpdateMatrix(transform_);
	object_->SetAnimationTime(TimeScale::GetInstance().GetTimeScale());
	object_->Update(wt_);

	//影更新
	shadow_->SetScale(transform_.scale * kTwice_);//少し大きく(二倍)
	ShadowUpdate();
}

void Boss::Draw() {
	//オブジェクト描画
	object_->Draw();

	EngineLayer::Object3dCommon::GetInstance().Command();
	//弾丸の描画
	for (auto& bullet : bullets_) {
		bullet->Draw();
	}
	shadow_->Draw();//影の描画
}

void Boss::Active() {

	//ステートパターン
	bossState_->Update(*this);
	bossState_->ChangeCommand(*this);
	//変更フラグ後
	if (motionFinish_) {
		motionFinish_ = false;//リセット
	}

	reaction_->ScaleReaction(transform_.scale, isDamageReaction_, damageReactionPower_, damageReactionTimer_, kDamageReactionTimeMax_);
	
	//体力バースプライト更新
	HpSpriteUpdate();

	collisionAABB_.max = transform_.translate + colliderSize_;
	collisionAABB_.min = transform_.translate - colliderSize_;
	center_ = transform_.translate;
	//当たり判定設定
	CollisionManager::GetInstance().FrameCollision(this);
}

void Boss::Dead() {

	if (deadTimer_ < TimeScale::GetInstance().GetTimeScale()) {
		deadPosition_ = transform_.translate;
		deadScale_ = transform_.scale;
	}

	std::random_device seed;
	std::mt19937 random(seed());

	std::uniform_real_distribution<float>shake(-kShakePower, kShakePower);

	//上下左右にシェイク(z軸は関係ない)
	transform_.translate = deadPosition_ + Vector3(shake(random), shake(random), 0.0f);

	transform_.translate.x = std::clamp(transform_.translate.x, deadPosition_.x - kShakePower, deadPosition_.x + kShakePower);
	transform_.translate.y = std::clamp(transform_.translate.y, deadPosition_.y - kShakePower, deadPosition_.y + kShakePower);

	deadTimer_ += TimeScale::GetInstance().GetTimeScale();

	transform_.scale = deadScale_ - deadTimer_ / kDeadTimeMax_;

	if (deadTimer_ >= kDeadTimeMax_) {
		isDeadMotionFinish_ = true;
		transform_.scale = { 0,0,0 };//消えるようにする
	}
}

void Boss::Performance() {}

void Boss::CommandMove() {
	move_.origin.y = transform_.translate.y;
	move_.diff.y = transform_.translate.y;

	//偶数か奇数か
	if (std::fmod(addCount_ + actionCount_, 2) == 0) {
		//右位置設定
		SetMovePoint(moveCenter_ + kEdge_);

	}
	else {
		//左位置設定
		SetMovePoint(moveCenter_ - kEdge_);
	}

	//補間移動(イーズインアウト)
	EaseMove();

	if (EaseGoal()) {
		addCount_++;
	}
	else {
		return;
	}

	if (addCount_ == 3) {
		//行動前モーションステートに変更
		motionFinish_ = true;
		addCount_ = 0;
	}
}

void Boss::SetMovePoint(const Vector3& point, float speedDivision) {
	move_.diff = point;//目的地設定
	timerMax_ = speedDivision;//スピード(○○秒)
}

void Boss::CommandFire(float kFrame, float bulletSpeed, uint32_t bulletMax) {
	//速さを代入
	bulletSpeed_ = bulletSpeed;

	//撃ち始めSE(時間が半分くらいの時)
	if (rapidFireTime_ == kRapidFireTimeMax_ * kDivideByTwo_) {
		EngineLayer::Audio::GetInstance().StopWave(kFireSoundName_);//音ズレが起きないよう
		EngineLayer::Audio::GetInstance().SoundPlayWave(kFireSoundName_, kVolume_);//発砲SE
	}

	//連射で時間を開ける
	rapidFireTime_ += TimeScale::GetInstance().GetTimeScale() / kFrame;
	if (rapidFireTime_ >= kRapidFireTimeMax_) {

		FireBullet();//敵の発泡攻撃
		rapidCount_++;//カウント
		rapidFireTime_ = 0;//もう一度
	}

	//最大弾丸数を超えた場合
	if (rapidCount_ == bulletMax) {
		rapidCount_ = 0;//カウントリセット
		motionFinish_ = true;;
	}
}

void Boss::FireBullet() {
	//現在位置の設定
	Vector3 enemyPosition;
	enemyPosition = { wt_.GetMatWorld().m[3][0],wt_.GetMatWorld().m[3][1],wt_.GetMatWorld().m[3][2] };

	//弾丸速度
	Vector3 velocity;
	//一度だけ
	if (rapidCount_ == 0 || transform_.translate.z >= 10.0f) {
		//プレイヤーの座標
		targetPosition_ = player_->GetWorldPosition();
	}
	//敵とプレイヤーの距離
	Vector3 distance = targetPosition_ - enemyPosition;
	//
	Vector3 normal = Normalize(distance);

	//スピードを合わせる
	normal *= bulletSpeed_;
	velocity = normal;

	//弾丸を生み出す
	std::unique_ptr<EnemyBullet> bullet = std::make_unique<EnemyBullet>();
	bullet->Initialize();
	bullet->SetTranslate(enemyPosition);
	bullet->SetVelocity(velocity);
	bullets_.push_back(std::move(bullet));
}

void Boss::CommandAroundMove() {
	//空っぽか値なしか
	if (movePoints_.empty() || movePoints_.size() == 0) {
		//移動を設定
		movePoints_.push_back({ Vector3(kAroundRightX_ ,kAroundUnder_, 0),kStandardMoveSpeed_ * kDivideByTwo_ });
		movePoints_.push_back({ Vector3(kAroundLeftX_, kAroundUnder_, 0), kStandardMoveSpeed_ });
		movePoints_.push_back({ Vector3(kAroundLeftX_, kGoUpPointY_, 0), kStandardMoveSpeed_ * kDivideByTwo_ });
		movePoints_.push_back({ Vector3(kAroundRightX_, kGoUpPointY_, 0), kStandardMoveSpeed_ });
	}

	//目的地に着いたら
	if (EaseGoal()) {
		aroundMoveCount_++;//カウント加算
	}

	//補間移動(イーズインアウト)
	EaseMove();

	//全てのポイントに移動できたら
	if (aroundMoveCount_ >= movePoints_.size()) {
		motionFinish_ = true;
		aroundMoveCount_ = 0;
		movePoints_.clear();
	}
	else {
		SetMovePoint(movePoints_[aroundMoveCount_].position, movePoints_[aroundMoveCount_].division);
	}
}

void Boss::CommandFarMove() {

	SetMovePoint(kFarPlace_);

	//補間移動(イーズインアウト)
	EaseMove();

	if (EaseGoal()) {
		isFarMoveSuccess_ = true;
	}
}

void Boss::CommandFarTackle() {
	if (transform_.translate == kFarPlace_) {
		SetMovePoint(player_->GetTranslate());
	}

	transform_.translate  += GoDestination(move_) * (TimeScale::GetInstance().GetTimeScale() / kTwice_);//[GoDestination / 120.0f]

	if (transform_.translate.z <= kNearEnd) {
		motionFinish_ = true;
	}
}

void Boss::CommandFallPlayer() {
	fallTimer_ += TimeScale::GetInstance().GetTimeScale();

	if (fallTimer_ < kPrepareFallTimeMax_) {
		movePoint_ = player_->GetTranslate();
		movePoint_.y = kStartPointY_ - fallTimer_;
		SetTranslate(movePoint_);
		SetOrigin(movePoint_);
	}
	else if (fallTimer_ >= kGoUpTime_) {
		if (fallTimer_ < kGoUpTime_ + TimeScale::GetInstance().GetTimeScale()) {
			SetOrigin(transform_.translate);
			moveTimer_ = 0.0f;
		}
		movePoint_.y = kGoUpPointY_;
		SetMovePoint(movePoint_, moveFrame_);
		//補間移動(イーズインアウト)
		EaseMove();
		if (EaseGoal()) {
			motionFinish_ = true;
			fallTimer_ = 0.0f;
		}
	}
	else {
		movePoint_.y = kFallPointY_;
		moveFrame_ = kFallTimeMax_;
		SetMovePoint(movePoint_, moveFrame_);
		//補間移動(イーズインアウト)
		EaseMove();
	}
}

void Boss::CommandBeforeActionMotion() {
	
	//SEはすでに鳴っているか
	if (!EngineLayer::Audio::GetInstance().IsPlayingSound(kFireBeforeSoundName_)) {
		EngineLayer::Audio::GetInstance().SoundPlayWave(kFireBeforeSoundName_, kVolume_);
	}

	if (transform_.rotate.z >= kRotateOneLap_) {
		//モーション終了
		transform_.rotate.z = 0.0f;
	}
	else if(moveCoolTimer_ == 0.0f){
		transform_.rotate.z += kRotationX_ * TimeScale::GetInstance().GetTimeScaleFacto();
		return;
	}

	if (moveCoolTimer_ < kMoveCoolTimeMax_) {
		moveCoolTimer_ += TimeScale::GetInstance().GetTimeScale();
		return;
	}

	motionFinish_ = true;
	moveCoolTimer_ = 0.0f;
	EngineLayer::Audio::GetInstance().StopWave(kFireBeforeSoundName_);//予備音声を止める
}


void Boss::ChangeStatePattern(std::unique_ptr<BaseBossState> state) {
	bossState_.reset();
	bossState_ = std::move(state);
}

void Boss::BossCenter(const Vector3& center) {
	//センターを設定
	moveCenter_ = center;
	//右位置をセグメント終点に設定
	SetMovePoint(moveCenter_ + kEdge_);
}

void Boss::IsDamage() {
	if (hp_ == 0) {
		return;
	}
	hp_--;
	isDamageReaction_ = true;
}

void Boss::ImGuiUpdate() {
#ifdef USE_IMGUI

	ImGui::Begin("boss");
	ImGui::Text("translate: %f, %f, %f", transform_.translate.x, transform_.translate.y, transform_.translate.z);
	ImGui::Text("Hp: %d",hp_);
	ImGui::End();

#endif // USE_IMGUI

}

void Boss::EaseMove() {
	moveTimer_ += TimeScale::GetInstance().GetTimeScale();
	moveTimer_ = std::clamp(moveTimer_, 0.0f, timerMax_);
	transform_.translate = EaseInOut(move_.diff, move_.origin, moveTimer_ / timerMax_);
}

bool Boss::EaseGoal() {
	if (moveTimer_ == timerMax_) {
		moveTimer_ = 0.0f;
		transform_.translate = move_.diff;//現在地を目的地にする
		move_.origin = transform_.translate;//セグメントのスタート値を設定
		return true;
	}
	
	return false;
}


void Boss::OnCollision(CollisionSource* collision) {
	if (collision->GetType() == CollisionTypes::TypePlayerBullet) {
		IsDamage();
	}
}

bool Boss::TypeCheckUp(const CollisionTypes& collisionType) {
	if (collisionType == CollisionTypes::TypePlayerBullet) {
		return true;
	}
	return false;
}	

void Boss::HpSpriteUpdate(){
	//ウィンドウズの画像範囲
	Vector2 windows = { (float)EngineLayer::WinApp::kClientWidth_,(float)EngineLayer::WinApp::kClientHeight_ };
	windows *= kSpriteWindowsPosition_;
	//バーの設定
	underBarSprite_->SetPosition(windows);

	Vector2 hpPosition = { windows.x + kHpSpriteSize_.x * kSpriteRatio_ * kDivideByTwo_,windows.y };//バーに左右両方間をあける
	float nowHp = (float)hp_ / (float)maxHp_;//現在体力と最大体力の比率
	float barRatio = (1.0f - kSpriteRatio_);//少しだけ小さく(これもバーに左右両方間をあけるため)
	//体力バーの設定
	hpSprite_->SetPosition(hpPosition);
	hpSprite_->SetSize({ (kHpSpriteSize_.x * nowHp) * barRatio, kHpSpriteSize_.y });
	//フレーム読み込み
	UIManager::GetInstance().FrameSprite(&*underBarSprite_);
	UIManager::GetInstance().FrameSprite(&*hpSprite_);
}