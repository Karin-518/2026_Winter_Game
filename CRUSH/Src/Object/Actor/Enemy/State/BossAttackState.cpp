#include "../../../Common/AnimationController/AnimationController.h"
#include "../Boss.h"

#include "BossAttackState.h"

void BossAttackState::Enter(Boss& owner)
{
    owner.StartAttack();
}

BOSS_STATE BossAttackState::Update(Boss& owner)
{
    if (owner.GetAnimationController()->IsEnd())
    {
        return BOSS_STATE::IDLE;
    }

    // âΩÇ‡Ç»ÇØÇÍÇŒÅAêÿÇËë÷Ç¶Ç»Ç¢
    return BOSS_STATE::NONE;
}

void BossAttackState::Exit(Boss& owner)
{
    owner.EndAttack();
}