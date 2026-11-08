#pragma once
#include <DxLib.h>
#include "ColliderBase.h"

class ActorBase;

class Hurtbox
{
public:

	// コンストラクタ
	Hurtbox(ColliderBase::TAG tag, ActorBase* owner, float radius);

	// デストラクタ
	virtual ~Hurtbox(void);

	// 更新
	virtual void Update(void) = 0;

	// 線分上で指定位置に一番近い点
	VECTOR GetClosestPos(const VECTOR& pos) const;

	// 半径
	float GetRadius(void) const { return radius_; }

	// 線分の始点
	const VECTOR& GetStartPos(void) const { return startPos_; }

	// 線分の終点
	const VECTOR& GetEndPos(void) const { return endPos_; }

	// 衝突種別
	ColliderBase::TAG GetTag(void) const { return tag_; }

	// 持ち主
	ActorBase* GetOwner(void) const { return owner_; }

	// デバッグ描画
	void DrawDebug(void) const;

protected:

	// 両端の設定
	void SetSegment(const VECTOR& startPos, const VECTOR& endPos);

private:

	// 衝突種別
	ColliderBase::TAG tag_;

	// 持ち主
	ActorBase* owner_;

	// 半径
	float radius_;

	// 線分の始点
	VECTOR startPos_;

	// 線分の終点
	VECTOR endPos_;
};