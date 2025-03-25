#pragma once
#include "UObject.h"
#include "Core/Math/Vector.h"
#include "Core/Container/Set.h"
#include "Core/Math/Transform.h"
#include "Core/Math/Matrix.h"
#include "Object/ActorComponent/ActorComponent.h"

class USceneComponent : public UActorComponent
{
	DECLARE_CLASS(USceneComponent, UActorComponent)

	friend class AActor;
public:
	USceneComponent();

public:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

public:
	FTransform GetRelativeTransform() const;

	FVector GetRelativePosition() const { return RelativeTransform.GetPosition(); }
	FQuat GetRelativeRotation() const { return RelativeTransform.GetRotation(); }
	FVector GetRelativeEuler() const { return RelativeTransform.GetRotation().GetEuler(); }
	FVector GetRelativeScale() const { return RelativeTransform.GetScale(); }

	//월드 회전 값을 부모의 좌표계를 기준으로 해석하여 상대 회전 값으로 변환해주는 함수
	FQuat GetRelativeRotationFromWorld(const FQuat& NewRotation) const;

	void SetRelativeTransform(const FTransform& InTransform);

	void SetRelativePosition(const FVector& InPosition);
	void SetRelativeRotation(const FVector& InRotation);
	void SetRelativeRotation(const FQuat& InRotation);

	void SetRelativePositionAndRotation(const FVector& InPosition, const FVector& InRotation);
	void SetRelativePositionAndRotation(const FVector& InPosition, const FQuat& InRotation);

	void SetRelatvieTransform(const FTransform& NewTransform);
	void SetRelativeScale(const FVector& InScale);

	void AddRelativePosition(const FVector& Delta);
	void AddRelativeRotation(const FVector& Delta);
	void AddRelativeRotation(const FQuat& Delta);

	void AddLocalOffset(const FVector& DeltaPosition);
	void AddLocalRotation(const FVector& DeltaRotation);
	void AddLocalRotation(const FQuat& DeltaRotation);

	void AddLocalTransform(const FTransform& DeltaTransform);
public:
	void SetWorldPosition(const FVector& InPosition);
	void SetWorldRotation(const FVector& InRotation);
	void SetWorldRotation(const FQuat& InRotation);

	void SetWorldPositionAndRotation(const FVector& InPosition, const FVector& InRotation);
	void SetWorldPositionAndRotation(const FVector& InPosition, const FQuat& InRotation);

	void SetWorldScale(const FVector& InScale);

	void SetWorldTransform(const FTransform& InTransform);

	void AddWorldOffset(const FVector& Delta);
	void AddWorldRotation(const FVector& Delta);
	void AddWorldRotation(const FQuat& Delta);

	void AddWorldTransform(const FTransform& Delta);
	void AddWorldTransformKeepScale(const FTransform& Delta);

	FVector GetForwardVector() const;
	FVector GetRightVector() const;
	FVector GetUpVector() const;

	const FTransform& GetComponentTransform() const { return WorldTransform; }
	FVector GetComponentLocation() const { return GetComponentTransform().GetPosition(); }
	FQuat GetComponentRotation() const { return GetComponentTransform().GetRotation(); }
	FVector GetComponentScale() const { return GetComponentTransform().GetScale(); }

	/* 월드 트랜스폼을 반환, 이걸로 렌더링한다*/
	const FTransform& GetWorldTransform();

	FMatrix GetWorldMatrix() const;
	const FMatrix GetLocalMatrix() const { return RelativeTransform.GetMatrix(); }

	FTransform GetWorldTransform() const;

	USceneComponent* GetParent() const;

	bool MoveComponent(const FVector& Delta, const FQuat& NewRotation);
	bool MoveComponent(const FVector& Delta, const FVector& NewRotation);

	inline void SetComponentToWorld(const FTransform& NewTransform)
	{
		RelativeTransform = NewTransform;
		bComponentToWorldUpdated = true;
	}

	inline void ConditionalUpdateComponentToWorld()
	{
		if (bComponentToWorldUpdated == false)
		{
			UpdateComponentToWorld();
		}
	}

public:
	void UpdateComponentToWorld();

	void UpdateChildTransforms();
protected:
	FTransform CalcNewWorldTransform(const FTransform& NewTransform, const USceneComponent* Parent = nullptr) const;

	bool InternalSetWorldPositionAndRotation(FVector& InPosition, const FQuat& InRotation);

	void UpdateComponentToWorldWithParent(USceneComponent* Parent, const FQuat& RelativeRotationQuat);

	void PropagateTransformUpdate(bool bTransformChanged);

	bool MoveComponentImpl(const FVector& Delta, const FQuat& NewRotation);

	void Pick(bool bPicked);
public:
	bool IsPicked() const { return bIsPicked; }


public:
	void SetupAttachment(USceneComponent* InParent, bool bUpdateChildTransform = false);

	TArray<USceneComponent*> GetChildren() const { return Children.Array(); }
	USceneComponent* GetParent() { return Parent; }

protected:
	USceneComponent* Parent = nullptr;
	TSet<USceneComponent*> Children;

	// 이건 내 로컬 트랜스폼
	FTransform RelativeTransform = FTransform();

	// 이건 내 월드 트랜스폼
	FTransform WorldTransform = FTransform();

	// debug
protected:
	bool bIsPicked = false;

	bool bHiddenInGame = false;

public:
	// bUsedAttachParentBound: 이 값이 true이면, 해당 컴포넌트는 부착되었을 때 부모 컴포넌트의 경계를 사용합니다.
	// 많은 컴포넌트가 함께 부착되어 있을 때, 각각의 경계를 개별적으로 계산하는 비용을 절감할 수 있어 성능 최적화에 도움이 됩니다.
	bool bUsedAttachParentBound = false;

	// bComputeFastLocalBounds: 이 값이 true이면, 컴포넌트는 로컬 경계를 보다 빠른 방법으로 계산합니다.
	// 정확도보다는 성능을 우선시하는 최적화 옵션으로, 계산 비용을 줄이는 대신 경계 계산의 정밀도가 다소 떨어질 수 있습니다.
	bool bComputeFastLocalBounds = false;
private:
	bool bComponentToWorldUpdated = false;

};