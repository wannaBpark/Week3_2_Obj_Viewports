#pragma once
#include "ICollider.h"

class ULineCollider;
class UBoxCollider;
class USphereCollider;

// 충돌 시스템 클래스
class CollisionSystem {
private:
    // 실제 충돌 로직을 구현하는 정적 메서드들
    // TODO: point는 라인의 dir가 일정 길이 이하일 때로
    static bool LineToLine(const ULineCollider& a, const ULineCollider& b);
    static bool LineToBox(const ULineCollider& a, const UBoxCollider& b);
    static bool LineToSphere(const ULineCollider& a, const USphereCollider& b);
    static bool BoxToBox(const UBoxCollider& a, const UBoxCollider& b);
    static bool BoxToSphere(const UBoxCollider& a, const USphereCollider& b);
    static bool SphereToSphere(const USphereCollider& a, const USphereCollider& b);

public:
    // 외부에서 사용할 충돌 검사 인터페이스
    static bool CheckCollision(const ICollider& a, const ICollider& b) {
        return a.CheckCollision(b);
    }

    // 콜라이더 클래스에서만 접근할 친구 메서드들
    friend class USphereCollider;
    friend class UBoxCollider;
    friend class ULineCollider;
};