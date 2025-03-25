#include"WorldGizmo.h"
#include"Object/Actor/WorldText.h"
#include"Object/World/World.h"
#include"Object/UtilComponent/UBillboardUtilComponent.h"	
#include"Core/Engine.h"
#include"Static/FEditorManager.h"
#include"Object/Actor/Camera.h"	

AWorldGizmo::AWorldGizmo()
{
	bIsGizmo = true;

	UCylinderComp* ZArrow = AddComponent<UCylinderComp>();
	RootComponent = ZArrow;
	ZArrow->SetRelativeTransform(FTransform(FVector(0.0f, 0.0f, 0.0f), FVector(0.0f, 0.0f, 0.0f), FVector(1, 1, 1)));
	ZArrow->SetCustomColor(FVector4(0.0f, 0.0f, 1.0f, 1.0f));
	CylinderComponents.Add(ZArrow);

	// x
	UCylinderComp* XArrow = AddComponent<UCylinderComp>();
	XArrow->SetupAttachment(ZArrow);
	XArrow->SetRelativeTransform(FTransform(FVector(0.0f, 0.0f, 0.0f), FVector(0.0f, -90.0f, 0.0f), FVector(1, 1, 1)));
	XArrow->SetCustomColor(FVector4(1.0f, 0.0f, 0.0f, 1.0f));
	CylinderComponents.Add(XArrow);


	// y
	UCylinderComp* YArrow = AddComponent<UCylinderComp>();
	YArrow->SetupAttachment(ZArrow);
	YArrow->SetRelativeTransform(FTransform(FVector(0.0f, 0.0f, 0.0f), FVector(90.0f, 0.0f, 0.0f), FVector(1, 1, 1)));
	YArrow->SetCustomColor(FVector4(0.0f, 1.0f, 0.0f, 1.0f));
	CylinderComponents.Add(YArrow);


	/*AWorldText* ZText = UEngine::Get().GetWorld()->SpawnActor<AWorldText>();
	ZText->GetRootComponent()->SetupAttachment(ZArrow);
	ZText->SetCharComps("Z");
	ZText->AddComponent<UBillboardUtilComponent>();
	ZText->SetUseBillboardUtil(true);
	ZText->SetActorTransform(FTransform(FVector(0.0f, 0.0f, 1.0f), FVector(0.0f, 0.0f, 0.0f), FVector(0.5, 0.5, 0.5)));
	CharComponents.Add(ZText);

	AWorldText* XText = UEngine::Get().GetWorld()->SpawnActor<AWorldText>();
	XText->GetRootComponent()->SetupAttachment(ZArrow);
	XText->SetCharComps("X");
	XText->AddComponent<UBillboardUtilComponent>();
	XText->SetUseBillboardUtil(true);
	XText->SetActorTransform(FTransform(FVector(1.0f, 0.0f, 0.0f), FVector(0.0f, 0.0f, 0.0f), FVector(0.5, 0.5, 0.5)));
	CharComponents.Add(XText);

	AWorldText* YText = UEngine::Get().GetWorld()->SpawnActor<AWorldText>();
	YText->GetRootComponent()->SetupAttachment(ZArrow);
	YText->SetCharComps("Y");
	YText->SetUseBillboardUtil(true);
	YText->SetActorTransform(FTransform(FVector(0.0f, 1.0f, 0.0f), FVector(0.0f, 0.0f, 0.0f), FVector(0.5, 0.5, 0.5)));
	CharComponents.Add(YText);

	UEngine::Get().GetWorld()->AddZIgnoreComponent(&YText->CharComps[0]);
	UEngine::Get().GetWorld()->AddZIgnoreComponent(&XText->CharComps[0]);
	UEngine::Get().GetWorld()->AddZIgnoreComponent(&ZText->CharComps[0]);*/
}

void AWorldGizmo::Tick(float DeltaTime)
{
	AActor::Tick(DeltaTime);
	FQuat camRotation = FEditorManager::Get().GetCamera()->GetActorTransform().GetRotation();
	//FQuat InverseQuat = camRotation.Inverse();
	//this->GetActorTransform().SetRotation(InverseQuat);
}

const char* AWorldGizmo::GetTypeName()
{
	return "WorldGizmo";
}

