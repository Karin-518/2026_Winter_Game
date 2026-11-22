#pragma once
#include <vector>
#include <DxLib.h>

struct AttackParam;
class AnimationController;
class Hurtbox;

/// <summary>
/// 攻撃の判定を管理する部品
/// </summary>
class AttackController
{
public:

	// コンストラクタ
	AttackController(void);

	// デストラクタ
	~AttackController(void);

	// 攻撃開始
	void Start(const AttackParam& param);

	// 攻撃終了
	void Stop(void);

	// 判定区画かどうかを更新
	void Update(const AnimationController& anim);

	// 判定のオンオフを外から決める
	void SetActive(bool isActive) { manualActive_ = isActive; }

	// このフレームで当てていいか
	bool CanHit(void) const;

	// 攻撃のカプセルと食らい判定が重なっているか
	bool IsHit(const Hurtbox& hurtbox) const;

	// カプセルの位置を外から渡す
	void SetCapsulePos(int index, const VECTOR& startPos, const VECTOR& endPos);

	// 当てたことを記録
	void NotifyHit(void);

	// 攻撃中か
	bool IsAttacking(void) const { return param_ != nullptr; }

	// 今の攻撃データ
	const AttackParam* GetParam(void) const { return param_; }

	// デバッグ表示
	void DrawDebug(void) const;

private:

	// 実行中のカプセル1つ分
	struct HitCapsule
	{
		VECTOR startPos;	// 始点
		VECTOR endPos;		// 終点
		bool isActive;		// このフレームで判定が出ているか
	};

	// 今の攻撃データ
	const AttackParam* param_;

	// 判定区域内か
	bool isActive_;

	// 1振りで当たったか
	bool hasHit_;

	// 外から指定された判定のオン・オフ
	bool manualActive_;

	// 今の攻撃カプセル
	std::vector<HitCapsule> capsules_;

	// カプセルごとに判定が出ているか決める
	void UpdateCapsuleActive(const AnimationController& anim);

	// カプセル1つ分の判定が出ているか
	bool IsVolumeActive(int volumeIndex, const AnimationController& anim) const;

	// カプセル1つ分のデバッグ描画の色
	unsigned int GetDebugColor(const HitCapsule& capsule) const;
};