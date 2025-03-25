#include "Transform.h"

FTransform2 FTransform2::Concatenate(const FSlateLayoutTransform& RHS) const
{
	return Concatenate(FTransform2(FVector2D(RHS.GetScale(), RHS.GetScale()), Translation));
}
