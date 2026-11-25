#include "../../../Common/AnimationController/AnimationController.h"
#include "../Boss.h"

#include "BossFireballAction.h"

BossFireballAction::BossFireballAction(void)
{
}

BossFireballAction::~BossFireballAction(void)
{
}

void BossFireballAction::OnActiveStart(Boss& owner)
{
	owner.ShootFireball();
}

bool BossFireballAction::UpdateActive(Boss& owner)
{
	// アニメが最後まで再生されたら、攻撃本体を終える
	return owner.GetAnimationController()->IsEnd();
}

SOUND_ID BossFireballAction::GetSignSoundID(void) const
{
    return SOUND_ID::SE_BOSS_FIREBALL_SIGN;
}

SOUND_ID BossFireballAction::GetAttackSoundID(void) const
{
	return SOUND_ID::SE_BOSS_BREATH;
}