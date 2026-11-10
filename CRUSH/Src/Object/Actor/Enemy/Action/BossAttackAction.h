#pragma once
#include <DxLib.h>
#include "../../../../Audio/SoundInfo.h"

class Boss;

class BossAttackAction
{
public:

	// コンストラクタ
	BossAttackAction(void) = default;

	// デストラクタ
	virtual ~BossAttackAction(void) = default;

	// 溜めに入った時
	virtual void OnWindupStart(Boss& owner) {}

	// 溜め中
	virtual void UpdateWindup(Boss& owner) {}

	// 攻撃本体に入った時
	virtual void OnActiveStart(Boss& owner) {}

	// 攻撃本体の間
	virtual bool UpdateActive(Boss& owner) = 0;

	// 予備動作の音
	virtual SOUND_ID GetSignSoundID(void) const = 0;
	
	// 攻撃本体の音
	virtual SOUND_ID GetAttackSoundID(void) const = 0;
};