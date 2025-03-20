#pragma once
#include "Object/Actor/Actor.h"
#include "Object/ObjectMacro.h"

class ATriangle : public AActor
{
    DECLARE_CLASS(ATriangle, AActor)
    using Super = AActor;
    
public:
    ATriangle();
    virtual ~ATriangle() = default;
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;
    virtual const char* GetTypeName() override;

};