#pragma once
#include "Actor.h"
#include "Object/ObjectMacro.h"


class ACone : public AActor
{
    DECLARE_CLASS(ACone, AActor)
    using Super = AActor;

public:
    ACone();
    virtual ~ACone() = default;
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;
    virtual const char* GetTypeName() override;


};

