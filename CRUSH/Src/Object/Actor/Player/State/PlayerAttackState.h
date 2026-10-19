#pragma once
#include "../../../Common/State/StateBase.h"
#include "PlayerState.h"

class Player;

class PlayerAttackState : public StateBase<Player, PLAYER_STATE>
{
public:

	void Enter(Player& owner) override;

	PLAYER_STATE Update(Player& owner) override;
};