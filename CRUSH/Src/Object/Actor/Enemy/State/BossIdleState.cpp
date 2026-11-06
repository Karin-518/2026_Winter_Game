#include "../../../Common/AnimationController/AnimationController.h"
#include "../Boss.h"

#include "BossIdleState.h"

namespace
{
    // 待機時間
    constexpr int WAIT_FRAME = 1*60;
}

void BossIdleState::Enter(Boss& owner)
{
    // フレームの初期化
    frame_ = 0;

    // 待機アニメーションを再生
    owner.GetAnimationController()->Play(static_cast<int>(Boss::ANIM_TYPE::IDLE));
}

BOSS_STATE BossIdleState::Update(Boss& owner)
{
    frame_++;

    // 待機時間に達していなければ、待ち続ける
    if (frame_ < WAIT_FRAME) return BOSS_STATE::NONE;

    // 攻撃を選べたら、攻撃ステートへ
    if (owner.SelectAttack()) return BOSS_STATE::ATTACK;

    // 届く攻撃がなければ、もう一度待つ
    frame_ = 0;

    // 何もなければ、切り替えない
    return BOSS_STATE::NONE;
}