#pragma once
#include <map>
#include "Resource.h"

class ResourceManager
{
public:

	// リソース名
	enum class SRC
	{
		PLAYER,
		BOSS,
		STAGE,
	};

	// シングルトン（生成・取得・削除）
	static void CreateInstance(void) { if (instance_ == nullptr) { instance_ = new ResourceManager(); } }
	static ResourceManager* GetInstance(void) { return instance_; }
	static void DeleteInstance(void) { if (instance_ != nullptr) { delete instance_; instance_ = nullptr; } }

	// 初期化
	void Init(void);

	// 解放(シーン切替時に一旦解放)
	void Release(void);

	// リソースのロード
	const Resource& Load(SRC src);

	// リソースの複製ロード(モデル用)
	int LoadModelDuplicate(SRC src);

private:

	// 静的インスタンス
	static ResourceManager* instance_;

	// 登録されているリソース
	std::map<SRC, Resource*> resources_;

	// 登録されていない名前を指定されたときに返す空のリソース
	Resource dummy_;

	// デフォルトコンストラクタをprivateにして、
	// 外部から生成できない様にする
	ResourceManager(void);

	// デストラクタも同様
	~ResourceManager(void);

	// コピー・ムーブ操作を禁止
	ResourceManager(const ResourceManager&) = delete;
	ResourceManager& operator=(const ResourceManager&) = delete;
	ResourceManager(ResourceManager&&) = delete;
	ResourceManager& operator=(ResourceManager&&) = delete;

	// リソースを1つ登録する
	void Register(SRC src, Resource::TYPE type, const std::string& path);

	// 登録されているリソースを探す(なければ nullptr)
	Resource* Find(SRC src) const;
};
