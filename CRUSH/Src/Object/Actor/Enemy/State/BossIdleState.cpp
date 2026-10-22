#include "../../../Common/AnimationController/AnimationController.h"
#include "../Boss.h"

#include "BossIdleState.h"

namespace
{
    // 待機時間
    constexpr int WAIT_FRAME = 3*60;
}

void BossIdleState::Enter(Boss& owner)
{
    // 待機アニメーションを再生
    owner.GetAnimationController()->Play(static_cast<int>(Boss::ANIM_TYPE::IDLE));
}

BOSS_STATE BossIdleState::Update(Boss& owner)
{
    frame_++;

    // 待機時間に達したら、攻撃ステートへ遷移
    if (frame_ >= WAIT_FRAME)
    {
        return BOSS_STATE::ATTACK;
    }

    // 何もなければ、切り替えない
    return BOSS_STATE::NONE;
}