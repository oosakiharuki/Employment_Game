#include "EnemyCommand.h"
#include "UseEveryOne.h"

using namespace MyMath;
using namespace UseEveryOne;

void EnemyFireCommand::BulletUpdate() {
	//弾丸更新処理
	for (auto& bullet : bullets_) {
		bullet->Update();
	}
	//消滅処理
	bullets_.remove_if([](auto& bullet) {
		if (bullet->IsDead()) {
			bullet.reset();
			return true;
		}
		return false;
		});
}

void EnemyFireCommand::Fire() {

	//クールタイム
	coolTime_ += kDeltaTime_;
	if (coolTime_ >= kCoolTimeMax_) {

		//連射で時間を開ける
		rapidFireTime_ += kDeltaTime_;
		if (rapidFireTime_ >= kRapidFireTimeMax_) {
			FireBullet();//敵の発泡攻撃
			rapidCount_++;//カウント
			rapidFireTime_ = 0;//もう一度
		}

		//最大弾丸数を超えた場合
		if (rapidCount_ == rapidCountMax_) {
			rapidCount_ = 0;//カウントリセット
			coolTime_ = 0;//クールタイム発動
		}
	}
}
