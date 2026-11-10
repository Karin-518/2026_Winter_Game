#pragma once
#include <DxLib.h>

class Shader
{
public:

	// シェーダーのパラメータを保持する構造体
	struct Vignette
	{
		float vignettePower = 0.0f;		// ビネットの鋭さ
		float padding1;			// 定数バッファを16バイト境界に揃えるため
		float padding2;			// 定数バッファを16バイト境界に揃えるため
		float padding3;			// 定数バッファを16バイト境界に揃えるため
	};

	// デフォルト値の初期化
	static constexpr float DEFAULT_VIGNETTE_POWER = 0.0f;		// ビネットの鋭さ

	static constexpr Vignette DEFAULT_PARAM = {
		DEFAULT_VIGNETTE_POWER,
		0.0f,
		0.0f,
		0.0f
	};

	// ダメージ時のビネットの鋭さ
	static constexpr float DAMAGE_VIGNETTE_POWER = 0.3f;

	// コンストラクタ
	Shader(void);

	// デストラクタ
	~Shader(void);

	// 初期化
	void Init(void);

	// 描画
	void Draw(int texture);

	// 解放
	void Release(void);

	// ビネットの鋭さの変更
	void SetVignettePower(float val) { targetCrtParam_.vignettePower = val; }

private:

	// 頂点数
	static constexpr int SQUARE_VERTEX_NUM = 4;

	// インデックス数
	static constexpr int SQUARE_INDEX_NUM = 6;

	// 補間の速さ(値が大きいほど素早く目標値に到達)
	static constexpr float LERP_SPEED = 0.05f;

	// シェーダのパラメータ
	// 目標値、セッターで変更する
	Vignette targetCrtParam_ = {};

	// 現在値、シェーダーへ渡す
	Vignette currentCrtParam_ = {};

	// 頂点情報
	// 頂点座標、色、テクスチャ座標を保持する構造体
	VERTEX2DSHADER vertex_[SQUARE_VERTEX_NUM] = {};

	// 頂点インデックス
	WORD index_[SQUARE_INDEX_NUM] = {};

	// ピクセルシェーダーのハンドル
	int psCrt_ = -1;

	// ピクセルシェーダー用の定数バッファのハンドル
	int psCrtConstBuf_ = -1;

	// 描画用の四角頂点を作成
	void MakeSquareVertex(void);

	// パラメータを補間する
	void UpdateParam(float& current, float target);
};