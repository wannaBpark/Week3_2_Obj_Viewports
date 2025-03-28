#include "Actor.h"
#include "Object/USceneComponent.h"
#include "Debug/DebugConsole.h"
#include "Object/World/World.h"
#include "Object/PrimitiveComponent/UPrimitiveComponent.h"
#include "Static/FEditorManager.h"
#include "Object/ActorComponent/Colliders/UBoxCollider.h"

AActor::AActor() : Depth{ 0 }
{
}


void AActor::BeginPlay()
{
	for (auto& Component : Components)
	{
		Component->BeginPlay();

		if (UPrimitiveComponent* PrimitiveComponent = dynamic_cast<UPrimitiveComponent*>(Component))
		{
			PrimitiveComponent->RegisterComponentWithWorld(World);
		}
	}
}

void AActor::Tick(float DeltaTime)
{
	for (auto& Component : Components)
	{
		if (Component->CanEverTick())
		{
			Component->Tick(DeltaTime);
		}
	}
}

void AActor::LateTick(float DeltaTime)
{
}

void AActor::Destroyed()
{
	EndPlay(EEndPlayReason::Destroyed);
}


void AActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	for (auto& Component : Components)
	{		
		Component->EndPlay(EndPlayReason);
		if (const auto PrimitiveComp = dynamic_cast<UPrimitiveComponent*>(Component))
		{
			if (World->ContainsZIgnoreComponent(PrimitiveComp))
			{
				World->RemoveZIgnoreComponent(PrimitiveComp);
			}
			
			GetWorld()->RemoveRenderComponent(PrimitiveComp);
		}
		if (FEditorManager::Get().GetSelectedActor() == this)
		{
			FEditorManager::Get().SelectActor(nullptr);
		}
		UEngine::Get().GObjects.Remove(Component->GetUUID());
		RemoveFromClassMap(Component);
	}
	Components.Empty();
}

void AActor::Pick()
{
	if (RootComponent)
	{
		bIsPicked = true;
		RootComponent->Pick(true);
	}
}

void AActor::UnPick()
{
	if (RootComponent)
	{
		bIsPicked = false;
		RootComponent->Pick(false);
	}	
}

FTransform AActor::GetActorTransform() const
{
	return RootComponent != nullptr ? RootComponent->GetRelativeTransform() : FTransform();
}

void AActor::SetActorTransform(const FTransform& InTransform)
{
	// InTransform은 월드 기준임

	if (RootComponent)
	{
		RootComponent->SetRelativeTransform(InTransform);
	}
	else
	{
		UE_LOG("RootComponent is nullptr");
	}
}

const char* AActor::GetTypeName()
{
	return "Actor";
}

bool AActor::Destroy()
{
	return GetWorld()->DestroyActor(this);
}

void AActor::SetColor(FVector4 InColor)
{
	if (RootComponent == nullptr)
	{
		return;
	}

	UPrimitiveComponent* RootPrimitive = dynamic_cast<UPrimitiveComponent*>(RootComponent);
	if (RootPrimitive)
	{
		RootPrimitive->SetCustomColor(InColor);
	}

	for (auto& Component : Components)
	{
		UPrimitiveComponent* PrimitiveComponent = dynamic_cast<UPrimitiveComponent*>(Component);
		if (PrimitiveComponent)
		{
			PrimitiveComponent->SetCustomColor(InColor);
		}
	}
}

void AActor::SetUseVertexColor(bool bUseVertexColor)
{
	if (RootComponent == nullptr)
	{
		return;
	}

	UPrimitiveComponent* RootPrimitive = dynamic_cast<UPrimitiveComponent*>(RootComponent);
	if (RootPrimitive)
	{
		RootPrimitive->SetUseVertexColor(bUseVertexColor);
	}

	for (auto& Component : Components)
	{
		UPrimitiveComponent* PrimitiveComponent = dynamic_cast<UPrimitiveComponent*>(Component);
		if (PrimitiveComponent)
		{
			PrimitiveComponent->SetUseVertexColor(bUseVertexColor);
		}
	}
}

void AActor::LoadAndConstruct(const TArray<FActorComponentInfo>& InfoArray)
{
	// 이 시점에 액터의 생성자를 통과하여 컴포넌트들은 다 생성되어있는 상태임. InfoArray의 크기와 Components의 크기는 같아야함
	// 또한 InfoArray와 Components의 순서는 같아야함

	if (InfoArray.Num() != Components.Num())
	{
		UE_LOG("InfoArray.Num() != Components.Num()");
		return;
	}

	for (int i = 0; i < InfoArray.Num(); i++)
	{
		Components[i]->LoadAndConstruct(InfoArray[i]);
	}
}

FActorInfo AActor::GetActorInfo()
{
	FActorInfo Info;
	Info.Type = GetTypeName();
	Info.ActorTransform = GetActorTransform();
	for (UActorComponent* Component : Components)
	{
		Info.ComponentInfos.Add(Component->GetActorComponentInfo());
	}
	return Info;
}

