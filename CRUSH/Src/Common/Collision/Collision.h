#pragma once
#include <DxLib.h>

namespace Collision
{
    // ‹…‘Ì“¯m‚ÌÕ“Ë”»’è
    bool HitSpheres(const VECTOR& a, float ar, const VECTOR& b, float br);
    
    // ‹…‘Ì‚ÆƒJƒvƒZƒ‹‚ÌÕ“Ë”»’è
    bool HitSphereCapsule(
        const VECTOR& sphPos, float sphRadius,
        const VECTOR& capA, const VECTOR& capB, float capRadius);
}
