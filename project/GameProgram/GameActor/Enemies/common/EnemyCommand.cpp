#include "EnemyCommand.h"
#include "UseEveryOne.h"

using namespace MyMath;
using namespace UseEveryOne;

void EnemyFireCommand::InitAudio() {
	Audio::GetInstance().LoadWave(kFireSoundName_);
	Audio::GetInstance().LoadWave(kFireBeforeSoundName_);
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
			Audio::GetInstance().StopWave(kFireBeforeSoundName_);
			Audio::GetInstance().StopWave(kFireSoundName_);//音ズレが起きないよう
			Audio::GetInstance().SoundPlayWave(kFireSoundName_, kVolume_);
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
		if (!Audio::GetInstance().IsPlayingSound(kFireBeforeSoundName_)) {
			Audio::GetInstance().SoundPlayWave(kFireBeforeSoundName_, kVolume_);
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
