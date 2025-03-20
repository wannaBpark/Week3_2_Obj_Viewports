#pragma once
#include "Object/Actor/Actor.h"
#include "Object/PrimitiveComponent/UPrimitiveComponent.h"
#include "Object/ObjectMacro.h"


class AAxis : public AActor
{
	DECLARE_CLASS(AAxis, AActor)
	using Super = AActor;

public:
	AAxis();
	virtual ~AAxis() = default;

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual const char* GetTypeName() override;
};

