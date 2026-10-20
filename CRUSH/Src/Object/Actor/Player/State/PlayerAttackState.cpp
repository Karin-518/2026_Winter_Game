#include "../../../../Input/InputManager.h"
#include "../../../Common/AnimationController/AnimationController.h"
#include "../Player.h"
#include "PlayerAttackState.h"

void PlayerAttackState::Enter(Player& owner)
{
}

PLAYER_STATE PlayerAttackState::Update(Player& owner)
{
	if (owner.GetAnimationController()->IsEnd())
	{
		return PLAYER_STATE::IDLE;
	}

	return PLAYER_STATE::NONE;
}
