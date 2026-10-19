#include "../../../../Input/InputManager.h"
#include "../../../Common/AnimationController/AnimationController.h"
#include "../Player.h"
#include "PlayerAttackState.h"

void PlayerAttackState::Enter(Player& owner)
{
	// 待機アニメーションを再生
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
