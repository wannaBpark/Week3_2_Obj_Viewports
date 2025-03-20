#pragma once
#include"Object/Actor/Actor.h"
#include"Object/PrimitiveComponent/UPrimitiveComponent.h"
#include "../EEnum.h"

class AWorldText;

class AWorldGizmo : public AActor
{
public:
	AWorldGizmo();
	virtual void Tick(float DeltaTime) override;

private:
	TArray<class UCylinderComp*> CylinderComponents;
	//TArray<AWorldText*> CharComponents;
};