#include <memory>
#include <cmath>
#include <limits>
#include <algorithm>
#include "../../../Audio/AudioManager.h"
#include "../../../Resource/ResourceManager.h"
#include "../../../Effect/EffectManager.h"
#include "../../../Common/Math/Math.h"
#include "../../../Common/Math/MatrixUtility.h"
#include "../../Common/AnimationController/AnimationController.h"
#include "../../Common/Attack/HitInfo.h"
#include "../../Common/HitStop/HitStop.h"
#include "../../Collider/Line/ColliderLine.h"
#include "../../Collider/Capsule/ColliderCapsule.h"
#include "../Player/Player.h"
#include "State/BossIdleState.h"
#include "State/BossAttackState.h"
#include "State/BossDeadState.h"
#include "Action/BossBiteAction.h"
#include "Action/BossRushAction.h"
#include "Action/BossTailSpinAction.h"
#include "Action/BossFireballAction.h"
#include "Action/BossFlyFireAction.h"

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

	// 噛みつきアニメーション速度
	constexpr float BITE_ANIMATION_SPEED = 0.7f;

	// 初期位置
	constexpr VECTOR INIT_POS = { -138.0f, 0.0f, -830.0f };

	// 初期の向き(Y軸、度)
	constexpr float INIT_ANGLE_DEG_Y = 255.0f;

	constexpr BossPartParam PART_PARAMS[] =
	{
		{ BOSS_PART::HEAD,   "Tongue_3",   "Neck_3",     70.0f, 1.0f },
		{ BOSS_PART::BODY,   "Chest",  "Tail_01",     100.0f, 1.0f },
	};

	// 押し出し用のカプセル
	constexpr PushboxParam PUSHBOX_PARAMS[] =
	{
		{ "Tongue_3", "Neck_3",  60.0f },	// 頭
		{ "Chest",    "Tail_01", 90.0f },	// 胴
	};

	// 近距離とみなす半径
	constexpr float NEAR_RADIUS = 900.0f;

	// 長距離とみなす半径
	constexpr float FAR_RADIUS = 3000.0f;

	// 火の玉を出す口のフレーム名
	constexpr const char* MOUTH_FRAME_NAME = "Jaw";

	// 火の玉が狙う高さ
	constexpr float FIREBALL_AIM_HEIGHT = 100.0f;
}

Boss::Boss(Player* player, HitStop* hitStop)
	:
	player_(player),
	hitStop_(hitStop),
	health_(),
	state_(*this),
	attackController_(),
	attackParams_(),
	attackSelector_(),
	nextAttackId_(BOSS_ATTACK_ID::NONE),
	fireballs_(),
	mouthFrameIndex_(-1)
{
}

Boss::~Boss(void)
{
}

void Boss::Update(void)
{
	if (hitStop_->IsStopping()) return;

	// ステートの更新
	state_.Update();

	// アニメ・回転・位置の更新
	ActorBase::Update();

	for (auto& part : parts_)
	{
		part->Update();
	}

	for (auto& pushbox : pushboxes_)
	{
		pushbox->Update();
	}

	// 攻撃のボーンの位置をとる
	UpdateAttackCapsules();

	// 重みの回復
	attackSelector_.Update();

	// 火の玉の更新
	UpdateFireballs();

	// 攻撃の更新
	attackController_.Update(*animationController_);
}

void Boss::Draw(void)
{
	ActorBase::Draw();

	// 火の玉の描画
	for (const auto& fireball : fireballs_)
	{
		fireball->Draw();
	}

#ifdef _DEBUG
	// デバッグ表示
	DrawFormatString(0, 130, 0xffffff, "ボスHP：%d / %d", health_.GetHp(), health_.GetMaxHp());

	attackController_.DrawDebug();

	for (const auto& part : parts_)
	{
		part->DrawDebug();
	}

	for (const auto& pushbox : pushboxes_)
	{
		pushbox->DrawDebug();
	}

	attackSelector_.DrawDebug();

	DrawSphere3D(pos_, NEAR_RADIUS, 8, 0xff0000, 0xff0000, false);
	DrawSphere3D(pos_, FAR_RADIUS, 8, 0x00ff00, 0x00ff00, false);
#endif //_DEBUG
}

bool Boss::OnHit(const HitInfo& hitInfo)
{
	EffectManager::GetInstance()->Play(EFFECT_ID::HIT_SPARK, hitInfo.hitPos);
	
	// SEを再生
	AudioManager::GetInstance()->PlaySE(SOUND_ID::SE_PLAYER_SWING_1);
	
	hitStop_->Start(4);

	return health_.Damage(hitInfo.damage);
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

	// 溜めで流すアニメ（溜め専用がなければ、本体のアニメを溜めから流す）
	int windupAnim = param.animType;
	if (param.windupAnimType != NO_ANIM)
	{
		windupAnim = param.windupAnimType;
	}

	// 攻撃アニメーションを再生
	AnimationController::PlayParam playParam;
	playParam.isLoop = false;
	playParam.blendFrame = 10;
	playParam.isForce = true;
	animationController_->Play(windupAnim, playParam);

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

std::vector<const Pushbox*> Boss::GetPushboxes(void) const
{
	std::vector<const Pushbox*> pushboxes;

	for (const auto& pushbox : pushboxes_)
	{
		pushboxes.push_back(pushbox.get());
	}

	return pushboxes;
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

	MV1SetScale(modelId_, VGet(1.5f, 1.5f, 1.5f));
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

	// 押し出し用のカプセル
	InitPushboxes();
}

void Boss::InitAnimation(void)
{
	// モデルアニメーション制御の初期化
	animationController_ = new AnimationController(modelId_);

	AddAnimation(ANIM_TYPE::BITE_1, ResourceManager::SRC::BOSS_ANIM_BITE_1, ANIMATION_SPEED);
	AddAnimation(ANIM_TYPE::BITE_2, ResourceManager::SRC::BOSS_ANIM_BITE_2, ANIMATION_SPEED);
	AddAnimation(ANIM_TYPE::BITE_3, ResourceManager::SRC::BOSS_ANIM_BITE_3, ANIMATION_SPEED);
	AddAnimation(ANIM_TYPE::DEATH, ResourceManager::SRC::BOSS_ANIM_DEATH, ANIMATION_SPEED);
	AddAnimation(ANIM_TYPE::FIRE_BALL, ResourceManager::SRC::BOSS_ANIM_FIRE_BALL, ANIMATION_SPEED);
	AddAnimation(ANIM_TYPE::FLY_FIRE, ResourceManager::SRC::BOSS_ANIM_FLY_FIRE, ANIMATION_SPEED);
	AddAnimation(ANIM_TYPE::IDLE, ResourceManager::SRC::BOSS_ANIM_IDLE_1, ANIMATION_SPEED);
	AddAnimation(ANIM_TYPE::IMPACT_STRONG, ResourceManager::SRC::BOSS_ANIM_IMPACT_STRONG, ANIMATION_SPEED);
	AddAnimation(ANIM_TYPE::IMPACT_WEAK, ResourceManager::SRC::BOSS_ANIM_IMPACT_WEAK, ANIMATION_SPEED);
	AddAnimation(ANIM_TYPE::REPOSE, ResourceManager::SRC::BOSS_ANIM_REPOSE, ANIMATION_SPEED);
	AddAnimation(ANIM_TYPE::RUN, ResourceManager::SRC::BOSS_ANIM_RUN, ANIMATION_SPEED);
	AddAnimation(ANIM_TYPE::TAIL_1, ResourceManager::SRC::BOSS_ANIM_TAIL_1, ANIMATION_SPEED);
	AddAnimation(ANIM_TYPE::TAIL_2, ResourceManager::SRC::BOSS_ANIM_TAIL_2, ANIMATION_SPEED);
	AddAnimation(ANIM_TYPE::TAKEOFF, ResourceManager::SRC::BOSS_ANIM_TAKEOFF, ANIMATION_SPEED);
	AddAnimation(ANIM_TYPE::TOLAND, ResourceManager::SRC::BOSS_ANIM_TORAND, ANIMATION_SPEED);
	AddAnimation(ANIM_TYPE::TURNBACK, ResourceManager::SRC::BOSS_ANIM_TURNBACK, ANIMATION_SPEED);
	AddAnimation(ANIM_TYPE::TURNLEFT, ResourceManager::SRC::BOSS_ANIM_TURNLEFT, ANIMATION_SPEED);
	AddAnimation(ANIM_TYPE::TURNRIGHT, ResourceManager::SRC::BOSS_ANIM_TURNRIGHT, ANIMATION_SPEED);
	AddAnimation(ANIM_TYPE::WALK, ResourceManager::SRC::BOSS_ANIM_WALK, ANIMATION_SPEED);

	animationController_->SetRootFrame(MV1SearchFrame(modelId_, "Root"));
	animationController_->SetCancelRootMove(static_cast<int>(ANIM_TYPE::RUN));
	animationController_->SetCancelRootMove(static_cast<int>(ANIM_TYPE::WALK));
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

	// 攻撃の判定のボーンを準備
	InitAttackSegments();

	attackSelector_.AddCandidate(BOSS_ATTACK_ID::BITE, 0.0f, NEAR_RADIUS, 50.0f);
	attackSelector_.AddCandidate(BOSS_ATTACK_ID::TAIL_RIGHT, 0.0f, NEAR_RADIUS, 30.0f);
	attackSelector_.AddCandidate(BOSS_ATTACK_ID::TAIL_LEFT, 0.0f, NEAR_RADIUS, 30.0f);
	attackSelector_.AddCandidate(BOSS_ATTACK_ID::RUSH, NEAR_RADIUS, FAR_RADIUS, 40.0f);
	attackSelector_.AddCandidate(BOSS_ATTACK_ID::FIRE_BALL, NEAR_RADIUS, FAR_RADIUS, 50.0f);
	attackSelector_.AddCandidate(BOSS_ATTACK_ID::FLY_FIRE, NEAR_RADIUS, FAR_RADIUS, 20.0f);

	mouthFrameIndex_ = MV1SearchFrame(modelId_, MOUTH_FRAME_NAME);
}

void Boss::InitParts(void)
{
	for (const auto& param : PART_PARAMS)
	{
		parts_.push_back(std::make_unique<ColliderPart>(modelId_, param,this));
	}
}

void Boss::InitPushboxes(void)
{
	for (const auto& param : PUSHBOX_PARAMS)
	{
		pushboxes_.push_back(std::make_unique<Pushbox>(modelId_, param));
	}
}

void Boss::OnDead(void)
{
	// 死亡は外部から切り替えを依頼
	state_.RequestState(BOSS_STATE::DEAD);
}

void Boss::InitAttackParam(void)
{
	// 噛みつき
	// ---------------------------------------
	AttackParam& bite = attackParams_[static_cast<int>(BOSS_ATTACK_ID::BITE)];
	bite.animType = static_cast<int>(ANIM_TYPE::BITE_1);
	bite.damage = 10;

	// 頭のカプセル
	AttackHitVolume headVolume;
	headVolume.startFrameName = "Jaw";
	headVolume.endFrameName = "Tongue_3";
	headVolume.radius = 80.0f;
	headVolume.activeStart = 20.0f;
	headVolume.activeEnd = 30.0f;
	bite.hitVolumes.push_back(headVolume);

	// 溜めと隙
	bite.windupFrame = 35;
	bite.windupAnimRate = 0.2f;
	bite.recoveryFrame = 20;

	// 突進
	// ---------------------------------------
	AttackParam& rush = attackParams_[static_cast<int>(BOSS_ATTACK_ID::RUSH)];
	rush.animType = static_cast<int>(ANIM_TYPE::RUN);
	rush.damage = 20;

	// 頭のカプセル
	AttackHitVolume rushHeadVolume;
	rushHeadVolume.startFrameName = "Jaw";
	rushHeadVolume.endFrameName = "Tongue_3";
	rushHeadVolume.radius = 100.0f;
	rush.hitVolumes.push_back(rushHeadVolume);

	// 胴のカプセル
	AttackHitVolume rushBodyVolume;
	rushBodyVolume.startFrameName = "Chest";
	rushBodyVolume.endFrameName = "Tail_01";
	rushBodyVolume.radius = 150.0f;
	rush.hitVolumes.push_back(rushBodyVolume);

	// 溜めと隙
	rush.windupFrame = 60;
	rush.windupAnimRate = 0.2f;
	rush.recoveryFrame = 100;
	rush.isManualActive = true;

	// 被弾リアクションと吹き飛ばし量
	rush.reaction = HIT_REACTION::KNOCKBACK;
	rush.knockbackSpeed = 50.0f;
	rush.knockbackSideRate = 1.5f;

	// 尻尾打ち付け（左右）
	// ---------------------------------------
	AttackParam& tailRight = attackParams_[static_cast<int>(BOSS_ATTACK_ID::TAIL_RIGHT)];
	tailRight.animType = static_cast<int>(ANIM_TYPE::TAIL_1);
	tailRight.damage = 10;

	// 尻尾全体のカプセル
	AttackHitVolume tailVolume;
	tailVolume.startFrameName = "Tail_01";
	tailVolume.endFrameName = "Tail_09";
	tailVolume.radius = 80.0f;
	tailRight.hitVolumes.push_back(tailVolume);

	// 溜めと隙
	tailRight.windupFrame = 45;
	tailRight.windupAnimRate = 1.0f;
	tailRight.recoveryFrame = 60;
	tailRight.isManualActive = true;

	// 被弾リアクションと吹き飛ばし量
	tailRight.reaction = HIT_REACTION::KNOCKBACK;
	tailRight.knockbackSpeed = 30.0f;
	tailRight.knockbackSideRate = 0.7f;

	AttackParam& tailLeft = attackParams_[static_cast<int>(BOSS_ATTACK_ID::TAIL_LEFT)];
	tailLeft.animType = static_cast<int>(ANIM_TYPE::TAIL_2);
	tailLeft.damage = 10;
	tailLeft.hitVolumes.push_back(tailVolume);

	// 溜めと隙
	tailLeft.windupFrame = 45;
	tailLeft.windupAnimRate = 1.0f;
	tailLeft.recoveryFrame = 60;
	tailLeft.isManualActive = true;

	// 被弾リアクションと吹き飛ばし量
	tailLeft.reaction = HIT_REACTION::KNOCKBACK;
	tailLeft.knockbackSpeed = 30.0f;
	tailLeft.knockbackSideRate = 0.7f;

	// 単発ブレス
	// ---------------------------------------
	AttackParam& fireball = attackParams_[static_cast<int>(BOSS_ATTACK_ID::FIRE_BALL)];
	fireball.animType = static_cast<int>(ANIM_TYPE::FIRE_BALL);
	fireball.damage = 20;

	// 溜めと隙
	fireball.windupFrame = 60;
	fireball.windupAnimRate = 1.0f;
	fireball.recoveryFrame = 100;
	fireball.isManualActive = true;

	// 空中ブレス
	// ---------------------------------------
	AttackParam& flyFire = attackParams_[static_cast<int>(BOSS_ATTACK_ID::FLY_FIRE)];
	flyFire.windupAnimType = static_cast<int>(ANIM_TYPE::TAKEOFF);
	flyFire.animType = static_cast<int>(ANIM_TYPE::FLY_FIRE);
	flyFire.recoveryAnimType = static_cast<int>(ANIM_TYPE::TOLAND);
	flyFire.damage = 20;

	// 溜めと隙
	flyFire.windupFrame = 70;
	flyFire.windupAnimRate = 1.0f;
	flyFire.recoveryFrame = 60;
	flyFire.isManualActive = true;

	// 攻撃ごとの動き
	attackActions_[static_cast<int>(BOSS_ATTACK_ID::BITE)] = std::make_unique<BossBiteAction>();
	attackActions_[static_cast<int>(BOSS_ATTACK_ID::RUSH)] = std::make_unique<BossRushAction>();
	attackActions_[static_cast<int>(BOSS_ATTACK_ID::TAIL_RIGHT)] = std::make_unique<BossTailSpinAction>();
	attackActions_[static_cast<int>(BOSS_ATTACK_ID::TAIL_LEFT)] = std::make_unique<BossTailSpinAction>();
	attackActions_[static_cast<int>(BOSS_ATTACK_ID::FIRE_BALL)] = std::make_unique<BossFireballAction>();
	attackActions_[static_cast<int>(BOSS_ATTACK_ID::FLY_FIRE)] = std::make_unique<BossFlyFireAction>();

}

void Boss::InitAttackSegments(void)
{
	for (int attackIndex = 0; attackIndex < static_cast<int>(BOSS_ATTACK_ID::MAX); attackIndex++)
	{
		InitSegmentsOfAttack(attackIndex);
	}
}

void Boss::InitSegmentsOfAttack(int attackIndex)
{
	// この攻撃のデータ
	const AttackParam& param = attackParams_[attackIndex];

	// この攻撃の判定のボーン（判定の数だけ用意する）
	std::vector<BoneSegment>& segments = attackSegments_[attackIndex];
	segments.resize(param.hitVolumes.size());

	for (int volumeIndex = 0; volumeIndex < static_cast<int>(param.hitVolumes.size()); volumeIndex++)
	{
		// この判定のボーン名
		const AttackHitVolume& volume = param.hitVolumes[volumeIndex];

		// ボーンを探す
		segments[volumeIndex].Init(modelId_, volume.startFrameName, volume.endFrameName);
	}
}

void Boss::UpdateAttackCapsules(void)
{
	// 攻撃中でなければ何もしない
	if (!attackController_.IsAttacking()) return;
	if (nextAttackId_ == BOSS_ATTACK_ID::NONE) return;

	// 今の攻撃の判定のボーン
	std::vector<BoneSegment>& segments = attackSegments_[static_cast<int>(nextAttackId_)];

	for (int volumeIndex = 0; volumeIndex < static_cast<int>(segments.size()); volumeIndex++)
	{
		// 今のアニメのボーン位置を取って、カプセルに渡す
		segments[volumeIndex].Update();
		attackController_.SetCapsulePos(
			volumeIndex, segments[volumeIndex].GetStartPos(), segments[volumeIndex].GetEndPos());
	}
}

void Boss::UpdateFireballs(void)
{
	// 全部の火の玉を進める
	for (auto& fireball : fireballs_)
	{
		fireball->Update();
	}

	// 消えた火の玉を後ろに寄せて、その先頭の位置を受け取る
	auto removeBegin = std::remove_if(fireballs_.begin(), fireballs_.end(),
		[](const std::unique_ptr<BossFireball>& fireball)
		{
			return !fireball->IsAlive();
		});

	// 後ろに寄せた分を一覧から消す
	fireballs_.erase(removeBegin, fireballs_.end());
}

void Boss::FacePlayer(void)
{
	// プレイヤーがいなければ何もしない
	if (player_ == nullptr) return;

	// ボスからプレイヤーへのベクトル
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

bool Boss::IsPlayerKnockedDown(void) const
{
	// プレイヤーがいなければ、倒れていない
	if (player_ == nullptr) return false;

	return player_->IsPlayerKnockedDown();
}

void Boss::ShootFireball(void)
{
	// プレイヤーがいなければ撃たない
	if (player_ == nullptr) return;

	// 口のフレームが見つかっていなければ撃たない
	if (mouthFrameIndex_ == -1) return;

	// 口の位置（ワールド座標）
	VECTOR mouthPos = MV1GetFramePosition(modelId_, mouthFrameIndex_);

	// 狙う位置（プレイヤーの足元から少し上）
	VECTOR aimPos = player_->GetPos();
	aimPos.y += FIREBALL_AIM_HEIGHT;

	// 口から狙う位置への向き（長さはBossFireballの中でそろえる）
	VECTOR shootDir = VSub(aimPos, mouthPos);

	// ダメージは攻撃データから取る
	int damage = GetCurrentAttackParam().damage;

	fireballs_.push_back(std::make_unique<BossFireball>(mouthPos, shootDir, damage));
}