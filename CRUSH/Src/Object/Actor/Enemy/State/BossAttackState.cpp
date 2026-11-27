#include "../../../../Audio/AudioManager.h"
#include "../../../../Effect/EffectManager.h"
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
	PlayActiveAnim(owner);
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

	// 隙のアニメを再生する
	PlayRecoveryAnim(owner);

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

	if (phase_ == ATTACK_PHASE::WINDUP)
	{
		AudioManager::GetInstance()->PlaySE(action_->GetSignSoundID());
		return;
	}

	if (phase_ == ATTACK_PHASE::ACTIVE)
	{
		AudioManager::GetInstance()->PlaySE(action_->GetAttackSoundID());
		return;
	}
}

void BossAttackState::PlayActiveAnim(Boss& owner)
{
	const AttackParam& param = owner.GetCurrentAttackParam();

	// 溜め専用のアニメがなければ、本体のアニメが溜めから流れているので何もしない
	if (param.windupAnimType == NO_ANIM) return;

	// 本体のアニメに切り替える
	AnimationController::PlayParam playParam;
	playParam.isLoop = false;
	playParam.blendFrame = 10;
	playParam.isForce = true;
	owner.GetAnimationController()->Play(param.animType, playParam);
}

void BossAttackState::PlayRecoveryAnim(Boss& owner)
{
	const AttackParam& param = owner.GetCurrentAttackParam();

	// 隙専用のアニメがなければ、待機アニメをループで流す
	int recoveryAnim = static_cast<int>(Boss::ANIM_TYPE::IDLE);
	bool isLoop = true;

	// 隙専用のアニメがあれば、それを1回だけ流す
	if (param.recoveryAnimType != NO_ANIM)
	{
		recoveryAnim = param.recoveryAnimType;
		isLoop = false;
	}

	AnimationController::PlayParam playParam;
	playParam.isLoop = isLoop;
	playParam.isForce = true;
	owner.GetAnimationController()->Play(recoveryAnim, playParam);
}