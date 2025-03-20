#include "Object/Actor/SubUV.h"
#include "Core/Rendering/SubUVManager.h"
#include "Object/PrimitiveComponent/UPrimitiveComponent.h"
#include "Core/EngineTypes.h"
#include "Debug/DebugConsole.h"


ASubUV::ASubUV()
{

	bCanEverTick = true;
	SubUVComponent = AddComponent<USubUVComponent>();
	RootComponent = SubUVComponent;
}

void ASubUV::BeginPlay()
{
	Super::BeginPlay();
}

void ASubUV::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	SetFrame(SubUVComponent->GetFrame() % USubUVManager::GetTotalFrames() + 1);
}

const char* ASubUV::GetTypeName()
{
	return "SubUV";
}

void ASubUV::SetFrame(int32 frame)
{
	SubUVComponent->SetFrame(frame, AtlasName);
}

void ASubUV::SetAtlas(std::string InAtlasName)
{
	AtlasName = InAtlasName;
	SubUVComponent->AddTextureID(USubUVManager::GetTextureIndex(AtlasName));
}






