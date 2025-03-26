#include "URaycastSystem.h"
#include "Object/World/World.h"

// Raycast 구현
bool URaycastSystem::Raycast(const FVector& Origin, const FVector& Direction, float MaxDistance, FHitResult& OutHit)
{
    // 방향 벡터 정규화
    FVector NormalizedDirection = Direction.GetSafeNormal();

    return RaycastClosest(Origin, NormalizedDirection, MaxDistance, OutHit);
}

bool URaycastSystem::RaycastSingle(const FVector& Origin, const FVector& Direction, float MaxDistance,
    ICollider* collider, FHitResult& OutHit)
{
    // 방향 벡터 정규화
    FVector NormalizedDirection = Direction.GetSafeNormal();
    
    UClass* colliderClass = collider->GetClass();
    // 콜라이더 타입에 따라 적절한 Raycast 함수 호출
    if (colliderClass == USphereCollider::StaticClass())
    {
        USphereCollider* Spherecollider = dynamic_cast<USphereCollider*>(collider);
        return RayToSphere(Origin, NormalizedDirection, *Spherecollider, MaxDistance, OutHit);
    }
    else if (colliderClass == UBoxCollider::StaticClass())
    {
        UBoxCollider* BoxCollider = dynamic_cast<UBoxCollider*>(collider);
        return RayToBox(Origin, NormalizedDirection, *BoxCollider, MaxDistance, OutHit);
    }
    else if (colliderClass == ULineCollider::StaticClass())
    {
        ULineCollider* LineCollider = dynamic_cast<ULineCollider*>(collider);
        return RayToLine(Origin, NormalizedDirection, *LineCollider, MaxDistance, OutHit);
    }
    /*if (USphereCollider* SphereCollider = dynamic_cast<USphereCollider*>(collider))
    {
        return RayToSphere(Origin, NormalizedDirection, *SphereCollider, MaxDistance, OutHit);
    }
    else if (UBoxCollider* BoxCollider = dynamic_cast<UBoxCollider*>(collider))
    {
        return RayToBox(Origin, NormalizedDirection, *BoxCollider, MaxDistance, OutHit);
    }
    else if (ULineCollider* LineCollider = dynamic_cast<ULineCollider*>(collider))
    {
        return RayToLine(Origin, NormalizedDirection, *LineCollider, MaxDistance, OutHit);
    }*/

    return false;
}

bool URaycastSystem::RaycastClosest(const FVector& Origin, const FVector& Direction, float MaxDistance, FHitResult& OutHit)
{
    // 방향 벡터 정규화
    FVector NormalizedDirection = Direction.GetSafeNormal();

    FHitResult TempHit;
    bool bHit = false;
    float ClosestDistance = MaxDistance;

    TArray<ICollider*> colliders = UEngine::Get().GetWorld()->GetCollidrs();

    // 모든 콜라이더에 대해 검사
    for (ICollider* collider : colliders)
    {
        if (RaycastSingle(Origin, NormalizedDirection, MaxDistance, collider, TempHit))
        {
            // 이전에 찾은 충돌보다 더 가까우면 업데이트
            if (TempHit.distance < ClosestDistance)
            {
                OutHit = TempHit;
                ClosestDistance = TempHit.distance;
                bHit = true;
            }
        }
    }

    return bHit;
}

bool URaycastSystem::RaycastAll(const FVector& Origin, const FVector& Direction, float MaxDistance, TArray<FHitResult>& OutHits)
{
    // 방향 벡터 정규화
    FVector NormalizedDirection = Direction.GetSafeNormal();

    OutHits.Empty();
    bool bHit = false;

    TArray<ICollider*> colliders = UEngine::Get().GetWorld()->GetCollidrs();

    // 모든 콜라이더에 대해 검사
    for (ICollider* collider : colliders)
    {
        FHitResult TempHit;
        if (RaycastSingle(Origin, NormalizedDirection, MaxDistance, collider, TempHit))
        {
            OutHits.Add(TempHit);
            bHit = true;
        }
    }

    // 거리에 따라 Hit 결과 정렬
    OutHits.Sort([](const FHitResult& A, const FHitResult& B) {
        return A.distance < B.distance;
        });

    return bHit;
}

// Ray와 구체의 충돌 검사
bool URaycastSystem::RayToSphere(const FVector& Origin, const FVector& Direction,
    const USphereCollider& Sphere, float MaxDistance, FHitResult& OutHit)
{
    FTransform worldTransform = Sphere.GetComponentTransform();
    FVector SphereCenter = worldTransform.GetPosition();
    float SphereRadius = Sphere.GetRadius() * worldTransform.GetScale().X;

    // Ray 시작점에서 구체 중심까지의 벡터
    FVector OriginToCenter = SphereCenter - Origin;

    // 방향 벡터에 투영
    float Projection = FVector::DotProduct(OriginToCenter, Direction);

    // 구체 중심이 Ray의 반대 방향에 있고, Ray 시작점이 구체 밖에 있으면 충돌 없음
    if (Projection < 0.0f && OriginToCenter.Length() > SphereRadius)
    {
        return false;
    }

    // Ray 상에서 구체 중심에 가장 가까운 점 찾기
    FVector ClosestPointOnRay = Origin + Direction * Projection;

    // 가장 가까운 점에서 구체 중심까지의 거리 제곱
    float Distance = (ClosestPointOnRay - SphereCenter).Length();

    // 거리가 반지름보다 크면 충돌 없음
    if (Distance > SphereRadius)
    {
        return false;
    }

    // 교차점까지의 거리 계산
    float DistanceToIntersection = Projection - FMath::Sqrt(SphereRadius * SphereRadius - Distance * Distance);

    // 최대 거리 내에 있는지 확인
    if (DistanceToIntersection > MaxDistance || DistanceToIntersection < 0.0f)
    {
        return false;
    }

    // 충돌 결과 설정
    OutHit.bBlockingHit = true;
    OutHit.distance = DistanceToIntersection;
    OutHit.location = Origin + Direction * DistanceToIntersection;
    OutHit.normal = (OutHit.location - SphereCenter).GetSafeNormal();
    OutHit.hitObject = const_cast<USphereCollider*>(&Sphere);

    return true;
}

// Ray와 박스의 충돌 검사 (OBB)
bool URaycastSystem::RayToBox(const FVector& Origin, const FVector& Direction,
    const UBoxCollider& Box, float MaxDistance, FHitResult& OutHit)
{
    //// 박스 속성 가져오기
    FTransform worldTransform = Box.GetComponentTransform();

    FVector BoxCenter = worldTransform.GetPosition();
    FVector BoxExtent = worldTransform.GetScale() / 2;
    FQuat BoxRotation = worldTransform.GetRotation();

    //// Ray를 박스의 로컬 공간으로 변환
    FVector LocalOrigin = BoxRotation.Inverse().RotateVector(Origin - BoxCenter);
    FVector LocalDirection = BoxRotation.Inverse().RotateVector(Direction);

    //// 각 축별 슬랩 테스트를 위한 변수
    float tMin = -FLT_MAX;
    float tMax = FLT_MAX;

    // X축 슬랩 테스트
    if (FMath::Abs(LocalDirection.X) < KINDA_SMALL_NUMBER)
    {
        // Ray가 X축과 평행하고 원점이 슬랩 외부에 있으면 충돌 없음
        if (LocalOrigin.X < -BoxExtent.X || LocalOrigin.X > BoxExtent.X)
            return false;
    }
    else
    {
        // X축을 따라 진입/탈출 t값 계산
        float invDirX = 1.0f / LocalDirection.X;
        float t1 = (-BoxExtent.X - LocalOrigin.X) * invDirX;
        float t2 = (BoxExtent.X - LocalOrigin.X) * invDirX;

        if (t1 > t2) {
            float temp = t1;
            t1 = t2;
            t2 = temp;
        }

        tMin = FMath::Max(tMin, t1);
        tMax = FMath::Min(tMax, t2);

        if (tMin > tMax) return false;
    }

    // Y축 슬랩 테스트
    if (FMath::Abs(LocalDirection.Y) < KINDA_SMALL_NUMBER)
    {
        if (LocalOrigin.Y < -BoxExtent.Y || LocalOrigin.Y > BoxExtent.Y)
            return false;
    }
    else
    {
        float invDirY = 1.0f / LocalDirection.Y;
        float t1 = (-BoxExtent.Y - LocalOrigin.Y) * invDirY;
        float t2 = (BoxExtent.Y - LocalOrigin.Y) * invDirY;

        if (t1 > t2) {
            float temp = t1;
            t1 = t2;
            t2 = temp;
        }

        tMin = FMath::Max(tMin, t1);
        tMax = FMath::Min(tMax, t2);

        if (tMin > tMax) return false;
    }

    // Z축 슬랩 테스트
    if (FMath::Abs(LocalDirection.Z) < KINDA_SMALL_NUMBER)
    {
        if (LocalOrigin.Z < -BoxExtent.Z || LocalOrigin.Z > BoxExtent.Z)
            return false;
    }
    else
    {
        float invDirZ = 1.0f / LocalDirection.Z;
        float t1 = (-BoxExtent.Z - LocalOrigin.Z) * invDirZ;
        float t2 = (BoxExtent.Z - LocalOrigin.Z) * invDirZ;

        if (t1 > t2) {
            float temp = t1;
            t1 = t2;
            t2 = temp;
        }

        tMin = FMath::Max(tMin, t1);
        tMax = FMath::Min(tMax, t2);

        if (tMin > tMax) return false;
    }

    // 충돌 지점이 유효한 범위 내에 있는지 확인
    if (tMin < 0.0f)
    {
        // Ray의 시작점이 박스 내부에 있는 경우
        if (tMax < 0.0f) return false;  // Ray가 완전히 박스 밖을 향하는 경우

        // tMax 사용 (박스에서 나가는 지점)
        if (tMax > MaxDistance) return false;

        OutHit.distance = tMax;
    }
    else
    {
        // tMin 사용 (박스에 들어가는 지점)
        if (tMin > MaxDistance) return false;

        OutHit.distance = tMin;
    }

    //// 충돌 위치 계산
    OutHit.location = Origin + Direction * OutHit.distance;

    //// 로컬 충돌 위치
    FVector LocalHitPoint = LocalOrigin + LocalDirection * OutHit.distance;

    //// 법선 벡터 계산: 충돌이 발생한 면의 법선
    FVector LocalNormal = FVector::ZeroVector;

    //// 가장 가까운 면 찾기
    float epsilon = 0.001f;  // 부동소수점 오차를 위한 작은 값

    if (FMath::Abs(LocalHitPoint.X - BoxExtent.X) < epsilon)
        LocalNormal = FVector(1, 0, 0);
    else if (FMath::Abs(LocalHitPoint.X + BoxExtent.X) < epsilon)
        LocalNormal = FVector(-1, 0, 0);
    else if (FMath::Abs(LocalHitPoint.Y - BoxExtent.Y) < epsilon)
        LocalNormal = FVector(0, 1, 0);
    else if (FMath::Abs(LocalHitPoint.Y + BoxExtent.Y) < epsilon)
        LocalNormal = FVector(0, -1, 0);
    else if (FMath::Abs(LocalHitPoint.Z - BoxExtent.Z) < epsilon)
        LocalNormal = FVector(0, 0, 1);
    else if (FMath::Abs(LocalHitPoint.Z + BoxExtent.Z) < epsilon)
        LocalNormal = FVector(0, 0, -1);

    //// 로컬 법선을 월드 공간으로 변환
    OutHit.normal = BoxRotation.RotateVector(LocalNormal);
    OutHit.bBlockingHit = true;
    OutHit.hitObject = const_cast<UBoxCollider*>(&Box);

    return true;
}

// Ray와 선분의 충돌 검사
bool URaycastSystem::RayToLine(const FVector& Origin, const FVector& Direction,
    const ULineCollider& Line, float MaxDistance, FHitResult& OutHit)
{
    //FVector LineStart = Line.GetStart();
    //FVector LineEnd = Line.GetEnd();
    //FVector LineDirection = LineEnd - LineStart;
    //float LineLength = LineDirection.Size();

    //// 선분의 길이가 0이면 충돌 없음
    //if (LineLength < KINDA_SMALL_NUMBER)
    //    return false;

    //LineDirection /= LineLength;  // 정규화

    //// Ray와 Line이 평행한지 확인
    //float Parallel = FMath::Abs(FVector::CrossProduct(Direction, LineDirection).SizeSquared());

    //if (Parallel < KINDA_SMALL_NUMBER)
    //{
    //    // 평행한 경우: 두 선이 같은 직선 상에 있는지 확인
    //    FVector RayToLineStart = LineStart - Origin;
    //    FVector CrossCheck = FVector::CrossProduct(RayToLineStart, Direction);

    //    if (CrossCheck.SizeSquared() > KINDA_SMALL_NUMBER)
    //        return false;  // 같은 직선 상에 있지 않음

    //    // 같은 직선 상에 있다면, 선분의 시작점이나 끝점이 Ray의 범위 내에 있는지 확인
    //    float ProjectionStart = FVector::DotProduct(RayToLineStart, Direction);
    //    float ProjectionEnd = FVector::DotProduct(LineEnd - Origin, Direction);

    //    // Ray 시작점과 선분 사이의 거리 계산
    //    if (ProjectionStart >= 0.0f && ProjectionStart <= MaxDistance)
    //    {
    //        // 선분 시작점이 Ray의 범위 내에 있음
    //        OutHit.bBlockingHit = true;
    //        OutHit.distance = ProjectionStart;
    //        OutHit.location = LineStart;
    //        OutHit.normal = -Direction;  // Ray 방향의 반대
    //        OutHit.hitObject = const_cast<ULineCollider*>(&Line);
    //        return true;
    //    }
    //    else if (ProjectionEnd >= 0.0f && ProjectionEnd <= MaxDistance)
    //    {
    //        // 선분 끝점이 Ray의 범위 내에 있음
    //        OutHit.bBlockingHit = true;
    //        OutHit.distance = ProjectionEnd;
    //        OutHit.location = LineEnd;
    //        OutHit.normal = -Direction;  // Ray 방향의 반대
    //        OutHit.hitObject = const_cast<ULineCollider*>(&Line);
    //        return true;
    //    }
    //    else if (ProjectionStart <= 0.0f && ProjectionEnd >= 0.0f)
    //    {
    //        // Ray 시작점이 선분 내에 있음
    //        OutHit.bBlockingHit = true;
    //        OutHit.distance = 0.0f;
    //        OutHit.location = Origin;
    //        OutHit.normal = -Direction;  // Ray 방향의 반대
    //        OutHit.hitObject = const_cast<ULineCollider*>(&Line);
    //        return true;
    //    }

    //    return false;
    //}

    //// 평행하지 않은 경우: 두 선의 최단 거리 계산
    //// 파라미터 방정식: Ray = Origin + t1 * Direction, Line = LineStart + t2 * LineDirection
    //FVector w0 = Origin - LineStart;
    //float a = FVector::DotProduct(Direction, Direction);
    //float b = FVector::DotProduct(Direction, LineDirection);
    //float c = FVector::DotProduct(LineDirection, LineDirection);
    //float d = FVector::DotProduct(Direction, w0);
    //float e = FVector::DotProduct(LineDirection, w0);

    //float denominator = a * c - b * b;

    //// 두 선의 최단 거리를 위한 파라미터 계산
    //float t1 = (b * e - c * d) / denominator;
    //float t2 = (a * e - b * d) / denominator;

    //// Ray 범위 및 선분 범위 내에 있는지 확인
    //if (t1 < 0.0f || t1 > MaxDistance || t2 < 0.0f || t2 > 1.0f)
    //    return false;

    //// 교차점 계산
    //FVector RayPoint = Origin + Direction * t1;
    //FVector LinePoint = LineStart + LineDirection * t2;

    //// 두 점 사이의 거리가 0이어야 함
    //if ((RayPoint - LinePoint).SizeSquared() > KINDA_SMALL_NUMBER)
    //    return false;

    //// 충돌 정보 설정
    //OutHit.bBlockingHit = true;
    //OutHit.distance = t1;
    //OutHit.location = RayPoint;
    //OutHit.normal = FVector::CrossProduct(LineDirection, Direction).GetSafeNormal();
    //if (FVector::DotProduct(OutHit.normal, w0) < 0)
    //    OutHit.normal = -OutHit.normal;
    //OutHit.hitObject = const_cast<ULineCollider*>(&Line);

    return true;
}