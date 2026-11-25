#pragma once

// ボスの攻撃の種類
enum class BOSS_ATTACK_ID
{
	NONE = -1,	// 届く攻撃がない

	BITE,		// 噛みつき（近距離）
	RUSH,		// 突進（中～遠距離）
	TAIL_RIGHT,	// 右尻尾なぎ倒し
	TAIL_LEFT,	// 左尻尾なぎ倒し

	MAX
};