#pragma once
#include <DxLib.h>

class BoneSegment
{
public:

	// コンストラクタ
	BoneSegment(void);

	// デストラクタ
	~BoneSegment(void);

	// 初期化
	bool Init(int modelId, const TCHAR* startFrameName, const TCHAR* endFrameName);

	// 更新
	void Update(void);

	// 両方のフレームが見つかっているか
	bool IsValid(void) const;

	// 線分の両端
	const VECTOR& GetStartPos(void) const { return startPos_; }
	const VECTOR& GetEndPos(void) const { return endPos_; }

private:

	// モデルのハンドル
	int modelId_;

	// 始点・終点のフレーム番号
	int startFrameIndex_;
	int endFrameIndex_;

	// 始点・終点の位置
	VECTOR startPos_;
	VECTOR endPos_;
};