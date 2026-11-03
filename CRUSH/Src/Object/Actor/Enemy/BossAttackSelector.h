#pragma once
#include <vector>
#include <random>
#include "BossAttackType.h"

class BossAttackSelector
{
public:

	// コンストラクタ
	BossAttackSelector(void);

	// デストラクタ
	~BossAttackSelector(void);

	// 更新
	void Update(void);

	// デバッグ描画
	void DrawDebug(void) const;

	// 候補の登録
	void AddCandidate(BOSS_ATTACK_ID attackId, float minDistance, float maxDistance, float weight);

	// 攻撃を1つ選んで返す(届く攻撃がなければNONE)
	BOSS_ATTACK_ID Select(float distance);

private:

	// 候補の構造体
	struct AttackCandidate
	{
		BOSS_ATTACK_ID attackId;	// 攻撃の種類
		float minDistance;			// 攻撃を出せる距離の最小値
		float maxDistance;			// 攻撃を出せる距離の最大値
		float defaultWeight;		// 基本の重み
		float currentWeight;		// 今の重み
	};

	// 候補の一覧
	std::vector<AttackCandidate> candidates_;

	// 乱数の生成器
	std::mt19937 randomEngine_;

	// 今選ばれている攻撃の種類
	BOSS_ATTACK_ID currentAttackId_;

	// 距離の条件を満たす候補を集める
	std::vector<AttackCandidate> CollectReachableCandidates(float distance) const;

	// 集めた候補の中から重みで1つ選ぶ
	BOSS_ATTACK_ID PickByWeight(const std::vector<AttackCandidate>& candidates);

	// 選んだ攻撃の重みを下げる
	void LowerWeight(BOSS_ATTACK_ID attackId);
};