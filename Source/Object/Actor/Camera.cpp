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
    
    FTransform Transform = GetActorTransform();
    Transform.SetPosition(FVector(-5, 0, 0));
    SetActorTransform(Transform);
}

ACamera::ACamera(FVector Position, FVector Rotation, ECameraProjectionMode::Type ProjMode)
{
    Near = 0.1f;
    Far = 100.f;
    FieldOfView = 45.f;
    ViewportSize = 10.f;
    ProjectionMode = ECameraProjectionMode::Perspective;

    RootComponent = AddComponent<USceneComponent>();

    FTransform Transform = GetActorTransform();
    Transform.SetPosition(Position);
    Transform.SetRotation(Rotation);
    SetActorTransform(Transform);
}


void ACamera::SetFieldOfVew(float Fov)
{
    FieldOfView = Fov;
}

void ACamera::SetViewportSize(float inViewprotSize)
{
    ViewportSize = inViewprotSize;
}

void ACamera::SetViewportSize(float InWidth, float InHeight)
{
    Width = InWidth;
    Height = InHeight;
}

void ACamera::SetFar(float Far)
{
    this->Far = Far;
}

void ACamera::SetNear(float Near)
{
    this->Near = Near;
}

void ACamera::SetProjectionMode(ECameraProjectionMode::Type InMode)
{
    this->ProjectionMode = InMode;
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

float ACamera::GetAspectRatio() const
{
    return Width / Height;
}

float ACamera::GetWidth() const
{
    return Width;
}

float ACamera::GetHeight() const
{
    return Height;
}

ECameraProjectionMode::Type ACamera::GetProjectionMode() const
{
    return this->ProjectionMode;
}
