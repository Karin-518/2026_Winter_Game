#include "Hurtbox.h"

namespace
{
	// デバッグ描画の色
	constexpr unsigned int DEBUG_COLOR = 0x00ff00;

	// デバッグ描画のカプセルの分割数
	constexpr int DEBUG_DIV_NUM = 8;
}

Hurtbox::Hurtbox(ColliderBase::TAG tag, ActorBase* owner, float radius)
	:
	tag_(tag),
	owner_(owner),
	radius_(radius),
	startPos_({}),
	endPos_({})
{
}

Hurtbox::~Hurtbox(void)
{
}

VECTOR Hurtbox::GetClosestPos(const VECTOR& pos) const
{
	// 始点から終点へのベクトル
	VECTOR segmentDir = VSub(endPos_, startPos_);

	// 線分の長さの2乗
	float lenSq = VDot(segmentDir, segmentDir);

	// 始点と終点が同じ
	if (lenSq <= 0.0f) return startPos_;

	// posから線分に垂線を下ろした点が、始点から何割進んだ位置か
	float rateOnSegment = VDot(VSub(pos, startPos_), segmentDir) / lenSq;

	// 始点より手前なら始点
	if (rateOnSegment < 0.0f) return startPos_;

	// 終点より先なら終点
	if (rateOnSegment > 1.0f) return endPos_;

	return VAdd(startPos_, VScale(segmentDir, rateOnSegment));
}

void Hurtbox::DrawDebug(void) const
{
	DrawCapsule3D(startPos_, endPos_, radius_, DEBUG_DIV_NUM, DEBUG_COLOR, DEBUG_COLOR, false);
}

void Hurtbox::SetSegment(const VECTOR& startPos, const VECTOR& endPos)
{
	startPos_ = startPos;
	endPos_ = endPos;
}