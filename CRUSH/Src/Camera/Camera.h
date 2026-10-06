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

private:

	// 追従相手
	ActorBase* follow_;

	// カメラモード
	MODE mode_;

	// カメラの位置
	VECTOR pos_;

	// カメラの角度
	VECTOR angle_;
	
	// 注視点
	VECTOR targetPos_;

	// 初回追従用のフラグ
	bool isFollowInitialized_;
	
	// 方向回転によるXYZの移動（マウス）
	void MoveXYZDirection(void);

	// 方向回転によるXYZの移動（ゲームパッド）
	void MoveXYZDirectionPad(void);

	// ピッチ角度の制限
	void ClampPitch(void);
};
