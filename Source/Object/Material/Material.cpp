#include "Material.h"


void UMaterial::SetMaterialInfo(const FName& Name, const FObjMaterialInfo& ObjInfo)
{
	MaterialName = Name;

	Ambient = ObjInfo.Ambient;
	Diffuse = ObjInfo.Diffuse;
	Specular = ObjInfo.Specular;
	Emissive = ObjInfo.Emissive;
	Shininess = ObjInfo.Shininess;
	Opacity = ObjInfo.Opacity;
	OpticalDensity = ObjInfo.OpticalDensity;
	TextureName = ObjInfo.TextureName;
	TextureMapIndex = ObjInfo.TextureMapIndex;
	Illum = ObjInfo.Illum;
}
