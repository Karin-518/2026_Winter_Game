#pragma once

// ボスの攻撃の種類
enum class BOSS_ATTACK_ID
{
	NONE = -1,	// 届く攻撃がない

	BITE,		// 噛みつき（近距離）
	RUSH,		// 突進（中～遠距離）

	MAX
};