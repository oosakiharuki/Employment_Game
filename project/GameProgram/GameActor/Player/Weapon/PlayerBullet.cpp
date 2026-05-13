#include "PlayerBullet.h"
#include "UseEveryOne.h"

using namespace MyMath;
using namespace UseEveryOne;

PlayerBullet::~PlayerBullet() {}


void PlayerBullet::Initialize() {
	object_ = std::make_unique<Object3d>();
	object_->Initialize();
	object_->SetModelFile("PlayerBullet.obj");

	wt_.Initialize();
	//Transform更新処理
	transform_ = wt_.UpdateTransform();

	shadow_ = std::make_unique<Shadow>();
	shadow_->Initialize();

	//AABBの大きさ設定
	bulletAABB_.min = -kBulletSize_ * kDivideByTwo_;
	bulletAABB_.max = kBulletSize_ * kDivideByTwo_;

	collisionType_ = CollisionTypes::TypePlayerBullet;
}

void PlayerBullet::Update() {

	deathTimer_ += kDeltaTime_;
	
	//弾丸速度
	//徐々に減速する
	Vector3 velocity = EaseOut({0,0,0}, velocity_, deathTimer_ / kEndTime_);
	transform_.translate += velocity;
	transform_.scale = EaseIn(kDefaultScale_ * 1.5f ,{ 0,0,0 },  deathTimer_ / kEndTime_);

	//時間がたったら消える
	if (deathTimer_ >= kEndTime_) {
		isDead_ = true;
	}

	object_->Update(wt_);
	wt_.UpdateMatrix(transform_);

	//影更新
	shadow_->SetScale(transform_.scale * kDivideByTwo_);//少し小さく(0.5倍)
	shadow_->SetTranslate(transform_.translate);
	shadow_->Update();

	//当たり判定設定
	collisionAABB_.min = transform_.translate + bulletAABB_.min;
	collisionAABB_.max = transform_.translate + bulletAABB_.max;
	center_ = transform_.translate;
	CollisionManager::GetInstance().FrameCollision(this);
}

void PlayerBullet::Draw() {
	shadow_->Draw();//影の描画
	//オブジェクト描画
	object_->Draw();
}

void PlayerBullet::OnCollision(CollisionSource* collision) {
	if (collision->GetType() == CollisionTypes::TypeStage ||
		collision->GetType() == CollisionTypes::TypeEnemy ||
		collision->GetType() == CollisionTypes::TypeEnemyDamageBody ||
		collision->GetType() == CollisionTypes::TypeBoss) {
		BulletDamage();
	}
}

bool PlayerBullet::TypeCheckUp(const CollisionTypes& collisionType) {
	if (collisionType == CollisionTypes::TypeStage ||
		collisionType == CollisionTypes::TypeEnemy ||
		collisionType == CollisionTypes::TypeEnemyDamageBody ||
		collisionType == CollisionTypes::TypeBoss) {
		return true;
	}
	return false;
}

void PlayerBullet::BulletDamage() {
	bulletPower_--;//耐久値が減る
	//耐久値が0の時
	if (bulletPower_ <= 0) {
		isDead_ = true;//消滅
	}
}
