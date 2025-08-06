#include "IPostEffects.h"

int IPostEffects::effectNo = Mode_Normal_Image;

IPostEffects::~IPostEffects() {}

//素早く変更可能
void IPostEffects::ChangeNumber() {
	if (Input::GetInstance()->TriggerKey(DIK_SPACE)) {
		effectNo++;
	}
	//最大→0に
	if (effectNo == Max) {
		effectNo = 0;
	}
}
