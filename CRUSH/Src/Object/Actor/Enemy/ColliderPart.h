#pragma once
#include <DxLib.h>
#include "BossPart.h"

class ColliderPart
{
public:

	// コンストラクタ
	ColliderPart(int modelId, const BossPartParam& param);

	// デストラクタ
	~ColliderPart(void);

	void Update(void);	// 更新
	void Draw(void);	// 描画

	// 線分上で指定位置に近い点を取得
	VECTOR GetClosestPos(const VECTOR& pos) const;

	// 半径を取得
	float GetRadius(void) const { return radius_; }

	// 部位を取得
	BOSS_PART GetPart(void) const { return part_; }

	// ダメージ倍率を取得
	float GetDamageMultiplier(void) const { return damageMultiplier_; }

private:

	// モデルのハンドル
	int modelId_;

	// 開始フレームの番号
	int startFrameIndex_;

	// 終了フレームの番号
	int endFrameIndex_;

	// 半径
	float radius_;

	// 部位
	BOSS_PART part_;

	// ダメージ倍率
	float damageMultiplier_;

	// 現在のカプセルの両端
	VECTOR startPos_;
	VECTOR endPos_;
};