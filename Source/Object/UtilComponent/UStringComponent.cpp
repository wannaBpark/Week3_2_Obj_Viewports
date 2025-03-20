#include "UStringComponent.h"
#include "Static/FEditorManager.h"
#include "Core/FSceneManager.h"
#include "Core/EngineTypes.h"

UStringComponent::UStringComponent()
{
	bCanEverTick = true;
}

void UStringComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UStringComponent::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	uint32 num = CharComps.Num();

	if (((FSceneManager::Get().GetShowFlagMask() & EShowFlag::Text) != 0) && ((FSceneManager::Get().GetShowFlagMask() & EShowFlag::Primitive) != 0))
	{
		for (int32 i = 0; i < num; i++)
		{
			CharComps[i]->Render();
		}
	}
}

void UStringComponent::ClearCharComps()
{
	uint32 num = CharComps.Num();
	for (uint32 i = 0; i < num; i++) 
	{
		CharComps[i]->EndPlay(EEndPlayReason::Destroyed);
	}
	CharComps.Empty();
}

void UStringComponent::SetCharComps(std::string InText, std::string InAtlasName)
{
	if (CharComps.Num() == InText.size()) 
	{
		for (uint32 i = 0; i < CharComps.Num(); i++) 
		{
			CharComps[i]->SetChar(InText[i], InAtlasName);
		}
		return;
	}

	ClearCharComps();

	if (InText.size() == 0)
		return;
	float TextSize = static_cast<float>(InText.size());
	float Middle = (TextSize + (TextSize - 1.0f) * LetterSpacing) / 2.0f;
	for (int32 i = 0; i < InText.size(); i++)
	{
		UCharComp* CharComponent = FObjectFactory::ConstructObject<UCharComp>();
		CharComponent->Parent = this;
		CharComponent->SetOwner(GetOwner());
		CharComponent->SetRelativeTransform(
			FTransform(FVector(0.f, -Middle + 0.5f + static_cast<float>(i) * (1 + LetterSpacing), 0.f),
				FQuat(0, 0, 0, 1),
				FVector(1, 1, 1)));
		CharComponent->SetChar(InText[i], InAtlasName);
		CharComps.Add(CharComponent);
	}
}

std::string UStringComponent::GetString()
{
	std::string CharString = "";
	for (uint32 i = 0; i < CharComps.Num(); i++) 
	{
		CharString += CharComps[i]->GetChar();
	}

	return CharString;
}

float UStringComponent::GetLetterSpacing()
{
	return LetterSpacing;
}

void UStringComponent::SetLetterSpacing(float InLetterSpacing)
{
	LetterSpacing = InLetterSpacing;

	if (CharComps.Num() == 0)
		return;

	float TextSize = static_cast<float>(CharComps.Num());
	float Middle = (TextSize + (TextSize - 1.0f) * LetterSpacing) / 2.0f;
	for (uint32 i = 0; i < CharComps.Num(); i++)
	{
		CharComps[i]->SetRelativeTransform(
			FTransform(FVector(0.f, -Middle + 0.5f + static_cast<float>(i) * (1 + LetterSpacing), 0.f),
				FQuat(0, 0, 0, 1),
				FVector(1, 1, 1)));
	}

}

void UStringComponent::SetActive(bool bActive)
{
	bIsActive = bActive;
	for (uint32 i = 0; i < CharComps.Num(); i++) 
	{
		CharComps[i]->SetCanBeRendered(bIsActive);
	}
}

void UStringComponent::SetUseBillboardUtil(bool bUse)
{
	bUseBillboardUtil = bUse;
	for (uint32 i = 0; i < CharComps.Num(); i++) 
	{
		CharComps[i]->SetUseBillboardUtil(bUse);
	}
}


