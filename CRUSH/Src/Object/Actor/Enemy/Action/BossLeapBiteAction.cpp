#include "../../../Common/AnimationController/AnimationController.h"
#include "../Boss.h"

#include "BossLeapBiteAction.h"

namespace
{
    // 手前で止まる距離
    constexpr float STOP_BEFORE_PLAYER_DISTANCE = 500.0f;

    // 飛び込みの最大値
    constexpr float MAX_LEAP_DISTANCE = 1500.0f;

    // 踏切のStep値
    constexpr float LEAP_START_STEP = 23.0f;

    // 着地のStep値
    constexpr float LEAP_END_STEP = 31.0f;
}

BossLeapBiteAction::BossLeapBiteAction(void)
    :
    leapDistance_(0.0f),
    movedDistance_(0.0f)
{
}

BossLeapBiteAction::~BossLeapBiteAction(void)
{
}

void BossLeapBiteAction::UpdateWindup(Boss& owner)
{
    // 溜めの間はプレイヤーを向き続ける
    owner.FacePlayer();
}

void BossLeapBiteAction::OnActiveStart(Boss& owner)
{
    // 向きを決める
    owner.FacePlayer();
    leapDistance_ = owner.CalcDistanceToPlayer() - STOP_BEFORE_PLAYER_DISTANCE;

    // 0未満なら0にする
    if (leapDistance_ < 0.0f)
    {
        leapDistance_ = 0.0f;
    }

    // leapDistanceがMAXより大きければ、MAX_LEAP_DISTANCEにする
    if (leapDistance_ > MAX_LEAP_DISTANCE)
    {
        leapDistance_ = MAX_LEAP_DISTANCE;
    }

    movedDistance_ = 0.0f;
}

bool BossLeapBiteAction::UpdateActive(Boss& owner)
{
    // 移動処理
    UpdateLeapMove(owner);

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

void BossLeapBiteAction::UpdateLeapMove(Boss& owner)
{
    // まだ踏み切っていないので何もしない
    float step = owner.GetAnimationController()->GetStep();
    if (step < LEAP_START_STEP) return;

    // 飛び切ったら終わり
    if (movedDistance_ >= leapDistance_) return;
 
    // 進み具合を出す
    float leapProgress = (step - LEAP_START_STEP) / (LEAP_END_STEP - LEAP_START_STEP);

    if (leapProgress > 1.0f)
    {
        leapProgress = 1.0f;
    }

    // 今いるべき距離を出す
    float targetDistance = leapDistance_ * Math::Ease(Math::EASE_TYPE::OUT_QUAD, leapProgress);

    // 今フレームに進む距離
    float moveAmount = targetDistance - movedDistance_;

    // moveDir_の方向へ進む
    owner.MoveByDir(moveAmount);
    movedDistance_ += moveAmount;
}