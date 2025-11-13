#include "IPostEffects.h"

int IPostEffects::sEffectNo = Mode_DepthBasedOutline;

IPostEffects::~IPostEffects() {}

//素早く変更可能
void IPostEffects::ChangeNumber() {

#ifdef _DEBUG
	if (Input::GetInstance()->TriggerKey(DIK_F1)) {
		sEffectNo++;
	}
	//最大→0に
	if (sEffectNo == Max) {
		sEffectNo = 0;
	}
#endif // _DEBUG
}
