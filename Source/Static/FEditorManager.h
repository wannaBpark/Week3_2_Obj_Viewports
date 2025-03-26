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

    inline ACamera* GetCamera() const { return Camera; }
    inline ACamera* GetInputCamera() const {return (InputCamera == nullptr) ? Camera : InputCamera;}

    void SetCamera(ACamera* NewCamera); // Render대상 카메라를 설정합니다.
    void SetInputCamera(ACamera* NewInputCamera);      // 움직일 대상인 카메라를 설정합니다.

    AGizmoHandle* GetGizmoHandle() const {return GizmoHandle;}

	UStringComponent* GetStringComp() const { return StringComp; }

    void SetBoundingBox(UBoundingBoxComp* InBoundingBoxComp);
    void SetStringComp(UStringComponent* InStringComp);

    void SelectComponent(USceneComponent* NewSceneComponent);
    USceneComponent* GetSelectedComponent() { return SelectedComponent; }
    
private:
    ACamera* Camera = nullptr;
    ACamera* InputCamera = nullptr;
    AActor* SelectedActor = nullptr;
    AGizmoHandle* GizmoHandle = nullptr;
	UStringComponent* StringComp = nullptr;
    UBoundingBoxComp* BoundingBoxComp = nullptr;
    USceneComponent* SelectedComponent = nullptr;
};
