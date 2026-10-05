#include "../../../Application.h"
#include "../../../Input/InputManager.h"
#include "../../../Common/Math/Math.h"
#include "../../../Common/Transform/MatrixUtility.h"
#include "../../Common/AnimationController.h"
#include "../../../Camera/Camera.h"

#include "Player.h"

namespace
{
	// 初期ローカル角度
	constexpr float DEFAULT_LOCAL_ANGLE_DEG_Y = 180.0f;

	// 当たり判定のカプセルの開始位置
	constexpr VECTOR START_CAPSULE_POS = { 0.0f, 110.0f, 0.0f };

	// 当たり判定のカプセルの終了位置
	constexpr VECTOR END_CAPSULE_POS = { 0.0f, 30.0f, 0.0f };

	// 当たり判定のカプセルの半径
	constexpr float CAPSULE_RADIUS = 20.0f;

	// アニメーションの速度
	constexpr float ANIMATION_SPEED = 0.5f;

	// 移動速度
	const float MOVE_POW = 5.0f;

	// 入力値の補間地（小さいほど慣性が強い）
	const float SMOOTH = 0.25f;
}

Player::Player(Camera* camera)
{
	camera_ = camera;
}

Player::~Player(void)
{
}

void Player::InitLoad(void)
{
	// モデルの読み込み
	modelId_ = MV1LoadModel((Application::PATH_MODEL + "Player/Player.mv1").c_str());
}

void Player::InitTransform(void)
{
	// モデルの角度
	angle_ = Math::VECTOR_ZERO;
	localAngle_ = { 0.0f, Math::Deg2Rad(DEFAULT_LOCAL_ANGLE_DEG_Y), 0.0f };

	// 角度から方向に変換する
	moveDir_ = { sinf(angle_.y), 0.0f, cosf(angle_.y) };
	preInputDir_ = moveDir_;

	// 行列の合成(子, 親と指定すると親⇒子の順に適用される)
	MATRIX mat = Matrix::Multiplication(localAngle_, angle_);

	// 回転行列をモデルに反映
	MV1SetRotationMatrix(modelId_, mat);

	// モデルの位置設定
	pos_ = Math::VECTOR_ZERO;
	MV1SetPosition(modelId_, pos_);

	// 当たり判定を作成
	startCapsulePos_ = START_CAPSULE_POS;
	endCapsulePos_ = END_CAPSULE_POS;
	capsuleRadius_ = CAPSULE_RADIUS;
	
	// 当たり判定を取るか
	isCollision_ = true;
}

void Player::InitAnimation(void)
{
	// モデルアニメーション制御の初期化
	animationController_ = new AnimationController(modelId_);

	// アニメーションの追加
	animationController_->Add(
		static_cast<int>(ANIM_TYPE::IDLE), ANIMATION_SPEED, Application::PATH_MODEL + "Player/Idle.mv1");
	animationController_->Add(
		static_cast<int>(ANIM_TYPE::WALK), ANIMATION_SPEED, Application::PATH_MODEL + "Player/Walk.mv1");

	// 初期アニメーションの再生
	animationController_->Play(static_cast<int>(ANIM_TYPE::IDLE));
}

void Player::InitPost(void)
{
}

void Player::Update(void)
{
	ActorBase::Update();

	// アニメーションの更新
	animationController_->Update();
}

void Player::Draw(void)
{
	ActorBase::Draw();

#ifdef _DEBUG
	DrawFormatString(
		0, 50, 0xffffff,
		"キャラ角度　 ：(%.1f, %.1f, %.1f)",
		Math::Rad2Deg(angle_.x),
		Math::Rad2Deg(angle_.y),
		Math::Rad2Deg(angle_.z)
	);
#endif //_DEBUG
}

void Player::Release(void)
{
	ActorBase::Release();
}

void Player::Move(void)
{
	// カメラ角度を取得
	VECTOR cameraAngles = camera_->GetAngle();

	// 移動量
	VECTOR dir = Math::VECTOR_ZERO;

	if (InputManager::GetInstance()->IsAction(INPUT_INFO::ACTION::MOVE_UP)) { dir = VAdd(dir, Math::DIR_F); }
	if (InputManager::GetInstance()->IsAction(INPUT_INFO::ACTION::MOVE_LEFT)) { dir = VAdd(dir, Math::DIR_L); }
	if (InputManager::GetInstance()->IsAction(INPUT_INFO::ACTION::MOVE_DOWN)) { dir = VAdd(dir, Math::DIR_B); }
	if (InputManager::GetInstance()->IsAction(INPUT_INFO::ACTION::MOVE_RIGHT)) { dir = VAdd(dir, Math::DIR_R); }

	if (!Math::EqualsVZero(dir))
	{
		dir.x = preInputDir_.x + (dir.x - preInputDir_.x) * SMOOTH;
		dir.z = preInputDir_.z + (dir.z - preInputDir_.z) * SMOOTH;
		preInputDir_ = dir;

		// 正規化
		dir = VNorm(dir);

		// XYZの回転行列
		// XZ平面移動にする場合は、XZの回転を考慮しないようにする
		MATRIX mat = MGetIdent();
		mat = MMult(mat, MGetRotY(cameraAngles.y));

		// 回転行列を使用して、ベクトルを回転させる
		moveDir_ = VTransform(dir, mat);

		// 方向×スピードで移動量を作って、座標に足して移動
		pos_ = VAdd(pos_, VScale(moveDir_, MOVE_POW));

		// 歩くアニメーションの再生
		animationController_->Play(static_cast<int>(ANIM_TYPE::WALK));
	}
	else
	{
		// 待機アニメーションの再生
		animationController_->Play(static_cast<int>(ANIM_TYPE::IDLE));
	}
}