#pragma once
#include <DxLib.h>
#include "HitReaction.h"

// 攻撃が当たった時に当たった側へ渡す情報
struct HitInfo
{
	int damage = 0;									// ダメージ量
	HIT_REACTION reaction = HIT_REACTION::FLINCH;	// 当たった側の反応
	float knockbackSpeed = 0.0f;					// 吹き飛ばす速さ
	VECTOR attackerPos = {};						// 攻撃した側の位置
	VECTOR attackerForward = {};					// 攻撃した側の向き
	float knockbackSideRate = 0.0f;					// 横に飛ばす割合
	VECTOR hitPos = {};								// 当たった位置
};