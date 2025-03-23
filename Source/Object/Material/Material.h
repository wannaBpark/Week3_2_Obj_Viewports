#pragma once
#include "Object/UObject.h"
#include "Core/Math/Vector.h"
#include "Object/Mesh/StaticMesh.h"
#include "Object/ObjectMacro.h"
#include "Object/NameTypes.h"

class UMaterial :
    public UObject
{
	DECLARE_CLASS(UMaterial, UObject);
public:
	UMaterial() = default;
	virtual ~UMaterial() = default;

public:
	// ConstructObject를 거칠 때 기본생성자 이외엔 사용할 수 없으므로 이 함수가 반드시 호출되어야 함
	void SetMaterialInfo(const FName& Name, const FObjMaterialInfo& ObjInfo);
	
	const FName& GetMaterialName() const { return MaterialName; }
public:
	FName MaterialName;
	FName TextureName;
	uint32 TextureMapIndex;

	FVector4 Ambient;
	FVector4 Diffuse;
	FVector4 Specular;
	FVector Emissive;
	float Shininess;
	float Opacity;
	float OpticalDensity;
	uint32 Illum;
};

