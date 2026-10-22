#pragma once

class HpBar
{
public:

	// コンストラクタ
	HpBar(int posX, int posY, int width, int height, unsigned int color);

	// デストラクタ
	~HpBar(void);

	void Update(int hp, int maxHp);	// 更新
	void Draw(void) const;			// 描画

private:

	int posX_;				// 左上のX座標
	int posY_;				// 左上のY座標
	int width_;				// バー全体の幅
	int height_;			// バーの高さ
	unsigned int color_;	// 残りHPの色

	float rate_;			// 残りHPの割合（0.0～1.0）
};