#include <memory>
#include <cmath>
#include "../../../Common/Math/Math.h"
#include "../../../Common/Math/MatrixUtility.h"
#include "../../../Resource/ResourceManager.h"
#include "../../Common/AnimationController/AnimationController.h"
#include "../../Collider/Line/ColliderLine.h"
#include "../../Collider/Capsule/ColliderCapsule.h"
#include "../Player/Player.h"
#include "State/BossIdleState.h"
#include "State/BossAttackState.h"

#include "Boss.h"

namespace
{
	// 視野の広さ
	constexpr float VIEW_RANGE = 600.0f;

	// 視野角
	constexpr float VIEW_ANGLE = 30.0f;

	// 聴覚の広さ
	constexpr float HEARING_RANGE = 400.0f;

	// 足元の線分の始点
	constexpr VECTOR LINE_START_POS = { 0.0f, 10.0f, 0.0f };

	// 足元の線分の終点
	constexpr VECTOR LINE_END_POS = { 0.0f, -10.0f, 0.0f };

	// 当たり判定のカプセルの開始位置
	constexpr VECTOR START_CAPSULE_POS = { 0.0f, 250.0f, 0.0f };

	// 当たり判定のカプセルの終了位置
	constexpr VECTOR END_CAPSULE_POS = { 0.0f, 120.0f, 0.0f };

	// 当たり判定のカプセルの半径
	constexpr float CAPSULE_RADIUS = 120.0f;

	// ボスの最大HP
	constexpr int MAX_HP = 500;

	// アニメーション速度
	constexpr float ANIMATION_SPEED = 0.3f;

	// 初期位置
	constexpr VECTOR INIT_POS = { -138.0f, 0.0f, -830.0f };

	// 初期の向き(Y軸、度)
	constexpr float INIT_ANGLE_DEG_Y = 255.0f;
}

Boss::Boss(Player* player)
	:
	player_(player),
	health_(),
	state_(*this)
{
}

Boss::~Boss(void)
{
}

void Boss::Update(void)
{
	// ステートの更新
	state_.Update();

	ActorBase::Update();

	// 攻撃の更新
	attackController_.Update(*animationController_);
}

void Boss::Draw(void)
{
	ActorBase::Draw();

#ifdef _DEBUG
	// デバッグ表示
	DrawFormatString(0, 130, 0xffffff, "ボスHP：%d / %d", health_.GetHp(), health_.GetMaxHp());

	attackController_.DrawDebug(pos_, GetForward());
#endif //_DEBUG
}

void Boss::OnHit(int damage)
{
	health_.Damage(damage);
}

void Boss::StartAttack(void)
{
	// プレイヤーの方向
	FacePlayer();

	// 攻撃アニメーションを再生
	AnimationController::PlayParam playParam;
	playParam.isLoop = false;
	playParam.blendFrame = 10;
	playParam.isForce = true;
	animationController_->Play(static_cast<int>(ANIM_TYPE::ATTACK_BASIC), playParam);

	attackController_.Start(attackParam_);
}

void Boss::EndAttack(void)
{
	attackController_.Stop();
}

void Boss::OnAttackHit(void)
{
	attackController_.NotifyHit();
}

void Boss::InitLoad(void)
{
	ResourceManager::GetInstance()->Load(ResourceManager::SRC::BOSS);
}

void Boss::InitTransform(void)
{
	if (modelId_ == -1)
	{
		modelId_ = ResourceManager::GetInstance()->LoadModelDuplicate(
			ResourceManager::SRC::BOSS);
	}

	// モデルの角度
	angle_ = { 0.0f, Math::Deg2Rad(INIT_ANGLE_DEG_Y), 0.0f };
	localAngle_ = { 0.0f, Math::Deg2Rad(180.0f), 0.0f };

	// 角度から方向に変換する
	moveDir_ = { sinf(angle_.y), 0.0f, cosf(angle_.y) };

	// 行列の合成(子, 親と指定すると親⇒子の順に適用される)
	MATRIX mat = Matrix::Multiplication(localAngle_, angle_);

	// 回転行列をモデルに反映
	MV1SetRotationMatrix(modelId_, mat);

	// モデルの位置設定
	pos_ = INIT_POS;
	MV1SetPosition(modelId_, pos_);
}

void Boss::InitCollider(void)
{
	// 足元の線分(地面判定)
	groundLine_ = new ColliderLine(
		ColliderBase::TAG::ENEMY, &pos_, LINE_START_POS, LINE_END_POS);

	// 体のカプセル(壁判定)
	bodyCapsule_ = new ColliderCapsule(
		ColliderBase::TAG::ENEMY, &pos_, START_CAPSULE_POS, END_CAPSULE_POS, CAPSULE_RADIUS);
}

void Boss::InitAnimation(void)
{
	// モデルアニメーション制御の初期化
	animationController_ = new AnimationController(modelId_);

	animationController_->AddInFbx(static_cast<int>(ANIM_TYPE::IDLE), ANIMATION_SPEED, 10);
	animationController_->AddInFbx(static_cast<int>(ANIM_TYPE::WALK), ANIMATION_SPEED, 17);
	animationController_->AddInFbx(static_cast<int>(ANIM_TYPE::ATTACK_BASIC), ANIMATION_SPEED, 0);
}

void Boss::InitPost(void)
{
	state_.Register(BOSS_STATE::IDLE, std::make_unique<BossIdleState>());
	state_.Register(BOSS_STATE::ATTACK, std::make_unique<BossAttackState>());

	state_.Start(BOSS_STATE::IDLE);

	// HP を満タンにして、0 になったときの通知先を登録する
	health_.Init(MAX_HP);
	health_.SetOnDead([this]() { OnDead(); });

	InitAttackParam();
}

void Boss::OnDead(void)
{
}

void Boss::InitAttackParam(void)
{
	attackParam_.animType = static_cast<int>(ANIM_TYPE::ATTACK_BASIC);

	// 判定が出る区間
	attackParam_.hitStart = 10.0f;
	attackParam_.hitEnd = 20.0f;

	// 判定の形
	attackParam_.shape = HIT_SHAPE::FAN;
	attackParam_.offset = 30.0f;
	attackParam_.length = 50.0f;
	attackParam_.width = 400.0f;
	attackParam_.halfAngleDeg = 60.0f;
}

void Boss::FacePlayer(void)
{
}