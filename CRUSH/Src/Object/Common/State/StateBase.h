#pragma once

//持ち主の型とステートIDの型をテンプレートで受け取る
template <typename TOwner, typename TStateId>

class StateBase
{
public:

	// デストラクタ
	virtual ~StateBase(void) = default;

	/// <summary>
	/// ステートに入ったときに1度だけ呼ばれる
	/// </summary>
	/// <param name="owner">持ち主</param>
	virtual void Enter(TOwner& owner) {}

	/// <summary>
	/// 次に切り替えたいステートIDを返す
	/// </summary>
	/// <param name="owner">持ち主</param>
	/// <returns>ステートID</returns>
	virtual TStateId Update(TOwner& owner) = 0;

	/// <summary>
	/// ステートから出るときに1度だけ呼ばれる
	/// </summary>
	/// <param name="owner">持ち主</param>
	virtual void Exit(TOwner& owner) {}
};