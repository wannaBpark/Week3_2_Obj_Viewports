#pragma once
#include "Object/Actor/Actor.h"
#include "Object/USceneComponent.h"

class AGizmoHandle;
class UStringComponent;

class FEditorManager : public TSingleton<FEditorManager>
{
public:
    
    inline AActor* GetSelectedActor() const {return SelectedActor;}
    
    void SelectActor(AActor* NewActor);

    inline ACamera* GetCamera() const {return Camera;}

    void SetCamera(ACamera* NewCamera);

    AGizmoHandle* GetGizmoHandle() const {return GizmoHandle;}

	UStringComponent* GetStringComp() const { return StringComp; }

    void SetBoundingBox(UBoundingBoxComp* InBoundingBoxComp);
    void SetStringComp(UStringComponent* InStringComp);

    void SelectComponent(USceneComponent* NewSceneComponent);
    USceneComponent* GetSelectedComponent() { return SelectedComponent; }
    
private:
    ACamera* Camera = nullptr;
    AActor* SelectedActor = nullptr;
    AGizmoHandle* GizmoHandle = nullptr;
	UStringComponent* StringComp = nullptr;
    UBoundingBoxComp* BoundingBoxComp = nullptr;
    USceneComponent* SelectedComponent = nullptr;
};
