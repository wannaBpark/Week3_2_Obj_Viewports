#pragma once

#include "Core/HAL/PlatformType.h"
#include "Core/Math/Matrix.h"
#include "Core/Math/Transform.h"
#include "Core/Math/Vector.h"
#include "Object/Actor/Actor.h"
#include "Object/ObjectMacro.h"

namespace ECameraProjectionMode
{
    enum Type : uint8
    {
        Perspective,
        Orthographic
    };
}

class ACamera : public AActor
{
    DECLARE_CLASS(ACamera, AActor)
    using Super = AActor;
    

public:
    ACamera();
    ACamera(FVector Position, FVector Rotation, ECameraProjectionMode::Type ProjMode); // Constructor : Set Orthographic Camera

private:    
    float Near;
    float Far;
    // 화면각  
    float FieldOfView;
    // orthographic 사용 시 view범위
    float ViewportSize;
    float AspectRatio;
    float Width;
    float Height;

public:
    const float MaxYDegree = 89.8f;
    //카메라 스피드 IMGui용 나중에 Velocity로 관리하면 없어질애라 편하게 public에서 관리
    float CameraSpeed = 1.0f;
    
    // 투영 타입 - Perspective, Orthographic
    ECameraProjectionMode::Type ProjectionMode;
    // float AspectRatio;	// 카메라 비율 (이번 프로젝트에서는 사용 안할듯) 

    void ResetRotation() { RootComponent->RelativeTransform.SetRotation(FVector(0, 0, 0)); }

    void SetFieldOfVew(float Fov);
    void SetViewportSize(float inViewprotSize);
    void SetViewportSize(float InWidth, float InHeight);
    void SetFar(float Far);
    void SetNear(float Near);
    void SetProjectionMode(ECameraProjectionMode::Type InMode);
    void SetAspectRatio(float InAspectRatio) { AspectRatio = InAspectRatio; }
    
    float GetFieldOfView() const;
    float GetViewportSize() const;
    float GetNear() const;
    float GetFar() const;
    float GetAspectRatio() const;
    float GetWidth() const;
    float GetHeight() const;
    ECameraProjectionMode::Type GetProjectionMode() const;

        
    FVector GetForward() const
    {
        return GetActorTransform().GetForward();
    }
    
    FVector GetRight() const
    {
        return GetActorTransform().GetRight();
    }
    
    FVector GetUp() const
    {
        return GetActorTransform().GetUp();
    }

    FMatrix GetViewMatrix() const
    {
        return GetActorTransform().GetViewMatrix();
    }
};