#include <DxLib.h>
#include "../Application.h"

#include "ResourceManager.h"

ResourceManager* ResourceManager::instance_ = nullptr;

ResourceManager::ResourceManager(void)
{
}

ResourceManager::~ResourceManager(void)
{
	// 読み込んだものを解放してから、登録を消す
	Release();

	for (auto& [src, res] : resources_)
	{
		delete res;
	}
	resources_.clear();
}

void ResourceManager::Init(void)
{
	// プレイヤーモデル
	Register(SRC::PLAYER, Resource::TYPE::MODEL, Application::PATH_MODEL + "Player/Warrior.mv1");

	// プレイヤーの剣モデル
	Register(SRC::PLAYER_SWORD, Resource::TYPE::MODEL, Application::PATH_MODEL + "Player/sword.mv1");

	// プレイヤーのアニメーション
	Register(SRC::PLAYER_ANIM_IDLE, Resource::TYPE::MODEL, Application::PATH_MODEL + "Player/Animations/Idle.mv1");
	Register(SRC::PLAYER_ANIM_WALK, Resource::TYPE::MODEL, Application::PATH_MODEL + "Player/Animations/Walk.mv1");
	Register(SRC::PLAYER_ANIM_RUN, Resource::TYPE::MODEL, Application::PATH_MODEL + "Player/Animations/Run.mv1");
	Register(SRC::PLAYER_ANIM_ATTACK_1, Resource::TYPE::MODEL, Application::PATH_MODEL + "Player/Animations/Attack1.mv1");
	Register(SRC::PLAYER_ANIM_ROLL, Resource::TYPE::MODEL, Application::PATH_MODEL + "Player/Animations/Roll.mv1");
	Register(SRC::PLAYER_ANIM_HIT, Resource::TYPE::MODEL, Application::PATH_MODEL + "Player/Animations/Hit.mv1");
	Register(SRC::PLAYER_ANIM_DEATH, Resource::TYPE::MODEL, Application::PATH_MODEL + "Player/Animations/Death.mv1");
	Register(SRC::PLAYER_ANIM_KNOCKBACK, Resource::TYPE::MODEL, Application::PATH_MODEL + "Player/Animations/Knockback.mv1");
	Register(SRC::PLAYER_ANIM_STANDING, Resource::TYPE::MODEL, Application::PATH_MODEL + "Player/Animations/Standing.mv1");

	// ボスモデル
	Register(SRC::BOSS, Resource::TYPE::MODEL, Application::PATH_MODEL + "Boss/Wyvern.mv1");

	// ボスのアニメーション
	Register(SRC::BOSS_ANIM_BITE_1, Resource::TYPE::MODEL, Application::PATH_MODEL + "Boss/Animations/Bite01.mv1");
	Register(SRC::BOSS_ANIM_BITE_2, Resource::TYPE::MODEL, Application::PATH_MODEL + "Boss/Animations/Bite02.mv1");
	Register(SRC::BOSS_ANIM_BITE_3, Resource::TYPE::MODEL, Application::PATH_MODEL + "Boss/Animations/Bite03.mv1");
	Register(SRC::BOSS_ANIM_DEATH, Resource::TYPE::MODEL, Application::PATH_MODEL + "Boss/Animations/Death.mv1");
	Register(SRC::BOSS_ANIM_FIRE_BALL, Resource::TYPE::MODEL, Application::PATH_MODEL + "Boss/Animations/FireBall.mv1");
	Register(SRC::BOSS_ANIM_FLY_FIRE, Resource::TYPE::MODEL, Application::PATH_MODEL + "Boss/Animations/FlyFire.mv1");
	Register(SRC::BOSS_ANIM_IDLE_1, Resource::TYPE::MODEL, Application::PATH_MODEL + "Boss/Animations/Idle01.mv1");
	Register(SRC::BOSS_ANIM_IDLE_2, Resource::TYPE::MODEL, Application::PATH_MODEL + "Boss/Animations/Idle02.mv1");
	Register(SRC::BOSS_ANIM_IDLE_3, Resource::TYPE::MODEL, Application::PATH_MODEL + "Boss/Animations/Idle03.mv1");
	Register(SRC::BOSS_ANIM_IMPACT_STRONG, Resource::TYPE::MODEL, Application::PATH_MODEL + "Boss/Animations/ImpactStrong.mv1");
	Register(SRC::BOSS_ANIM_IMPACT_WEAK, Resource::TYPE::MODEL, Application::PATH_MODEL + "Boss/Animations/ImpactWeak.mv1");
	Register(SRC::BOSS_ANIM_REPOSE, Resource::TYPE::MODEL, Application::PATH_MODEL + "Boss/Animations/Repose.mv1");
	Register(SRC::BOSS_ANIM_RUN, Resource::TYPE::MODEL, Application::PATH_MODEL + "Boss/Animations/Run.mv1");
	Register(SRC::BOSS_ANIM_TAIL_1, Resource::TYPE::MODEL, Application::PATH_MODEL + "Boss/Animations/Tail01.mv1");
	Register(SRC::BOSS_ANIM_TAIL_2, Resource::TYPE::MODEL, Application::PATH_MODEL + "Boss/Animations/Tail02.mv1");
	Register(SRC::BOSS_ANIM_TAKEOFF, Resource::TYPE::MODEL, Application::PATH_MODEL + "Boss/Animations/Takeoff.mv1");
	Register(SRC::BOSS_ANIM_TORAND, Resource::TYPE::MODEL, Application::PATH_MODEL + "Boss/Animations/ToLand.mv1");
	Register(SRC::BOSS_ANIM_TURNBACK, Resource::TYPE::MODEL, Application::PATH_MODEL + "Boss/Animations/TurnBack.mv1");
	Register(SRC::BOSS_ANIM_TURNLEFT, Resource::TYPE::MODEL, Application::PATH_MODEL + "Boss/Animations/TurnLeft.mv1");
	Register(SRC::BOSS_ANIM_TURNRIGHT, Resource::TYPE::MODEL, Application::PATH_MODEL + "Boss/Animations/TurnRight.mv1");
	Register(SRC::BOSS_ANIM_WALK, Resource::TYPE::MODEL, Application::PATH_MODEL + "Boss/Animations/Walk.mv1");

	// ステージモデル
	Register(SRC::STAGE, Resource::TYPE::MODEL, Application::PATH_MODEL + "Stage/Stage.mv1");

	// タイトルロゴ
	Register(SRC::TITLE_LOGO, Resource::TYPE::IMG, Application::PATH_IMAGE + "CRUSHLOGO.png");

	// エフェクト
	Register(SRC::EFFECT_HIT_SPARK, Resource::TYPE::EFFEKSEER, Application::PATH_EFFECT + "HitSpark.efkefc");
	Register(SRC::EFFECT_RUSH_DUST, Resource::TYPE::EFFEKSEER, Application::PATH_EFFECT + "Dust.efkefc");
	Register(SRC::EFFECT_WINDUP_GLOW, Resource::TYPE::EFFEKSEER, Application::PATH_EFFECT + "WindupGlow.efkefc");
}

void ResourceManager::Release(void)
{
	for (auto& [src, res] : resources_)
	{
		res->Release();
	}
}

const Resource& ResourceManager::Load(SRC src)
{
	Resource* res = Find(src);

	// 登録されていない
	if (res == nullptr) return dummy_;

	res->Load();
	return *res;
}

int ResourceManager::LoadModelDuplicate(SRC src)
{
	Resource* res = Find(src);

	// 登録されていない
	if (res == nullptr) return -1;

	res->Load();
	return res->DuplicateModel();
}

void ResourceManager::Register(SRC src, Resource::TYPE type, const std::string& path)
{
	// 同じ名前で2回登録しない
	if (Find(src) != nullptr) return;

	resources_.emplace(src, new Resource(type, path));
}

Resource* ResourceManager::Find(SRC src) const
{
	const auto it = resources_.find(src);

	// 見つからない
	if (it == resources_.end()) return nullptr;

	return it->second;
}