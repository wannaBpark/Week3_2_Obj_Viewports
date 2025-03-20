#pragma once
#include "Actor.h"
#include "Object/ObjectMacro.h"


class AWorldGrid : public AActor
{
	DECLARE_CLASS(AWorldGrid, AActor)
	using Super = AActor;
	
public:
	AWorldGrid();
	virtual ~AWorldGrid() = default;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual const char* GetTypeName() override;

};

