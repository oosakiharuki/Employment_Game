#include "BaseEnemy.h"
#include "ImGuiManager.h"
#include "ParticleManager.h"

#include "TimeScale.h"

using namespace MyMath;
using namespace UseEveryOne;

BaseEnemy::BaseEnemy() {}

BaseEnemy::~BaseEnemy(){}

void BaseEnemy::Enemy_InitializeCommon(const std::string& objectName) {
	//アクターの共通初期化処理
	GameActor::Initialize();

	object_ = std::make_unique<EngineLayer::Object_glTF>();
	object_->Initialize();
	object_->SetModelFile(objectName);

	//見つけたときの「!」マーク
	objectFound_ = std::make_unique<EngineLayer::Object3d>();
	objectFound_->Initialize();
	objectFound_->SetModelFile("player_found_mark.obj");

	//見失ったときのの「?」マーク
	objectNoFound_ = std::make_unique<EngineLayer::Object3d>();
	objectNoFound_->Initialize();
	objectNoFound_->SetModelFile("player_lost_mark.obj");

	//パーティクル
	//ダメージ
	particles_[particleDamage_] = EngineLayer::ParticleManager::GetInstance().InitParticle(particleDamage_);

	//マークのワールド座標
	wtMark_.Initialize();	
	//Transform更新処理
	transformMark_ = wtMark_.UpdateTransform();

	collisionType_ = CollisionTypes::TypeEnemy;

	enemyEye_ = std::make_unique<EnemyEye>();



	hpSprite_ = std::make_unique<EngineLayer::Sprite>();
	hpSprite_->Initialize("enemyHp.png");
	hpSprite_->SetSize(kHpSpriteSize_);

	underBarSprite_ = std::make_unique<EngineLayer::Sprite>();
	underBarSprite_->Initialize("hpBar.png");
	underBarSprite_->SetSize(kHpSpriteSize_);
}

void BaseEnemy::Update() {
	GameActor::Update();//ステートパターンが入っている

	//演出中の場合
	if (player_->GetPerformanceMode()) {
		//オブジェクトのみの更新
		UpdateBehind();
		return;
	}

	//移動量を加算
	CollisionUtility::GetInstance().OnMoveGround(transform_.translate);

	//リアクション
	reaction_->ScaleReaction(transform_.scale,isDamageMotion_, damageScale_, scaleTimer_, kDamageMaxTime_);

	//体力バースプライト更新
	HpSpriteUpdate();

#ifdef USE_IMGUI
	
	//ImGui 更新
	UpdateImGui();

#endif // USE_IMGUI

	//オブジェクト更新
	UpdateBehind();

}

void BaseEnemy::UpdateBehind() {
	//影
	ShadowUpdate();

	wt_.UpdateMatrix(transform_);
	object_->Update(wt_);

	//設定した全てのパーティクル更新処理
	for (auto& particle : particles_) {
		particle.second->Update();
	}

	//死んだときは当たり判定を取らない
	if (hp_ == 0) return;

	//当たり判定設定
	collisionAABB_.min = transform_.translate - colliderSize_;
	collisionAABB_.max = transform_.translate + colliderSize_;
	center_ = transform_.translate;

	CollisionManager::GetInstance().FrameCollision(this);
	collisionOverlap = CollisionUtility::GetInstance().SetTarget(GetTranslate(), GetAABB());

}

void BaseEnemy::MarkUpdate() {

	//!,?のマーク表示時間の間
	if (attackSwitch_) {
		markTimer_ += TimeScale::GetInstance().GetTimeScale();
	}
	else {
		markTimer_ -= TimeScale::GetInstance().GetTimeScale();
	}
	markTimer_ = std::clamp(markTimer_, 0.0f, kMarkMaxTime_);//0 ～ kMarkMaxTime

	// - マーク -
	transformMark_.translate = transform_.translate;  //敵の座標位置に
	transformMark_.translate.y += kMarkPositionY_;//敵の少し上の位置に
	
	//!,?マークの更新処理
	wtMark_.UpdateMatrix(transformMark_);
	objectFound_->Update(wtMark_);
	objectNoFound_->Update(wtMark_);
}

void BaseEnemy::MarkDraw() {
	//見つけたとき+マークが出る時間
	if(attackSwitch_ && markTimer_ < kMarkMaxTime_)
		objectFound_->Draw();

	//見失った+マークが出る時間
	if (!attackSwitch_ && !enemyEye_->IsFound() && markTimer_ > 0.0f)
		objectNoFound_->Draw();
}

void BaseEnemy::DrawCommon() {
	if (hp_ == 0) return;
	MarkDraw();
}

void BaseEnemy::DrawParticle() {
	//設定した全てのパーティクル描画処理
	for (auto& particle : particles_) {
		particle.second->Draw();
	}
}

void BaseEnemy::IsDamage() {
	//ダメージのパーティクルを出す
	particles_[particleDamage_]->SetTranslate(transform_.translate); //座標を読み取る
	particles_[particleDamage_]->SetParticleBorn(EngineLayer::ParticleBorn::MomentMode); // 発生モード(一度だけ)の変更
	isDamageMotion_ = true;

	//連続ヒット時、元に戻す
	transform_.scale = kDefaultScale_;
	scaleTimer_ = 0.0f;

	//Hpが0なら
	if (hp_ == 0) {
		return;
	}
	//体力 -1
	hp_--;
}

void BaseEnemy::PlayerTarget() {
	//見つかった瞬間だけtrueに
	if (markTimer_ < kFoundMotionMaxTime_) {
		isFoundReaction_ = true;
		preTranslate_ = transform_.translate;
	}
}

void BaseEnemy::SearchRange() {
	if (hp_ != 0) {
		enemyEye_->Update(transform_.translate, eyeReach_);
	}
}

void BaseEnemy::DirectionDegree() {

	//360度以上の場合 [360除算のあまり]
	transform_.rotate.y = std::fmod(transform_.rotate.y, kMaxAngle);
	//-の場合 [0以上になるまで360加算]
	while (transform_.rotate.y < 0) {
		transform_.rotate.y += kMaxAngle;
	}

	///0~179は右
	if (transform_.rotate.y >= 0.0f && transform_.rotate.y < kMaxAngle * kDivideByTwo_) {
		transform_.rotate.y = kDirectionRight_;
	}///180~360は左
	else if (transform_.rotate.y <= kMaxAngle) {
		transform_.rotate.y = kDirectionLeft_;
	}
}

void BaseEnemy::FoundReaction() {

	//伸びる強さ(y軸のみ)
	Vector3 reaction = { 0,damageScale_.y * kDivideByTwo_,0 };

	reaction_->ScaleReaction(transform_.scale, isFoundReaction_, reaction, scaleTimer_, kFoundMotionMaxTime_);
	reaction_->FoundReaction(transform_.translate,isFoundReaction_, reaction, foundTimer_, kFoundMotionMaxTime_, preTranslate_);
}

void BaseEnemy::DeadReaction() {
	transform_.rotate -= TransformNormal(Vector3{ kDeadRotation_,0,0 }, wt_.GetMatWorld()) * TimeScale::GetInstance().GetTimeScaleFacto();

	//リアクションフラグ
	bool isReaction = true;

	//伸びる強さ(y軸のみ)
	Vector3 reaction = { 0,damageScale_.y * kDivideByTwo_,0 };
	reaction_->FoundReaction(transform_.translate, isReaction, reaction, foundTimer_, kMarkMaxTime_ * kDivideByTwo_, preTranslate_);

	//リアクションが終わったら
	if (!isReaction) {
		isDeleteEnemy_ = true;
	}
}

bool BaseEnemy::IsLostFound() {
	//見失ってから少したってから通常処理に戻す
	if (!attackSwitch_ && markTimer_ <= 0.0f) {
		return true;
	}
	return false;
}

void BaseEnemy::HpSpriteUpdate() {
	//ダメージを食らったら、タイマーがリセットされてないなら
	if (isDamageMotion_ || hpSpriteTimer_ < kMaxHpSpriteTimer_) {
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

		hpSpriteTimer_ -= kDeltaTime_;
	}

	if (hpSpriteTimer_ < 0.0f) {
		hpSpriteTimer_ = kMaxHpSpriteTimer_;
	}
}



void BaseEnemy::OnCollision(CollisionSource* collision) {
	if (collision->GetType() == CollisionTypes::TypePlayerBullet) {
		IsDamage();
	}

	if (collision->GetType() == CollisionTypes::TypeStage || collision->GetType() == CollisionTypes::TypeMoveGround) {
		CollisionUtility::GetInstance().GameActorAndStageCollision(collisionOverlap, *this, *this, collision->GetAABB());
	}
}

bool BaseEnemy::TypeCheckUp(const CollisionTypes& collisionType) {
	if (collisionType == CollisionTypes::TypePlayerBullet ||
		collisionType == CollisionTypes::TypeStage || collisionType == CollisionTypes::TypeMoveGround) {
		return true;
	}
	return false;
}


void BaseEnemy::StatePatternUpdate() {
	enemyState_->Update(*this);	
	enemyState_->ChangeState(*this);
}

void BaseEnemy::ChangeStatePattern(std::unique_ptr<BaseEnemyState> enemyState) {
	enemyState_.reset();
	enemyState_ = std::move(enemyState);
}
