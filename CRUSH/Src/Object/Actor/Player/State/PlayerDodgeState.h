#pragma once
#include "../../../Common/State/StateBase.h"
#include "PlayerState.h"

class Player;

class PlayerDodgeState : public StateBase<Player, PLAYER_STATE>
{
public:

	void Enter(Player& owner) override;

	PLAYER_STATE Update(Player& owner) override;

private:

	// 回避を始めてからの経過フレーム
	int frame_ = 0;
};