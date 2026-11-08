#include <memory>
#include <cmath>
#include <limits>
#include "../../../Common/Math/Math.h"
#include "../../../Common/Math/MatrixUtility.h"
#include "../../../Resource/ResourceManager.h"
#include "../../Common/AnimationController/AnimationController.h"
#include "../../Collider/Line/ColliderLine.h"
#include "../../Collider/Capsule/ColliderCapsule.h"
#include "../Player/Player.h"
#include "State/BossIdleState.h"
#include "State/BossAttackState.h"
#include "State/BossDeadState.h"
#include "Action/BossBiteAction.h"
#include "Action/BossRushAction.h"
#include "Action/BossTailSpinAction.h"

#include "Boss.h"

namespace
{
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
	constexpr float ANIMATION_SPEED = 0.5f;
	constexpr float BITE_ANIMATION_SPEED = 0.7f;

	// 初期位置
	constexpr VECTOR INIT_POS = { -138.0f, 0.0f, -830.0f };

	// 初期の向き(Y軸、度)
	constexpr float INIT_ANGLE_DEG_Y = 255.0f;

	constexpr BossPartParam PART_PARAMS[] =
	{
		{ BOSS_PART::HEAD,   "Tongue02",   "Neck01",     70.0f, 1.0f },
		{ BOSS_PART::BODY,   "Chest",  "joint19",     100.0f, 1.0f },
	};

	// 近距離とみなす半径
	constexpr float NEAR_RADIUS = 900.0f;

	// 長距離とみなす半径
	constexpr float FAR_RADIUS = 2000.0f;
}

Boss::Boss(Player* player)
	:
	player_(player),
	health_(),
	state_(*this),
	attackController_(),
	attackParams_(),
	attackSelector_(),
	nextAttackId_(BOSS_ATTACK_ID::NONE)
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

	for (auto& part : parts_)
	{
		part->Update();
	}

	// 重みの回復
	attackSelector_.Update();

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

	for (const auto& part : parts_)
	{
		part->DrawDebug();
	}

	attackSelector_.DrawDebug();

	DrawSphere3D(pos_, NEAR_RADIUS, 8, 0xff0000, 0xff0000, false);
	DrawSphere3D(pos_, FAR_RADIUS, 8, 0x00ff00, 0x00ff00, false);
#endif //_DEBUG
}

bool Boss::OnHit(int damage)
{
	return health_.Damage(damage);
}

bool Boss::IsDead(void) const
{
	// 死亡ステートでなければ、終わっていない
	if (state_.GetStateId() != BOSS_STATE::DEAD) return false;

	// アニメーションが最後まで再生したか
	return animationController_->IsEnd();
}

void Boss::StartAttack(void)
{
	// 攻撃が選ばれていなければ何もしない
	if (nextAttackId_ == BOSS_ATTACK_ID::NONE) return;

	// 使う攻撃のデータ
	const AttackParam& param = attackParams_[static_cast<int>(nextAttackId_)];

	// プレイヤーの方向
	FacePlayer();

	// 攻撃アニメーションを再生
	AnimationController::PlayParam playParam;
	playParam.isLoop = false;
	playParam.blendFrame = 10;
	playParam.isForce = true;
	animationController_->Play(param.animType, playParam);

	attackController_.Start(param);
}

void Boss::EndAttack(void)
{
	attackController_.Stop();
}

void Boss::OnAttackHit(void)
{
	attackController_.NotifyHit();
}

std::vector<const Hurtbox*> Boss::GetHurtboxes(void) const
{
	std::vector<const Hurtbox*> hurtboxes;

	for (const auto& part : parts_)
	{
		hurtboxes.push_back(part.get());
	}

	return hurtboxes;
}

bool Boss::SelectAttack(void)
{
	// 距離を測る
	float distance = CalcDistanceToPlayer();

	// 攻撃を選択
	nextAttackId_ = attackSelector_.Select(distance);

	if (nextAttackId_ != BOSS_ATTACK_ID::NONE) return true;

	return false;
}

void Boss::SetAttackActive(bool isActive)
{
	attackController_.SetActive(isActive);
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

	// 部位ごとの当たり判定
	InitParts();
}

void Boss::InitAnimation(void)
{
	// モデルアニメーション制御の初期化
	animationController_ = new AnimationController(modelId_);

	animationController_->AddInFbx(static_cast<int>(ANIM_TYPE::IDLE), ANIMATION_SPEED, 10);
	animationController_->AddInFbx(static_cast<int>(ANIM_TYPE::WALK), ANIMATION_SPEED, 17);
	animationController_->AddInFbx(static_cast<int>(ANIM_TYPE::RUN), ANIMATION_SPEED, 13);
	animationController_->AddInFbx(static_cast<int>(ANIM_TYPE::ATTACK_BITE), BITE_ANIMATION_SPEED, 0);
	animationController_->AddInFbx(static_cast<int>(ANIM_TYPE::DEAD), ANIMATION_SPEED, 3);
}

void Boss::InitPost(void)
{
	// ステートの追加
	state_.Register(BOSS_STATE::IDLE, std::make_unique<BossIdleState>());
	state_.Register(BOSS_STATE::ATTACK, std::make_unique<BossAttackState>());
	state_.Register(BOSS_STATE::DEAD, std::make_unique<BossDeadState>());

	state_.Start(BOSS_STATE::IDLE);

	// HPを満タンにして、0になったときの通知先を登録する
	health_.Init(MAX_HP);
	health_.SetOnDead([this]() { OnDead(); });

	// 攻撃パラメータの設定
	InitAttackParam();

	attackSelector_.AddCandidate(BOSS_ATTACK_ID::BITE, 0.0f, NEAR_RADIUS, 50.0f);
	attackSelector_.AddCandidate(BOSS_ATTACK_ID::TAIL_SPIN, 0.0f, NEAR_RADIUS, 30.0f);
	attackSelector_.AddCandidate(BOSS_ATTACK_ID::RUSH, NEAR_RADIUS, FAR_RADIUS, 40.0f);
}

void Boss::InitParts(void)
{
	for (const auto& param : PART_PARAMS)
	{
		parts_.push_back(std::make_unique<ColliderPart>(modelId_, param,this));
	}
}

void Boss::OnDead(void)
{
	// 死亡は外部から切り替えを依頼
	state_.RequestState(BOSS_STATE::DEAD);
}

void Boss::InitAttackParam(void)
{
	AttackParam& bite = attackParams_[static_cast<int>(BOSS_ATTACK_ID::BITE)];
	bite.animType = static_cast<int>(ANIM_TYPE::ATTACK_BITE);
	bite.damage = 10;

	// 判定が出る区間
	bite.hitStart = 15.0f;
	bite.hitEnd = 20.0f;

	// 判定の形
	bite.shape = HIT_SHAPE::RECT;
	bite.offset = 500.0f;
	bite.length = 300.0f;
	bite.width = 100.0f;

	// 溜めと隙
	bite.windupFrame = 35;
	bite.windupAnimRate = 0.2f;
	bite.recoveryFrame = 20;

	AttackParam& rush = attackParams_[static_cast<int>(BOSS_ATTACK_ID::RUSH)];
	rush.animType = static_cast<int>(ANIM_TYPE::RUN);
	rush.damage = 20;

	// 判定の形
	rush.shape = HIT_SHAPE::RECT;
	rush.offset = -300.0f;
	rush.length = 1000.0f;
	rush.width = 300.0f;

	// 溜めと隙
	rush.windupFrame = 55;
	rush.windupAnimRate = 0.2f;
	rush.recoveryFrame = 75;
	rush.isManualActive = true;

	AttackParam& tailSpin = attackParams_[static_cast<int>(BOSS_ATTACK_ID::TAIL_SPIN)];
	tailSpin.animType = static_cast<int>(ANIM_TYPE::IDLE);
	tailSpin.damage = 20;

	// 判定の形
	tailSpin.shape = HIT_SHAPE::CAPSULE;

	// 尻尾全体のカプセル
	AttackHitVolume tailVolume;
	tailVolume.startFrameName = "joint19";
	tailVolume.endFrameName = "joint23";
	tailVolume.radius = 50.0f;
	tailSpin.hitVolumes.push_back(tailVolume);

	// 溜めと隙
	tailSpin.windupFrame = 45;
	tailSpin.windupAnimRate = 1.0f;
	tailSpin.recoveryFrame = 40;
	tailSpin.isManualActive = true;

	// 攻撃ごとの動き
	attackActions_[static_cast<int>(BOSS_ATTACK_ID::BITE)] = std::make_unique<BossBiteAction>();
	attackActions_[static_cast<int>(BOSS_ATTACK_ID::RUSH)] = std::make_unique<BossRushAction>();
	attackActions_[static_cast<int>(BOSS_ATTACK_ID::TAIL_SPIN)] = std::make_unique<BossTailSpinAction>();

}

void Boss::FacePlayer(void)
{
	// 追従対象か向き先がなければ何もしない
	if (player_ == nullptr) return;

	// 追従対象から向き先へのベクトル
	VECTOR dir = VSub(player_->GetPos(), pos_);

	// 左右の角度だけ扱うため、上下の成分を0にする
	dir.y = 0.0f;

	// 同じ位置にいたら、向きを変えない
	if (Math::EqualsVZero(dir)) return;

	// 向きを変える
	moveDir_ = VNorm(dir);
}

float Boss::CalcDistanceToPlayer(void) const
{
	// プレイヤーがいなければ、どの攻撃も届かない遠いところを設定する
	if (player_ == nullptr) return (std::numeric_limits<float>::max)();

	VECTOR playerPos = player_->GetPos();
	VECTOR bossPos = pos_;

	// XZで距離を出すため、Y軸は0.0ｆにする
	playerPos.y = bossPos.y = 0.0f;

	return Math::Distance(playerPos, bossPos);
}

void Boss::RotateMoveDir(float addRad)
{
	// 移動方向をY軸まわりに回す
	moveDir_ = VTransform(moveDir_, MGetRotY(addRad));
}