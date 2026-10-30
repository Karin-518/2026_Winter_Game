#include <DxLib.h>
#include "Loading/Loading.h"
#include "TitleScene/TitleScene.h"
#include "../Resource/ResourceManager.h"

#include "SceneManager.h"

SceneManager* SceneManager::instance_ = nullptr;

SceneManager::SceneManager(void)
	:
	scenes_(),
	load_(nullptr),
	isGameEnd_(false),
	request_(REQUEST::NONE),
	nextScene_(nullptr)
{
}

SceneManager::~SceneManager(void)
{
}

void SceneManager::Init(void)
{
	// ロード画面生成
	load_ = new Loading();
	load_->Init();
	load_->Load();

	// 3D情報の初期化
	Init3D();

	// 最初はタイトル画面から
	ChangeScene(std::make_shared<TitleScene>());
}

void SceneManager::Init3D(void)
{
	// 背景色設定
	SetBackgroundColor(0, 0, 0);

	// Zバッファを有効にする
	SetUseZBuffer3D(true);

	// Zバッファへの書き込みを有効にする
	SetWriteZBuffer3D(true);

	// バックカリングを有効にする
	SetUseBackCulling(true);

	// ライトの設定
	SetUseLighting(true);

	// 正面から斜め下に向かったライト
	ChangeLightTypeDir({ 0.00f, -1.00f, 1.00f });
}

// 更新
void SceneManager::Update(void)
{
	ApplyRequest();

	// シーンがなければ終了
	if (scenes_.empty()) { return; }

	// ロード中
	if (load_->IsLoading())
	{
		// ロード更新
		load_->Update();

		// ロードの更新が終了していたら
		if (!load_->IsLoading())
		{
			// ロード後の初期化
			scenes_.back()->LoadEnd();
		}

		return;
	}

	// 現在のシーンの更新
	scenes_.back()->Update();
}

void SceneManager::Draw(void)
{
	// ロード中ならロード画面を描画
	if (load_->IsLoading())
	{
		// ロードの描画
		load_->Draw();
		return;
	}

	// 積まれているもの全てを描画する
	for (auto& scene : scenes_)
	{
		// シーンの描画
		scene->Draw();
	}
}

void SceneManager::Delete(void)
{
	// 全てのシーンの解放・削除
	for (auto& scene : scenes_) { scene->Release(); }
	scenes_.clear();

	// 実行されなかった予約を捨てる
	request_ = REQUEST::NONE;
	nextScene_ = nullptr;

	// ロード画面の削除
	load_->Release();
	delete load_;
	load_ = nullptr;
}

void SceneManager::ChangeScene(std::shared_ptr<SceneBase> scene)
{
	Request(REQUEST::CHANGE, scene);
}

void SceneManager::PushScene(std::shared_ptr<SceneBase> scene)
{
	Request(REQUEST::PUSH, scene);
}

void SceneManager::PopScene(void)
{
	Request(REQUEST::POP, nullptr);
}

void SceneManager::JumpScene(std::shared_ptr<SceneBase> scene)
{
	Request(REQUEST::JUMP, scene);
}

void SceneManager::Request(REQUEST request, std::shared_ptr<SceneBase> scene)
{
	// 同じフレームに2つ目の予約が来たら、最初の予約を優先する
	if (request_ != REQUEST::NONE) return;

	request_ = request;
	nextScene_ = scene;
}

void SceneManager::ApplyRequest(void)
{
	if (request_ == REQUEST::NONE) return;

	// 実行中に新しい予約が入っても消さないよう、先に取り出して空にする
	REQUEST request = request_;
	std::shared_ptr<SceneBase> scene = nextScene_;
	request_ = REQUEST::NONE;
	nextScene_ = nullptr;

	switch (request)
	{
	case REQUEST::CHANGE:
		ApplyChange(scene);
		break;
	case REQUEST::PUSH:
		ApplyPush(scene);
		break;
	case REQUEST::POP:
		ApplyPop();
		break;
	case REQUEST::JUMP:
		ApplyJump(scene);
		break;
	default:
		break;
	}
}

void SceneManager::ApplyChange(std::shared_ptr<SceneBase> scene)
{
	// 最初のシーンなら、そのまま積む
	if (scenes_.empty())
	{
		scenes_.push_back(scene);
		LoadBackScene();
		return;
	}

	// 一番上のシーンを、新しいシーンに入れ替える
	scenes_.back()->Release();
	scenes_.back() = scene;

	// 前のシーンで読み込んだ画像・モデルをまとめて解放
	ResourceManager::GetInstance()->Release();

	LoadBackScene();
}

void SceneManager::ApplyPush(std::shared_ptr<SceneBase> scene)
{
	// 下のシーンは残したまま積む
	scenes_.push_back(scene);

	// 同期で読み込んで、すぐ初期化する
	scenes_.back()->Load();
	scenes_.back()->LoadEnd();
}

void SceneManager::ApplyPop(void)
{
	if (scenes_.empty()) return;

	// 一番上を消して、下のシーンに戻る
	scenes_.back()->Release();
	scenes_.pop_back();
}

void SceneManager::ApplyJump(std::shared_ptr<SceneBase> scene)
{
	// 積んであるシーンを全て解放
	for (auto& s : scenes_) { s->Release(); }
	scenes_.clear();

	// 前のシーンで読み込んだ画像・モデルをまとめて解放
	ResourceManager::GetInstance()->Release();

	// 新しいシーンだけにして読み込む
	scenes_.push_back(scene);
	LoadBackScene();
}

void SceneManager::LoadBackScene(void)
{
	// 読み込み。終わったら Update で LoadEnd が呼ばれる
	load_->StartAsyncLoad();
	scenes_.back()->Load();
	load_->EndAsyncLoad();
}