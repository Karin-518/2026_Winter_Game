#include <memory>
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
#include "State/PlayerAttackState.h"
#include "State/PlayerDeadState.h"
#include "State/PlayerDodgeState.h"

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

	// 攻撃時のアニメーションの速度
	constexpr float ATTACK_ANIMATION_SPEED = 0.7f;

	// 入力値の補間地（小さいほど慣性が強い）
	constexpr float INPUT_SMOOTH = 0.25f;

	// プレイヤーの最大HP
	constexpr int MAX_HP = 100;

	// 無敵時間のフレーム
	constexpr int HIT_INVINCIBLE_FRAME = 30;

	// 攻撃アニメへのブレンドのフレーム数
	constexpr int ATTACK_BLEND_FRAME = 4;

	// アニメーション速度
	constexpr float ROLL_ANIMATION_SPEED = 1.0f;

	// 初期位置
	constexpr VECTOR INIT_POS = { -1673.0f, 0.0f, -1234.0f };

	// 初期の向き(Y軸、度)
	constexpr float INIT_ANGLE_DEG_Y = 73.0f;
}

Player::Player(Camera* camera)
	:
	camera_(camera),
	health_(),
	state_(*this),
	attackParams_(),
	attackController_()
{
}

Player::~Player(void)
{
}

void Player::Update(void)
{
	// 無敵の残りを減らす
	health_.Update();

	// ステートの更新
	state_.Update();

	ActorBase::Update();

	attackController_.Update(*animationController_);
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

	attackController_.DrawDebug(pos_, GetForward());

#endif //_DEBUG
}

void Player::MoveByInput(float speed)
{
	// 入力の合成
	VECTOR dir = CalcInputDir();

	if (Math::EqualsVZero(dir)) return;

	dir.x = preInputDir_.x + (dir.x - preInputDir_.x) * INPUT_SMOOTH;
	dir.z = preInputDir_.z + (dir.z - preInputDir_.z) * INPUT_SMOOTH;
	preInputDir_ = dir;

	// 正規化とカメラの回転
	moveDir_ = ToWorldDir(dir);

	// 方向×スピードで移動量を作って、座標に足して移動
	pos_ = VAdd(pos_, VScale(moveDir_, speed));
}

void Player::OnHit(int damage)
{
	// ダメージが通らなかったら何もしない
	if (!health_.Damage(damage)) return;

	// 倒れたら、のけぞらない
	if (health_.IsDead()) return;

	// 被弾後の無敵とのけぞり
	health_.SetInvincible(HIT_INVINCIBLE_FRAME);
	
	//state_.RequestState(PLAYER_STATE::HIT);
}

void Player::InitLoad(void)
{
	ResourceManager::GetInstance()->Load(ResourceManager::SRC::PLAYER);
	ResourceManager::GetInstance()->Load(ResourceManager::SRC::PLAYER_ANIM_IDLE);
	ResourceManager::GetInstance()->Load(ResourceManager::SRC::PLAYER_ANIM_WALK);
	ResourceManager::GetInstance()->Load(ResourceManager::SRC::PLAYER_ANIM_RUN);
	ResourceManager::GetInstance()->Load(ResourceManager::SRC::PLAYER_ANIM_ATTACK_1);
	ResourceManager::GetInstance()->Load(ResourceManager::SRC::PLAYER_ANIM_ROLL);
}

void Player::InitTransform(void)
{
	if (modelId_ == -1)
	{
		modelId_ = ResourceManager::GetInstance()->LoadModelDuplicate(
												ResourceManager::SRC::PLAYER);
	}

	// モデルの角度
	angle_ = { 0.0f, Math::Deg2Rad(INIT_ANGLE_DEG_Y), 0.0f };
	localAngle_ = { 0.0f, Math::Deg2Rad(DEFAULT_LOCAL_ANGLE_DEG_Y), 0.0f };

	// 角度から方向に変換する
	moveDir_ = { sinf(angle_.y), 0.0f, cosf(angle_.y) };
	preInputDir_ = moveDir_;

	// 行列の合成(子, 親と指定すると親⇒子の順に適用される)
	MATRIX mat = Matrix::Multiplication(localAngle_, angle_);

	// 回転行列をモデルに反映
	MV1SetRotationMatrix(modelId_, mat);

	// モデルの位置設定
	pos_ = INIT_POS;
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

	AddAnimation(ANIM_TYPE::IDLE, ResourceManager::SRC::PLAYER_ANIM_IDLE, ANIMATION_SPEED);
	AddAnimation(ANIM_TYPE::WALK, ResourceManager::SRC::PLAYER_ANIM_WALK, ANIMATION_SPEED);
	AddAnimation(ANIM_TYPE::RUN, ResourceManager::SRC::PLAYER_ANIM_RUN, ANIMATION_SPEED);
	AddAnimation(ANIM_TYPE::ATTACK_1, ResourceManager::SRC::PLAYER_ANIM_ATTACK_1, ATTACK_ANIMATION_SPEED);
	AddAnimation(ANIM_TYPE::ROLL, ResourceManager::SRC::PLAYER_ANIM_ROLL, ROLL_ANIMATION_SPEED);
}

void Player::InitPost(void)
{
	state_.Register(PLAYER_STATE::IDLE, std::make_unique<PlayerIdleState>());
	state_.Register(PLAYER_STATE::MOVE, std::make_unique<PlayerMoveState>());
	state_.Register(PLAYER_STATE::ATTACK, std::make_unique<PlayerAttackState>());
	state_.Register(PLAYER_STATE::DEAD, std::make_unique<PlayerDeadState>());
	state_.Register(PLAYER_STATE::DODGE, std::make_unique<PlayerDodgeState>());

	state_.Start(PLAYER_STATE::IDLE);

	// HP を満タンにして、0 になったときの通知先を登録する
	health_.Init(MAX_HP);
	health_.SetOnDead([this]() { OnDead(); });

	InitAttackParam();
}

void Player::OnDead(void)
{
	// 死亡は外部から切り替えを依頼
	state_.RequestState(PLAYER_STATE::DEAD);
}

void Player::InitAttackParam(void)
{
	AttackParam& slash1 = attackParams_[static_cast<int>(ATTACK_ID::SLASH_1)];
	slash1.attackId = ATTACK_ID::SLASH_1;
	slash1.damage = 100;
	slash1.animType = static_cast<int>(ANIM_TYPE::ATTACK_1);

	// 判定が出る区間
	slash1.hitStart = 10.0f;
	slash1.hitEnd = 20.0f;

	// 判定の形（縦振りなら RECT、横振りなら FAN にする）
	slash1.shape = HIT_SHAPE::RECT;
	slash1.offset = 30.0f;
	slash1.length = 250.0f;
	slash1.width = 150.0f;
	slash1.halfAngleDeg = 60.0f;
}

VECTOR Player::CalcInputDir(void) const
{
	VECTOR dir = Math::VECTOR_ZERO;

	if (InputManager::GetInstance()->IsAction(INPUT_INFO::ACTION::MOVE_UP)) { dir = VAdd(dir, Math::DIR_F); }
	if (InputManager::GetInstance()->IsAction(INPUT_INFO::ACTION::MOVE_LEFT)) { dir = VAdd(dir, Math::DIR_L); }
	if (InputManager::GetInstance()->IsAction(INPUT_INFO::ACTION::MOVE_DOWN)) { dir = VAdd(dir, Math::DIR_B); }
	if (InputManager::GetInstance()->IsAction(INPUT_INFO::ACTION::MOVE_RIGHT)) { dir = VAdd(dir, Math::DIR_R); }

	return dir;
}

VECTOR Player::ToWorldDir(const VECTOR& localDir) const
{
	// カメラの Y 回転だけを使う
	VECTOR cameraAngles = camera_->GetAngle();
	MATRIX mat = MGetRotY(cameraAngles.y);

	return VTransform(VNorm(localDir), mat);
}

void Player::FaceInputDir(void)
{
	VECTOR dir = CalcInputDir();

	// ??がなければ、今の向きのまま
	if (Math::EqualsVZero(dir)) return;

	// 移動の補間の基準もそろえておく
	preInputDir_ = VNorm(dir);

	// 向きを即座に変える
	moveDir_ = ToWorldDir(dir);
	angle_.y = atan2f(moveDir_.x, moveDir_.z);
}

void Player::StartAttack(ATTACK_ID id)
{
	const AttackParam& param = attackParams_[static_cast<int>(id)];

	// 入力があれば、その方向へすぐ向く
	FaceInputDir();

	// 攻撃アニメーションを再生
	AnimationController::PlayParam playParam;
	playParam.isLoop = false;
	playParam.blendFrame = ATTACK_BLEND_FRAME;
	playParam.isForce = true;
	animationController_->Play(param.animType, playParam);

	// 判定の準備
	attackController_.Start(param);
}

void Player::EndAttack(void)
{
	attackController_.Stop();
}

void Player::OnAttackHit(void)
{
	attackController_.NotifyHit();
}

void Player::StartDodge(int invincibleFrame)
{
	animationController_->Play(static_cast<int>(ANIM_TYPE::ROLL), false);

	// 入力があれば、その方向へすぐ向く
	FaceInputDir();

	// 前半だけ無敵
	health_.SetInvincible(invincibleFrame);
}

void Player::MoveForward(float speed)
{
	// 方向×スピードで移動量を作って、座標に足して移動
	pos_ = VAdd(pos_, VScale(moveDir_, speed));
}
