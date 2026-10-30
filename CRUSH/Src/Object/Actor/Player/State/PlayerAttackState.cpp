#include "../../../Common/AnimationController/AnimationController.h"
#include "../../../Common/Attack/AttackParam.h"
#include "../Player.h"

#include "PlayerAttackState.h"

void PlayerAttackState::Enter(Player& owner)
{
	owner.StartAttack(ATTACK_ID::SLASH_1);
}

PLAYER_STATE PlayerAttackState::Update(Player& owner)
{
	if (owner.GetAnimationController()->IsEnd())
	{
		return PLAYER_STATE::IDLE;
	}

	// ‰½‚à‚È‚¯‚ê‚ÎAØ‚è‘Ö‚¦‚È‚¢
	return PLAYER_STATE::NONE;
}

void PlayerAttackState::Exit(Player& owner)
{
	owner.EndAttack();
}