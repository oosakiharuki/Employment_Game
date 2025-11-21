#include "IPostEffects.h"

int IPostEffects::sEffectNo_ = Mode_DepthBasedOutline;

IPostEffects::~IPostEffects() {}

//素早く変更可能
void IPostEffects::ChangeNumber() {

#ifdef _DEBUG
	if (Input::GetInstance()->TriggerKey(DIK_F1)) {
		sEffectNo_++;
	}
	//最大→0に
	if (sEffectNo_ == Max) {
		sEffectNo_ = 0;
	}
#endif // _DEBUG
}
