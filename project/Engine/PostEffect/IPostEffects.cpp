#include "IPostEffects.h"

int IPostEffects::effectNo = Mode_DepthBasedOutline;

IPostEffects::~IPostEffects() {}

//素早く変更可能
void IPostEffects::ChangeNumber() {

#ifdef _DEBUG
	if (Input::GetInstance()->TriggerKey(DIK_F1)) {
		effectNo++;
	}
	//最大→0に
	if (effectNo == Max) {
		effectNo = 0;
	}
#endif // _DEBUG
}
