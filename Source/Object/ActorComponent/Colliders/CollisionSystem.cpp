#include "CollisionSystem.h";
#include "ULineCollider.h"
#include "UBoxCollider.h"
#include "USphereCollider.h"
#include "Object/Actor/Actor.h"

bool CollisionSystem::LineToLine(const ULineCollider& a, const ULineCollider& b)
{
	return false;
}

bool CollisionSystem::LineToBox(const ULineCollider& a, const UBoxCollider& b)
{
	return false;
}

bool CollisionSystem::LineToSphere(const ULineCollider& a, const USphereCollider& b)
{
    // Get line properties
    FVector lineStart = a.GetStart();
    FVector lineEnd = a.GetEnd();
    FVector lineDir = a.Dir();
    float lineLength = lineDir.Length();

    FVector sphereCenter = b.GetCenter();
    float sphereRadius = b.GetRadius();

    if (lineLength < KINDA_SMALL_NUMBER)
    {
        // Line is a point, so check distance to sphere center
        return (lineStart - sphereCenter).Length() <= b.GetRadius();
    }

    // Calculate vector from line start to sphere center
    FVector startToCenter = sphereCenter - lineStart;

    // Project startToCenter onto the line direction
    float projection = FVector::DotProduct(startToCenter, lineDir);

    // If the projection is negative, the closest point is lineStart
    if (projection < 0.0f)
    {
        return startToCenter.Length() <= sphereRadius;
    }

    // If the projection is larger than the line length, the closest point is lineEnd
    if (projection > lineLength)
    {
        return (sphereCenter - lineEnd).Length() <= sphereRadius;
    }

    // The closest point is on the line
    FVector closestPoint = lineStart + lineDir * projection;

    // Check if the closest point is within the sphere's radius
    return (closestPoint - sphereCenter).Length() <= sphereRadius;
}

bool CollisionSystem::BoxToBox(const UBoxCollider& a, const UBoxCollider& b)
{
	return false;
}

bool CollisionSystem::BoxToSphere(const UBoxCollider& a, const USphereCollider& b)
{

	return false;
}

bool CollisionSystem::SphereToSphere(const USphereCollider& a, const USphereCollider& b)
{
    // Get sphere A properties
    FVector centerA = a.GetCenter();
    float radiusA = a.GetRadius();

    // Get sphere B properties
    FVector centerB = b.GetCenter();
    float radiusB = b.GetRadius();

    // Calculate the squared distance between the centers
    float distance = (centerB - centerA).Length();

    // Collision occurs if the distance is less than or equal to the sum of radii
    return distance <= radiusA + radiusB;
}
