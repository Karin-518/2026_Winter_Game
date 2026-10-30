#include "ColliderPart.h"

ColliderPart::ColliderPart(int modelId, const BossPartParam& param)
	:
	modelId_(modelId),
	startFrameIndex_(MV1SearchFrame(modelId, param.startFrameName_)),
	endFrameIndex_(MV1SearchFrame(modelId, param.endFrameName_)),
	radius_(param.radius_),
	part_(param.part_),
	damageMultiplier_(param.damageMultiplier_),
	startPos_({}),
	endPos_({})
{
}

ColliderPart::~ColliderPart(void)
{
}

void ColliderPart::Update(void)
{
	// フレームが見つかっていない
	if (startFrameIndex_ == -1 || endFrameIndex_ == -1) return;

	startPos_ = MV1GetFramePosition(modelId_, startFrameIndex_);
	endPos_ = MV1GetFramePosition(modelId_, endFrameIndex_);
}

void ColliderPart::Draw(void)
{
	DrawCapsule3D(startPos_, endPos_, radius_, 8, 0xff0000, 0xff0000, FALSE);
}

VECTOR ColliderPart::GetClosestPos(const VECTOR& pos) const
{
	VECTOR seg = VSub(endPos_, startPos_);
	float lenSq = VDot(seg, seg);

	// 始点と終点が同じ
	if (lenSq <= 0.0f) return startPos_;

	// 線分上のどこに一番近いか
	float t = VDot(VSub(pos, startPos_), seg) / lenSq;

	// 始点より手前なら始点
	if (t < 0.0f) return startPos_;

	// 終点より先なら終点
	if (t > 1.0f) return endPos_;

	return VAdd(startPos_, VScale(seg, t));
}