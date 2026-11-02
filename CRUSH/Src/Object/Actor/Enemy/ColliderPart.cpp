#include "ColliderPart.h"

ColliderPart::ColliderPart(int modelId, const BossPartParam& param, ActorBase* owner)
	:
	Hurtbox(ColliderBase::TAG::ENEMY, owner, param.radius_),
	modelId_(modelId),
	startFrameIndex_(MV1SearchFrame(modelId, param.startFrameName_)),
	endFrameIndex_(MV1SearchFrame(modelId, param.endFrameName_)),
	part_(param.part_),
	damageMultiplier_(param.damageMultiplier_)
{
}

ColliderPart::~ColliderPart(void)
{
}

void ColliderPart::Update(void)
{
	// ƒtƒŒ[ƒ€‚ªŒ©‚Â‚©‚Á‚Ä‚¢‚È‚¢
	if (startFrameIndex_ == -1 || endFrameIndex_ == -1) return;

	VECTOR startFramePos = MV1GetFramePosition(modelId_, startFrameIndex_);
	VECTOR endFramePos = MV1GetFramePosition(modelId_, endFrameIndex_);

	SetSegment(startFramePos, endFramePos);
}