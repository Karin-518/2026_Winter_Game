#pragma once

class FpsControl
{
public:

	// コンストラクタ
	FpsControl(void);
	
	// デストラクタ
	~FpsControl(void);

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
	// 現在の時間
	long long currentTime_;

	// 前回のフレームを始めた予定の時間
	long long prevFrameTime_;

	// フレームレートを更新した時間
	long long updateFrameRateTime_;

	// フレームカウント用
	int frameCnt_;

	// フレームレート (表示用)
	float frameRate_;

	// 次のフレームの開始時間を決める
	void AdvanceFrameTime(long long elapsed);
};