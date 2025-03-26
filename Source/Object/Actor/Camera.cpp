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

void ACamera::Tick(float DeltaTime)
{
    if (this->ProjectionMode == ECameraProjectionMode::Orthographic)
    {
        FTransform CamTransform = GetActorTransform();
        FVector Dir = Pivot/* - CamTransform.GetPosition()*/;
        // orthogonal 카메라가 너무 안움직여서 하드코딩함
        CamTransform.Translate (Dir * DeltaTime * CameraSpeed * 10.f); 
        SetActorTransform(CamTransform);
    }
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

FMatrix ACamera::GetProjectionMatrix() const
{
    FMatrix ProjectionMatrix;
    if (ProjectionMode == ECameraProjectionMode::Perspective)
    {
        ProjectionMatrix = FMatrix::PerspectiveFovLH(FieldOfView, AspectRatio, Near, Far);
    }
    else if (ProjectionMode == ECameraProjectionMode::Orthographic)
    {
        ProjectionMatrix = FMatrix::OrthoForLH(Width / 100.f, Height / 100.f, Near, Far);
    }
    
    return ProjectionMatrix;
}
