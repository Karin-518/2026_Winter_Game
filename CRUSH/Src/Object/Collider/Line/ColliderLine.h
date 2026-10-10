#pragma once
#include "../ColliderBase.h"

class ColliderModel;

// 線分の当たり判定(足元の地面探しに使う)
class ColliderLine : public ColliderBase
{
public:

	// コンストラクタ
	// localStart / localEnd：持ち主の位置から見た、線分の始点と終点
	ColliderLine(TAG tag, const VECTOR* followPos, const VECTOR& localStart, const VECTOR& localEnd);

	// デストラクタ
	~ColliderLine(void) override;

	// デバッグ描画
	void Draw(void) const override;

	// ワールド座標での始点
	VECTOR GetPosStart(void) const;

	// ワールド座標での終点
	VECTOR GetPosEnd(void) const;

	// 足元の地面を探して、見つかったら pos の高さを地面に合わせる
	// 地面が見つかったら true
	bool SnapToGround(const ColliderModel& model, VECTOR& pos) const;

private:

	// 線分の始点(ローカル座標)
	VECTOR localStart_;

	// 線分の終点(ローカル座標)
	VECTOR localEnd_;
};