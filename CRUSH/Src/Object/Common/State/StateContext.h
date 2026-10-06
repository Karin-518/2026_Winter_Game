#pragma once
#include <map>
#include <memory>
#include <utility>
#include "StateBase.h"

template <typename TOwner, typename TStateId>

class StateContext
{
public:

	// ステートの型
	using State = StateBase<TOwner, TStateId>;

	// 持ち主を受け取る
	explicit StateContext(TOwner& owner) : owner_(owner) {}

	/// <summary>
	/// ステートを登録
	/// </summary>
	/// <param name="id">ステートID</param>
	/// <param name="state">実体</param>
	void Register(TStateId id, std::unique_ptr<State>state)
	{ 
		states_[id] = std::move(state);
	}

	/// <summary>
	/// 最初のステートを設定
	/// </summary>
	/// <param name="id">ステートID</param>
	void Start(TStateId id)
	{
		ChangeState(id);
	}

	// 更新
	void Update(void)
	{
		if (currentState_ == nullptr) return;

		// 今のステートを更新して、次のステートIDを取得
		TStateId nextId = currentState_->Update(owner_);

		// 外部からリクエストがあれば、優先
		if (nextId_ != TStateId::NONE)
		{
			nextId = nextId_;
			nextId_ = TStateId::NONE;
		}

		// ステートの切り替え
		if (nextId != TStateId::NONE)
		{
			ChangeState(nextId);
		}
	}

	/// <summary>
	/// 外部から切り替えを依頼
	/// </summary>
	/// <param name="id">ステートID</param>
	void RequestState(TStateId id)
	{
		nextId_ = id;
	}

	// 現在のステートIDを取得
	TStateId GetStateId(void) const { return currentId_; }

private:

	/// <summary>
	/// 切り替え
	/// </summary>
	/// <param name="id">ステートID</param>
	void ChangeState(TStateId id)
	{
		// IDが登録されていなければ、何もしない
		auto it = states_.find(id);
		if (it == states_.end()) return;

		//現在のステートから抜ける
		if (currentState_ != nullptr)
		{
			currentState_->Exit(owner_);
		}

		// 切り替えて、新しいステートに入る
		currentState_ = it->second.get();
		currentId_ = id;
		currentState_->Enter(owner_);
	}

	// 持ち主
	TOwner& owner_;

	// ステートのマップ
	std::map<TStateId, std::unique_ptr<State>> states_;

	// 現在のステート
	State* currentState_ = nullptr;

	// 現在のステートID
	TStateId currentId_ = TStateId::NONE;

	// 次のステートID
	TStateId nextId_ = TStateId::NONE;
};