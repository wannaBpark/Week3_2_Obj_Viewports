#include "USceneComponent.h"
#include "Debug/DebugConsole.h"
#include "PrimitiveComponent/UPrimitiveComponent.h"
#include "Object/Actor/Actor.h"

USceneComponent::USceneComponent()
{
	RelativeTransform = FTransform();
	WorldTransform = FTransform();

	bIsPicked = false;
	bHiddenInGame = false;
	bUsedAttachParentBound = false;
	bComponentToWorldUpdated = false;
}

void USceneComponent::BeginPlay()
{
	Super::BeginPlay();
}

void USceneComponent::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// 내 월드 트랜스폼 반환
const FTransform& USceneComponent::GetWorldTransform()
{
	return WorldTransform;
}

FMatrix USceneComponent::GetWorldMatrix() const
{
	if (Parent != nullptr)
	{
		FMatrix ParentWorld = Parent->GetWorldMatrix();
		FMatrix MyLocalWithoutScale = RelativeTransform.GetLocalMatrixWithOutScale();
		FMatrix LocalMatrix = MyLocalWithoutScale * ParentWorld;

		return LocalMatrix * RelativeTransform.GetScaleMatrix();
	}
	else
	{
		return RelativeTransform.GetMatrix();
	}
}

FTransform USceneComponent::GetWorldTransform() const
{
	if (Parent != nullptr)
	{
		FTransform parnetTrans = Parent->GetComponentTransform();
		return parnetTrans;
	}
	else
	{
		// 부모가 없을 경우 내 로컬
		return RelativeTransform;
	}
}

USceneComponent* USceneComponent::GetParent() const
{
	return Parent;
}

bool USceneComponent::MoveComponent(const FVector& Delta, const FQuat& NewRotation)
{
	return MoveComponentImpl(Delta, NewRotation);
}

bool USceneComponent::MoveComponent(const FVector& Delta, const FVector& NewRotation)
{
	return MoveComponent(Delta, FQuat(NewRotation));
}

void USceneComponent::UpdateChildTransforms()
{
	for (auto& Child : Children)
	{
		if (Child->bComponentToWorldUpdated == false)
		{
			Child->UpdateComponentToWorld();
		}
		else
		{
			Child->UpdateComponentToWorld();
		}
	}
}

void USceneComponent::UpdateComponentToWorld()
{
	UpdateComponentToWorldWithParent(Parent, RelativeTransform.GetRotation());
}

FTransform USceneComponent::CalcNewWorldTransform(const FTransform& NewTransform, const USceneComponent* Parent) const
{
	Parent = Parent ? Parent : GetParent();
	if (Parent != nullptr)
	{
		const FTransform ParentToWorld = Parent->GetWorldTransform();
		FTransform NewWorldTransform = FTransform::MultiPly(NewTransform, ParentToWorld);
		return NewWorldTransform;
	}
	else
	{
		return NewTransform;
	}
}

bool USceneComponent::InternalSetWorldPositionAndRotation(FVector& InPosition, const FQuat& InRotation)
{
	FQuat NewRotation = InRotation;
	if (Parent != nullptr)
	{
		FTransform parentToWorld = Parent->GetWorldTransform();

		InPosition = parentToWorld.InverseTransformPosition(InPosition);
		NewRotation = FQuat::MultiplyQuaternions(parentToWorld.GetRotation().GetInverse(), NewRotation);
	}

	bool bDiffPosition = InPosition != GetRelativePosition();
	bool bDiffRotation = NewRotation != GetRelativeRotation();

	if (bDiffPosition || bDiffRotation)
	{
		RelativeTransform.SetPosition(InPosition);

		if (bDiffRotation)
		{
			RelativeTransform.SetRotation(NewRotation);
		}

		UpdateComponentToWorldWithParent(Parent, NewRotation);
		return true;
	}

	return false;
}

void USceneComponent::UpdateComponentToWorldWithParent(USceneComponent* Parent, const FQuat& RelativeRotationQuat)
{
	if (Parent != nullptr && Parent->bComponentToWorldUpdated == false)
	{
		Parent->UpdateComponentToWorld();
		if (bComponentToWorldUpdated == true)
		{
			return;
		}
	}

	bComponentToWorldUpdated = true;

	FTransform NewTransform;

	const FTransform RelativeTransform(GetRelativePosition(), RelativeRotationQuat, GetRelativeScale());

	NewTransform = CalcNewWorldTransform(RelativeTransform, Parent);

	bool bHasChanged;

	if (GetWorldTransform().Equal(NewTransform) == false)
	{
		bHasChanged = true;
	}
	else
	{
		bHasChanged = false;
	}


	if (bHasChanged)
	{
		WorldTransform = NewTransform;
		PropagateTransformUpdate(true);
	}
	else
	{
		PropagateTransformUpdate(false);
	}
}

void USceneComponent::PropagateTransformUpdate(bool bTransformChanged)
{
	if (bTransformChanged)
	{
		UpdateChildTransforms();
	}
	else
	{
		// TODO: Update Bounds
		UpdateChildTransforms();
	}
}

bool USceneComponent::MoveComponentImpl(const FVector& Delta, const FQuat& NewRotation)
{
	ConditionalUpdateComponentToWorld();

	if (Delta.X == 0 && Delta.Y == 0 && Delta.Z == 0 && NewRotation.Equals(GetWorldTransform().GetRotation()))
	{
		return true;
	}

	FVector NewPosition = GetWorldTransform().GetPosition() + Delta;
	const bool bMoved = InternalSetWorldPositionAndRotation(NewPosition, NewRotation);

	return true;
}

void USceneComponent::SetRelativeTransform(const FTransform& InTransform)
{
	SetRelativePositionAndRotation(InTransform.GetPosition(), InTransform.GetRotation());
	SetRelativeScale(InTransform.GetScale());
}

FQuat USceneComponent::GetRelativeRotationFromWorld(const FQuat& NewRotation) const
{
	FQuat NewRelRotation = NewRotation;

	if (Parent != nullptr)
	{
		FTransform parentToWorld = Parent->GetWorldTransform();

		const FQuat ParentToWorldQuat = parentToWorld.GetRotation();

		const FQuat NewRelativeQuat = FQuat::MultiplyQuaternions(ParentToWorldQuat.GetInverse(), NewRotation);

		NewRelRotation = NewRelativeQuat;
	}

	return NewRelRotation;
}

void USceneComponent::SetRelativePosition(const FVector& InPosition)
{
	SetRelativePositionAndRotation(InPosition, GetRelativeRotationFromWorld(RelativeTransform.GetRotation()));
}

void USceneComponent::SetRelativeRotation(const FVector& InRotation)
{
	SetRelativePositionAndRotation(GetRelativePosition(), FQuat(InRotation));
}

void USceneComponent::SetRelativeRotation(const FQuat& InRotation)
{
	SetRelativePositionAndRotation(GetRelativePosition(), InRotation);
}

void USceneComponent::SetRelativePositionAndRotation(const FVector& InPosition, const FVector& InRotation)
{
	if (InPosition != GetRelativePosition())
	{
		SetRelativePositionAndRotation(InPosition, FQuat(InRotation));
	}
	else if (FQuat(InRotation) != GetRelativeRotation())
	{
		SetRelativePositionAndRotation(InPosition, FQuat(InRotation));
	}
}

void USceneComponent::SetRelativePositionAndRotation(const FVector& InPosition, const FQuat& InRotation)
{
	//RelativeTransform.SetPosition(InPosition);
	//RelativeTransform.SetRotation(InRotation);

	ConditionalUpdateComponentToWorld();

	const FTransform desiredRelTransform = FTransform(InPosition, InRotation, FVector(1.0f, 1.0f, 1.0f));
	const FTransform desiredWorldTransform = CalcNewWorldTransform(desiredRelTransform);
	const FVector desiredDelta = desiredWorldTransform.GetPosition() - WorldTransform.GetPosition();

	MoveComponent(desiredDelta, desiredWorldTransform.GetRotation());
}

void USceneComponent::SetRelativeScale(const FVector& InScale)
{
	if (InScale != GetRelativeScale())
	{
		RelativeTransform.SetScale(InScale);
		UpdateComponentToWorld();
	}
}

void USceneComponent::SetRelatvieTransform(const FTransform& NewTransform)
{
	SetRelativePositionAndRotation(NewTransform.GetPosition(), NewTransform.GetRotation());
	SetRelativeScale(NewTransform.GetScale());
}

void USceneComponent::AddRelativePosition(const FVector& Delta)
{
	SetRelativePositionAndRotation(GetRelativePosition() + Delta, GetRelativeRotationFromWorld(RelativeTransform.GetRotation()));
}

void USceneComponent::AddRelativeRotation(const FVector& Delta)
{
	SetRelativePositionAndRotation(GetRelativePosition(), FQuat::MultiplyQuaternions(FQuat(Delta), RelativeTransform.GetRotation()));
}

void USceneComponent::AddRelativeRotation(const FQuat& Delta)
{
	SetRelativePositionAndRotation(GetRelativePosition(), FQuat::MultiplyQuaternions(Delta, RelativeTransform.GetRotation()));
}

void USceneComponent::AddLocalOffset(const FVector& DeltaPosition)
{
	const FQuat CurRelativeRotaion = RelativeTransform.GetRotation();
	const FVector LocalOffset = CurRelativeRotaion.RotateVector(DeltaPosition);
	SetRelativePositionAndRotation(GetRelativePosition() + LocalOffset, CurRelativeRotaion);
}

void USceneComponent::AddLocalRotation(const FVector& DeltaRotation)
{
	const FQuat CurRelativeRotaion = RelativeTransform.GetRotation();
	const FQuat NewRelativeRotation = FQuat::MultiplyQuaternions(CurRelativeRotaion, FQuat(DeltaRotation));
	SetRelativePositionAndRotation(GetRelativePosition(), NewRelativeRotation);
}

void USceneComponent::AddLocalRotation(const FQuat& DeltaRotation)
{
	const FQuat CurRelativeRotaion = RelativeTransform.GetRotation();
	const FQuat NewRelativeRotation = FQuat::MultiplyQuaternions(CurRelativeRotaion, DeltaRotation);
	SetRelativePositionAndRotation(GetRelativePosition(), NewRelativeRotation);
}

void USceneComponent::AddLocalTransform(const FTransform& DeltaTransform)
{
	const FTransform RelativeTransform(GetRelativePosition(), GetRelativeRotation(), FVector(1.0f, 1.0f, 1.0f));
	const FTransform NewRelativeTransform = FTransform::MultiPly(DeltaTransform, RelativeTransform);
	SetRelatvieTransform(NewRelativeTransform);
}

void USceneComponent::SetWorldPosition(const FVector& InPosition)
{
	FVector NewPosition = InPosition;

	if (Parent != nullptr)
	{
		FTransform parentToWorld = Parent->GetWorldTransform();
		NewPosition = parentToWorld.InverseTransformPosition(InPosition);
	}

	SetRelativePosition(NewPosition);
}

void USceneComponent::SetWorldRotation(const FVector& InRotation)
{
	if (Parent == nullptr)
	{
		SetRelativeRotation(FQuat(InRotation));
	}
	else
	{
		SetWorldRotation(FQuat(InRotation));
	}
}

void USceneComponent::SetWorldRotation(const FQuat& InRotation)
{
	FQuat NewRotation = GetRelativeRotationFromWorld(InRotation);
	SetRelativeRotation(NewRotation);
}

void USceneComponent::SetWorldPositionAndRotation(const FVector& InPosition, const FVector& InRotation)
{
	if (Parent == nullptr)
	{
		SetRelativePositionAndRotation(InPosition, FQuat(InRotation));
	}
	else
	{
		SetWorldPositionAndRotation(InPosition, FQuat(InRotation));
	}
}

void USceneComponent::SetWorldPositionAndRotation(const FVector& InPosition, const FQuat& InRotation)
{
	FQuat NewRotation = InRotation;
	if (Parent != nullptr)
	{
		FTransform parentToWorld = Parent->GetWorldTransform();
		FVector NewRelPosition = parentToWorld.InverseTransformPosition(InPosition);
		NewRotation = parentToWorld.InverseTransformRotation(InRotation);
	}

	SetRelativePositionAndRotation(InPosition, NewRotation);
}

void USceneComponent::SetWorldScale(const FVector& InScale)
{
	FVector NewRelScale = InScale;

	if (Parent != nullptr)
	{
		FTransform parentToWorld = Parent->GetWorldTransform();
		NewRelScale = parentToWorld.InverseTransformPosition(InScale);
	}

	SetRelativeScale(NewRelScale);
}

void USceneComponent::SetWorldTransform(const FTransform& InTransform)
{
	if (Parent != nullptr)
	{
		const FTransform parentToWorld = Parent->GetWorldTransform();
		FTransform RelativeTransform = InTransform.GetRelativeTransform(parentToWorld);

		SetRelatvieTransform(RelativeTransform);
	}
	else
	{
		SetRelatvieTransform(InTransform);
	}
}

void USceneComponent::AddWorldOffset(const FVector& Delta)
{
	const FVector NewPosition = GetWorldTransform().GetPosition() + Delta;
	SetWorldPosition(NewPosition);
}

void USceneComponent::AddWorldRotation(const FVector& Delta)
{
	const FQuat NewRotation = FQuat::MultiplyQuaternions(FQuat(Delta), GetWorldTransform().GetRotation());
	SetWorldRotation(NewRotation);
}

void USceneComponent::AddWorldRotation(const FQuat& Delta)
{
	const FQuat NewRotation = FQuat::MultiplyQuaternions(Delta, GetWorldTransform().GetRotation());
	SetWorldRotation(NewRotation);
}

void USceneComponent::AddWorldTransform(const FTransform& Delta)
{
	const FTransform& LocalTransform = GetWorldTransform();
	const FQuat NewRotation = FQuat::MultiplyQuaternions(Delta.GetRotation(), LocalTransform.GetRotation());
	const FVector NewPosition = LocalTransform.GetPosition() + Delta.GetPosition();
	SetWorldTransform(FTransform(NewPosition, NewRotation, FVector(1, 1, 1)));
}

void USceneComponent::AddWorldTransformKeepScale(const FTransform& Delta)
{
	const FTransform& LocalTransform = GetWorldTransform();
	const FQuat NewRotation = FQuat::MultiplyQuaternions(Delta.GetRotation(), LocalTransform.GetRotation());
	const FVector NewPosition = LocalTransform.GetPosition() + Delta.GetPosition();
	SetWorldTransform(FTransform(NewPosition, NewRotation, LocalTransform.GetScale()));
}

FVector USceneComponent::GetForwardVector() const
{
	return GetWorldTransform().GetForward();
}

FVector USceneComponent::GetRightVector() const
{
	return GetWorldTransform().GetRight();
}

FVector USceneComponent::GetUpVector() const
{
	return GetWorldTransform().GetUp();
}

FTransform USceneComponent::GetRelativeTransform() const
{
	const FTransform relativeTransform(GetRelativePosition(), GetRelativeRotation(), GetRelativeScale());
	return relativeTransform;
}

void USceneComponent::Pick(bool bPicked)
{
	bIsPicked = bPicked;
	for (auto& Child : Children)
	{
		Child->Pick(bPicked);
	}
}

void USceneComponent::SetupAttachment(USceneComponent* InParent, bool bUpdateChildTransform)
{
	if (InParent)
	{
		Parent = InParent;
		InParent->Children.Add(this);
	}
	else
	{
		UE_LOG("Parent is nullptr");
	}
}
