#pragma once
#include "BossAttackAction.h"

class BossLeapBiteAction : public BossAttackAction
{
public:

	// コンストラクタ
	BossLeapBiteAction(void);

	// デストラクタ
	~BossLeapBiteAction(void) override;

	// 溜め中
	void UpdateWindup(Boss& owner) override;

	// 攻撃本体に入った時
	void OnActiveStart(Boss& owner) override;

	// 攻撃本体の間、舞フレーム呼ばれる
	bool UpdateActive(Boss& owner) override;

	SOUND_ID GetSignSoundID(void) const override;
	SOUND_ID GetAttackSoundID(void) const override;

private:

	// 飛ぶ予定の距離
	float leapDistance_;

	// 進んだ距離
	float movedDistance_;

	// 移動処理
	void UpdateLeapMove(Boss& owner);
};