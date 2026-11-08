#include "ColliderPart.h"

ColliderPart::ColliderPart(int modelId, const BossPartParam& param, ActorBase* owner)
	:
	Hurtbox(ColliderBase::TAG::ENEMY, owner, param.radius_),
	part_(param.part_),
	boneSegment_(),
	damageMultiplier_(param.damageMultiplier_)
{
	boneSegment_.Init(modelId, param.startFrameName_, param.endFrameName_);
}

ColliderPart::~ColliderPart(void)
{
}

void ColliderPart::Update(void)
{
	boneSegment_.Update();

	// ƒtƒŒ[ƒ€‚ªŒ©‚Â‚©‚Á‚Ä‚¢‚È‚¢
	if (!boneSegment_.IsValid()) return;

	SetSegment(boneSegment_.GetStartPos(), boneSegment_.GetEndPos());
}