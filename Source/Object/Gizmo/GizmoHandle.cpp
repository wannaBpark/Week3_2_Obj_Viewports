#include "GizmoHandle.h"
#include "Object/Actor/Camera.h"
#include "Object/PrimitiveComponent/UPrimitiveComponent.h"
#include "Object/World/World.h"
#include "Static/FEditorManager.h"
#include <Core/Input/PlayerInput.h>

AGizmoHandle::AGizmoHandle()
{
	bIsGizmo = true;
	// !NOTE : Z방향으로 서있음
	// z
	UCylinderComp* ZArrow = AddComponent<UCylinderComp>();
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

	UCircleComp* ZCircle = AddComponent<UCircleComp>();
	ZCircle->SetupAttachment(ZArrow);
	ZCircle->SetRelativeTransform(FTransform(FVector(0.0f, 0.0f, 0.0f), FVector(90.0f, 0.0f, 0.0f), FVector(1, 1, 1)));
	ZCircle->SetCustomColor(FVector4(0.0f, 0.0f, 1.0f, 1.0f));
	CircleComponents.Add(ZCircle);

	// x
	UCircleComp* XCircle = AddComponent<UCircleComp>();
	XCircle->SetupAttachment(ZArrow);
	XCircle->SetRelativeTransform(FTransform(FVector(0.0f, 0.0f, 0.0f), FVector(0.0f, 0.0f, 90.0f), FVector(1, 1, 1)));
	XCircle->SetCustomColor(FVector4(1.0f, 0.0f, 0.0f, 1.0f));
	CircleComponents.Add(XCircle);

	UCircleComp* YCircle = AddComponent<UCircleComp>();
	YCircle->SetupAttachment(ZArrow);
	YCircle->SetRelativeTransform(FTransform(FVector(0.0f, 0.0f, 0.0f), FVector(0.0f, 0.0f, 0.0f), FVector(1, 1, 1)));
	YCircle->SetCustomColor(FVector4(0.0f, 1.0f, 0.0f, 1.0f));
	CircleComponents.Add(YCircle);

	RootComponent = ZArrow;

	UEngine::Get().GetWorld()->AddZIgnoreComponent(ZArrow);
	UEngine::Get().GetWorld()->AddZIgnoreComponent(XArrow);
	UEngine::Get().GetWorld()->AddZIgnoreComponent(YArrow);
	UEngine::Get().GetWorld()->AddZIgnoreComponent(ZCircle);
	UEngine::Get().GetWorld()->AddZIgnoreComponent(XCircle);
	UEngine::Get().GetWorld()->AddZIgnoreComponent(YCircle);

	SetActive(false);
}

void AGizmoHandle::Tick(float DeltaTime)
{
	AActor* SelectedActor = FEditorManager::Get().GetSelectedActor();
	if (SelectedActor != nullptr && bIsActive)
	{
		FTransform GizmoTr = GetActorTransform();
		// Gizmo의 위치정보를 받아옵니다.
		//FTransform GizmoTr;
		GizmoTr.SetPosition(SelectedActor->GetActorTransform().GetPosition());
		//GizmoTr.SetRotation(SelectedActor->GetActorTransform().GetRotation());
		GizmoTr.SetScale(FVector(1, 1, 1));
		// Actor의 Root component == 위치정보를 수정합니다.
		SetActorTransform(GizmoTr);
	}

	SetScaleByDistance();  // 직교 투영 행렬로 변경 해야함.

	AActor::Tick(DeltaTime);

	SetPickGizmo((int32)SelectedAxis);

	if (SelectedAxis != ESelectedAxis::None)
	{
		if (AActor* Actor = FEditorManager::Get().GetSelectedActor())
		{
			// 마우스의 커서 위치를 가져오기
			POINT pt;
			GetCursorPos(&pt);
			ScreenToClient(UEngine::Get().GetWindowHandle(), &pt);

			RECT Rect;
			GetClientRect(UEngine::Get().GetWindowHandle(), &Rect);
			int ScreenWidth = Rect.right - Rect.left;
			int ScreenHeight = Rect.bottom - Rect.top;

			// 커서 위치를 NDC로 변경
			float PosX = 2.0f * pt.x / ScreenWidth - 1.0f;
			float PosY = -2.0f * pt.y / ScreenHeight + 1.0f;

			// Projection 공간으로 변환
			FVector4 RayOrigin{ PosX, PosY, 0.0f, 1.0f };
			FVector4 RayEnd{ PosX, PosY, 1.0f, 1.0f };

			// View 공간으로 변환
			FMatrix InvProjMat = UEngine::Get().GetRenderer()->GetProjectionMatrix().Inverse();
			RayOrigin = InvProjMat.TransformVector4(RayOrigin);
			RayOrigin.W = 1;
			RayEnd = InvProjMat.TransformVector4(RayEnd);
			RayEnd *= 1000.0f;  // 프러스텀의 Far 값이 적용이 안돼서 수동으로 곱함
			RayEnd.W = 1;

			// 마우스 포인터의 월드 위치와 방향
			FMatrix InvViewMat = FEditorManager::Get().GetCamera()->GetViewMatrix().Inverse();
			RayOrigin = InvViewMat.TransformVector4(RayOrigin);
			RayOrigin /= RayOrigin.W = 1;
			RayEnd = InvViewMat.TransformVector4(RayEnd);
			RayEnd /= RayEnd.W = 1;
			FVector RayDir = (RayEnd - RayOrigin).GetSafeNormal();

			// 액터와의 거리
			float Distance = FVector::Distance(RayOrigin, Actor->GetActorTransform().GetPosition());

			// Ray 방향으로 Distance만큼 재계산
			FVector Result = RayOrigin + RayDir * Distance;

			if (CachedRayResult == FVector::ZeroVector)
			{
				CachedRayResult = Result;
				return;
			}

			FVector Delta = Result - CachedRayResult;
			CachedRayResult = Result;
			FTransform CompTransform = Actor->GetActorTransform();

			DoTransform(CompTransform, Delta, Actor);
		}
	}

	if (APlayerInput::Get().GetKeyDown(EKeyCode::Space))
	{
		int type = static_cast<int>(GizmoType);
		type = (type + 1) % static_cast<int>(EGizmoType::Max);
		GizmoType = static_cast<EGizmoType>(type);
		if (GizmoType == EGizmoType::Rotate)
		{
			for (auto& Circle : CircleComponents)
			{
				Circle->SetCanBeRendered(true);
			}
			for (auto& Cylinder : CylinderComponents)
			{
				Cylinder->SetCanBeRendered(false);
			}
		}
		else
		{
			for (auto& Cylinder : CylinderComponents)
			{
				Cylinder->SetCanBeRendered(true);
			}
			for (auto& Circle : CircleComponents)
			{
				Circle->SetCanBeRendered(false);
			}
		}
	}

}

void AGizmoHandle::SetScaleByDistance()
{
	FTransform MyTransform = GetActorTransform();

	// 액터의 월드 위치
	FVector actorWorldPos = MyTransform.GetPosition();

	FTransform CameraTransform = FEditorManager::Get().GetCamera()->GetActorTransform();

	// 카메라의 월드 위치
	FVector cameraWorldPos = CameraTransform.GetPosition();

	// 거리 계산
	float distance = (actorWorldPos - cameraWorldPos).Length();

	float baseScale = 1.0f;    // 기본 스케일
	float scaleFactor = distance * 0.1f; // 거리에 비례하여 스케일 증가

	// float minScale = 1.0f;     // 최소 스케일
	// float maxScale = 1.0f;     // 최대 스케일
	// float scaleFactor = clamp(1.0f / distance, minScale, maxScale);

	MyTransform.SetScale(scaleFactor, scaleFactor, scaleFactor);
}

void AGizmoHandle::SetPickGizmo(int index)
{
	if (index == 0)
	{
		for (int i = 0; i < 3; ++i)
		{
			CylinderComponents[i]->SetIsPicked(false);
			CircleComponents[i]->SetIsPicked(false);
		}
	}
	else
	{
		for (int i = 0; i < 3; ++i)
		{
			CylinderComponents[i]->SetIsPicked(i == index%3);
			CircleComponents[i]->SetIsPicked(i == index%3);
		}
	}
}


void AGizmoHandle::SetActive(bool bActive)
{
	bIsActive = bActive;
	if (bIsActive)
	{
		if (GizmoType == EGizmoType::Rotate)
		{
			for (auto& Circle : CircleComponents)
			{
				Circle->SetCanBeRendered(bActive);
			}
			for (auto& Cylinder : CylinderComponents)
			{
				Cylinder->SetCanBeRendered(!bActive);
			}
		}
		else
		{
			for (auto& Cylinder : CylinderComponents)
			{
				Cylinder->SetCanBeRendered(bActive);
			}
			for (auto& Circle : CircleComponents)
			{
				Circle->SetCanBeRendered(!bActive);
			}
		}
	}
	else
	{
		for (auto& Cylinder : CylinderComponents)
		{
			Cylinder->SetCanBeRendered(false);
		}
		for (auto& Circle : CircleComponents)
		{
			Circle->SetCanBeRendered(false);
		}
	}
}

const char* AGizmoHandle::GetTypeName()
{
	return "GizmoHandle";
}

void AGizmoHandle::DoTransform(FTransform& AT, FVector Delta, AActor* Actor)
{
	FVector WorldDirection;

	FVector CamToComp = (Actor->GetActorTransform().GetPosition() - FEditorManager::Get().GetCamera()->GetActorTransform().GetPosition()).GetSafeNormal();

	FVector RotationDelta = FVector::CrossProduct(Delta, CamToComp);

	switch (SelectedAxis)
	{
	case ESelectedAxis::X:
		WorldDirection = FVector(1, 0, 0);
		break;
	case ESelectedAxis::Y:
		WorldDirection = FVector(0, 1, 0);
		break;
	case ESelectedAxis::Z:
		WorldDirection = FVector(0, 0, 1);
		break;
	}

	Delta = WorldDirection * FVector::DotProduct(Delta, WorldDirection);
	int Sign = FVector::DotProduct(RotationDelta, WorldDirection) > 0 ? 1 : -1;

	switch (GizmoType)
	{
	case EGizmoType::Translate:
		AT.Translate(Delta);
		break;
	case EGizmoType::Rotate:
		AT.Rotate(Delta * 10);
		break;
	case EGizmoType::Scale:
		Delta = WorldDirection * FVector::DotProduct(Delta, WorldDirection) * 2;
		AT.AddScale(Delta);
		break;
	}


	Actor->SetActorTransform(AT);
}


