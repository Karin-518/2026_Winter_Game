#pragma once
#include <vector>
#include "AttackHitVolume.h"
#include "HitReaction.h"

// 判定の形
enum class HIT_SHAPE
{
	RECT,		// 四角：縦振り・突き
	FAN,		// 扇　：横振り
	CAPSULE,	// ボーンに付くカプセル
};

// 攻撃ごとのデータ
struct AttackParam
{
	// ダメージ量
	int damage = 0;

	// 再生するアニメーション
	int animType = 0;

	// 判定が出る区間
	float hitStart = 0.0f;
	float hitEnd = 0.0f;

	// 判定の形
	HIT_SHAPE shape = HIT_SHAPE::RECT;

	// 体の中心から前へずらす距離
	float offset = 0.0f;

	// 長さ
	float length = 0.0f;

	// 幅
	float width = 0.0f;

	// 半分の角度
	float halfAngleDeg = 0.0f;

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
};