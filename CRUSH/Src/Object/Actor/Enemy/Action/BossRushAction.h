#pragma once
#include "BossAttackAction.h"

class BossRushAction : public BossAttackAction
{
public:

	// コンストラクタ
	BossRushAction(void);

	// デストラクタ
	~BossRushAction(void) override;

	// 溜めに入った時
	void OnWindupStart(Boss& owner) override;

	// 溜め中
	void UpdateWindup(Boss& owner) override;

	// 攻撃本体に入ったとき
	void OnActiveStart(Boss& owner) override;

	// 攻撃本体の間
	bool UpdateActive(Boss& owner) override;

	SOUND_ID GetSignSoundID(void) const override;
	SOUND_ID GetAttackSoundID(void) const override;

private:

	// 走る予定の距離
	float chargeDistance_;

	// 走った距離
	float movedDistance_;

	// 走り始めてからのフレーム数
	int chargeFrame_;
};