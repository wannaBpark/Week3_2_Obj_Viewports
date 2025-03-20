#include "Camera.h"

#include "Core/Rendering/URenderer.h"
#include "Object/PrimitiveComponent/UPrimitiveComponent.h"


ACamera::ACamera()
{

    bIsGizmo = true;
    
    Near = 0.1f;
    Far = 100.f;
    FieldOfView = 45.f;
    ViewportSize = 10.f;
    ProjectionMode = ECameraProjectionMode::Perspective;

    RootComponent = AddComponent<USceneComponent>();
    
    FTransform StartPos = GetActorTransform();
    StartPos.SetPosition(FVector(-5, 0, 0));
    SetActorTransform(StartPos);
}


void ACamera::SetFieldOfVew(float Fov)
{
    FieldOfView = Fov;
}

void ACamera::SetViewportSize(float inViewprotSize)
{
    ViewportSize = inViewprotSize;
}

void ACamera::SetFar(float Far)
{
    this->Far = Far;
}

void ACamera::SetNear(float Near)
{
    this->Near = Near;
}

float ACamera::GetFieldOfView() const
{
    return  FieldOfView;
}

float ACamera::GetViewportSize() const
{
    return ViewportSize;
}

float ACamera::GetNear() const
{
    return Near;
}

float ACamera::GetFar() const
{
    return Far;
}
