#pragma once
#include "Core/Math/Vector.h"
#include "Core/Container/Array.h"
#include "UColliders.h"

class UObject;

// HitResult 구조체 정의
struct FHitResult
{
    bool bBlockingHit;         // 충돌이 발생했는지 여부
    float distance;            // 충돌 지점까지의 거리
    FVector location;          // 충돌 발생 위치
    FVector normal;            // 충돌 표면의 법선 벡터
    UObject* hitObject;        // 충돌한 오브젝트

    FHitResult() : bBlockingHit(false), distance(0.0f), location(FVector::Zero()),
        normal(FVector::Up()), hitObject(nullptr) {
    }
};

// Raycast 클래스 및 메서드 구현
class URaycastSystem
{
public:
    // 기본 Raycast 함수 - 모든 콜라이더를 검사
    static bool Raycast(const FVector& Origin, const FVector& Direction, float MaxDistance, FHitResult& OutHit);

    // 특정 콜라이더와의 충돌만 검사하는 Raycast
    static bool RaycastSingle(const FVector& Origin, const FVector& Direction, float MaxDistance,
        ICollider* Object, FHitResult& OutHit);

    // 모든 충돌을 감지하고 가장 가까운 것을 반환하는 Raycast
    static bool RaycastClosest(const FVector& Origin, const FVector& Direction, float MaxDistance, FHitResult& OutHit);

    // 모든 충돌을 배열로 반환하는 Raycast
    static bool RaycastAll(const FVector& Origin, const FVector& Direction, float MaxDistance, TArray<FHitResult>& OutHits);

private:
    // 구체적인 콜라이더 타입에 대한 Raycast 구현
    static bool RayToSphere(const FVector& Origin, const FVector& Direction, const USphereCollider& Sphere,
        float MaxDistance, FHitResult& OutHit);
    static bool RayToBox(const FVector& Origin, const FVector& Direction, const UBoxCollider& Box,
        float MaxDistance, FHitResult& OutHit);
    static bool RayToLine(const FVector& Origin, const FVector& Direction, const ULineCollider& Line,
        float MaxDistance, FHitResult& OutHit);
};