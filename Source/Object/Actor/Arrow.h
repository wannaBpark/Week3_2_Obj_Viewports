#pragma once
#include "Object/Actor/Actor.h"
#include "Object/ObjectMacro.h"

class AArrow : public AActor
{
	DECLARE_CLASS(AArrow, AActor)
	using Super = AActor;

public:
	AArrow();
	virtual ~AArrow() = default;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual const char* GetTypeName() override;


};

