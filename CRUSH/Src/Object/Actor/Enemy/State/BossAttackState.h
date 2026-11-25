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

	// 攻撃の段階
	enum class ATTACK_PHASE
	{
		WINDUP,		// 溜め
		ACTIVE,		// 攻撃本体
		RECOVERY,	// 攻撃後の隙
	};

	// 今の段階
	ATTACK_PHASE phase_ = ATTACK_PHASE::WINDUP;

	// 今の段階に入ってからのフレーム数
	int phaseFrame_ = 0;

	// 今の攻撃の動き
	BossAttackAction* action_ = nullptr;

	// 溜め中
	void UpdateWindup(Boss& owner);

	// 攻撃本体の間
	void UpdateActive(Boss& owner);

	// 隙の間
	BOSS_STATE UpdateRecovery(Boss& owner);

	// 段階を切り替える
	void ChangePhase(ATTACK_PHASE nextPhase);

	// 溜め専用のアニメがあったときだけ、本体のアニメに切り替える
	void PlayActiveAnim(Boss& owner);

	// 隙のアニメを再生する
	void PlayRecoveryAnim(Boss& owner);
};