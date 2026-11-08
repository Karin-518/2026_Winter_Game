#include "../../../Common/Math/Math.h"
#include "../../../Common/Collision/Collision.h"
#include "../../Collider/Hurtbox.h"
#include "../AnimationController/AnimationController.h"
#include "AttackParam.h"

#include "AttackController.h"

namespace
{
    // デバッグ描画の高さ
    constexpr float DEBUG_DRAW_HEIGHT = 5.0f;

    // デバッグ描画の透明度（0～255）
    constexpr int DEBUG_DRAW_ALPHA = 120;

    // 扇を何枚の三角形で描くか
    constexpr int FAN_DIVISION = 12;

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

    // カプセルの攻撃は、カプセルごとに決める
    if (param_->shape == HIT_SHAPE::CAPSULE)
    {
        UpdateCapsuleActive(anim);
        return;
    }

    // 手動の攻撃は、ステートが決めた値をそのまま使う
    if (param_->isManualActive)
    {
        isActive_ = manualActive_;
        return;
    }

    // 再生位置が判定区間に入っているか
    isActive_ = anim.IsInRange(param_->hitStart, param_->hitEnd);
}

bool AttackController::CanHit(void) const
{
    return isActive_ && !hasHit_;
}

bool AttackController::IsHit(const VECTOR& ownerPos, const VECTOR& forward, const Hurtbox& hurtbox) const
{
    if (param_ == nullptr) return false;

    // カプセル
    if (param_->shape == HIT_SHAPE::CAPSULE)
    {
        return IsHitCapsule(hurtbox);
    }

    // 四角・扇
    return IsHitXZ(ownerPos, forward, hurtbox);
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

void AttackController::DrawDebug(const VECTOR& ownerPos, const VECTOR& forward) const
{
    // 攻撃中でなければ描かない
    if (param_ == nullptr) return;

    // カプセルの攻撃判定
    if (param_->shape == HIT_SHAPE::CAPSULE)
    {
        DrawDebugCapsules();
        return;
    }

    VECTOR origin = CalcOrigin(ownerPos, forward);
    origin.y += DEBUG_DRAW_HEIGHT;
    unsigned int color = GetDebugColor();

    // 半透明にして、キャラが隠れないようにする
    SetDrawBlendMode(DX_BLENDMODE_ALPHA, DEBUG_DRAW_ALPHA);

    if (param_->shape == HIT_SHAPE::FAN)
    {
        DrawDebugFan(origin, forward, color);
    }
    else
    {
        DrawDebugRect(origin, forward, color);
    }

    SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}

void AttackController::UpdateCapsuleActive(const AnimationController& anim)
{
    for (int volumeIndex = 0; volumeIndex < static_cast<int>(capsules_.size()); volumeIndex++)
    {
        // カプセルのデータ
        const AttackHitVolume& volume = param_->hitVolumes[volumeIndex];

        // 手動の攻撃はステートが決めた値、それ以外はアニメの区間で決める
        bool isVolumeActive = manualActive_;
        if (!param_->isManualActive)
        {
            isVolumeActive = anim.IsInRange(volume.activeStart, volume.activeEnd);
        }

        capsules_[volumeIndex].isActive = isVolumeActive;

        // どれか1つでも出ていれば、この攻撃は判定中
        if (isVolumeActive) isActive_ = true;
    }
}

bool AttackController::IsHitXZ(const VECTOR& ownerPos, const VECTOR& forward, const Hurtbox& hurtbox) const
{
    // 攻撃の形のおおよその真ん中
    float distanceToCenter = param_->offset + param_->length * 0.5f;
    VECTOR attackCenter = VAdd(ownerPos, VScale(forward, distanceToCenter));

    // 食らい判定のうち、攻撃の真ん中に一番近い点を円として調べる
    VECTOR targetPos = hurtbox.GetClosestPos(attackCenter);
    float targetRadius = hurtbox.GetRadius();

    VECTOR origin = CalcOrigin(ownerPos, forward);

    // 扇
    if (param_->shape == HIT_SHAPE::FAN)
    {
        float halfRad = Math::Deg2Rad(param_->halfAngleDeg);
        return CollisionXZ::HitFanCircle(
            origin, forward, param_->length, halfRad, targetPos, targetRadius);
    }

    // 四角
    return CollisionXZ::HitRectCircle(
        origin, forward, param_->length, param_->width, targetPos, targetRadius);
}

bool AttackController::IsHitCapsule(const Hurtbox& hurtbox) const
{
    for (int volumeIndex = 0; volumeIndex < static_cast<int>(capsules_.size()); volumeIndex++)
    {
        // カプセルの今の位置
        const HitCapsule& capsule = capsules_[volumeIndex];

        // 判定が出ていないカプセルは飛ばす
        if (!capsule.isActive) continue;

        // 攻撃のカプセルと食らい判定のカプセルが重なっているか
        int hitResult = HitCheck_Capsule_Capsule(
            capsule.startPos, capsule.endPos, param_->hitVolumes[volumeIndex].radius,
            hurtbox.GetStartPos(), hurtbox.GetEndPos(), hurtbox.GetRadius());

        if (hitResult == TRUE) return true;
    }

    return false;
}

VECTOR AttackController::CalcOrigin(const VECTOR& ownerPos, const VECTOR& forward) const
{
    return VAdd(ownerPos, VScale(forward, param_->offset));
}

unsigned int AttackController::GetDebugColor(void) const
{
    if (hasHit_) return COLOR_HIT;
    if (isActive_) return COLOR_ACTIVE;
    return COLOR_WAIT;
}

void AttackController::DrawDebugRect(const VECTOR& origin, const VECTOR& forward, unsigned int color) const
{
    // 右方向
    VECTOR right = { forward.z, 0.0f, -forward.x };
    VECTOR halfWidth = VScale(right, param_->width * 0.5f);
    VECTOR front = VScale(forward, param_->length);

    // 四隅
    VECTOR nearLeft = VSub(origin, halfWidth);
    VECTOR nearRight = VAdd(origin, halfWidth);
    VECTOR farRight = VAdd(nearRight, front);
    VECTOR farLeft = VAdd(nearLeft, front);

    // 三角形2枚で四角にする
    DrawTriangle3D(nearLeft, farLeft, farRight, color, true);
    DrawTriangle3D(nearLeft, farRight, nearRight, color, true);
}

void AttackController::DrawDebugFan(const VECTOR& origin, const VECTOR& forward, unsigned int color) const
{
    float halfRad = Math::Deg2Rad(param_->halfAngleDeg);
    float step = (halfRad * 2.0f) / static_cast<float>(FAN_DIVISION);

    // 左端から右端へ、少しずつ回しながら三角形を描く
    VECTOR prevDir = VTransform(forward, MGetRotY(-halfRad));
    VECTOR prevPos = VAdd(origin, VScale(prevDir, param_->length));

    for (int i = 1; i <= FAN_DIVISION; i++)
    {
        VECTOR nextDir = VTransform(
            forward, MGetRotY(-halfRad + step * static_cast<float>(i)));

        VECTOR nextPos = VAdd(origin, VScale(nextDir, param_->length));

        DrawTriangle3D(origin, prevPos, nextPos, color, true);

        prevPos = nextPos;
    }
}

void AttackController::DrawDebugCapsules(void) const
{
    // カプセルの分割数
    const int capsuleDivision = 8;
    for (int volumeIndex = 0; volumeIndex < static_cast<int>(capsules_.size()); volumeIndex++)
    {
        // カプセルの今の位置
        const HitCapsule& capsule = capsules_[volumeIndex];

        // 区間外は灰色、判定中は赤、当てた後は黄色
        unsigned int color = COLOR_WAIT;
        if (capsule.isActive) color = COLOR_ACTIVE;
        if (hasHit_) color = COLOR_HIT;

        DrawCapsule3D(capsule.startPos, capsule.endPos,
            param_->hitVolumes[volumeIndex].radius, capsuleDivision, color, color, false);
    }
}