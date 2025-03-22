#pragma once
#include "Object/UObject.h"
#include "Core/Math/Vector.h"
#include "Object/Mesh/StaticMesh.h"
#include "Object/ObjectMacro.h"

class UMaterial :
    public UObject
{
	DECLARE_CLASS(UMaterial, UObject);
public:
	UMaterial(const FString& Name, const FObjMaterialInfo& ObjInfo);
	virtual ~UMaterial() = default;

public:
	const FString& GetMaterialName() const { return MaterialName; }
public:
	FString MaterialName;
	FString TextureName;

	FVector4 Ambient;
	FVector4 Diffuse;
	FVector4 Specular;
	FVector Emissive;
	float Shininess;
	float Opacity;
	float OpticalDensity;
	uint32 Illum;
};

