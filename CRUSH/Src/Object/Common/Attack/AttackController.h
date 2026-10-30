#pragma once
#include <DxLib.h>

struct AttackParam;
class AnimationController;

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

	// このフレームで当てていいか
	bool CanHit(void) const;

	// 判定の形と相手の円が重なっているか
	bool IsHit(
		const VECTOR& ownerPos, const VECTOR& forward,
		const VECTOR& targetPos, float targetRadius) const;

	// 当てたことを記録
	void NotifyHit(void);

	// 攻撃中か
	bool IsAttacking(void) const { return param_ != nullptr; }

	// 今の攻撃データ
	const AttackParam* GetParam(void) const { return param_; }

	// デバッグ表示
	void DrawDebug(const VECTOR& ownerPos, const VECTOR& forward) const;

private:

	// 今の攻撃データ
	const AttackParam* param_;

	// 判定区域内か
	bool isActive_;

	// 1振りで当たったか
	bool hasHit_;

	// 判定の根元
	VECTOR CalcOrigin(const VECTOR& ownerPos, const VECTOR& forward) const;

	// デバッグ描画の色
	unsigned int GetDebugColor(void) const;

	// 四角形を描く
	void DrawDebugRect(const VECTOR& origin, const VECTOR& forward, unsigned int color) const;

	// 扇を描く
	void DrawDebugFan(const VECTOR& origin, const VECTOR& forward, unsigned int color) const;
};