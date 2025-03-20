#pragma once
#include "Object/PrimitiveComponent/UPrimitiveComponent.h"

class UMeshComponent : public UPrimitiveComponent
{
	using Super = UPrimitiveComponent;
public:
	UMeshComponent() = default;

public:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void Render() override;

	void SetMeshName(FName InMeshName) { MeshName = InMeshName; }
	FName GetMeshName() const { return MeshName; }

private:
	FName MeshName;

};