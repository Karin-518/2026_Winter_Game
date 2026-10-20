#pragma once

// 攻撃の種類
enum class ATTACK_ID
{
	SHASH_1,
	SHASH_2,

	MAX
};

// 判定の形
enum class HIT_SHAPE
{
	RECT,		// 四角：縦振り・突き
	FAN,		// 扇　：横振り
};

// 攻撃ごとのデータ
struct AttackParam
{
	// 攻撃の種類
	ATTACK_ID attackId = ATTACK_ID::SHASH_1;

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
};