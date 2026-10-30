#pragma once
#include <map>
#include "../../../Common/Math/Math.h"

class AnimationController
{
public:

	// ブレンドのデフォルトフレーム数
	static constexpr float DEFAULT_BLEND_FRAME = 15.0f;

	// アニメーションデータ
	struct Animation
	{
		int model = -1;				// アニメ元モデル
		int attachNo = -1;			// アタッチ番号
		int animIndex = 0;			// アニメ番号
		float speed = 0.0f;			// 基本の再生速度
		float totalTime = 0.0f;		// 総再生時間
		float step = 0.0f;			// 現在の再生位置

		float curveStartRate = 1.0f;		// 開始倍率
		float curveEndRate = 1.0f;			// 終了倍率
		Math::EASE_TYPE curveEase = Math::EASE_TYPE::LINEAR;	// 速度カーブ
	};

	// 再生時の設定
	struct PlayParam
	{
		bool isLoop = true;											// ループするか
		float blendFrame = DEFAULT_BLEND_FRAME;						// ブレンドにかける
		Math::EASE_TYPE blendEase = Math::EASE_TYPE::IN_OUT_QUAD;	// ブレンドのイージング
		bool isForce = false;										// 同じアニメでも最初から再生し直すか
		float startStep = 0.0f;										// 再生開始位置
	};

	// コンストラクタ
	AnimationController(int modelId);

	// デストラクタ
	~AnimationController(void);

	//　登録
	// 外部FBXからアニメーション追加
	void Add(int type, float speed, int animSrcHandle, int animIndex = 0);
	
	// 同じFBX内のアニメーションを準備
	void AddInFbx(int type, float speed, int animIndex);

	// 速度カーブを設定
	void SetSpeedCurve(int type, float startRate, float endRate, Math::EASE_TYPE ease);

	// 再生
	// アニメーション再生（簡易版）
	void Play(int type, bool isLoop = true);

	// アニメーション再生（詳細設定版）
	void Play(int type, const PlayParam& param);

	// 更新
	void Update(void);
	
	// 解放
	void Release(void);

	// 速度制御
	// 再生速度の倍率（ずっと有効。1.0が通常）
	// 速度の倍率をセット
	void SetSpeedRate(float rate);

	// 速度の倍率を取得
	float GetSpeedRate(void)const { return speedRate_; }

	// 指定フレームだけ再生速度を変える
	void ChangeSpeedForFrame(float rate, int frame);

	// 状態取得
	// 再生中のアニメーション
	int GetPlayType(void) const { return playType_; }

	// 再生終了
	bool IsEnd(void) const;

	// ブレンド中か
	bool IsBlending(void) const { return prevAnim_.attachNo != -1; }

	// 再生の進み具合
	float GetRate(void) const;

	// 現在の再生位置
	float GetStep(void) const { return playAnim_.step; }

	// このフレームで指定時間を通過したか
	bool IsPassed(float time) const;

	// 再生位置が指定区域内か
	bool IsInRange(float startTime, float endTime) const;

	// ループした回数
	int GetLoopCount(void) const { return loopCount_; }

	// 再生中のアニメーション情報を取得
	const Animation& GetPlayAnim(void) const { return playAnim_; }

private:

	// アニメーションするモデルのハンドルID
	int modelId_;

	// 種類別のアニメーションデータ
	std::map<int, Animation> animations_;

	// アニメーションタイプ
	int playType_;

	// 再生中のアニメーション
	Animation playAnim_;

	// アニメーションをループするか
	bool isLoop_;

	// フェードアウト中のアニメーション
	Animation prevAnim_;

	// ループだったか
	bool isPrevLoop_;

	// ブレンドの神勅
	float blendRate_;

	// 1フレームの増加量
	float blendStep_;

	// イージング
	Math::EASE_TYPE blendEase_;

	 // 再生速度の倍率
	float speedRate_;

	// 一時的な再生倍率
	float tempSpeedRate_;

	// 残りフレーム
	int tempSpeedFrame_;

	// 前フレームの再生位置
	float preStep_;

	// このフレームでループしたか
	bool isLooped_;

	// ループ回数
	int loopCount_;

	// 追加の共通処理
	void Register(int type, float speed, Animation animation);

	// モデルにアニメーションをアタッチ
	int Attach(const Animation& anim);

	// ブレンド開始
	void StartBlend(float blendFrame, Math::EASE_TYPE ease);

	// 再生位置を進める
	bool StepAnim(Animation& anim, bool isLoop, float rate);

	// ブレンドの更新
	void UpdateBlend(float rate);

	// ブレンド率をモデルに反映
	void ApplyBlendRate(void);

	// 旧アニメーションを外す
	void DetachPrev(void);

	// 一時速度の残りフレームを減らす
	void UpdatetempSpeed(void);

	// 全体の速度倍率
	float CalcGlobalRate(void) const;

	// 速度カーブの倍率
	float CalcCurveRate(void) const;
};