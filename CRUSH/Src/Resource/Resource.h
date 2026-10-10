#pragma once
#include <string>
#include <vector>

class Resource
{
public:
	
	// リソースタイプ
	enum class TYPE
	{
		NONE,
		IMG,
		IMGS,
		MODEL,
		EFFEKSEER,
	};

	// コンストラクタ
	Resource(void);
	// コンストラクタ
	Resource(TYPE type, const std::string& path);
	// コンストラクタ(IMGS用)
	Resource(TYPE type, const std::string& path, int numX, int numY, int sizeX, int sizeY);

	// デストラクタ
	~Resource(void);

	// 読み込み
	void Load(void);

	// 解放
	void Release(void);

	// モデルを複製して、そのハンドルを返す
	int DuplicateModel(void);

	// 複数画像ハンドルを別配列にコピー
	void CopyHandle(int* imgs) const;

	// 種類
	TYPE GetType(void) const { return type_; }

	// 画像・モデルのハンドル
	int GetHandleId(void) const { return handleId_; }

	// 読み込み済みか
	bool IsLoaded(void) const;

private:

	// リソースタイプ
	TYPE type_;

	// リソースの読み込み先
	std::string path_;

	// 画像とモデルのハンドルID
	int handleId_;

	// 分割画像のハンドル
	std::vector<int> handleIds_;

	// 分割画像の分割数とサイズ
	int numX_;
	int numY_;
	int sizeX_;
	int sizeY_;

	// モデル複製用
	std::vector<int> duplicateModelIds_;

	// 分割画像の読み込み
	void LoadImgs(void);

	// 分割画像の解放
	void ReleaseImgs(void);

	// モデル(複製を含む)の解放
	void ReleaseModel(void);
};
