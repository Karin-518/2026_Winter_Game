#include <DxLib.h>
#include <EffekseerForDXLib.h>
#include "Resource.h"

namespace
{
	// ハンドルを持っていないことを表す値
	constexpr int INVALID_HANDLE = -1;
}


Resource::Resource(void)
	:
	Resource(TYPE::NONE, "")
{
}

Resource::Resource(TYPE type, const std::string& path)
	:
	Resource(type, path, 0, 0, 0, 0)
{
}

Resource::Resource(TYPE type, const std::string& path, int numX, int numY, int sizeX, int sizeY)
	:
	type_(type),
	path_(path),
	handleId_(INVALID_HANDLE),
	numX_(numX),
	numY_(numY),
	sizeX_(sizeX),
	sizeY_(sizeY)
{
}

Resource::~Resource(void)
{
}

void Resource::Load(void)
{
	// 読み込み済みなら何もしない
	if (IsLoaded()) return;

	switch (type_)
	{
	case Resource::TYPE::IMG:
		// 画像
		handleId_ = LoadGraph(path_.c_str());
		break;

	case Resource::TYPE::IMGS:
		LoadImgs();
		break;

	case Resource::TYPE::MODEL:
		// モデル
		handleId_ = MV1LoadModel(path_.c_str());
		break;

	case Resource::TYPE::EFFEKSEER:

		handleId_ = LoadEffekseerEffect(path_.c_str());
		break;

	default:
		break;
	}
}

void Resource::Release(void)
{
	if (!IsLoaded()) return;

	switch (type_)
	{
	case Resource::TYPE::IMG:
		DeleteGraph(handleId_);
		break;

	case Resource::TYPE::IMGS:
		ReleaseImgs();
		break;

	case Resource::TYPE::MODEL:
		ReleaseModel();
		break;

	case Resource::TYPE::EFFEKSEER:

		DeleteEffekseerEffect(handleId_);
		break;

	default:
		break;
	}

	handleId_ = INVALID_HANDLE;
}

int Resource::DuplicateModel(void)
{
	// モデル以外は複製できない
	if (type_ != TYPE::MODEL) return INVALID_HANDLE;

	const int duplicateId = MV1DuplicateModel(handleId_);
	duplicateModelIds_.push_back(duplicateId);

	return duplicateId;
}

void Resource::CopyHandle(int* imgs) const
{
	for (int i = 0; i < static_cast<int>(handleIds_.size()); i++)
	{
		imgs[i] = handleIds_[i];
	}
}

bool Resource::IsLoaded(void) const
{
	return handleId_ != INVALID_HANDLE || !handleIds_.empty();
}

void Resource::LoadImgs(void)
{
	const int allNum = numX_ * numY_;

	handleIds_.resize(allNum);

	LoadDivGraph(
		path_.c_str(), 
		allNum, 
		numX_, 
		numY_, 
		sizeX_, 
		sizeY_, 
		handleIds_.data());
}

void Resource::ReleaseImgs(void)
{
	for (int handle : handleIds_)
	{
		DeleteGraph(handle);
	}
	handleIds_.clear();
}

void Resource::ReleaseModel(void)
{
	// 複製したモデルから解放する
	for (int id : duplicateModelIds_)
	{
		MV1DeleteModel(id);
	}

	duplicateModelIds_.clear();

	// 元のモデルを解放する
	MV1DeleteModel(handleId_);
}
