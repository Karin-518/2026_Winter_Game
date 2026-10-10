#include "ColliderBase.h"

ColliderBase::ColliderBase(SHAPE shape, TAG tag, const VECTOR* followPos)
	:
	shape_(shape),
	tag_(tag),
	followPos_(followPos)
{
}

ColliderBase::~ColliderBase(void)
{
}

void ColliderBase::Draw(void) const
{
}

VECTOR ColliderBase::GetRotPos(const VECTOR& localPos) const
{
	// 追従相手の回転に合わせて指定ローカル座標を回転し、
	// 基準座標に加えることでワールド座標へ変換
	if (followPos_ == nullptr) return localPos;

	return VAdd(*followPos_, localPos);
}

