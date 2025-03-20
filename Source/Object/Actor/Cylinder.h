#pragma once
#include "Actor.h"
#include "Object/ObjectMacro.h"

class ACylinder : public AActor
{
    DECLARE_CLASS(ACylinder, AActor)
    using Super = AActor;
    
public:
    ACylinder();
    virtual ~ACylinder() = default;
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;
    virtual const char* GetTypeName() override;

};

