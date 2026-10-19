#include "../../../../Input/InputManager.h"
#include "../../../Common/AnimationController/AnimationController.h"
#include "../Player.h"
#include "PlayerAttackState.h"

namespace
{
	constexpr int ATTACK_BLEND_FRAME = 4;
}

void PlayerAttackState::Enter(Player& owner)
{
	// 攻撃アニメーションを再生
	AnimationController::PlayParam param;
	param.isLoop = false;
	param.blendFrame = ATTACK_BLEND_FRAME;
	param.isForce = true;

	owner.GetAnimationController()->Play(static_cast<int>(Player::ANIM_TYPE::ATTACK_1),false);
}

PLAYER_STATE PlayerAttackState::Update(Player& owner)
{
	if (owner.GetAnimationController()->IsEnd())
	{
		return PLAYER_STATE::IDLE;
	}

	return PLAYER_STATE::NONE;
}
