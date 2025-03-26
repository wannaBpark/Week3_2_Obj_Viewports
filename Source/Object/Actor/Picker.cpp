#include "Core/HAL/PlatformType.h"
#include "Core/Rendering/URenderer.h"
#include "Picker.h"

#include "Core/Input/PlayerInput.h"
#include "Object/Gizmo/GizmoHandle.h"
#include "Object/PrimitiveComponent/UPrimitiveComponent.h"
#include "Static/FEditorManager.h"
#include "ImGui/imgui.h"
#include "Camera.h"
#include "Object/Gizmo/WorldGizmo.h"
#include "Object/UtilComponent/UStringComponent.h"

#include "../URaycastSystem.h"


APicker::APicker()
{   
    bIsGizmo = true;
    
    RootComponent = AddComponent<USceneComponent>();

    BoundingBoxComp = AddComponent<UBoundingBoxComp>();
    BoundingBoxComp->SetCanBeRendered(false);

    StringComponent = AddComponent<UStringComponent>();
    StringComponent->SetActive(false);
}

FVector4 APicker::EncodeUUID(uint32 UUID)
{
    float a = (UUID >> 24) & 0xff;
    float b = (UUID >> 16) & 0xff;
    float g = (UUID >> 8) & 0xff;
    float r = UUID & 0xff;
	
    FVector4 color = {r, g, b, a};
    
    return color;
}

uint32_t APicker::DecodeUUID(FVector4 color)
{
    uint32_t W = static_cast<uint32_t>(color.W) << 24;
    uint32_t Z = static_cast<uint32_t>(color.Z) << 16;
    uint32_t Y = static_cast<uint32_t>(color.Y) << 8;
    uint32_t X = static_cast<uint32_t>(color.X);

    return W | Z | Y | X;
}

void APicker::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void APicker::LateTick(float DeltaTime)
{
    AActor::LateTick(DeltaTime);

    if (APlayerInput::Get().IsPressedMouse(false) && !ImGui::GetIO().WantCaptureMouse)
    {
        POINT pt = GetMousePoint();
        UActorComponent* PickedComponent = GetAcotrByPixelPicking(pt);

        if (PickedComponent != nullptr && dynamic_cast<AGizmoHandle*>(PickedComponent->GetOwner())) {
            PickLocalGizmo(PickedComponent);
            return;
        }
    }
    else
    {
        if (AGizmoHandle* Handle = FEditorManager::Get().GetGizmoHandle())
        {
            Handle->SetSelectedAxis(ESelectedAxis::None);
        }
    }

    if (APlayerInput::Get().GetMouseDown(false) && !ImGui::GetIO().WantCaptureMouse) {

        POINT pt = GetMousePoint();
        UActorComponent* PickedComponent = GetAcotrByPixelPicking(pt);
        if (PickedComponent != nullptr)
            if (SetSelectActor(PickedComponent)) return;

        // 검출된 오브젝트가 없을 시 RayTracing으로도 검사
        TArray<FHitResult> resultAll;
        FVector location = FEditorManager::Get().GetCamera()->GetActorTransform().GetPosition();
        FVector dir = UEngine::Get().GetRenderer()->GetRayDirectionFromClick(FVector(pt.x, pt.y, 0));
        URaycastSystem::RaycastAll(location, dir, 100, resultAll);

        if (resultAll.Len() != 0 && resultAll[0].bBlockingHit) {
            PickedComponent = dynamic_cast<UActorComponent*>(resultAll[0].hitObject);
            if (PickedComponent != nullptr)
                if (SetSelectActor(PickedComponent)) return;
        }
    }
}

const char* APicker::GetTypeName()
{
    return "Picker";
}

POINT APicker::GetMousePoint()
{
    POINT pt;
    GetCursorPos(&pt);
    ScreenToClient(UEngine::Get().GetWindowHandle(), &pt);

    float ratioX = UEngine::Get().GetInitializedScreenWidth() / (float)UEngine::Get().GetScreenWidth();
    float ratioY = UEngine::Get().GetInitializedScreenHeight() / (float)UEngine::Get().GetScreenHeight();
    pt.x = pt.x * ratioX;
    pt.y = pt.y * ratioY;

    return pt;
}

UActorComponent* APicker::GetAcotrByPixelPicking(const POINT& pt) {
    FVector4 color = UEngine::Get().GetRenderer()->GetPixel(FVector(pt.x, pt.y, 0));
    uint32_t UUID = DecodeUUID(color);
    UActorComponent* PickedComponent = UEngine::Get().GetObjectByUUID<UActorComponent>(UUID);
    return PickedComponent;
}

void APicker::PickLocalGizmo(UActorComponent* actor) {
    AGizmoHandle* Gizmo = static_cast<AGizmoHandle*>(actor->GetOwner());
    if (Gizmo->GetSelectedAxis() != ESelectedAxis::None) return;

    if (UCylinderComp* CylinderComp = dynamic_cast<UCylinderComp*>(actor))
    {
        FVector4 CompColor = CylinderComp->GetCustomColor();
        if (1.0f - FMath::Abs(CompColor.X) < KINDA_SMALL_NUMBER) // Red - X축
        {
            Gizmo->SetSelectedAxis(ESelectedAxis::X);
        }
        else if (1.0f - FMath::Abs(CompColor.Y) < KINDA_SMALL_NUMBER) // Green - Y축
        {
            Gizmo->SetSelectedAxis(ESelectedAxis::Y);
        }
        else  // Blue - Z축
        {
            Gizmo->SetSelectedAxis(ESelectedAxis::Z);
        }
    }
    else if (UCircleComp* CircleComp = dynamic_cast<UCircleComp*>(actor))
    {
        FVector4 CompColor = CircleComp->GetCustomColor();
        if (1.0f - FMath::Abs(CompColor.X) < KINDA_SMALL_NUMBER) // Red - X축
        {
            Gizmo->SetSelectedAxis(ESelectedAxis::X);
        }
        else if (1.0f - FMath::Abs(CompColor.Y) < KINDA_SMALL_NUMBER) // Green - Y축
        {
            Gizmo->SetSelectedAxis(ESelectedAxis::Y);
        }
        else  // Blue - Z축
        {
            Gizmo->SetSelectedAxis(ESelectedAxis::Z);
        }
    }
}

void APicker::PickWorldGizmo() {
    FEditorManager::Get().GetCamera()->ResetRotation();
}


bool APicker::SetSelectActor(const UActorComponent* actorComponent) {
    // null이라면 return false
    AActor* actor = actorComponent->GetOwner();
    if (actor == nullptr) return false;
    // 로컬 기즈모면 true
    if (actorComponent->GetOwner()->IsGizmoActor()) return true;
    // 월드 기즈모면 true
    if (dynamic_cast<AWorldGizmo*>(actor)) {
        PickWorldGizmo();
        return true;
    }
    // 모든 경우를 통과했다면 Select
    if (actor == FEditorManager::Get().GetSelectedActor())
    {
        FEditorManager::Get().SelectActor(nullptr);
    }
    else
    {
        FEditorManager::Get().SelectActor(actor);
    }
    //UE_LOG("Pick - UUID: %u", PickedComponent->GetUUID());
    return true;
}

bool APicker::SetSelectedComponent(USceneComponent* actorComponent)
{
    FEditorManager::Get().SelectComponent(actorComponent);
    return true;
}

