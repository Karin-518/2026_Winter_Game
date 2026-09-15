#include "EnemyBase.h"

EnemyBase::~EnemyBase()
{
	if (modelHandle_ != -1)	MV1DeleteModel(modelHandle_);
}
