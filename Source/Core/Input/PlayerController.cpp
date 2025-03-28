#include "PlayerController.h"

#include <unordered_map>
#include "Object//Actor/Camera.h"
#include "PlayerInput.h"
#include "Static/FEditorManager.h"
#include "Core/Math/Plane.h"

#include "../URaycastSystem.h"
#include "Core/EngineConfig.h"

APlayerController::APlayerController() {

}

void APlayerController::HandleCameraMovement(float DeltaTime) {

    FVector NewVelocity(0, 0, 0);
    ACamera* Camera = FEditorManager::Get().GetInputCamera(); 
    float CamSpeed = Camera->CameraSpeed;

    // 입력이 안 들어올 경우 
    if (APlayerInput::Get().IsPressedMouse(true) == false)
    {
        Camera->SetOrthoPivot(FVector::ZeroVector);
        return; 
    }

    // 현재 input대상이 되는 카메라를 가져옴
    
    //전프레임이랑 비교
    //x좌표 받아와서 x만큼 x축회전
    //y좌표 받아와서 y만큼 y축 회전
    FVector MousePrePos = APlayerInput::Get().GetMousePrePos();
    FVector MousePos = APlayerInput::Get().GetMousePos();
    FVector DeltaPos = MousePos - MousePrePos;
    //FQuat CameraRot = FEditorManager::Get().GetCamera()->GetActorTransform().GetRotation();

    FTransform CameraTransform = Camera->GetActorTransform();
    FVector TargetRotation = CameraTransform.GetRotation().GetEuler();
    // 원근 투영의 경우 회전 허용
    if (Camera->GetProjectionMode() == ECameraProjectionMode::Perspective) 
    {
        TargetRotation.Y += Camera->CameraSpeed * DeltaPos.Y * DeltaTime;
        TargetRotation.Z += Camera->CameraSpeed * DeltaPos.X * DeltaTime;
        TargetRotation.Y = FMath::Clamp(TargetRotation.Y, -Camera->MaxYDegree, Camera->MaxYDegree);
        CameraTransform.SetRotation(TargetRotation);
    }
    // Orthogonal 카메라는 이동만 허용 
    else if (Camera->GetProjectionMode() == ECameraProjectionMode::Orthographic)
    {
        FVector Delta = Camera->GetRight() * (DeltaPos.X * Camera->CameraSpeed * DeltaTime) + Camera->GetUp() * (-DeltaPos.Y * Camera->CameraSpeed * DeltaTime);
        FVector MoveDir = Camera->GetOrthoPivot() - Delta;
        Camera->SetOrthoPivot(-Delta);              // Orthogonal 카메라가 움직일 delta 벡터
        CameraTransform.Translate(MoveDir * DeltaTime * CamSpeed * 10.f);
        Camera->SetActorTransform(CameraTransform); 
        return;
    }
    

    
    //CameraTransform.Rotate({0, Camera->CameraSpeed * DeltaPos.Y * DeltaTime, Camera->CameraSpeed * DeltaPos.X * DeltaTime});
    /*FQuat xDelta = FQuat(FVector(0, 0, 1), DeltaPos.X * DeltaTime);
	FQuat yDelta = FQuat(FVector(0, 1, 0), DeltaPos.Y * DeltaTime);
	FQuat newRot = FQuat::MultiplyQuaternions(CameraRot, xDelta);
	newRot = FQuat::MultiplyQuaternions(newRot, yDelta);*/

    //FTransform NewTransf = Camera->GetActorTransform();
    //NewTransf.SetRotation(FQuat::AddQuaternions(CameraRot, DeltaQuaternion));
    //Camera->SetActorTransform(NewTransf);
    

    if (APlayerInput::Get().IsPressedKey(EKeyCode::A)) {
        NewVelocity -= Camera->GetRight();
    }
    if (APlayerInput::Get().IsPressedKey(EKeyCode::D)) {
        NewVelocity += Camera->GetRight();
    }
    if (APlayerInput::Get().IsPressedKey(EKeyCode::W)) {
        NewVelocity += Camera->GetForward();
    }
    if (APlayerInput::Get().IsPressedKey(EKeyCode::S)) {
        NewVelocity -= Camera->GetForward();
    }
    if (APlayerInput::Get().IsPressedKey(EKeyCode::Q))
    {
        NewVelocity -= {0.0f, 0.0f, 1.0f};
    }
    if (APlayerInput::Get().IsPressedKey(EKeyCode::E))
    {
        NewVelocity += {0.0f, 0.0f, 1.0f};
    }
    if (NewVelocity.Length() > 0.001f)
    {
        NewVelocity = NewVelocity.GetSafeNormal();
    }

    //회전이랑 마우스클릭 구현 카메라로 해야할듯?
    CameraTransform.Translate(NewVelocity * DeltaTime * CamSpeed);
    Camera->SetActorTransform(CameraTransform); //임시용
    // FCamera::Get().SetVelocity(NewVelocity);

    // ini에 Camera위치 저장
	FEngineConfig* EngineConfig = UEngine::Get().GetEngineConfig();
	EngineConfig->SaveEngineConfig<float>(EEngineConfigValueType::EEC_EditorCameraPosX, CameraTransform.GetPosition().X);
	EngineConfig->SaveEngineConfig<float>(EEngineConfigValueType::EEC_EditorCameraPosY, CameraTransform.GetPosition().Y);
	EngineConfig->SaveEngineConfig<float>(EEngineConfigValueType::EEC_EditorCameraPosZ, CameraTransform.GetPosition().Z);

	EngineConfig->SaveEngineConfig<float>(EEngineConfigValueType::EEC_EditorCameraRotX, CameraTransform.GetRotation().X);
	EngineConfig->SaveEngineConfig<float>(EEngineConfigValueType::EEC_EditorCameraRotY, CameraTransform.GetRotation().Y);
	EngineConfig->SaveEngineConfig<float>(EEngineConfigValueType::EEC_EditorCameraRotZ, CameraTransform.GetRotation().Z);
	EngineConfig->SaveEngineConfig<float>(EEngineConfigValueType::EEC_EditorCameraRotW, CameraTransform.GetRotation().W);
}

void APlayerController::HandleGizmoMovement(float DeltaTime)
{
    if (APlayerInput::Get().IsPressedMouse(false) == false)
    {
        return;
    }

    AActor* SelectedActor = FEditorManager::Get().GetSelectedActor();
    
    if (SelectedActor == nullptr) //이거를 나중엔 기즈모로 체크
    {
        return;
    }

    
}

void APlayerController::ProcessPlayerInput(float DeltaTime) {
    HandleGizmoMovement(DeltaTime);
    HandleCameraMovement(DeltaTime);
}
