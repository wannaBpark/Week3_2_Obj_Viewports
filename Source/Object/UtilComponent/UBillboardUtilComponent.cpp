#include "UBillboardUtilComponent.h"
#include "Core/Math/Matrix.h"
#include "Object/Actor/Camera.h"
#include "Static/FEditorManager.h"
#include "Object/Actor/Picker.h"


void UBillboardUtilComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UBillboardUtilComponent::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

FMatrix UBillboardUtilComponent::GetBillboardMVPMat(URenderer*& Renderer)
{

	if (IsUseBillboard) 
	{
		// 카메라와 빌보드 위치 계산
		ACamera* camera = FEditorManager::Get().GetCamera();
		FVector billboardToEye = camera->GetActorTransform().GetPosition() - this->GetComponentTransform().GetPosition();
		billboardToEye.Normalize();

		// 카메라의 오른쪽, Up 벡터 계산
		FVector upVector = camera->GetActorTransform().GetLocalUp();
		upVector.Normalize();
		FVector rightVector = FVector::CrossProduct(upVector, billboardToEye);
		
		rightVector.Normalize();


		FVector adjustedUp = FVector::CrossProduct(billboardToEye, rightVector);
		adjustedUp.Normalize();

		// 빌보드 회전 행렬 생성
		FMatrix BillboardRotation = FMatrix(
			{ billboardToEye.X, billboardToEye.Y, billboardToEye.Z, 0.0f },
			{ rightVector.X,    rightVector.Y,    rightVector.Z,    0.0f },
			{ adjustedUp.X,     adjustedUp.Y,     adjustedUp.Z,     0.0f },
			{ 0.0f, 0.0f, 0.0f, 1.0f }
		);

		// 빌보드 위치를 고려한 최종 변환 행렬
		FMatrix BillBoardTransform = BillboardRotation * this->GetComponentTransformMatrix();

		// MVP 행렬 생성 (뷰 행렬을 Renderer에서 가져옴)
		FMatrix MVP = FMatrix::Transpose(Renderer->GetProjectionMatrix())
			* FMatrix::Transpose(Renderer->GetViewMatrix())   // 기존의 View Matrix를 사용
			* FMatrix::Transpose(BillBoardTransform);         // 빌보드 변환 적용
		return MVP;
	}
	else 
	{
		FMatrix WorldPosition = this->GetComponentTransformMatrix();
		FMatrix MVP = FMatrix::Transpose(Renderer->GetProjectionMatrix())
			* FMatrix::Transpose(Renderer->GetViewMatrix())
			* FMatrix::Transpose(WorldPosition);

		return MVP;
	}
	
}

FMatrix UBillboardUtilComponent::GetBillboardMVPMatForText(URenderer*& Renderer, USceneComponent* RootComponent, float YOffset)
{
	// 카메라와 빌보드 위치 계산
	ACamera* camera = FEditorManager::Get().GetCamera();
	FVector billboardToEye = RootComponent->GetComponentTransform().GetPosition() - camera->GetActorTransform().GetPosition();
	billboardToEye.Normalize();

	// 카메라의 오른쪽, Up 벡터 계산
	FVector upVector = camera->GetActorTransform().GetLocalUp();
	upVector.Normalize();
	FVector rightVector = FVector::CrossProduct(upVector, billboardToEye);
	rightVector.Normalize();


	FVector adjustedUp = FVector::CrossProduct(billboardToEye, rightVector);
	adjustedUp.Normalize();

	FVector rightOffset = rightVector * YOffset;
	// 빌보드 회전 행렬 생성
	FMatrix BillboardRotation = FMatrix(
		{ billboardToEye.X, billboardToEye.Y, billboardToEye.Z, 0.0f },
		{ rightVector.X,    rightVector.Y,    rightVector.Z,    0.0f },
		{ adjustedUp.X,     adjustedUp.Y,     adjustedUp.Z,     0.0f },
		{ rightOffset.X, rightOffset.Y, rightOffset.Z, 1.0f }
	);

	// 빌보드 위치를 고려한 최종 변환 행렬
	FMatrix BillBoardTransform = BillboardRotation * RootComponent->GetComponentTransformMatrix();


	// MVP 행렬 생성 (뷰 행렬을 Renderer에서 가져옴)
	FMatrix MVP = FMatrix::Transpose(Renderer->GetProjectionMatrix())
		* FMatrix::Transpose(Renderer->GetViewMatrix())   // 기존의 View Matrix를 사용
		* FMatrix::Transpose(BillBoardTransform);         // 빌보드 변환 적용
	return MVP;
	
}



FMatrix3 UBillboardUtilComponent::GetBillboardMVPMat3(URenderer*& Renderer)
{
	FMatrix3 MVP;
	if (IsUseBillboard)
	{
		// 카메라와 빌보드 위치 계산
		ACamera* camera = FEditorManager::Get().GetCamera();
		FVector billboardToEye = camera->GetActorTransform().GetPosition() - this->GetComponentTransform().GetPosition();
		billboardToEye.Normalize();

		// 카메라의 오른쪽, Up 벡터 계산
		FVector upVector = camera->GetActorTransform().GetLocalUp();
		upVector.Normalize();
		FVector rightVector = FVector::CrossProduct(upVector, billboardToEye);

		rightVector.Normalize();


		FVector adjustedUp = FVector::CrossProduct(billboardToEye, rightVector);
		adjustedUp.Normalize();

		// 빌보드 회전 행렬 생성
		FMatrix BillboardRotation = FMatrix(
			{ billboardToEye.X, billboardToEye.Y, billboardToEye.Z, 0.0f },
			{ rightVector.X,    rightVector.Y,    rightVector.Z,    0.0f },
			{ adjustedUp.X,     adjustedUp.Y,     adjustedUp.Z,     0.0f },
			{ 0.0f, 0.0f, 0.0f, 1.0f }
		);

		// 빌보드 위치를 고려한 최종 변환 행렬
		FMatrix BillBoardTransform = BillboardRotation * this->GetComponentTransformMatrix();

		// MVP 행렬 생성 (뷰 행렬을 Renderer에서 가져옴)
		MVP = {
			.Model = BillBoardTransform,
			.View = Renderer->GetViewMatrix(),
			.Projection = Renderer->GetProjectionMatrix()
		};

	}
	else
	{
		MVP = {
			.Model = this->GetComponentTransformMatrix(),
			.View = Renderer->GetViewMatrix(),
			.Projection = Renderer->GetProjectionMatrix()
		};
	}
	return MVP;
}