#include <cmath>
#include <DxLib.h>
#include "AnimationController.h"

AnimationController::AnimationController(int modelId)
	:
	modelId_(modelId),
	animations_(),
	playType_(-1),
	playAnim_(),
	isLoop_(true),
	prevAnim_(),
	isPrevLoop_(true),
	blendRate_(1.0f),
	blendStep_(0.0f),
	blendEase_(Math::EASE_TYPE::LINEAR),
	speedRate_(1.0f),
	tempSpeedRate_(1.0f),
	tempSpeedFrame_(0),
	isLooped_(false),
	loopCount_(0)
{
}

AnimationController::~AnimationController(void)
{
}

void AnimationController::Add(int type, float speed, int animSrcHandle, int animIndex)
{
	Animation animation;
	animation.model = animSrcHandle;
	animation.animIndex = animIndex;
	Register(type, speed, animation);
}

void AnimationController::AddInFbx(int type, float speed, int animIndex)
{
	Animation animation;
	animation.model = -1;
	animation.animIndex = animIndex;
	Register(type, speed, animation);
}

void AnimationController::SetSpeedCurve(int type, float startRate, float endRate, Math::EASE_TYPE ease)
{
	auto it = animations_.find(type);
	if (it == animations_.end()) return;

	it->second.curveStartRate = startRate;
	it->second.curveEndRate = endRate;
	it->second.curveEase = ease;
}

void AnimationController::Register(int type, float speed, Animation animation)
{
	animation.speed = speed;

	// 登録済みなら上書きしない
	if (animations_.count(type) != 0) return;

	animations_.emplace(type, animation);
}

void AnimationController::Play(int type, bool isLoop)
{
	PlayParam param;
	param.isLoop = isLoop;
	Play(type, param);
}

void AnimationController::Play(int type, const PlayParam& param)
{
	// 同じアニメーションなら継続
	if (playType_ == type && !param.isForce) return;

	// 未登録なら何もしない
	auto it = animations_.find(type);
	if (it == animations_.end()) return;

	// ブレンド途中の旧アニメーションがあれば外し、現在のものを旧へ
	DetachPrev();
	prevAnim_ = playAnim_;
	isPrevLoop_ = isLoop_;

	// 新しいアニメーションをアタッチ
	playType_ = type;
	playAnim_ = it->second;
	playAnim_.attachNo = Attach(playAnim_);
	if (playAnim_.attachNo == -1)
	{
		playType_ = -1;
		return;
	}
	playAnim_.totalTime = MV1GetAttachAnimTotalTime(modelId_, playAnim_.attachNo);

	// 再生位置の初期化
	playAnim_.step = param.startStep;
	MV1SetAttachAnimTime(modelId_, playAnim_.attachNo, playAnim_.step);
	preStep_ = playAnim_.step;
	isLooped_ = false;
	loopCount_ = 0;
	isLoop_ = param.isLoop;

	StartBlend(param.blendFrame, param.blendEase);
}

void AnimationController::Update(void)
{
	if (playType_ == -1) return;

	float globalRate = CalcGlobalRate();

	// 再生中のアニメーション
	preStep_ = playAnim_.step;
	isLooped_ = StepAnim(playAnim_, isLoop_, globalRate * CalcCurveRate());
	if (isLooped_) loopCount_++;

	// 旧アニメーションも動かし続ける
	StepAnim(prevAnim_, isPrevLoop_, globalRate);

	// ブレンド
	UpdateBlend(globalRate);

	UpdatetempSpeed();
}

void AnimationController::Release(void)
{
	// 本体モデルからアタッチだけ外す
	DetachPrev();
	if (playAnim_.attachNo != -1) MV1DetachAnim(modelId_, playAnim_.attachNo);

	playAnim_ = Animation();
	playType_ = -1;
	animations_.clear();
}

int AnimationController::Attach(const Animation& anim)
{
	// 本体モデルと同じファイル内のアニメーション
	if (anim.model == -1) return MV1AttachAnim(modelId_, anim.animIndex);

	// 別のモデルファイルのアニメーション
	return MV1AttachAnim(modelId_, anim.animIndex, anim.model);
}

bool AnimationController::StepAnim(Animation& anim, bool isLoop, float rate)
{
	if (anim.attachNo == -1) return false;

	anim.step += anim.speed * rate;

	bool isOver = anim.step > anim.totalTime;
	bool isWrap = isOver && isLoop && anim.totalTime > 0.0f;

	// ループは端数を残す
	if (isWrap) anim.step = fmodf(anim.step, anim.totalTime);
	if (isOver && !isWrap) anim.step = anim.totalTime;

	MV1SetAttachAnimTime(modelId_, anim.attachNo, anim.step);
	return isWrap;
}

void AnimationController::StartBlend(float blendFrame, Math::EASE_TYPE ease)
{
	blendEase_ = ease;

	// 初回再生 or ブレンドなし指定
	if (prevAnim_.attachNo == -1 || blendFrame <= 0.0f)
	{
		DetachPrev();
		blendRate_ = 1.0f;
		ApplyBlendRate();
		return;
	}

	blendRate_ = 0.0f;
	blendStep_ = 1.0f / blendFrame;
	ApplyBlendRate();
}

void AnimationController::UpdateBlend(float rate)
{
	if (prevAnim_.attachNo == -1) return;

	blendRate_ += blendStep_ * rate;
	if (blendRate_ >= 1.0f)
	{
		blendRate_ = 1.0f;
		DetachPrev();
	}

	ApplyBlendRate();
}

void AnimationController::ApplyBlendRate(void)
{
	// 進捗をイージングで変換してから反映
	float rate = Math::Ease(blendEase_, blendRate_);

	MV1SetAttachAnimBlendRate(modelId_, playAnim_.attachNo, rate);
	if (prevAnim_.attachNo == -1) return;
	MV1SetAttachAnimBlendRate(modelId_, prevAnim_.attachNo, 1.0f - rate);
}

void AnimationController::SetSpeedRate(float rate)
{
	speedRate_ = rate;
}

void AnimationController::ChangeSpeedForFrame(float rate, int frame)
{
	tempSpeedRate_ = rate;
	tempSpeedFrame_ = frame;
}

bool AnimationController::IsEnd(void) const
{
	if (isLoop_) return false;
	if (playType_ == -1) return false;

	return playAnim_.step >= playAnim_.totalTime;
}

float AnimationController::GetRate(void) const
{
	// 0 で割らないようにする
	if (playAnim_.totalTime <= 0.0f) return 0.0f;
	return playAnim_.step / playAnim_.totalTime;
}

bool AnimationController::IsPassed(float time) const
{
	if (playType_ == -1) return false;

	// 通常
	// 前回位置 < time <= 今回位置
	if (!isLooped_) return preStep_ < time && time <= playAnim_.step;

	// ループ
	// 前回位置～終端か、先頭～今回位置のどちらかに入っていれば通過
	return time > preStep_ || time <= playAnim_.step;
}

bool AnimationController::IsInRange(float startTime, float endTime) const
{
	if (playType_ == -1) return false;
	return startTime <= playAnim_.step && playAnim_.step <= endTime;
}

void AnimationController::DetachPrev(void)
{
	if (prevAnim_.attachNo == -1) return;

	MV1DetachAnim(modelId_, prevAnim_.attachNo);
	prevAnim_.attachNo = -1;
}

void AnimationController::UpdatetempSpeed(void)
{
	if (tempSpeedFrame_ <= 0) return;

	tempSpeedFrame_--;
	if (tempSpeedFrame_ == 0) tempSpeedRate_ = 1.0f;
}

float AnimationController::CalcGlobalRate(void) const
{
	return speedRate_ * tempSpeedRate_;
}

float AnimationController::CalcCurveRate(void) const
{
	float t = Math::Ease(playAnim_.curveEase, GetRate());
	return Math::Lerp(playAnim_.curveStartRate, playAnim_.curveEndRate, t);
}
