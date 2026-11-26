#include "../../../Common/AnimationController/AnimationController.h"
#include "../Boss.h"

#include "BossLeapBiteAction.h"

bool BossLeapBiteAction::UpdateActive(Boss& owner)
{
    // アニメが最後まで再生されたら、攻撃本体を終える
    return owner.GetAnimationController()->IsEnd();
}

SOUND_ID BossLeapBiteAction::GetSignSoundID(void) const
{
    return SOUND_ID::SE_BOSS_BITE_SIGN;
}

SOUND_ID BossLeapBiteAction::GetAttackSoundID(void) const
{
    return SOUND_ID::SE_BOSS_BITE;
}
