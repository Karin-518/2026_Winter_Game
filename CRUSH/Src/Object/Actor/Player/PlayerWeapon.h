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

	// 刃の根元（ワールド座標）
	const VECTOR& GetBladeRootPos(void) const { return bladeRootPos_; }

	// 刃の先端（ワールド座標）
	const VECTOR& GetBladeTipPos(void) const { return bladeTipPos_; }

	// 刃のデバッグ描画
	void DrawDebug(void) const;

private:

	// 武器のモデル
	int modelId_;

	// 持ち主のモデル
	int ownerModelId_;

	// 手のフレーム番号
	int handFrameIndex_;

	// 刃の根元（ワールド座標）
	VECTOR bladeRootPos_;

	// 刃の先端（ワールド座標）
	VECTOR bladeTipPos_;

	// 手から見た、武器の大きさ・回転・位置のずれ
	MATRIX CalcOffsetMatrix(void) const;
};