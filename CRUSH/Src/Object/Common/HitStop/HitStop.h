#pragma once

class HitStop
{
public:

	// コンストラクタ
	HitStop(void);

	// デストラクタ
	~HitStop(void);

	// 開始
	void Start(int frame);

	// 更新
	void Update(void);

	// 止まっているかどうか
	bool IsStopping(void) const;

private:

	// 残りの停止フレーム
	int remainFrame_;
};