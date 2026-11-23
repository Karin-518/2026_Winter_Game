#include "Pushbox.h"

namespace
{
	// デバッグ描画の色（食らい判定の緑と見分けるため青）
	constexpr unsigned int DEBUG_COLOR = 0x4080ff;

	// デバッグ描画のカプセルの分割数
	constexpr int DEBUG_DIV_NUM = 8;
}

Pushbox::Pushbox(int modelId, const PushboxParam& param)
	:
	boneSegment_(),
	radius_(param.radius_),
	isReady_(false)
{
	boneSegment_.Init(modelId, param.startFrameName_, param.endFrameName_);
}

Pushbox::~Pushbox(void)
{
}

void Pushbox::Update(void)
{
	// フレームが見つかっていなければ、押し出しに使わない
	if (!boneSegment_.IsValid()) return;

	boneSegment_.Update();
	isReady_ = true;
}

void Pushbox::DrawDebug(void) const
{
	if (!isReady_) return;

	DrawCapsule3D(GetStartPos(), GetEndPos(), radius_, DEBUG_DIV_NUM, DEBUG_COLOR, DEBUG_COLOR, false);
}