#pragma once
#include <vector>
#include "AttackHitVolume.h"
#include "HitReaction.h"

// アニメーションを指定しないときの値
constexpr int NO_ANIM = -1;

// 攻撃ごとのデータ
struct AttackParam
{
	// ダメージ量
	int damage = 0;

	// 攻撃本体のアニメーション
	int animType = 0;

	// 溜めアニメーション
	int windupAnimType = NO_ANIM;

	// 隙アニメーション
	int recoveryAnimType = NO_ANIM;

	// trueなら、アニメの区間を見ずにSetActiveで判定を出す
	bool isManualActive = false;

	// 溜めの長さ
	int windupFrame = 0;

	// 溜めの中のアニメの速さ
	float windupAnimRate = 1.0f;

	// 攻撃後の隙
	int recoveryFrame = 0;

	// CAPSULEの時の判定リスト
	std::vector<AttackHitVolume> hitVolumes;

	// 当たった時の反応
	HIT_REACTION reaction = HIT_REACTION::FLINCH;

	// 吹き飛ばす速さ
	float knockbackSpeed = 0.0f;

	// 横に飛ばす割合
	float knockbackSideRate = 0.0f;
};