#pragma once

#include<string>
#include<vector>
#include "../ColliderBase.h"

class ColliderModel : public ColliderBase
{
public:

	// コンストラクタ
	ColliderModel(TAG tag, int modelId);

	// デストラクタ
	~ColliderModel(void) override;

	// モデルのハンドルID
	int GetModelId(void) const { return modelId_; }

	// 線分とモデルの衝突のうち、start に一番近いものを返す
	// 当たらなければ HitFlag が 0 のものを返す
	MV1_COLL_RESULT_POLY GetNearestHitPolyLine(
		const VECTOR& start, 
		const VECTOR& end) const;

private:
	
	// モデルのハンドルID
	int modelId_;
};