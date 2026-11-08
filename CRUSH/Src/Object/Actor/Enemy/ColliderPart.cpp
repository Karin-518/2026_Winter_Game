#include "ColliderPart.h"

ColliderPart::ColliderPart(int modelId, const BossPartParam& param, ActorBase* owner)
	:
	Hurtbox(ColliderBase::TAG::ENEMY, owner, param.radius_),
	part_(param.part_),
	boneSegment_(),
	damageMultiplier_(param.damageMultiplier_)
{
	// ボーンを探す
	bool isFound = boneSegment_.Init(modelId, param.startFrameName_, param.endFrameName_);
	
	// 見つからなければ名前の間違いを知らせる
	if (!isFound)
	{
		printfDx("部位のボーンが見つかりません：%s / %s\n",
			param.startFrameName_, param.endFrameName_);
	}
}

ColliderPart::~ColliderPart(void)
{
}

void ColliderPart::Update(void)
{
	boneSegment_.Update();

	// フレームが見つかっていない
	if (!boneSegment_.IsValid()) return;

	SetSegment(boneSegment_.GetStartPos(), boneSegment_.GetEndPos());
}