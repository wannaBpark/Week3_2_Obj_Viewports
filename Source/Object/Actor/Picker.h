#pragma once

#include "Actor.h"
#include "Object/ObjectMacro.h"

class UBoundingBoxComp;
class UStringComponent;

class APicker : public AActor
{
    DECLARE_CLASS(APicker, AActor)
    using Super = AActor;
    
public:
    APicker();
    ~APicker() = default;
    
    static FVector4 EncodeUUID(unsigned int UUID);
    static uint32_t DecodeUUID(FVector4 color);
    
    virtual void Tick(float DeltaTime) override;
    virtual void LateTick(float DeltaTime) override;
    virtual const char* GetTypeName() override;
    
    static bool SetSelectActor(const UActorComponent* actorComponent);
	static bool SetSelectedComponent(USceneComponent* actorComponent);
private:
    static POINT GetMousePoint();
    UActorComponent* GetAcotrByPixelPicking(const POINT& pt);
    static void PickLocalGizmo(UActorComponent* actor);
    static void PickWorldGizmo();

public:
    UBoundingBoxComp* GetBoundingBoxComp() { return BoundingBoxComp; }
    UStringComponent* GetStringComponent() { return StringComponent; }

private:
    UBoundingBoxComp* BoundingBoxComp;
    UStringComponent* StringComponent;
};
