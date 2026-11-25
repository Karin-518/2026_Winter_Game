#pragma once

// ボスの攻撃の種類
enum class BOSS_ATTACK_ID
{
	NONE = -1,	// 届く攻撃がない

	BITE,		// 噛みつき
	RUSH,		// 突進
	TAIL_RIGHT,	// 右尻尾なぎ倒し
	TAIL_LEFT,	// 左尻尾なぎ倒し
	FIRE_BALL,	// 単発ブレス

	MAX
};