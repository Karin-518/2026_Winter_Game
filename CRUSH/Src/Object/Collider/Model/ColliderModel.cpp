#include "ColliderModel.h"

namespace
{
	// 全フレームを判定対象にする指定
	constexpr int ALL_FRAME = -1;
}

ColliderModel::ColliderModel(TAG tag, int modelId)
	:
	ColliderBase(SHAPE::MODEL, tag, nullptr),
	modelId_(modelId)
{
}

ColliderModel::~ColliderModel(void)
{
}

MV1_COLL_RESULT_POLY ColliderModel::GetNearestHitPolyLine(const VECTOR& start, const VECTOR& end) const
{
	// 線分に当たったポリゴンのうち、始点に一番近いものを返す
	return MV1CollCheck_Line(modelId_, ALL_FRAME, start, end);
}