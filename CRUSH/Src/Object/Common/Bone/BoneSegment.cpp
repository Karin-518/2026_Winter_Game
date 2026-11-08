#include "BoneSegment.h"

BoneSegment::BoneSegment(void)
	:
	modelId_(-1),
	startFrameIndex_(-1),
	endFrameIndex_(-1),
	startPos_({}),
	endPos_({})
{
}

BoneSegment::~BoneSegment(void)
{
}

bool BoneSegment::Init(int modelId, const TCHAR* startFrameName, const TCHAR* endFrameName)
{
	modelId_ = modelId;
	startFrameIndex_ = MV1SearchFrame(modelId, startFrameName);
	endFrameIndex_ = MV1SearchFrame(modelId, endFrameName);

	return IsValid();
}

void BoneSegment::Update(void)
{
	// フレームが見つかっていなければ何もしない
	if (!IsValid()) return;

	startPos_ = MV1GetFramePosition(modelId_, startFrameIndex_);
	endPos_ = MV1GetFramePosition(modelId_, endFrameIndex_);
}

bool BoneSegment::IsValid(void) const
{
	// フレームが見つからないならfalseを返す
	if (startFrameIndex_ == -1) return false;
	if (endFrameIndex_ == -1) return false;

	return true;
}
