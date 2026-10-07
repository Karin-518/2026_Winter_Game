#pragma once
#include <DxLib.h>

class ActorBase;

class Camera
{

public:
	
	// カメラモード
	enum class MODE
	{
		NONE,
		FIXED_POINT,	// 定点カメラ
		FREE,			// フリーモード
		FOLLOW,			// 追従モード
	};

	// コンストラクタ
	Camera(void);

	// デストラクタ
	~Camera(void);

	// 初期化
	void Init(void);

	// 更新
	void Update(void);

	// 描画前のカメラ設定
	void SetBeforeDraw(void);
	void SetBeforeDrawFixedPoint(void);
	void SetBeforeDrawFree(void);
	void SetBeforeDrawFollow(void);

	// デバッグ用描画
	void DrawDebug(void);

	// 解放
	void Release(void);

	// 座標の取得
	const VECTOR& GetPos(void) const { return pos_; }

	// 角度の取得
	const VECTOR& GetAngle(void) const { return angle_; }
	
	// 注視点の取得
	const VECTOR& GetTargetPos(void) const { return targetPos_; }

	// カメラモードの変更
	void ChangeMode(MODE mode);

	// 追従相手の設定
	void SetFollow(ActorBase* follow);

	// リセットの向き先を設定
	void SetLookTarget(ActorBase* target);

private:

	// 追従相手
	ActorBase* follow_;

	// リセットの向き先
	ActorBase* lookTarget_;

	// カメラモード
	MODE mode_;

	// カメラの位置
	VECTOR pos_;
	
	// 注視点
	VECTOR targetPos_;

	// カメラ位置の目標
	VECTOR goalPos_;

	// 注視点の目標
	VECTOR goalTargetPos_;

	// カメラの角度
	VECTOR angle_;

	// 初回追従用のフラグ
	bool isFollowInitialized_;

	// リセット中か
	bool isResetting_;

	// リセットの経過フレーム
	int resetFrame_;

	// 開始時の左右の角度
	float resetFromYaw_;

	// 開始時の上下の角度
	float resetFromPitch_;

	// 目標の左右の角度
	float resetToYaw_;

	// 目標の上下の角度
	float resetToPitch_;

private:
	
	// 方向回転によるXYZの移動（マウス）
	void MoveXYZDirection(void);

	// 方向回転によるXYZの移動（ゲームパッド）
	void MoveXYZDirectionPad(void);

	// ピッチ角度の制限
	void ClampPitch(void);

	// デバイスの振り分けと角度の更新
	void UpdateInput(void);

	// 追従対象の位置と角度から目標位置・注視点を計算
	void CalcFollowGoal(void);

	// 初回のみ補完せず目標へ合わせる
	void SnapToGoal(void);

	// 目標へ近づける
	void SmoothToGoal(void);

	// リセット開始
	void StartReset(void);

	// リセット更新
	void UpdateReset(void);
};
