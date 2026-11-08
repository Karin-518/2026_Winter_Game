#pragma once
#include "../../Collider/Hurtbox.h"
#include "../../Common/Bone/BoneSegment.h"
#include "BossPart.h"

class ActorBase;

class ColliderPart : public Hurtbox
{
public:

	// コンストラクタ
	ColliderPart(int modelId, const BossPartParam& param, ActorBase* owner);

	// デストラクタ
	~ColliderPart(void) override;

	// 更新
	void Update(void) override;

	// 部位を取得
	BOSS_PART GetPart(void) const { return part_; }

	// ダメージ倍率を取得
	float GetDamageMultiplier(void) const { return damageMultiplier_; }

private:

	// 部位
	BOSS_PART part_;

	// 部位のカプセルの芯になるボーン
	BoneSegment boneSegment_;

	// ダメージ倍率
	float damageMultiplier_;
};