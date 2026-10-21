#include <algorithm>
#include "../Common/Math/Math.h"
#include "../Input/InputManager.h"
#include "../Object/Actor/ActorBase.h"
#include "../Object/Collider/Model/ColliderModel.h"

#include "Camera.h"

namespace
{
	// カメラの初期座標
	constexpr VECTOR DEFAULT_POS = { 0.0f, 250.0f, -500.0f };

	// リセット後のピッチ角度（度）。0で水平
	constexpr float RESET_PITCH_DEG = 5.0f;

	// カメラの初期角度
	constexpr VECTOR DEFAULT_ANGLES = { RESET_PITCH_DEG * Math::DEG2RAD, 73.0f * Math::DEG2RAD, 0.0f };

	// 追従対象からカメラへの相対座標
	constexpr VECTOR FOLLOW_CAMERA_LOCAL_POS = { 0.0f, 160.0f, -350.0f };

	// 追従対象から注視点への相対座標
	constexpr VECTOR FOLLOW_TARGET_LOCAL_POS = { 0.0f, 150.0f, 100.0f };

	// カメラのクリップ範囲
	constexpr float VIEW_NEAR = 20.0f;
	constexpr float VIEW_FAR = 5000.0f;

	// マウスの感度
	constexpr float MOUSE_SENSITIVITY_DEG = 0.1f;

	// ゲームパッドの感度
	constexpr float PAD_ROT_SPEED_DEG = 2.0f;

	// 見下ろす側の制限
	constexpr float PITCH_DOWN_LIMIT_DEG = 30.0f;

	// 見上げる側の制限
	constexpr float PITCH_UP_LIMIT_DEG = 60.0f;

	// 回る中心が追従相手を追いかける補間率
	constexpr float FOLLOW_SMOOTH = 0.2f;

	// リセットにかけるフレーム数
	constexpr int RESET_FRAME = 20;

	// 壁を調べる線分の始点
	constexpr VECTOR WALL_CHECK_OFFSET = { 0.0f, 200.0f, 0.0f };

	// 壁に当たったとき、壁から離す距離
	constexpr float WALL_BACK_DIS = 30.0f;

	// 壁や地面に寄っても、頭からこれ以上は近づかない距離
	constexpr float MIN_CAMERA_DIS = 30.0f;
}

Camera::Camera(void)
	:
	follow_(nullptr),
	lookTarget_(nullptr),
	mode_(MODE::NONE),
	pos_(Math::VECTOR_ZERO),
	targetPos_(Math::VECTOR_ZERO),
	pivotPos_(Math::VECTOR_ZERO),
	angle_(Math::VECTOR_ZERO),
	isFollowInitialized_(false),
	isResetting_(false),
	resetFrame_(0),
	resetFromYaw_(0.0f),
	resetFromPitch_(0.0f),
	resetToYaw_(0.0f),
	resetToPitch_(0.0f)
{
}

Camera::~Camera(void)
{
}

void Camera::Init(void)
{
	// カメラの初期位置
	pos_ = DEFAULT_POS;

	// カメラの初期角度
	angle_ = DEFAULT_ANGLES;

	// 最初の追従か
	isFollowInitialized_ = false;
}

void Camera::Update(void)
{
	// モードが固定または、追従対象がいなければ処理しない
	if (mode_ == MODE::FIXED_POINT || follow_ == nullptr) return;

	// リセット中は入力を受け付けず、リセットだけ進める
	if (isResetting_)
	{
		UpdateReset();
	}
	else
	{
		UpdateInput();
	}

	// 回る中心だけを、追従相手へ少し遅れて追いかけさせる
	UpdatePivot();

	// カメラ位置は補間しない
	CalcFollowPos();

	const VECTOR lookDir = VSub(targetPos_, pos_);

	// 壁や地面があれば手前に寄せる
	AvoidWall();

	targetPos_ = VAdd(pos_, lookDir);
}

void Camera::SetBeforeDraw(void)
{
	// クリップ距離を設定する
	SetCameraNearFar(VIEW_NEAR, VIEW_FAR);

	switch (mode_)
	{
	case Camera::MODE::FIXED_POINT:
		SetBeforeDrawFixedPoint();
		break;

	case Camera::MODE::FREE:
		SetBeforeDrawFree();
		break;

	case Camera::MODE::FOLLOW:
		SetBeforeDrawFollow();
		break;
	}
}

void Camera::SetBeforeDrawFixedPoint(void)
{
	// カメラの設定
	SetCameraPositionAndAngle(
		pos_,
		angle_.x,
		angle_.y,
		angle_.z
	);
}

void Camera::SetBeforeDrawFree(void)
{

	// カメラの設定
	SetCameraPositionAndAngle(
		pos_,
		angle_.x,
		angle_.y,
		angle_.z
	);
}

void Camera::DrawDebug(void)
{
#ifdef _DEBUG
	DrawFormatString(
		0, 10, 0xffffff,
		"カメラ座標　 ：(%.1f, %.1f, %.1f)",
		pos_.x, pos_.y, pos_.z
	);

	DrawFormatString(
		0, 30, 0xffffff,
		"カメラ角度　 ：(%.1f, %.1f, %.1f)",
		Math::Rad2Deg(angle_.x),
		Math::Rad2Deg(angle_.y),
		Math::Rad2Deg(angle_.z)
	);
#endif //_DEBUG
}

void Camera::SetBeforeDrawFollow(void)
{
	// カメラの設定(位置と注視点による制御)
	SetCameraPositionAndTargetAndUpVec(
		pos_,
		targetPos_,
		Math::DIR_U
	);
}

void Camera::Release(void)
{
	hitStages_.clear();

	if (InputManager::GetInstance() != nullptr)
	{
		InputManager::GetInstance()->SetMouseLock(false);
	}
}

void Camera::ChangeMode(MODE mode)
{
	// カメラモードの変更
	mode_ = mode;

	// 変更時の初期化処理
	switch (mode_)
	{
	case Camera::MODE::FIXED_POINT:
		InputManager::GetInstance()->SetMouseLock(false);
		break;

	case Camera::MODE::FREE:
	case Camera::MODE::FOLLOW:
		// 追従中はマウスを固定して、移動量でカメラを回す
		InputManager::GetInstance()->SetMouseLock(true);
		break;
	}
}

void Camera::SetFollow(ActorBase* follow)
{
	follow_ = follow;
}

void Camera::SetLookTarget(ActorBase* target)
{
	lookTarget_ = target;
}

void Camera::AddHitStage(const ColliderModel* stage)
{
	// nullptr は登録しない
	if (stage == nullptr) return;

	hitStages_.push_back(stage);
}

void Camera::MoveXYZDirection(void)
{
	// 矢印キーでカメラの角度を変える
	float rotPow = 1.0f * DX_PI_F / 180.0f;

	if (InputManager::GetInstance()->IsAction(INPUT_INFO::ACTION::CAMERA_DOWN)) { angle_.x += rotPow; }
	if (InputManager::GetInstance()->IsAction(INPUT_INFO::ACTION::CAMERA_UP)) { angle_.x -= rotPow; }
	if (InputManager::GetInstance()->IsAction(INPUT_INFO::ACTION::CAMERA_RIGHT)) { angle_.y += rotPow; }
	if (InputManager::GetInstance()->IsAction(INPUT_INFO::ACTION::CAMERA_LEFT))	{ angle_.y -= rotPow; }

	// マウスの移動量でカメラの角度を変える
	const Vector2 mouseMove = InputManager::GetInstance()->GetMouseMove();
	const float mouseRot = MOUSE_SENSITIVITY_DEG * DX_PI_F / 180.0f;

	angle_.y += mouseMove.x * mouseRot;
	angle_.x += mouseMove.y * mouseRot;

	ClampPitch();
}

void Camera::MoveXYZDirectionPad(void)
{
	const Vector2 stick = InputManager::GetInstance()->
						GetRightStickAnalog(INPUT_INFO::JOYPAD_NO::PAD1);

	const float rotPow = PAD_ROT_SPEED_DEG * DX_PI_F / 180.0f;
	angle_.y += stick.x * rotPow;
	angle_.x += stick.y * rotPow;

	ClampPitch();
}

void Camera::ClampPitch(void)
{
	const float downLimit = Math::Deg2Rad(PITCH_DOWN_LIMIT_DEG);
	const float upLimit = -Math::Deg2Rad(PITCH_UP_LIMIT_DEG);

	// 見下ろしすぎない
	if (angle_.x > downLimit) { angle_.x = downLimit; }

	// 見上げすぎない
	if (angle_.x < upLimit) { angle_.x = upLimit; }
}

void Camera::UpdateInput(void)
{
	// 方向回転によるXYZの移動
	if (InputManager::GetInstance()->GetActiveDevice() == InputManager::ActiveDevice::KEY_MOUSE)
	{
		// キーボード・マウス
		MoveXYZDirection();
	}
	else
	{
		// ゲームパッド
		MoveXYZDirectionPad();
	}

	// リセットを開始
	if (InputManager::GetInstance()->IsActionDown(INPUT_INFO::ACTION::CAMERA_RESET))
	{
		StartReset();
	}

}

void Camera::UpdatePivot(void)
{
	const VECTOR followPos = follow_->GetPos();

	// 初回だけ補間せずに合わせる。2回目からは差の一部だけ近づける
	pivotPos_ = isFollowInitialized_ ? Math::Lerp(pivotPos_, followPos, FOLLOW_SMOOTH) : followPos;
	isFollowInitialized_ = true;
}

void Camera::CalcFollowPos(void)
{
	// mat: カメラ位置用（上下と左右の角度で回す）
	MATRIX mat = MGetIdent();
	mat = MMult(mat, MGetRotX(angle_.x));
	mat = MMult(mat, MGetRotY(angle_.y));

	// matY: 注視点用
	// 左右だけ回す。注視点の高さは変えず、キャラが画面から外れないようにする
	MATRIX matY = MGetIdent();
	matY = MMult(matY, MGetRotY(angle_.y));

	// 回る中心を基準に、カメラ位置と注視点を計算
	pos_ = VAdd(pivotPos_, VTransform(FOLLOW_CAMERA_LOCAL_POS, mat));
	targetPos_ = VAdd(pivotPos_, VTransform(FOLLOW_TARGET_LOCAL_POS, matY));
}

void Camera::AvoidWall(void)
{
	// 追従相手の頭あたりから、カメラまでの線分で調べる
	const VECTOR start = VAdd(follow_->GetPos(), WALL_CHECK_OFFSET);

	for (const ColliderModel* stage : hitStages_)
	{
		pos_ = AvoidWallByStage(*stage, start, pos_);
	}
}

VECTOR Camera::AvoidWallByStage(const ColliderModel& stage, const VECTOR& start, const VECTOR& goal) const
{
	const MV1_COLL_RESULT_POLY hit = stage.GetNearestHitPolyLine(start, goal);

	// 壁がない
	if (!hit.HitFlag) return goal;

	// 始点(頭)から壁までの距離
	const float hitDist = VSize(VSub(hit.HitPosition, start));

	// 壁の手前に置く。ただし頭から MIN_CAMERA_DIS より近くには来ない
	const float camDist = (std::max)(hitDist - WALL_BACK_DIS, MIN_CAMERA_DIS);

	// 始点からカメラの方向へ camDist だけ離れた位置
	const VECTOR dir = VNorm(VSub(goal, start));
	return VAdd(start, VScale(dir, camDist));
}

void Camera::StartReset(void)
{
	// 追従対象か向き先がなければ何もしない
	if (follow_ == nullptr || lookTarget_ == nullptr) return;

	// 追従対象から向き先へのベクトル
	VECTOR dir = VSub(lookTarget_->GetPos(), follow_->GetPos());

	// 左右の角度だけ扱うため、上下の成分を0にする
	dir.y = 0.0f;

	// ほぼ同じ位置だと向きが決まらないため中断
	if (Math::SqrMagnitudeF(dir) < 0.001f) return;

	// 開始角度と目標角度を保存
	resetFromYaw_ = angle_.y;
	resetFromPitch_ = angle_.x;
	resetToYaw_ = atan2f(dir.x, dir.z);
	resetToPitch_ = Math::Deg2Rad(RESET_PITCH_DEG);

	// リセット開始
	resetFrame_ = 0;
	isResetting_ = true;
}

void Camera::UpdateReset(void)
{
	resetFrame_++;

	// 進み具合　0.0～1.0
	float progress = 
		static_cast<float>(resetFrame_) / static_cast<float>(RESET_FRAME);
	
	if (progress > 1.0f)
	{
		progress = 1.0f;
	}

	// イージングをかける
	const float eased = Math::EaseOutCubic(progress);

	// 角度を補間（左右は最短経路で回る）
	angle_.y = Math::LerpAngle(resetFromYaw_, resetToYaw_, eased);
	angle_.x = Math::Lerp(resetFromPitch_, resetToPitch_, eased);

	// 最後まで進んだらリセット終了
	if (resetFrame_ >= RESET_FRAME)
	{
		isResetting_ = false;
	}
}
