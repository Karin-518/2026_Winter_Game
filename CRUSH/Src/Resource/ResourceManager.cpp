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
	// プレイヤーのアニメーション
	Register(SRC::PLAYER_ANIM_IDLE, Resource::TYPE::MODEL, Application::PATH_MODEL + "Player/Idle.mv1");
	Register(SRC::PLAYER_ANIM_WALK, Resource::TYPE::MODEL, Application::PATH_MODEL + "Player/Walk.mv1");
	Register(SRC::PLAYER_ANIM_RUN, Resource::TYPE::MODEL, Application::PATH_MODEL + "Player/Run.mv1");
	Register(SRC::PLAYER_ANIM_ATTACK_1, Resource::TYPE::MODEL, Application::PATH_MODEL + "Player/Attack1.mv1");
	Register(SRC::PLAYER_ANIM_ROLL, Resource::TYPE::MODEL, Application::PATH_MODEL + "Player/Roll.mv1");

	// ボスモデル
	Register(SRC::BOSS, Resource::TYPE::MODEL, Application::PATH_MODEL + "Boss/Dragon.mv1");

	// ステージモデル
	Register(SRC::STAGE, Resource::TYPE::MODEL, Application::PATH_MODEL + "Stage/Stage.mv1");

	Register(SRC::TITLE_LOGO, Resource::TYPE::IMG, Application::PATH_IMAGE + "CRUSHLOGO.png");

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