// ピクセルシェーダーの入力
struct PS_INPUT
{
    // 座標( プロジェクション空間 )
    float4 Position : SV_POSITION;
    
    // ディフューズカラー
    float4 Diffuse : COLOR0;
	
    // テクスチャ座標
    float2 TexCoords0 : TEXCOORD0;
};

// UVを中央にするための位置
static const float UV_CENTER = 0.5f;

// 中心からの距離を0～1にするための倍率
static const float CENTER_DIST_SCALE = 2.0f;

static const float EDGE_EXPONENT = 5.0f;

cbuffer cbParam : register(b1)
{
    float g_vignettePower;          // ビネットの鋭さ
    float3 g_padding;
}

// 描画するテクスチャ
Texture2D g_SrcTexture : register(t0);

// サンプラー：適切な色を決める処理
SamplerState g_SrcSampler : register(s0);

float4 main(PS_INPUT input) : SV_TARGET
{
    // テクスチャ座標を取得
    float2 uv = input.TexCoords0;
   
    // 中心を0.0にする
    float2 center = (uv - UV_CENTER);
   
    // CENTER_DIST_SCALE を掛けて0.0～1.0にする
    float x = abs(center.x) * CENTER_DIST_SCALE;
    float y = abs(center.y) * CENTER_DIST_SCALE;
    
    // テクスチャの色を取得
    float4 color = g_SrcTexture.Sample(g_SrcSampler, uv);
    
    float edgeX = pow(x, EDGE_EXPONENT);
    float edgeY = pow(y, EDGE_EXPONENT);
    
    // ビネットの計算
    float vignette = saturate(1.0f - edgeX * g_vignettePower)
                   * saturate(1.0f - edgeY * g_vignettePower);
    
    // ビネットを適用
    color.gb *= vignette;

    // 最終的な色を返す
    return color;
}