#include "../../../Common/AnimationController/AnimationController.h"
#include "../Boss.h"

#include "BossRushAction.h"

namespace
{
	// 1フレームで進む距離
	constexpr float CHARGE_SPEED = 20.0f;

	// プレイヤーの位置から走り抜ける距離
	constexpr float OVERSHOOT_DISTANCE = 500.0f;

	// 壁に引っかかったときの時間切れ
	constexpr int CHARGE_MAX_FRAME = 120;

	// 走りアニメに切り替えるときのブレンド
	constexpr float CHARGE_BLEND_FRAME = 5.0f;
}

BossRushAction::BossRushAction(void)
	:
	chargeDistance_(0.0f),
	movedDistance_(0.0f),
	chargeFrame_(0)
{
}

BossRushAction::~BossRushAction(void)
{
}

void BossRushAction::UpdateWindup(Boss& owner)
{
	// 溜めの間はプレイヤーを向き続ける
	owner.FacePlayer();
}

void BossRushAction::OnActiveStart(Boss& owner)
{
	// ここで方向と距離を決めて、以降は追いかけない
	owner.FacePlayer();
	chargeDistance_ = owner.CalcDistanceToPlayer() + OVERSHOOT_DISTANCE;
	movedDistance_ = 0.0f;
	chargeFrame_ = 0;

	// 走りアニメをループで再生し直す
	AnimationController::PlayParam playParam;
	playParam.isLoop = true;
	playParam.isForce = true;
	playParam.blendFrame = CHARGE_BLEND_FRAME;
	owner.GetAnimationController()->Play(static_cast<int>(Boss::ANIM_TYPE::RUN), playParam);
}

bool BossRushAction::UpdateActive(Boss& owner)
{
	chargeFrame_++;

	// moveDir_の方向へ進む
	owner.MoveByDir(CHARGE_SPEED);
	movedDistance_ += CHARGE_SPEED;

	// 走り切ったら終わり
	if (movedDistance_ >= chargeDistance_) return true;

	// 壁に引っかかったときの時間切れ
	if (chargeFrame_ >= CHARGE_MAX_FRAME) return true;

	return false;
}

SoundID BossRushAction::GetSignSoundID(void) const
{
	return SoundID::SE_BOSS_RUSH_SIGN;
}

SoundID BossRushAction::GetAttackSoundID(void) const
{
	return SoundID::SE_BOSS_RUSH;
}
