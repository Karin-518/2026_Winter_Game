#include <DxLib.h>
#include "../../Application.h"

#include "FpsControl.h"

namespace
{
	// 1秒（マイクロ秒）
	constexpr long long ONE_SECOND_US = 1000000;

	// 1フレームの時間（マイクロ秒）。60FPS なら 16666
	constexpr long long FRAME_TIME_US = ONE_SECOND_US / 60;

	// 残りがこれより長ければ Sleep で休む。短ければ寝すぎないよう、そのまま待つ
	constexpr long long SLEEP_MARGIN_US = 2000;

	// これ以上遅れたら取り戻さず、今から数え直す（ロード直後などで一気に進まないように）
	constexpr long long MAX_DELAY_FRAME = 5;
}

FpsControl::FpsControl(void)
	:
	currentTime_(0),
	prevFrameTime_(0),
	updateFrameRateTime_(0),
	frameCnt_(0),
	frameRate_(0.0f)
{
}

FpsControl::~FpsControl(void)
{
}

void FpsControl::Init(void)
{
	currentTime_ = GetNowHiPerformanceCount();
	prevFrameTime_ = currentTime_;
	updateFrameRateTime_ = currentTime_;
	frameCnt_ = 0;
	frameRate_ = 0.0f;
}

bool FpsControl::UpdateFrameRate(void)
{
	// 現在の時刻を取得
	currentTime_ = GetNowHiPerformanceCount();

	// 前のフレームから経った時間
	const long long elapsed = currentTime_ - prevFrameTime_;

	// 1フレーム分経っていたら、処理を実行する
	if (elapsed >= FRAME_TIME_US)
	{
		AdvanceFrameTime(elapsed);
		frameCnt_++;
		return true;
	}

	// まだ時間に余裕があるときだけ、システムに処理を返す
	if (FRAME_TIME_US - elapsed > SLEEP_MARGIN_US) { Sleep(1); }

	return false;
}

void FpsControl::AdvanceFrameTime(long long elapsed)
{
	// 大きく遅れたときは、取り戻そうとせず今から数え直す
	if (elapsed > FRAME_TIME_US * MAX_DELAY_FRAME)
	{
		prevFrameTime_ = currentTime_;
		return;
	}

	// 「実際に始めた時間」ではなく「始める予定だった時間」から次を数える
	// こうすると、少し遅れたフレームがあっても、次のフレームで取り戻せる
	prevFrameTime_ += FRAME_TIME_US;
}

void FpsControl::CalcFrameRate(void)
{
	// 前回のフレームレート更新からの経過時間
	const long long difTime = currentTime_ - updateFrameRateTime_;

	// 1秒以上経っていなければ、まだ更新しない
	if (difTime <= ONE_SECOND_US) return;

	// 理想通りなら 60 になる
	frameRate_ = static_cast<float>(
		static_cast<double>(frameCnt_) * ONE_SECOND_US / static_cast<double>(difTime));

	// カウントをリセット
	frameCnt_ = 0;
	updateFrameRateTime_ = currentTime_;
}

void FpsControl::DrawFrameRate(void)
{
	// スクリーンの右端に来るように設定
	DrawFormatString(
		Application::SCREEN_SIZE_X - 90,
		0,
		0xFF0000,
		"FPS[%.2f]",
		frameRate_
	);
}
