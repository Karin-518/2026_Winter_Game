#pragma once
#include "../SceneBase.h"

class GameClear : public SceneBase
{
public:

	// コンストラクタ
	GameClear(void);

	// デストラクタ
	~GameClear(void) override;

	// 初期化
	void Init(void) override;

	// 読み込み
	void Load(void) override;

	// 読み込み後の処理
	void LoadEnd(void) override;

	// 更新
	void Update(void) override;

	// 描画
	void Draw(void) override;

	// 解放
	void Release(void) override;

private:

	// ゲームクリアの画像ハンドル
	int handle_;

	// アルファ値
	float alpha_;

	// 値が増幅しているか
	bool isIncreasing_;
};

