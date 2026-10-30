#pragma once
#include <vector>
#include <DxLib.h>

class ActorBase;
class ColliderModel;

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

	// ぶつかる相手(ステージ)を登録
	void AddHitStage(const ColliderModel* stage);

private:

	// 追従相手
	ActorBase* follow_;

	// リセットの向き先
	ActorBase* lookTarget_;

	// ぶつかる相手
	std::vector<const ColliderModel*> hitStages_;

	// カメラモード
	MODE mode_;

	// カメラの位置
	VECTOR pos_;
	
	// 注視点
	VECTOR targetPos_;

	// カメラが回る中心
	VECTOR pivotPos_;

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
	
	// 方向回転によるXYZの移動（キーボード・マウス）
	void MoveXYZDirection(void);

	// 方向回転によるXYZの移動（ゲームパッド）
	void MoveXYZDirectionPad(void);

	// ピッチ角度の制限
	void ClampPitch(void);

	// デバイスの振り分けと角度の更新
	void UpdateInput(void);

	// 回る中心を、追従相手へ近づける
	void UpdatePivot(void);

	// 回る中心と角度から、カメラ位置と注視点を計算する
	void CalcFollowPos(void);

	// 追従相手とカメラの間に壁があれば、カメラを壁の手前に寄せる
	void AvoidWall(void);

	// ステージ1つ分の壁よけ。壁がなければ goal をそのまま返す
	VECTOR AvoidWallByStage(
		const ColliderModel& stage, 
		const VECTOR& start, 
		const VECTOR& goal) const;

	// リセット開始
	void StartReset(void);

	// リセット更新
	void UpdateReset(void);
};