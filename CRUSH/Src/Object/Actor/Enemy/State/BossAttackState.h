#pragma once
#include "../../../Common/State/StateBase.h"
#include "BossState.h"

class Boss;
class BossAttackAction;

class BossAttackState : public StateBase<Boss, BOSS_STATE>
{
public:

	void Enter(Boss& owner) override;

	BOSS_STATE Update(Boss& owner) override;

	void Exit(Boss& owner) override;

private:

	// UŒ‚‚Ì’iŠK
	enum class ATTACK_PHASE
	{
		WINDUP,		// —­‚ß
		ACTIVE,		// UŒ‚–{‘Ì
		RECOVERY,	// UŒ‚Œã‚ÌŒ„
	};

	// ¡‚Ì’iŠK
	ATTACK_PHASE phase_ = ATTACK_PHASE::WINDUP;

	// ¡‚Ì’iŠK‚É“ü‚Á‚Ä‚©‚ç‚ÌƒtƒŒ[ƒ€”
	int phaseFrame_ = 0;

	// ¡‚ÌUŒ‚‚Ì“®‚«
	BossAttackAction* action_ = nullptr;

	// —­‚ß’†
	void UpdateWindup(Boss& owner);

	// UŒ‚–{‘Ì‚ÌŠÔ
	void UpdateActive(Boss& owner);

	// Œ„‚ÌŠÔ
	BOSS_STATE UpdateRecovery(Boss& owner);

	// ’iŠK‚ğØ‚è‘Ö‚¦‚é
	void ChangePhase(ATTACK_PHASE nextPhase);
};