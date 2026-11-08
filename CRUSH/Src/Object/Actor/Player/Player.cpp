#include <memory>
#include "../../../Audio/AudioManager.h"
#include "../../../Input/InputManager.h"
#include "../../../Common/Math/Math.h"
#include "../../../Common/Math/MatrixUtility.h"
#include "../../../Camera/Camera.h"
#include "../../../Resource/ResourceManager.h"
#include "../../Common/AnimationController/AnimationController.h"
#include "../../Common/Attack/HitInfo.h"
#include "../../Collider/Line/ColliderLine.h"
#include "../../Collider/Capsule/ColliderCapsule.h"
#include "State/PlayerIdleState.h"
#include "State/PlayerMoveState.h"
#include "State/PlayerAttackState.h"
#include "State/PlayerDeadState.h"
#include "State/PlayerDodgeState.h"
#include "State/PlayerHitState.h"
#include "PlayerHurtbox.h"

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
	constexpr float ANIMATION_SPEED = 0.6f;

	// 攻撃時のアニメーションの速度
	constexpr float ATTACK_ANIMATION_SPEED = 0.8f;

	// 回避時のアニメーションの速度
	constexpr float ROLL_ANIMATION_SPEED = 1.0f;

	// 入力値の補間値（小さいほど慣性が強い）
	constexpr float INPUT_SMOOTH = 0.25f;

	// プレイヤーの最大HP
	constexpr int MAX_HP = 100;

	// 無敵時間のフレーム
	constexpr int HIT_INVINCIBLE_FRAME = 60;

	// 攻撃アニメへのブレンドのフレーム数
	constexpr int ATTACK_BLEND_FRAME = 4;

	// 初期位置
	constexpr VECTOR INIT_POS = { -1673.0f, 0.0f, -1234.0f };

	// 初期の向き(Y軸、度)
	constexpr float INIT_ANGLE_DEG_Y = 73.0f;

	// のけぞりのアニメーションの速度
	constexpr float HIT_ANIMATION_SPEED = 1.5f;

	// のけぞりアニメーションへのブレンドフレーム数
	constexpr int HIT_BLEND_FRAME = 4;

	// 剣を持たせる手のフレーム名
	constexpr TCHAR HAND_FRAME_NAME[] = "mixamorig:RightHand";

	// 腰のフレーム名
	constexpr TCHAR ROOT_FRAME_NAME[] = "mixamorig:Hips";
}

Player::Player(Camera* camera)
	:
	camera_(camera),
	health_(),
	attackController_(),
	state_(*this),
	attackParams_(),
	weapon_()
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

	// 移動が反映された位置に合わせる
	hurtbox_->Update();

	// 反映された手の位置に合わせる
	weapon_.Update();

	// 攻撃の更新
	attackController_.Update(*animationController_);
}

void Player::Draw(void)
{
	ActorBase::Draw();

	// 剣の描画
	weapon_.Draw();

#ifdef _DEBUG

	//attackController_.DrawDebug(pos_, GetForward());

	hurtbox_->DrawDebug();

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

bool Player::OnHit(const HitInfo& hitInfo)
{
	// ダメージが通らなかったら何もしない
	if (!health_.Damage(hitInfo.damage)) return false;

	// 倒れたら、のけぞらない
	if (health_.IsDead()) return true;

	// 無敵
	health_.SetInvincible(HIT_INVINCIBLE_FRAME);
	
	// のけぞり
	state_.RequestState(PLAYER_STATE::HIT);
	return true;
}

bool Player::IsDead(void) const
{
	// 死亡ステートでなければ、終わっていない
	if (state_.GetStateId() != PLAYER_STATE::DEAD) return false;

	// アニメーションが最後まで再生したか
	return animationController_->IsEnd();
}

void Player::InitLoad(void)
{
	// モデルのロード
	ResourceManager::GetInstance()->Load(ResourceManager::SRC::PLAYER);

	// 剣のロード
	ResourceManager::GetInstance()->Load(ResourceManager::SRC::PLAYER_SWORD);

	// モデルアニメーションのロード
	ResourceManager::GetInstance()->Load(ResourceManager::SRC::PLAYER_ANIM_IDLE);
	ResourceManager::GetInstance()->Load(ResourceManager::SRC::PLAYER_ANIM_WALK);
	ResourceManager::GetInstance()->Load(ResourceManager::SRC::PLAYER_ANIM_RUN);
	ResourceManager::GetInstance()->Load(ResourceManager::SRC::PLAYER_ANIM_ATTACK_1);
	ResourceManager::GetInstance()->Load(ResourceManager::SRC::PLAYER_ANIM_ROLL);
	ResourceManager::GetInstance()->Load(ResourceManager::SRC::PLAYER_ANIM_HIT);
	ResourceManager::GetInstance()->Load(ResourceManager::SRC::PLAYER_ANIM_DEATH);
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
	MV1SetPosition(modelId_, pos_);
}

void Player::InitCollider(void)
{
	// 足元の線分(地面判定)
	groundLine_ = new ColliderLine(
		ColliderBase::TAG::PLAYER, &pos_, LINE_START_POS, LINE_END_POS);

	// 体のカプセル(壁判定)
	bodyCapsule_ = new ColliderCapsule(
		ColliderBase::TAG::PLAYER, &pos_, START_CAPSULE_POS, END_CAPSULE_POS, CAPSULE_RADIUS);

	// 食らい判定
	hurtbox_ = std::make_unique<PlayerHurtbox>(this, bodyCapsule_);
}

void Player::InitAnimation(void)
{
	// モデルアニメーション制御の初期化
	animationController_ = new AnimationController(modelId_);

	// モデルアニメーションの追加
	AddAnimation(ANIM_TYPE::IDLE, ResourceManager::SRC::PLAYER_ANIM_IDLE, ANIMATION_SPEED);
	AddAnimation(ANIM_TYPE::WALK, ResourceManager::SRC::PLAYER_ANIM_WALK, ANIMATION_SPEED);
	AddAnimation(ANIM_TYPE::RUN, ResourceManager::SRC::PLAYER_ANIM_RUN, ANIMATION_SPEED);
	AddAnimation(ANIM_TYPE::ATTACK_1, ResourceManager::SRC::PLAYER_ANIM_ATTACK_1, ATTACK_ANIMATION_SPEED);
	AddAnimation(ANIM_TYPE::ROLL, ResourceManager::SRC::PLAYER_ANIM_ROLL, ROLL_ANIMATION_SPEED);
	AddAnimation(ANIM_TYPE::HIT, ResourceManager::SRC::PLAYER_ANIM_HIT, HIT_ANIMATION_SPEED);
	AddAnimation(ANIM_TYPE::DEAD, ResourceManager::SRC::PLAYER_ANIM_DEATH, ANIMATION_SPEED);

	// 回避アニメの前進を打ち消す
	animationController_->SetRootFrame(MV1SearchFrame(modelId_, ROOT_FRAME_NAME));
	animationController_->SetCancelRootMove(static_cast<int>(ANIM_TYPE::ROLL));
}

void Player::InitPost(void)
{
	// ステートの追加
	state_.Register(PLAYER_STATE::IDLE, std::make_unique<PlayerIdleState>());
	state_.Register(PLAYER_STATE::MOVE, std::make_unique<PlayerMoveState>());
	state_.Register(PLAYER_STATE::ATTACK, std::make_unique<PlayerAttackState>());
	state_.Register(PLAYER_STATE::DEAD, std::make_unique<PlayerDeadState>());
	state_.Register(PLAYER_STATE::DODGE, std::make_unique<PlayerDodgeState>());
	state_.Register(PLAYER_STATE::HIT, std::make_unique<PlayerHitState>());

	state_.Start(PLAYER_STATE::IDLE);

	// HP を満タンにして、0 になったときの通知先を登録する
	health_.Init(MAX_HP);
	health_.SetOnDead([this]() { OnDead(); });

	// 攻撃パラメータの設定
	InitAttackParam();

	// 剣を右手に持たせる
	weapon_.Init(modelId_, HAND_FRAME_NAME);
}

void Player::OnDead(void)
{
	// 死亡は外部から切り替えを依頼
	state_.RequestState(PLAYER_STATE::DEAD);
}

void Player::InitAttackParam(void)
{
	AttackParam& slash1 = attackParams_[static_cast<int>(PLAYER_ATTACK_ID::SLASH_1)];
	slash1.damage = 50;
	slash1.animType = static_cast<int>(ANIM_TYPE::ATTACK_1);

	// 判定が出る区間
	slash1.hitStart = 18.0f;
	slash1.hitEnd = 25.0f;

	// 判定の形（縦振りなら RECT、横振りなら FAN にする）
	slash1.shape = HIT_SHAPE::RECT;
	slash1.offset = 30.0f;
	slash1.length = 250.0f;
	slash1.width = 180.0f;
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

void Player::StartAttack(PLAYER_ATTACK_ID id)
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

	// SEを再生
	AudioManager::GetInstance()->PlaySE(SoundID::SE_PLAYER_SWING_1);

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
	AudioManager::GetInstance()->PlaySE(SoundID::SE_PLAYER_DODGE);

	animationController_->Play(static_cast<int>(ANIM_TYPE::ROLL), false);

	// 入力があれば、その方向へすぐ向く
	FaceInputDir();

	// 前半だけ無敵
	health_.SetInvincible(invincibleFrame);
}

const Hurtbox* Player::GetHurtbox(void) const
{
	return hurtbox_.get();
}

void Player::StartHit(void)
{
	AudioManager::GetInstance()->PlaySE(SoundID::SE_PLAYER_DAMAGE);

	// のけぞりアニメーションを再生
	AnimationController::PlayParam playParam;
	playParam.isLoop = false;
	playParam.blendFrame = HIT_BLEND_FRAME;
	playParam.isForce = true;
	animationController_->Play(static_cast<int>(ANIM_TYPE::HIT), playParam);
}

void Player::ExtendInvincible(int frame)
{
	health_.SetInvincible(frame);
}