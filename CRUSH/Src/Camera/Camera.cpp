#include "../Common/Math/Math.h"
#include "../Input/InputManager.h"
#include "../Object/Actor/ActorBase.h"

#include "Camera.h"


Camera::Camera(void)
{
}

Camera::~Camera(void)
{
}

void Camera::Init(void)
{
	// カメラの初期位置
	pos_ = DERFAULT_POS;

	// カメラの初期角度
	angle_ = DERFAULT_ANGLES;
}

void Camera::Update(void)
{
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

	// カメラの回転行列を作成（ピッチ+ヨー）
	MATRIX mat = MGetIdent();
	mat = MMult(mat, MGetRotX(angle_.x));
	mat = MMult(mat, MGetRotY(angle_.y));

	// 注視点用の回転行列を作成（ヨーのみ）
	MATRIX matY = MGetIdent();
	matY = MMult(matY, MGetRotY(angle_.y));

	// 注視点の移動
	VECTOR followPos = follow_->GetPos();
	VECTOR targetLocalRotPos = VTransform(FOLLOW_TARGET_LOCAL_POS, matY);
	targetPos_ = VAdd(followPos, targetLocalRotPos);

	// カメラの移動
	VECTOR cameraLocalRotPos = VTransform(FOLLOW_CAMERA_LOCAL_POS, mat);

	// 相対座標からワールド座標に直して、カメラ座標とする
	pos_ = VAdd(followPos, cameraLocalRotPos);

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
		InputManager::GetInstance()->SetMouseLock(true);
		break;
	}
}

void Camera::SetFollow(ActorBase* follow)
{
	follow_ = follow;
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