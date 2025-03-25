#pragma once
#include "Object/Actor/Actor.h"
#include "../EEnum.h"
#include "Object/ObjectMacro.h"

class AGizmoHandle : public AActor
{
	DECLARE_CLASS(AGizmoHandle, AActor)
public:
	AGizmoHandle();

public:
	virtual void Tick(float DeltaTime) override;
	void SetScaleByDistance();
	void SetActive(bool bActive);
	void SetSelectedAxis(ESelectedAxis NewAxis) { SelectedAxis = NewAxis; }
	void SetPickGizmo(int index);
	ESelectedAxis GetSelectedAxis() const { return SelectedAxis; }
	EGizmoType GetGizmoType() const { return GizmoType; }

private:
	bool bIsActive = false;
	TArray<class UCylinderComp*> CylinderComponents;
	TArray<class UCircleComp*> CircleComponents;

	ESelectedAxis SelectedAxis = ESelectedAxis::None;
	EGizmoType GizmoType = EGizmoType::Translate;

	virtual const char* GetTypeName() override;

private:
	void DoTransform(FTransform& AT, FVector Result, AActor* Actor);
	FVector CachedRayResult = FVector::ZeroVector;
};

