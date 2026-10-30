#pragma once

class HpBar
{
public:

	// コンストラクタ
	HpBar(int posX, int posY, int width, int height, unsigned int color);

	// デストラクタ
	~HpBar(void);

	// 更新
	void Update(int hp, int maxHp);

	// 描画
	void Draw(void) const;

private:

	// 左上のX座標
	int posX_;

	// 左上のY座標
	int posY_;

	// バー全体の幅
	int width_;

	// バーの高さ
	int height_;

	// 残りHPの色
	unsigned int color_;

	// 残りHPの割合（0.0～1.0）
	float rate_;
};