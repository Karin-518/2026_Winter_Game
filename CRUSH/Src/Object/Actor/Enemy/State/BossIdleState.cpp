#include "../../../Common/AnimationController/AnimationController.h"
#include "../Boss.h"

#include "BossIdleState.h"

void BossIdleState::Enter(Boss& owner)
{
    // 待機アニメーションを再生
    owner.GetAnimationController()->Play(static_cast<int>(Boss::ANIM_TYPE::IDLE));
}

BOSS_STATE BossIdleState::Update(Boss& owner)
{
    // 何もなければ、切り替えない
    return BOSS_STATE::NONE;
}