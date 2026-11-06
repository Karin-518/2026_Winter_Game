#include "../../../../Audio/AudioManager.h"
#include "../../../Common/AnimationController/AnimationController.h"
#include "../Boss.h"

#include "BossDeadState.h"

void BossDeadState::Enter(Boss& owner)
{
    AudioManager::GetInstance()->PlaySE(SoundID::SE_BOSS_DEAD);

    // 死亡アニメーションを再生
    owner.GetAnimationController()->Play(static_cast<int>(Boss::ANIM_TYPE::DEAD), false);
}

BOSS_STATE BossDeadState::Update(Boss& owner)
{
    // 死亡したら、ほかのステートに移らない
    return BOSS_STATE::NONE;
}
