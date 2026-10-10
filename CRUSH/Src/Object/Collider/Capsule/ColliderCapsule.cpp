#include "../Model/ColliderModel.h"
#include "ColliderCapsule.h"

namespace
{
	// 全フレームを判定対象にする指定
	constexpr int ALL_FRAME = -1;

	// ポリゴン1枚あたり押し出しを試す最大回数
	constexpr int PUSH_TRY_MAX = 10;

	// 1回の押し出し量
	constexpr float PUSH_DIS = 1.0f;

	// デバッグ表示の分割数
	constexpr int DEBUG_DIV_NUM = 8;
}

ColliderCapsule::ColliderCapsule(
	TAG tag, const VECTOR* followPos,
	const VECTOR& localTop, const VECTOR& localDown, float radius)
	:
	ColliderBase(SHAPE::CAPSULE, tag, followPos),
	localTop_(localTop),
	localDown_(localDown),
	radius_(radius)
{
}

ColliderCapsule::~ColliderCapsule(void)
{
}

void ColliderCapsule::Draw(void) const
{
	DrawCapsule3D(GetPosTop(), GetPosDown(), radius_, DEBUG_DIV_NUM, DEBUG_COLOR, DEBUG_COLOR, FALSE);
}

VECTOR ColliderCapsule::GetPosTop(void) const
{
	return GetRotPos(localTop_);
}

VECTOR ColliderCapsule::GetPosDown(void) const
{
	return GetRotPos(localDown_);
}

void ColliderCapsule::PushBackFromModel(const ColliderModel& stage, VECTOR& pos) const
{
	// カプセルと当たっているポリゴンを全て集める
	MV1_COLL_RESULT_POLY_DIM hits = MV1CollCheck_Capsule(
		stage.GetModelId(), ALL_FRAME,
		VAdd(pos, localTop_), VAdd(pos, localDown_), radius_);

	// ポリゴン1枚ずつ押し出す
	for (int i = 0; i < hits.HitNum; i++)
	{
		pos = PushBackFromPolygon(hits.Dim[i], pos);
	}

	// 検出したポリゴン情報の後始末
	MV1CollResultPolyDimTerminate(hits);
}

VECTOR ColliderCapsule::PushBackFromPolygon(const MV1_COLL_RESULT_POLY& poly, const VECTOR& pos) const
{
	VECTOR ret = pos;

	for (int tryCnt = 0; tryCnt < PUSH_TRY_MAX; tryCnt++)
	{
		// 当たらなくなったら終わり
		if (!IsHitPolygon(poly, ret)) break;

		// ポリゴンの法線方向へ少し押し出す
		ret = VAdd(ret, VScale(poly.Normal, PUSH_DIS));
	}

	return ret;
}

bool ColliderCapsule::IsHitPolygon(const MV1_COLL_RESULT_POLY& poly, const VECTOR& pos) const
{
	// GetPosTop() ではなく、引数の pos(押し出し途中の位置)で判定する
	return HitCheck_Capsule_Triangle(
		VAdd(pos, localTop_), VAdd(pos, localDown_), radius_,
		poly.Position[0], poly.Position[1], poly.Position[2]) != FALSE;
}