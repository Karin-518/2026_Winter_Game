#pragma once
#include <functional>

class Health
{
public:

	// コンストラクタ
	Health(void);

	// デストラクタ
	~Health(void) = default;

	void Init(int maxHp);
	void Update(void);
	
	// ダメージを与える
	bool Damage(int damage);

	// 無敵にする
	void SetInvincible(int frame);

	// 無敵中か
	bool IsInvincible(void) const { return invincibleFrame_ > 0; }

	// HPの取得
	int GetHp(void) const { return hp_; }
	int GetMaxHp(void) const { return maxHp_; }

	// 無敵の残りフレームの取得
	int GetInvincibleFrame(void) const { return invincibleFrame_; }

	// 通知の登録
	void SetOnDead(std::function<void(void)> func) { onDead_ = func; }
	void SetOnHalf(std::function<void(void)> func) { onHalf_ = func; }

	// 死亡したか確認
	bool IsDead(void) const { return hp_ <= 0; }

private:

	int hp_;							// 今のHP
	int maxHp_;							// 最大HP
	int invincibleFrame_;				// 無敵の残りフレーム
	bool isHalfNotified_;				// 半分以下を通知したか

	std::function<void(void)> onDead_;	// HPが0になったときの通知先
	std::function<void(void)> onHalf_;	// HPが半分以下になったときの通知先

	// 登録されていれば呼ぶ
	void Notify(const std::function<void(void)>& func) const;
};