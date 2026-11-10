#pragma once
#include "../../../Common/State/StateBase.h"
#include "PlayerState.h"

class Player;

class PlayerHitState : public StateBase<Player, PLAYER_STATE>
{
	void Enter(Player& owner) override;

	PLAYER_STATE Update(Player& owner) override;

	void Exit(Player& owner) override;
};