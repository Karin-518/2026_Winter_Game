#include "../../../Common/AnimationController/AnimationController.h"
#include "../Boss.h"

#include "BossBiteAction.h"

bool BossBiteAction::UpdateActive(Boss& owner)
{
    // アニメが最後まで再生されたら、攻撃本体を終える
    return owner.GetAnimationController()->IsEnd();
}

SOUND_ID BossBiteAction::GetSignSoundID(void) const
{
    return SOUND_ID::SE_BOSS_BITE_SIGN;
}

SOUND_ID BossBiteAction::GetAttackSoundID(void) const
{
    return SOUND_ID::SE_BOSS_BITE;
}