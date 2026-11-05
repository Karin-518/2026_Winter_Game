#include <DxLib.h>

#include "BossAttackSelector.h"

namespace
{
	// 選ばれたときに、今の重みにかける割合
	constexpr float LOWER_WEIGHT_RATE = 0.5f;

	// 重みの下限（絶対に来ない時間を作らないために下限を決める）
	constexpr float MIN_WEIGHT = 0.0f;

	// 1秒あたりに戻す重み
	constexpr float RECOVER_WEIGHT_PER_SEC = 1.0f;

	// フレームレート
	constexpr float FPS = 60.0f;

	// デバッグ表示の左上
	constexpr int DEBUG_POS_X = 0;
	constexpr int DEBUG_POS_Y = 160;

	// デバッグ表示の1行の高さ
	constexpr int DEBUG_LINE_HEIGHT = 20;

	// デバッグ表示の色
	constexpr unsigned int DEBUG_COLOR = 0xffffff;

	// 最後に選ばれた攻撃の色
	constexpr unsigned int DEBUG_SELECTED_COLOR = 0xffff00;

	// 攻撃の種類を、表示用の名前にする
	const char* ToAttackName(BOSS_ATTACK_ID attackId)
	{
		switch (attackId)
		{
		case BOSS_ATTACK_ID::BITE:	return "噛みつき";
		case BOSS_ATTACK_ID::RUSH:	return "突進";
		case BOSS_ATTACK_ID::NONE:	return "なし";
		default:					return "不明";
		}
	}
}

BossAttackSelector::BossAttackSelector(void)
	:
	candidates_({}),
	randomEngine_(std::random_device()()),
	currentAttackId_(BOSS_ATTACK_ID::NONE)
{
}

BossAttackSelector::~BossAttackSelector(void)
{
}

void BossAttackSelector::Update(void)
{
	for (auto& candidate : candidates_)
	{
		// 1フレーム分の回復量を足す
		candidate.currentWeight += RECOVER_WEIGHT_PER_SEC / FPS;

		// 基本の重みを超えたら、基本の重みにそろえる
		if (candidate.currentWeight > candidate.defaultWeight)
		{
			candidate.currentWeight = candidate.defaultWeight;
		}
	}
}

void BossAttackSelector::DrawDebug(void) const
{
	int lineY = DEBUG_POS_Y;

	// 最後に選ばれた攻撃
	DrawFormatString(DEBUG_POS_X, lineY, DEBUG_SELECTED_COLOR,
		"ボスの攻撃選択：%s", ToAttackName(currentAttackId_));
	lineY += DEBUG_LINE_HEIGHT;

	// 候補ごとの今の重みと、届く距離
	for (const auto& candidate : candidates_)
	{
		// 最後に選ばれた攻撃だけ色を変える
		unsigned int lineColor = DEBUG_COLOR;
		if (candidate.attackId == currentAttackId_)
		{
			lineColor = DEBUG_SELECTED_COLOR;
		}

		DrawFormatString(DEBUG_POS_X, lineY, lineColor,
			"  %s  重み %5.1f / %5.1f  距離 %.0f～%.0f",
			ToAttackName(candidate.attackId),
			candidate.currentWeight,
			candidate.defaultWeight,
			candidate.minDistance,
			candidate.maxDistance);

		lineY += DEBUG_LINE_HEIGHT;
	}
}

void BossAttackSelector::AddCandidate(BOSS_ATTACK_ID attackId, float minDistance, float maxDistance, float weight)
{
	// 候補の登録
	AttackCandidate candidate;
	candidate.attackId = attackId;
	candidate.minDistance = minDistance;
	candidate.maxDistance = maxDistance;
	candidate.defaultWeight = weight;

	// 登録時は選ばれていないので、今の重みは基本の重みと同じ
	candidate.currentWeight = weight;

	candidates_.push_back(candidate);
}

BOSS_ATTACK_ID BossAttackSelector::Select(float distance)
{
	// 攻撃が届く候補を集める
	std::vector<AttackCandidate> reachableCandidates = CollectReachableCandidates(distance);

	// 攻撃を選ぶ
	BOSS_ATTACK_ID pickedAttackId = PickByWeight(reachableCandidates);

	// 選んだ結果を覚えておく
	currentAttackId_ = pickedAttackId;

	// 選べなかったなら、そのまま返す
	if (pickedAttackId == BOSS_ATTACK_ID::NONE) return BOSS_ATTACK_ID::NONE;

	// 選んだ攻撃の重みを下げる
	LowerWeight(pickedAttackId);

	return currentAttackId_;
}

std::vector<BossAttackSelector::AttackCandidate> BossAttackSelector::CollectReachableCandidates(float distance) const
{
	// 届く候補の結果を入れる
	std::vector<AttackCandidate> reachableCandidates;

	for (const auto& candidate : candidates_)
	{
		// 最小値より近い、または最大値より遠いなら届かないので飛ばす
		if (candidate.minDistance > distance) continue;
		if (candidate.maxDistance < distance) continue;

		// 最小値以上、かつ最大値以下なら結果に追加
		reachableCandidates.push_back(candidate);
	}

	return reachableCandidates;
}

BOSS_ATTACK_ID BossAttackSelector::PickByWeight(const std::vector<AttackCandidate>& candidates)
{
	// 届く候補が空ならNONEを返す
	if (candidates.empty()) return BOSS_ATTACK_ID::NONE;

	// 候補の今の重みをすべて足す
	float totalWeight = 0.0f;
	for (const auto& candidate : candidates)
	{
		totalWeight += candidate.currentWeight;
	}

	// 0～重みの合計の範囲で、乱数を1つ引く
	std::uniform_real_distribution<float> weightRange(0.0f, totalWeight);
	float pickedValue = weightRange(randomEngine_);

	// 候補の重みを足した値
	float accumulatedWeight = 0.0f;
	for (const auto& candidate : candidates)
	{
		accumulatedWeight += candidate.currentWeight;

		// 足してきた重みが、引いた値を超えたら、その候補の攻撃種類を返す
		if (accumulatedWeight > pickedValue) return candidate.attackId;
	}

	// 誤差でどれも超えなかったときは、最後の候補を返す
	return candidates.back().attackId;
}

void BossAttackSelector::LowerWeight(BOSS_ATTACK_ID attackId)
{
	for (auto& candidate : candidates_)
	{
		// 違う候補は飛ばす
		if (candidate.attackId != attackId) continue;

		// 候補と同じ攻撃種類なら、今の重みを下げる
		candidate.currentWeight *= LOWER_WEIGHT_RATE;

		// 下限より小さくなっていたら、下限にそろえる
		if (candidate.currentWeight < MIN_WEIGHT)
		{
			candidate.currentWeight = MIN_WEIGHT;
		}
	}
}