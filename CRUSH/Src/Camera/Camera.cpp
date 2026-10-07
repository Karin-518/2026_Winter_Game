#include "../Common/Math/Math.h"
#include "../Input/InputManager.h"
#include "../Object/Actor/ActorBase.h"

#include "Camera.h"

namespace
{
	// カメラの初期座標
	constexpr VECTOR DEFAULT_POS = { 0.0f, 250.0f, -500.0f };

	// カメラの初期角度
	constexpr VECTOR DEFAULT_ANGLES = { 5.0f * Math::DEG2RAD, 0.0f, 0.0f };

	// 追従対象からカメラへの相対座標
	constexpr VECTOR FOLLOW_CAMERA_LOCAL_POS = { 0.0f, 160.0f, -350.0f };

	// 追従対象から注視点への相対座標
	constexpr VECTOR FOLLOW_TARGET_LOCAL_POS = { 0.0f, 150.0f, 100.0f };

	// カメラのクリップ範囲
	constexpr float VIEW_NEAR = 20.0f;
	constexpr float VIEW_FAR = 5000.0f;

	// マウスの感度（マウスの移動1ドットあたりの回転角度（度））
	constexpr float MOUSE_SENSITIVITY_DEG = 0.1f;

	// ゲームパッドの感度（スティックを倒し切ったときの1フレームの回転角度（度））
	constexpr float PAD_ROT_SPEED_DEG = 2.0f;

	// カメラのピッチ角度の制限
	constexpr float PITCH_LIMIT_DEG = 15.0f;

	// 補間率（1フレームで、目標との差の30%だけ近づく）
	constexpr float FOLLOW_SMOOTH = 0.3f;

	// リセットに×フレーム数
	constexpr int RESET_FRAME = 30.0f;

	// リセット後のピッチ角度（度）。0で水平
	constexpr float RESET_PITCH_DEG = 5.0f;
}

Camera::Camera(void)
	:
	follow_(nullptr),
	lookTarget_(nullptr),
	mode_(MODE::NONE),
	pos_(Math::VECTOR_ZERO),
	targetPos_(Math::VECTOR_ZERO),
	goalPos_(Math::VECTOR_ZERO),
	goalTargetPos_(Math::VECTOR_ZERO),
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

	CalcFollowGoal();

	if (!isFollowInitialized_)
	{
		SnapToGoal();
		return;
	}

	SmoothToGoal();
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
		break;

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
	const float limit = PITCH_LIMIT_DEG * DX_PI_F / 180.0f;

	if (angle_.x > limit) { angle_.x = limit; }
	if (angle_.x < -limit) { angle_.x = -limit; }
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

void Camera::CalcFollowGoal(void)
{
	// mat: カメラ位置用（上下と左右の角度で回す）
	MATRIX mat = MGetIdent();
	mat = MMult(mat, MGetRotX(angle_.x));
	mat = MMult(mat, MGetRotY(angle_.y));

	// matY: 注視点用
	// 左右だけ回す。注視点の高さは変えず、キャラが画面から外れないようにする
	MATRIX matY = MGetIdent();
	matY = MMult(matY, MGetRotY(angle_.y));

	const VECTOR followPos = follow_->GetPos();

	// カメラの目標位置と、目標の注視点を計算
	goalPos_ = VAdd(followPos, VTransform(FOLLOW_CAMERA_LOCAL_POS, mat));
	goalTargetPos_ =
		VAdd(followPos, VTransform(FOLLOW_TARGET_LOCAL_POS, matY));
}

void Camera::SnapToGoal(void)
{
	// 初回だけ補間せずに合わせる
	pos_ = goalPos_;
	targetPos_ = goalTargetPos_;
	isFollowInitialized_ = true;
}

void Camera::SmoothToGoal(void)
{
	// 毎フレーム目標との差の一部だけ近づける
	pos_ = Math::Lerp(pos_, goalPos_, FOLLOW_SMOOTH);
	targetPos_ = Math::Lerp(targetPos_, goalTargetPos_, FOLLOW_SMOOTH);
}

void Camera::StartReset(void)
{
	// 追従対象か向き先がなければ何もしない
	if (follow_ == nullptr || lookTarget_ == nullptr) return;

	// 追従対象（向き先のベクトル）
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
