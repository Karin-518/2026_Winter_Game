#pragma once
#include "../../../Common/State/StateBase.h"
#include "BossState.h"

class Boss;

class BossIdleState : public StateBase<Boss, BOSS_STATE>
{
public:

	void Enter(Boss& owner) override;

	BOSS_STATE Update(Boss& owner) override;
};