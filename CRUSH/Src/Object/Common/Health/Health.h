#pragma once
#include <functional>

class Health
{
public:

	// コンストラクタ
	Health(void);

	// デストラクタ
	~Health(void) = default;

	// 初期化処理
	void Init(int maxHp);

	// 更新
	void Update(void);
	
	// ダメージを与える
	bool Damage(int damage);

	// 無敵にする
	void SetInvincible(int frame);

	// 無敵中か
	bool IsInvincible(void) const { return invincibleFrame_ > 0; }

	// HPの取得
	int GetHp(void) const { return hp_; }

	// 最大HPの取得
	int GetMaxHp(void) const { return maxHp_; }

	// 無敵の残りフレームの取得
	int GetInvincibleFrame(void) const { return invincibleFrame_; }

	// 通知の登録
	void SetOnDead(std::function<void(void)> func) { onDead_ = func; }

	// 死亡したか確認
	bool IsDead(void) const { return hp_ <= 0; }

private:

	// 今のHP
	int hp_;

	// 最大HP
	int maxHp_;

	// 無敵の残りフレーム
	int invincibleFrame_;

	// 半分以下を通知したか
	bool isHalfNotified_;

	// HPが0になったときの通知先
	std::function<void(void)> onDead_;

	// 登録されていれば呼ぶ
	void Notify(const std::function<void(void)>& func) const;
};