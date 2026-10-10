#pragma once
#include "../ColliderBase.h"

class ColliderModel;

// カプセルの当たり判定(体の壁判定に使う)
class ColliderCapsule : public ColliderBase
{
public:

	// コンストラクタ
	// localTop / localDown：持ち主の位置から見た、カプセルの上端・下端の球の中心
	ColliderCapsule(
		TAG tag, const VECTOR* followPos,
		const VECTOR& localTop, const VECTOR& localDown, float radius);

	// デストラクタ
	~ColliderCapsule(void) override;

	// デバッグ描画
	void Draw(void) const override;

	// ワールド座標での上端
	VECTOR GetPosTop(void) const;

	// ワールド座標での下端
	VECTOR GetPosDown(void) const;

	// 半径
	float GetRadius(void) const { return radius_; }

	// 壁から押し出す
	void PushBackFromModel(const ColliderModel& model, VECTOR& pos) const;

private:

	// 上端の球の中心(ローカル座標)
	VECTOR localTop_;

	// 下端の球の中心(ローカル座標)
	VECTOR localDown_;

	// 半径
	float radius_;

	// ポリゴン1枚から、当たらなくなるまで法線方向へ押し出した位置を返す
	VECTOR PushBackFromPolygon(const MV1_COLL_RESULT_POLY& poly, const VECTOR& pos) const;

	// pos にいるとしたら、ポリゴン1枚と当たっているか
	bool IsHitPolygon(const MV1_COLL_RESULT_POLY& poly, const VECTOR& pos) const;
};