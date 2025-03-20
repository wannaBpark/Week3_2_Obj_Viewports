#pragma once

#include "Object/USceneComponent.h"
#include "Object/ObjectMacro.h"

struct FMatrix;

class UBillboardUtilComponent : public USceneComponent
{
	DECLARE_CLASS(UBillboardUtilComponent, USceneComponent)
	using Super = USceneComponent;

public:
	UBillboardUtilComponent() {};
	virtual ~UBillboardUtilComponent() = default;

public:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	FMatrix GetBillboardMVPMat(URenderer*& Renderer);
	FMatrix3 GetBillboardMVPMat3(URenderer*& Renderer);
	static FMatrix GetBillboardMVPMatForText(URenderer*& Renderer, USceneComponent* RootComponent, float YOffset);
	
	bool GetIsUseBillboard() const { return IsUseBillboard; }
	void SetIsUseBillboard(bool bUse) { IsUseBillboard = bUse; }

private:
	bool IsUseBillboard = true;
};