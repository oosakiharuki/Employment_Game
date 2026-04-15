#include "EnemyCommand.h"
#include "UseEveryOne.h"

using namespace MyMath;
using namespace UseEveryOne;

void EnemyFireCommand::InitAudio() {
	fireSound_ = Audio::GetInstance().LoadWave("resource/Sound/enemyFire.mp3");
	fireBeforeSound_ = Audio::GetInstance().LoadWave("resource/Sound/enemyBeforeFire.mp3");
}


void EnemyFireCommand::BulletUpdate() {	
	//消滅処理
	bullets_.remove_if([](auto& bullet) {
		return bullet->IsDead();
	});

	//弾丸更新処理
	for (auto& bullet : bullets_) {
		bullet->Update();
	}
}

void EnemyFireCommand::BulletDraw() {
	for (auto& bullet : bullets_) {
		bullet->Draw();
	}
}

void EnemyFireCommand::BulletReset() {
	for (auto& bullet : bullets_) {
		bullet.reset();
	}
}


void EnemyFireCommand::Fire(EnemyCanFireBullet& enemyCanFireBullet) {

	//クールタイム
	coolTime_ += kDeltaTime_;


	if (coolTime_ >= kCoolTimeMax_) {
		if (rapidFireTime_ == 0.0f && rapidCount_ == 0) {
			Audio::GetInstance().StopWave(*fireBeforeSound_);
			Audio::GetInstance().StopWave(*fireSound_);//音ズレが起きないよう
			Audio::GetInstance().SoundPlayWave(*fireSound_, kVolume_);
		}

		//連射で時間を開ける
		rapidFireTime_ += kDeltaTime_;
		if (rapidFireTime_ >= kRapidFireTimeMax_) {
			enemyCanFireBullet.FireBullet();//敵の発泡攻撃
			rapidCount_++;//カウント
			rapidFireTime_ = 0;//もう一度
		}

		//最大弾丸数を超えた場合
		if (rapidCount_ >= rapidCountMax_) {
			rapidCount_ = 0;//カウントリセット
			coolTime_ = 0;//クールタイム発動
			isFire_ = false;
		}
	}
	else if (coolTime_ >= kCoolTimeMax_ * kDivideByTwo_ && coolTime_ < kCoolTimeMax_) {
		//SEはすでに鳴っているか
		if (!Audio::GetInstance().IsPlayingSound(*fireBeforeSound_)) {
			Audio::GetInstance().SoundPlayWave(*fireBeforeSound_, kVolume_);
		}
	}
}

void EnemyFireCommand::AddBullet(const Vector3& startPoint, const Vector3& velocity) {
	std::unique_ptr<EnemyBullet> bullet = std::make_unique<EnemyBullet>();
	bullet->Initialize();
	bullet->SetTranslate(startPoint);
	bullet->SetVelocity(velocity);
	bullets_.push_back(std::move(bullet));
}
