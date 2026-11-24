#include "../../../Common/AnimationController/AnimationController.h"
#include "../Boss.h"

#include "BossTailSpinAction.h"

BossTailSpinAction::BossTailSpinAction(void)
{
}

BossTailSpinAction::~BossTailSpinAction(void)
{
}

bool BossTailSpinAction::UpdateActive(Boss& owner)
{
	// アニメが最後まで再生されたら、攻撃本体を終える
	return owner.GetAnimationController()->IsEnd();
}

SOUND_ID BossTailSpinAction::GetSignSoundID(void) const
{
	return SOUND_ID::SE_BOSS_TAILSPIN_SIGN;
}

SOUND_ID BossTailSpinAction::GetAttackSoundID(void) const
{
	return SOUND_ID::SE_BOSS_TAILSPIN;
}