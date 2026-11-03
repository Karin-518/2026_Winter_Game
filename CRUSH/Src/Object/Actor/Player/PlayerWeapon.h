#pragma once
#include <DxLib.h>

class PlayerWeapon
{
public:

	// コンストラクタ
	PlayerWeapon(void);

	// デストラクタ
	~PlayerWeapon(void);

	// 初期化
	void Init(int ownerModelId, const TCHAR* handFrameName);

	// 更新
	void Update(void);

	// 描画
	void Draw(void) const;

private:

	// 武器のモデル
	int modelId_;

	// 持ち主のモデル
	int ownerModelId_;

	// 手のフレーム番号
	int handFrameIndex_;

	// 手から見た、武器の大きさ・回転・位置のずれ
	MATRIX CalcOffsetMatrix(void) const;
};