#pragma once
#include <DxLib.h>

// 部位の種類
enum class BOSS_PART
{
	HEAD,
	BODY,
	TAIL,
	WING_L,
	WING_R,
};

// ボスの部位種類のパラメータ
struct BossPartParam
{
	BOSS_PART part_;				// 部位
	const TCHAR* startFrameName_;	// 開始フレーム名
	const TCHAR* endFrameName_;		// 終了フレーム名
	float radius_;					// 半径
	float damageMultiplier_;		// ダメージ倍率
};