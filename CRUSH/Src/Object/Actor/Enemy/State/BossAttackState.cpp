#include "../../../Common/AnimationController/AnimationController.h"
#include "../Action/BossAttackAction.h"
#include "../Boss.h"

#include "BossAttackState.h"

void BossAttackState::Enter(Boss& owner)
{
    // 向く・攻撃アニメ再生・判定の準備
   owner.StartAttack();
   action_ = owner.GetCurrentAction();

   // 溜めの間だけアニメの速さを変更
   const AttackParam& param = owner.GetCurrentAttackParam();
   owner.GetAnimationController()->ChangeSpeedForFrame(param.windupAnimRate, param.windupFrame);

   action_->OnWindupStart(owner);
   ChangePhase(ATTACK_PHASE::WINDUP);
}

BOSS_STATE BossAttackState::Update(Boss& owner)
{
	phaseFrame_++;

	switch (phase_)
	{
	case ATTACK_PHASE::WINDUP:
		UpdateWindup(owner);
		return BOSS_STATE::NONE;

	case ATTACK_PHASE::ACTIVE:
		UpdateActive(owner);
		return BOSS_STATE::NONE;

	default:
		return UpdateRecovery(owner);
	}
}

void BossAttackState::Exit(Boss& owner)
{
    owner.EndAttack();
}

void BossAttackState::UpdateWindup(Boss& owner)
{
	action_->UpdateWindup(owner);

	// 溜めが終わっていなければ待つ
	if (phaseFrame_ < owner.GetCurrentAttackParam().windupFrame) return;

	// 攻撃本体へ
	owner.SetAttackActive(true);
	action_->OnActiveStart(owner);
	ChangePhase(ATTACK_PHASE::ACTIVE);
}

void BossAttackState::UpdateActive(Boss& owner)
{
	// 攻撃本体が終わっていなければ続ける
	if (!action_->UpdateActive(owner)) return;

	// 隙へ
	owner.SetAttackActive(false);
	owner.EndAttack();
	owner.GetAnimationController()->Play(static_cast<int>(Boss::ANIM_TYPE::IDLE));
	ChangePhase(ATTACK_PHASE::RECOVERY);
}

BOSS_STATE BossAttackState::UpdateRecovery(Boss& owner)
{
	// 隙が終わっていなければ待つ
	if (phaseFrame_ < owner.GetCurrentAttackParam().recoveryFrame) return BOSS_STATE::NONE;

	return BOSS_STATE::IDLE;
}

void BossAttackState::ChangePhase(ATTACK_PHASE nextPhase)
{
	phase_ = nextPhase;
	phaseFrame_ = 0;
}