#include "Material.h"

UMaterial::UMaterial(const FString& Name, const FObjMaterialInfo& ObjInfo)
{
	MaterialName = Name;
	TextureName = ObjInfo.TextureName;
	Ambient = ObjInfo.Ambient;
	Diffuse = ObjInfo.Diffuse;
	Specular = ObjInfo.Specular;
	Emissive = ObjInfo.Emissive;
	Shininess = ObjInfo.Shininess;
	Opacity = ObjInfo.Opacity;
	OpticalDensity = ObjInfo.OpticalDensity;
	Illum = ObjInfo.Illum;
}
