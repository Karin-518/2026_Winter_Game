#include "../../../Common/AnimationController/AnimationController.h"
#include "../Boss.h"

#include "BossFlyFireAction.h"

BossFlyFireAction::BossFlyFireAction(void)
{
}

BossFlyFireAction::~BossFlyFireAction(void)
{
}

void BossFlyFireAction::OnActiveStart(Boss& owner)
{
	owner.ShootFireball();
}

bool BossFlyFireAction::UpdateActive(Boss& owner)
{
	// アニメが最後まで再生されたら、攻撃本体を終える
	return owner.GetAnimationController()->IsEnd();
}

SOUND_ID BossFlyFireAction::GetSignSoundID(void) const
{
	return SOUND_ID::SE_BOSS_FLYFIRE_SIGN;
}

SOUND_ID BossFlyFireAction::GetAttackSoundID(void) const
{
	return SOUND_ID::SE_BOSS_BREATH;
}
