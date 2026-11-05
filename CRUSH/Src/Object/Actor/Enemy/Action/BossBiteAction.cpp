#include "../../../Common/AnimationController/AnimationController.h"
#include "../Boss.h"

#include "BossBiteAction.h"

bool BossBiteAction::UpdateActive(Boss& owner)
{
    // アニメが最後まで再生されたら、攻撃本体を終える
    return owner.GetAnimationController()->IsEnd();
}