#include "../../../Common/AnimationController/AnimationController.h"
#include "../Boss.h"

#include "BossTailSpinAction.h"

namespace
{
	// —­‚ß’†‚É1ƒtƒŒ[ƒ€‚Å‚Ð‚Ë‚éŠp“x(‰ñ“]‚Æ‹tŒü‚«)
	constexpr float WINDUP_TWIST_RAD_PER_FRAME = -0.01f;

	// 1‰ñ“]‚É‚©‚¯‚éƒtƒŒ[ƒ€”
	constexpr float SPIN_FRAME = 30.0f;

	// 1ƒtƒŒ[ƒ€‚Å‰ñ‚éŠp“x
	constexpr float SPIN_RAD_PER_FRAME = DX_TWO_PI_F / SPIN_FRAME;
}

BossTailSpinAction::BossTailSpinAction(void)
	:
	spunRad_(0.0f)
{
}

BossTailSpinAction::~BossTailSpinAction(void)
{
}

void BossTailSpinAction::UpdateWindup(Boss& owner)
{
	// ‰ñ‚é•ûŒü‚Æ‹t‚É­‚µ‚¸‚Â‚Ð‚Ë‚Á‚ÄA—­‚ß‚é
	owner.RotateMoveDir(WINDUP_TWIST_RAD_PER_FRAME);
}

void BossTailSpinAction::OnActiveStart(Boss& owner)
{
	// ‰ñ‚Á‚½Šp“x‚ðƒŠƒZƒbƒg
	spunRad_ = 0.0f;
}

bool BossTailSpinAction::UpdateActive(Boss& owner)
{
	// 1ƒtƒŒ[ƒ€•ª‰ñ‚·
	owner.RotateMoveDir(SPIN_RAD_PER_FRAME);
	spunRad_ += SPIN_RAD_PER_FRAME;

	// 1‰ñ“]‚µ‚½‚çI‚í‚è
	if (spunRad_ >= DX_TWO_PI_F) return true;

	return false;
}

SoundID BossTailSpinAction::GetSignSoundID(void) const
{
	return SoundID::SE_BOSS_TAILSPIN_SIGN;
}

SoundID BossTailSpinAction::GetAttackSoundID(void) const
{
	return SoundID::SE_BOSS_TAILSPIN;
}
