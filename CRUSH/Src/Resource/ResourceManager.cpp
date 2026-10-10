#include <DxLib.h>
#include "../Application.h"
#include "ResourceManager.h"

ResourceManager* ResourceManager::instance_ = nullptr;

ResourceManager::ResourceManager(void)
{
}

ResourceManager::~ResourceManager(void)
{
	// “Ç‚Ýž‚ñ‚¾‚à‚Ì‚ð‰ð•ú‚µ‚Ä‚©‚çA“o˜^‚ðÁ‚·
	Release();

	for (auto& [src, res] : resources_)
	{
		delete res;
	}
	resources_.clear();
}

void ResourceManager::Init(void)
{
	// ƒvƒŒƒCƒ„[ƒ‚ƒfƒ‹
	Register(SRC::PLAYER, Resource::TYPE::MODEL, Application::PATH_MODEL + "Player/Player.mv1");

	// ƒXƒe[ƒWƒ‚ƒfƒ‹
	Register(SRC::STAGE, Resource::TYPE::MODEL, Application::PATH_MODEL + "Stage/Stage.mv1");
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

	// “o˜^‚³‚ê‚Ä‚¢‚È‚¢
	if (res == nullptr) return dummy_;

	res->Load();
	return *res;
}

int ResourceManager::LoadModelDuplicate(SRC src)
{
	Resource* res = Find(src);

	// “o˜^‚³‚ê‚Ä‚¢‚È‚¢
	if (res == nullptr) return -1;

	res->Load();
	return res->DuplicateModel();
}

void ResourceManager::Register(SRC src, Resource::TYPE type, const std::string& path)
{
	// “¯‚¶–¼‘O‚Å2‰ñ“o˜^‚µ‚È‚¢
	if (Find(src) != nullptr) return;

	resources_.emplace(src, new Resource(type, path));
}

Resource* ResourceManager::Find(SRC src) const
{
	const auto it = resources_.find(src);

	// Œ©‚Â‚©‚ç‚È‚¢
	if (it == resources_.end()) return nullptr;

	return it->second;
}
