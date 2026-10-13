#pragma once

class FpsControl
{
public:

	FpsControl(void);	// コンストラクタ
	~FpsControl(void);	// デストラクタ

public:
	// 初期化（DxLib_Init の後に呼ぶ）
	void Init(void);

	// フレームレート更新
	// 1フレーム分の時間が経っていれば true を返す
	bool UpdateFrameRate(void);

	// フレームレート計算
	void CalcFrameRate(void);

	// フレームレート表示 (デバッグ表示)
	void DrawFrameRate(void);

private:

	// 時間はすべてマイクロ秒（1秒 = 1,000,000）
	long long currentTime_;			// 現在の時間
	long long prevFrameTime_;		// 前回のフレームを始めた予定の時間
	long long updateFrameRateTime_;	// フレームレートを更新した時間

	int frameCnt_;					// フレームカウント用
	float frameRate_;				// フレームレート (表示用)

	// 次のフレームの開始時間を決める
	void AdvanceFrameTime(long long elapsed);
};