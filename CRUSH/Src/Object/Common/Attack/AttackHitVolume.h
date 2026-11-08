#pragma once
#include <DxLib.h>

// 攻撃の判定
struct AttackHitVolume
{
	// 始点のボーン名
	const TCHAR* startFrameName = nullptr;

	// 終点のボーン名
	const TCHAR* endFrameName = nullptr;

	// カプセルの半径
	float radius = 0.0f;

	// 判定が出始めるアニメの時間
	float activeStart = 0.0f;

	// 判定が消えるアニメの時間
	float activeEnd = 0.0f;
};