#pragma once
#include <DxLib.h>
#include "../Common/Bone/BoneSegment.h"

// 押し出し用カプセルの設定
struct PushboxParam
{
	const TCHAR* startFrameName_;	// 線分の始点のフレーム名
	const TCHAR* endFrameName_;		// 線分の終点のフレーム名
	float radius_;					// 半径
};

class Pushbox
{
public:

	// コンストラクタ
	Pushbox(int modelId, const PushboxParam& param);

	// デストラクタ
	~Pushbox(void);

	// 更新
	void Update(void);

	// 押し出しに使っていいか
	bool IsReady(void) const { return isReady_; }

	// 線分の始点
	const VECTOR& GetStartPos(void) const { return boneSegment_.GetStartPos(); }

	// 線分の終点
	const VECTOR& GetEndPos(void) const { return boneSegment_.GetEndPos(); }

	// 半径
	float GetRadius(void) const { return radius_; }

	// デバッグ描画
	void DrawDebug(void) const;

private:

	// ボーン
	BoneSegment boneSegment_;

	// 半径
	float radius_;

	// 押し出しに使っていいか
	bool isReady_;
};