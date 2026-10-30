#include "../Model/ColliderModel.h"

#include "ColliderLine.h"

ColliderLine::ColliderLine(TAG tag, const VECTOR* followPos, const VECTOR& localStart, const VECTOR& localEnd)
	:
	ColliderBase(SHAPE::LINE, tag, followPos),
	localStart_(localStart),
	localEnd_(localEnd)
{
}

ColliderLine::~ColliderLine(void)
{
}

void ColliderLine::Draw(void) const
{
	DrawLine3D(GetPosStart(), GetPosEnd(), DEBUG_COLOR);
}

VECTOR ColliderLine::GetPosStart(void) const
{
	return GetRotPos(localStart_);
}

VECTOR ColliderLine::GetPosEnd(void) const
{
	return GetRotPos(localEnd_);
}

bool ColliderLine::SnapToGround(const ColliderModel& model, VECTOR& pos) const
{
	const MV1_COLL_RESULT_POLY hit = model.GetNearestHitPolyLine(GetPosStart(), GetPosEnd());

	// ínñ Ç™Ç»Ç¢
	if (!hit.HitFlag) return false;

	// ë´å≥ÇÃçÇÇ≥Çínñ Ç…çáÇÌÇπÇÈ
	pos.y = hit.HitPosition.y;
	return true;
}