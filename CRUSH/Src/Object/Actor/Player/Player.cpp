#include <memory>
#include "../../../Application.h"
#include "../../../Input/InputManager.h"
#include "../../../Common/Math/Math.h"
#include "../../../Common/Math/MatrixUtility.h"
#include "../../../Camera/Camera.h"
#include "../../../Resource/ResourceManager.h"
#include "../../Common/AnimationController/AnimationController.h"
#include "../../Collider/Line/ColliderLine.h"
#include "../../Collider/Capsule/ColliderCapsule.h"
#include "State/PlayerIdleState.h"
#include "State/PlayerMoveState.h"

#include "Player.h"

namespace
{
	// 初期ローカル角度
	constexpr float DEFAULT_LOCAL_ANGLE_DEG_Y = 180.0f;

	// 足元の線分の始点
	constexpr VECTOR LINE_START_POS = { 0.0f, 10.0f, 0.0f };

	// 足元の線分の終点
	constexpr VECTOR LINE_END_POS = { 0.0f, -10.0f, 0.0f };

	// 当たり判定のカプセルの開始位置
	constexpr VECTOR START_CAPSULE_POS = { 0.0f, 110.0f, 0.0f };

	// 当たり判定のカプセルの終了位置
	constexpr VECTOR END_CAPSULE_POS = { 0.0f, 30.0f, 0.0f };

	// 当たり判定のカプセルの半径
	constexpr float CAPSULE_RADIUS = 20.0f;

	// アニメーションの速度
	constexpr float ANIMATION_SPEED = 0.5f;

	// 入力値の補間地（小さいほど慣性が強い）
	constexpr float INPUT_SMOOTH = 0.25f;

	// プレイヤーの最大HP
	constexpr int MAX_HP = 100;
}

Player::Player(Camera* camera)
	:
	camera_(camera),
	health_(),
	state_(*this)
{
}

Player::~Player(void)
{
}

void Player::MoveByInput(float speed)
{
	// カメラ角度を取得
	VECTOR cameraAngles = camera_->GetAngle();

	// 移動量
	VECTOR dir = Math::VECTOR_ZERO;

	if (InputManager::GetInstance()->IsAction(INPUT_INFO::ACTION::MOVE_UP)) { dir = VAdd(dir, Math::DIR_F); }
	if (InputManager::GetInstance()->IsAction(INPUT_INFO::ACTION::MOVE_LEFT)) { dir = VAdd(dir, Math::DIR_L); }
	if (InputManager::GetInstance()->IsAction(INPUT_INFO::ACTION::MOVE_DOWN)) { dir = VAdd(dir, Math::DIR_B); }
	if (InputManager::GetInstance()->IsAction(INPUT_INFO::ACTION::MOVE_RIGHT)) { dir = VAdd(dir, Math::DIR_R); }

	if (Math::EqualsVZero(dir)) return;

	dir.x = preInputDir_.x + (dir.x - preInputDir_.x) * INPUT_SMOOTH;
	dir.z = preInputDir_.z + (dir.z - preInputDir_.z) * INPUT_SMOOTH;
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
	pos_ = VAdd(pos_, VScale(moveDir_, speed));
}

void Player::OnHit(int damage)
{
	// ダメージをつけなかったら何もしない
	if (!health_.Damage(damage)) return;


}

void Player::InitLoad(void)
{
	ResourceManager::GetInstance()->Load(ResourceManager::SRC::PLAYER);
}

void Player::InitTransform(void)
{
	if (modelId_ == -1)
	{
		modelId_ = ResourceManager::GetInstance()->LoadModelDuplicate(
												ResourceManager::SRC::PLAYER);
	}

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
	MV1SetPosition(modelId_, pos_);;
}

void Player::InitCollider(void)
{
	// 足元の線分(地面判定)
	groundLine_ = new ColliderLine(
		ColliderBase::TAG::PLAYER, &pos_, LINE_START_POS, LINE_END_POS);

	// 体のカプセル(壁判定)
	bodyCapsule_ = new ColliderCapsule(
		ColliderBase::TAG::PLAYER, &pos_, START_CAPSULE_POS, END_CAPSULE_POS, CAPSULE_RADIUS);
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
}

void Player::InitPost(void)
{
	state_.Register(PLAYER_STATE::IDLE, std::make_unique<PlayerIdleState>());
	state_.Register(PLAYER_STATE::MOVE, std::make_unique<PlayerMoveState>());
	state_.Start(PLAYER_STATE::IDLE);

	// HP を満タンにして、0 になったときの通知先を登録する
	health_.Init(MAX_HP);
	health_.SetOnDead([this]() { OnDead(); });
}

void Player::OnDead(void)
{
	// 死亡は外部から切り替えを依頼
	state_.RequestState(PLAYER_STATE::DEAD);
}

void Player::Update(void)
{
	// 無敵の残りを減らす
	health_.Update();

	// ステートの更新
	state_.Update();

	ActorBase::Update();
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

	// デバッグ表示
	DrawFormatString(0, 90, 0xffffff, "HP：%d / %d", health_.GetHp(), health_.GetMaxHp());
	DrawFormatString(0, 110, 0xffffff, "無敵：%s", health_.IsInvincible() ? "あり" : "なし");

#endif //_DEBUG
}

void Player::Release(void)
{
	ActorBase::Release();
}