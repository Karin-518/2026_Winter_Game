#include "EnemyPool.h"

EnemyPool::EnemyPool(EnemyFactory* factory, PoolMode mode)
    :   factory_(factory)
    ,   mode_(mode)
{
}

EnemyPool::~EnemyPool()
{
    Clear();
}

void EnemyPool::PreAllocate(ENEMY_TYPE type, size_t count)
{
    auto& pool = pools_[type];
   
    // 渡された数、確保する
    for (size_t i = 0; i < count; ++i)
    {
        // ファクトリーから複製を作成
        pool.push(factory_->Create(type));
    }

    // 各タイプの最大数を記憶
    maxSizes_[type] = count;
}

EnemyBase* EnemyPool::Acquire(ENEMY_TYPE type)
{
    auto& pool = pools_[type];

    // プールに残っているので再利用
    if (!pool.empty())
    {
        // プールから抜き出す
        EnemyBase* enemy = pool.top();
        pool.pop();

        // 初期状態に戻す
        enemy->Init();
        return enemy;
    }

    // プールが空。
    // モードによって挙動を変える
    switch (mode_)
    {
        // 新規作成
    case PoolMode::EXPANDABLE:
        return factory_->Create(type);
        break;
        // 生成拒否
    case PoolMode::LIMITED:
        return nullptr;
        break;
        // 再利用
    case PoolMode::RECYCLE:
        return ForceRecycle(type);
        break;
    default:
        return nullptr;
        break;
    }
}

void EnemyPool::Release(ENEMY_TYPE type, EnemyBase* enemy)
{
    // プールに返却する(再利用するため)
    pools_[type].push(enemy);
}

void EnemyPool::Clear(void)
{
    // 全削除
    for (auto& p : pools_)
    {
        auto& stack = p.second;

        while (!stack.empty())
        {
            delete stack.top();
            stack.pop();
        }
    }
}

EnemyBase* EnemyPool::ForceRecycle(ENEMY_TYPE type)
{
    // "死んでない敵の中からどれかを強制退場させたい" ため、
    // EnemyManager に「現在使用中の敵」を問い合わせる必要あり。
    // シンプル版：最大数を上回れば新規生成禁止（LIMITED と同等挙動）

    // 上限なし
    if (maxSizes_.count(type) == 0)
        return nullptr;

    return nullptr;
}
