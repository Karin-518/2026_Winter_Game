#include "../Application.h"
#include "../Scene/SceneManager.h"

#include "Shader.h"

namespace
{
	// パラメータが目標値に到達したとみなす誤差
	constexpr float PARAM_EPSILON = 0.0001f;

	// 描画元テクスチャのスロット番号
	constexpr int SRC_TEXTURE_SLOT = 0;

	// 定数バッファのスロット番号
	constexpr int CRT_CBUFFER_SLOT = 1;

	// 四角形を構成する三角形ポリゴンの数
	constexpr int SQUARE_POLYGON_NUM = 2;
}

Shader::Shader(void)
{
}

Shader::~Shader(void)
{
}

void Shader::Init(void)
{
	// ピクセルシェーダーの読み込み
	psCrt_ = LoadPixelShader("Data/Shader/vignette.pso");

	// ピクセルシェーダー用の定数バッファ
	psCrtConstBuf_ = CreateShaderConstantBuffer(sizeof(Vignette));

	// 初期値のセット
	targetCrtParam_ = DEFAULT_PARAM;	// 目標値
	currentCrtParam_ = DEFAULT_PARAM;	// 現在値

	// ポリゴン生成
	MakeSquareVertex();
}

void Shader::Draw(int texture)
{
	// ビネット
	UpdateParam(currentCrtParam_.vignettePower, targetCrtParam_.vignettePower);

	// ピクセルシェーダーをセット
	SetUsePixelShader(psCrt_);

	// テクスチャをセット
	SetUseTextureToShader(SRC_TEXTURE_SLOT, texture);

	// ピクセルシェーダー用の定数バッファのアドレスを取得
	Vignette* cbBuf = (Vignette*)GetBufferShaderConstantBuffer(psCrtConstBuf_);
	*cbBuf = currentCrtParam_;

	// ピクセルシェーダー用の定数バッファを更新して書き込んだ内容を反映
	UpdateShaderConstantBuffer(psCrtConstBuf_);

	// ピクセルシェーダー用の定数バッファを定数バッファレジスタにセット
	SetShaderConstantBuffer(psCrtConstBuf_, DX_SHADERTYPE_PIXEL, CRT_CBUFFER_SLOT);

	// 画面外を黒枠にする
	SetTextureAddressModeUV(DX_TEXADDRESS_BORDER, DX_TEXADDRESS_BORDER);

	// 描画
	DrawPolygonIndexed2DToShader(vertex_, SQUARE_VERTEX_NUM, index_, SQUARE_POLYGON_NUM);
}

void Shader::Release(void)
{
	// ピクセルシェーダーの解放
	DeleteShader(psCrt_);

	// ピクセルシェーダー用の定数バッファの解放
	DeleteShaderConstantBuffer(psCrtConstBuf_);
}

void Shader::MakeSquareVertex(void)
{
	// ４頂点の初期化
	for (int i = 0; i < SQUARE_VERTEX_NUM; i++)
	{
		vertex_[i].rhw = 1.0f;
		vertex_[i].dif = GetColorU8(255, 255, 255, 255);
		vertex_[i].spc = GetColorU8(255, 255, 255, 255);
		vertex_[i].su = 0.0f;
		vertex_[i].sv = 0.0f;
	}

	// シェーダー追加時の作業を減らすため、あらかじめ四角形の頂点を作成
	int cnt = 0;
	float sX = 0;
	float sY = 0;
	float eX = static_cast<float>(Application::SCREEN_SIZE_X);
	float eY = static_cast<float>(Application::SCREEN_SIZE_Y);

	// 左上
	vertex_[cnt].pos = VGet(sX, sY, 0.0f);
	vertex_[cnt].u = 0.0f;
	vertex_[cnt].v = 0.0f;
	cnt++;

	// 右上
	vertex_[cnt].pos = VGet(eX, sY, 0.0f);
	vertex_[cnt].u = 1.0f;
	vertex_[cnt].v = 0.0f;
	cnt++;

	// 右下
	vertex_[cnt].pos = VGet(eX, eY, 0.0f);
	vertex_[cnt].u = 1.0f;
	vertex_[cnt].v = 1.0f;
	cnt++;

	// 左下
	vertex_[cnt].pos = VGet(sX, eY, 0.0f);
	vertex_[cnt].u = 0.0f;
	vertex_[cnt].v = 1.0f;

	// 頂点インデックス
	cnt = 0;
	index_[cnt++] = 0;
	index_[cnt++] = 1;
	index_[cnt++] = 3;

	index_[cnt++] = 1;
	index_[cnt++] = 2;
	index_[cnt++] = 3;
}

void Shader::UpdateParam(float& current, float target)
{
	// 目標値と現在値の差が0.0001未満なら、現在値を目標値に合わせる
	if (fabsf(target - current) < PARAM_EPSILON)
	{
		// 目標値と現在値を同期させる
		current = target;
	}
	else
	{
		// 線形補間で現在値を目標値に近づける
		current += (target - current) * LERP_SPEED;
	}
}