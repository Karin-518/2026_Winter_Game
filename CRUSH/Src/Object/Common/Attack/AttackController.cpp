#include "../../Collider/Hurtbox.h"
#include "../AnimationController/AnimationController.h"
#include "AttackParam.h"

#include "AttackController.h"

namespace
{
    // デバッグ描画のカプセルの分割数
    constexpr int DEBUG_CAPSULE_DIV_NUM = 8;

    // デバッグ描画の色
    constexpr unsigned int COLOR_WAIT = 0x808080; // 区間外
    constexpr unsigned int COLOR_ACTIVE = 0xff3030; // 区間内
    constexpr unsigned int COLOR_HIT = 0xffff00; // 当たった後
}

AttackController::AttackController(void)
    :
    param_(nullptr),
    isActive_(false),
    hasHit_(false),
    manualActive_(false),
    capsules_()
{
}

AttackController::~AttackController(void)
{
}

void AttackController::Start(const AttackParam& param)
{
    param_ = &param;
    isActive_ = false;
    hasHit_ = false;
    manualActive_ = false;

    // 判定の数だけカプセルを用意
    capsules_.assign(param.hitVolumes.size(), HitCapsule{});
}

void AttackController::Stop(void)
{
    param_ = nullptr;
    isActive_ = false;
    manualActive_ = false;

    // 前の攻撃のカプセルを残さない
    capsules_.clear();
}

void AttackController::Update(const AnimationController& anim)
{
    isActive_ = false;

    // 攻撃中でなければ何もしない
    if (param_ == nullptr) return;

    // カプセルごとに判定が出ているか決める
    UpdateCapsuleActive(anim);
}

bool AttackController::CanHit(void) const
{
    return isActive_ && !hasHit_;
}

bool AttackController::IsHit(const Hurtbox& hurtbox) const
{
    if (param_ == nullptr) return false;

    for (int volumeIndex = 0; volumeIndex < static_cast<int>(capsules_.size()); volumeIndex++)
    {
        // カプセルの今の位置
        const HitCapsule& capsule = capsules_[volumeIndex];

        // 判定が出ていないカプセルは飛ばす
        if (!capsule.isActive) continue;

        // 攻撃のカプセルの半径
        float attackRadius = param_->hitVolumes[volumeIndex].radius;

        // 攻撃のカプセルと食らい判定のカプセルが重なっているか
        int hitResult = HitCheck_Capsule_Capsule(
            capsule.startPos, capsule.endPos, attackRadius,
            hurtbox.GetStartPos(), hurtbox.GetEndPos(), hurtbox.GetRadius());

        if (hitResult == TRUE) return true;
    }

    return false;
}

void AttackController::SetCapsulePos(int index, const VECTOR& startPos, const VECTOR& endPos)
{
    // 範囲外の番号は無視
    if (index < 0) return;
    if (index >= static_cast<int>(capsules_.size())) return;

    capsules_[index].startPos = startPos;
    capsules_[index].endPos = endPos;
}

void AttackController::NotifyHit(void)
{
    hasHit_ = true;
}

void AttackController::DrawDebug(void) const
{
    // 攻撃中でなければ描かない
    if (param_ == nullptr) return;

    for (int volumeIndex = 0; volumeIndex < static_cast<int>(capsules_.size()); volumeIndex++)
    {
        // カプセルの今の位置
        const HitCapsule& capsule = capsules_[volumeIndex];

        // 区間外は灰色、判定中は赤、当てた後は黄色
        unsigned int color = GetDebugColor(capsule);

        DrawCapsule3D(capsule.startPos, capsule.endPos,
            param_->hitVolumes[volumeIndex].radius, DEBUG_CAPSULE_DIV_NUM, color, color, false);
    }
}

void AttackController::UpdateCapsuleActive(const AnimationController& anim)
{
    for (int volumeIndex = 0; volumeIndex < static_cast<int>(capsules_.size()); volumeIndex++)
    {
        bool isVolumeActive = IsVolumeActive(volumeIndex, anim);
        capsules_[volumeIndex].isActive = isVolumeActive;

        // どれか1つでも出ていれば、この攻撃は判定中
        if (isVolumeActive) isActive_ = true;
    }
}

bool AttackController::IsVolumeActive(int volumeIndex, const AnimationController& anim) const
{
    // 手動の攻撃は、ステートが決めた値をそのまま使う
    if (param_->isManualActive) return manualActive_;

    // それ以外は、再生位置がカプセルの判定区間に入っているか
    const AttackHitVolume& volume = param_->hitVolumes[volumeIndex];
    return anim.IsInRange(volume.activeStart, volume.activeEnd);
}

unsigned int AttackController::GetDebugColor(const HitCapsule& capsule) const
{
    if (hasHit_) return COLOR_HIT;

    if (capsule.isActive) return COLOR_ACTIVE;

    return COLOR_WAIT;
}