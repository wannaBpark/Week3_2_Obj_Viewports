#include "FEditorManager.h"
#include "Core/Engine.h"
#include "Object/World/World.h"
#include "Object/Gizmo/GizmoHandle.h"
#include "Core/Math/Vector.h"
#include "Core/Math/Transform.h"
#include "Object/ActorComponent/Colliders/UBoxCollider.h"
#include "Object/UtilComponent/UStringComponent.h"
#include <string>
#include "Debug/DebugConsole.h"


void FEditorManager::SelectActor(AActor* NewActor)
{
    if (GizmoHandle == nullptr)
    {
		GizmoHandle = UEngine::Get().GetWorld()->SpawnActor<AGizmoHandle>();
    	GizmoHandle->SetDepth(1);
        GizmoHandle->SetActive(false);
    }

    if (BoundingBoxComp == nullptr) {
        UE_LOG("FEditorManager BoudningBoxComp is NUll ERROR!");

    }

    if (StringComp == nullptr) 
    {
        UE_LOG("FEditorManager WorldText is NUll ERROR!");
    }

	if (SelectedActor == NewActor)
		return;
	
    if (SelectedActor != nullptr && SelectedActor != NewActor)
    {
        SelectedActor->UnPick();
        GizmoHandle->SetActive(false);
        BoundingBoxComp->SetCanBeRendered(false);
        StringComp->SetActive(false);
    }

	SelectedActor = NewActor;
	
    if (SelectedActor != nullptr)
    {
        BoundingBoxComp->SetupAttachment(SelectedActor->GetHitCollider());
        StringComp->SetupAttachment(SelectedActor->GetRootComponent());
        StringComp->SetRelativeTransform(
            FTransform(FVector(0.0f, 0.0f, 1.0f),
                FQuat(0, 0, 0, 1),
                FVector(1, 1, 1)));
        SelectedActor->Pick();
        GizmoHandle->SetActive(true);
        BoundingBoxComp->SetCanBeRendered(true);
		StringComp->SetActive(true);
        std::string ActorUUID = "UID: " + std::to_string(NewActor->GetUUID());
        StringComp->SetCharComps(ActorUUID, "koverwatch.png");
        //FVector Pos = SelectedActor->GetActorTransform().GetPosition();
		//FTransform GizmoTransform = GizmoHandle->GetActorTransform();
		//GizmoTransform.SetPosition(Pos);
		//GizmoHandle->SetActorTransform(GizmoTransform);
        //GizmoHandle
        SelectedComponent = nullptr;
	}
}

void FEditorManager::SetCamera(ACamera* NewCamera)
{
    Camera = NewCamera;
}

void FEditorManager::SetBoundingBox(UBoundingBoxComp* InBoundingBoxComp)
{
    BoundingBoxComp = InBoundingBoxComp;
}

void FEditorManager::SetStringComp(UStringComponent* InStringComp)
{
    StringComp = InStringComp;
}

void FEditorManager::SelectComponent(USceneComponent* NewSceneComponent)
{
    SelectedComponent = NewSceneComponent;
}


