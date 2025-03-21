#include "UPrimitiveComponent.h"
#include "Object/World/World.h"
#include "Object/Actor/Actor.h"
#include "../Source/Static/FEditorManager.h"
#include "Object/Actor/Camera.h"
#include "Core/Rendering/TextAtlasManager.h"
#include "Core/Rendering/SubUVManager.h"
#include "Core/Math/Vector.h"
#include "Object/UtilComponent/UBillboardUtilComponent.h"	
#include "Object/Gizmo/WorldGizmo.h"

void UPrimitiveComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UPrimitiveComponent::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void UPrimitiveComponent::UpdateConstantPicking(const URenderer& Renderer, const FVector4 UUIDColor)const
{
	Renderer.UpdateConstantPicking(UUIDColor);
}

void UPrimitiveComponent::UpdateConstantDepth(const URenderer& Renderer, const int Depth)const
{
	Renderer.UpdateConstantDepth(Depth);
}

void UPrimitiveComponent::Render() // 오버라이딩 필요
{
	URenderer* Renderer = UEngine::Get().GetRenderer();
	if (Renderer == nullptr || !bCanBeRendered)
	{
		return;
	}
	
	//Renderer->RenderPrimitive(this); // 실제 렌더링 요청 수행
	
	this->UpdateConstantData(Renderer);
	Renderer->RenderPrimitive(this, this->RenderResource);
}

void UPrimitiveComponent::RegisterComponentWithWorld(UWorld* World)
{
	World->AddRenderComponent(this);
}

void UPrimitiveComponent::SetUseBillboardUtil(bool bUse)
{
	if (BillboardUtil == nullptr)
	{
		BillboardUtil = GetOwner()->AddComponent<UBillboardUtilComponent>();
		BillboardUtil->SetupAttachment(GetOwner()->GetRootComponent(), true);
	}
	
	bUseBillboardUtil = bUse;
	BillboardUtil->SetIsUseBillboard(bUse);
}

void UPrimitiveComponent::UpdateConstantData(URenderer*& Renderer)
{
	FVector4 indexColor = APicker::EncodeUUID(this->GetUUID());
	indexColor /= 255.0f;
	if (GetOwner()->IsGizmoActor() == false)
	{
		if (GetOwner()== FEditorManager::Get().GetSelectedActor())
		{
			bIsPicked = true;
			//UE_LOG("ispicked ");
		}
		else
		{
			bIsPicked = false;
			//UE_LOG("bisunpicked ");
		}		
	}

	FConstants UpdateInfo{
		.MVP = this->GetComponentTransformMatrix(),
		.Color = this->GetCustomColor(),
		.bUseVertexColor = (uint32)this->IsUseVertexColor(),
		.eyeWorldPos = FEditorManager::Get().GetCamera()->GetActorTransform().GetPosition(),
		.indexColor = indexColor,
		.bIsPicked = (uint32)this->IsPicked(),
		.Padding = FEditorManager::Get().GetCamera()->GetActorTransform().GetPosition(),
	};

	FMatrix MVP;
	FMatrix& WorldPosition = UpdateInfo.MVP;

	if (BillboardUtil != nullptr) {
		MVP = BillboardUtil->GetBillboardMVPMat(Renderer);
	}
	else  {
		// 업데이트할 자료형들
		MVP = FMatrix::Transpose(Renderer->GetProjectionMatrix())
			* FMatrix::Transpose(Renderer->GetViewMatrix())
			* FMatrix::Transpose(WorldPosition);
	}

	if (dynamic_cast<AWorldGizmo*>(GetOwner()) != nullptr) {
		ACamera* Camera = FEditorManager::Get().GetCamera();
		float ViewportSize = Camera->GetViewportSize();
		float Near = Camera->GetNear();
		float Far = Camera->GetFar();
		// 카메라의 오른쪽, Up 벡터 계산
		// 업데이트할 자료형들
		// 단순하게 NDC 상의 오프셋을 적용하는 방법 (행렬 순서에 주의)
		//FMatrix NDCOffset = FMatrix::GetTranslateMatrix(0.9,0.1,0);
		MVP = FMatrix::Transpose(FMatrix::OrthoForLH(ViewportSize, ViewportSize, Near, Far))
			* FMatrix::Transpose(Renderer->GetViewMatrix())
			* FMatrix::Transpose(WorldPosition);

		FVector delta = FVector(-0.8,-0.8,0);

		MVP.M[0][3] = delta.X;
		MVP.M[1][3] = delta.Y;
		MVP.M[2][3] = delta.Z;

	}

	ConstantData = {
		MVP, UpdateInfo.Color,
		UpdateInfo.bUseVertexColor,
		UpdateInfo.eyeWorldPos,
		UpdateInfo.indexColor,
		UpdateInfo.bIsPicked,
		UpdateInfo.Padding,
	};
	

	Renderer->UpdateBuffer(ConstantData, RenderResource.VertexConstantIndex);
	Renderer->UpdateBuffer(ConstantData, RenderResource.PixelConstantIndex);		// 픽셀 상수 버퍼 업데이트 시 
}

void UPrimitiveComponent::UpdateLightConstantData(URenderer*& Renderer)
{

	FVector4 indexColor = APicker::EncodeUUID(this->GetUUID());
	indexColor /= 255.0f;
	if (GetOwner()->IsGizmoActor() == false)
	{
		if (GetOwner() == FEditorManager::Get().GetSelectedActor())
		{
			bIsPicked = true;
			//UE_LOG("ispicked ");
		}
		else
		{
			bIsPicked = false;
			//UE_LOG("bisunpicked ");
		}
	}

	FMatrix3 MVP;
	FMatrix modelWorld = this->GetComponentTransformMatrix();
	if (BillboardUtil != nullptr) {
		MVP = BillboardUtil->GetBillboardMVPMat3(Renderer);
	}
	modelWorld.M[3][0] = modelWorld.M[3][1] = modelWorld.M[3][2] = 0;
	modelWorld = modelWorld.Inverse();
	FMatrix InvTranspose = FMatrix::Transpose(FMatrix::Transpose(modelWorld));
	FMatrix ViewMatrices[4] = {
		FMatrix::Transpose(Renderer->GetViewMatrix()),
		FMatrix::Transpose(Renderer->GetViewportMatrixById(1)),
		FMatrix::Transpose(Renderer->GetViewportMatrixById(2)),
		FMatrix::Transpose(Renderer->GetViewportMatrixById(3)),
	};

	FLightConstants UpdateInfo{
		.Model = FMatrix::Transpose(this->GetComponentTransformMatrix()),
		.Views = {ViewMatrices[0],ViewMatrices[1],ViewMatrices[2],ViewMatrices[3]},
		.Projection = FMatrix::Transpose(Renderer->GetProjectionMatrix()),
		.InvTranspose = InvTranspose,
		.Color = this->GetCustomColor(),
		.bUseVertexColor = (uint32)this->IsUseVertexColor(),
		.eyeWorldPos = { FEditorManager::Get().GetCamera()->GetActorTransform().GetPosition() },
		//.bPadding = 0,
		.indexColor = indexColor,
		.bIsPicked = (uint32)this->IsPicked(),
		.Padding = FVector(0.0f, 0.0f, 0.0f),
		.ViewportIndex = 0,
		.Padding2 = FVector(0.0f, 0.0f, 0.0f),
	};

	if (BillboardUtil != nullptr) {
		UpdateInfo.Model = FMatrix::Transpose(MVP.Model);
		UpdateInfo.Views[0] = MVP.View;
		UpdateInfo.Views[1] = ViewMatrices[1];
		UpdateInfo.Views[2] = ViewMatrices[2];
		UpdateInfo.Views[3] = ViewMatrices[3];
		UpdateInfo.Projection = FMatrix::Transpose(MVP.Projection);
	};

	LightConstantData = UpdateInfo;

	Renderer->UpdateBuffer(LightConstantData, RenderResource.VertexConstantIndex);
	Renderer->UpdateBuffer(LightConstantData, RenderResource.PixelConstantIndex);		// 픽셀 상수 버퍼 업데이트 시 
}

void UBillBoardComp::SetUseBillboardUtil(bool bUse)
{
	bUseBillboardUtil = bUse;
}

void UBillBoardComp::UpdateConstantData(URenderer*& Renderer)
{
	ConstantUpdateInfo UpdateInfo{
		this->GetComponentTransformMatrix(),
		this->GetCustomColor(),
		this->IsUseVertexColor()
	};

	// 카메라와 빌보드 위치 계산
	ACamera* camera = FEditorManager::Get().GetCamera();
	FVector billboardToEye = camera->GetActorTransform().GetPosition() - this->GetComponentTransform().GetPosition();
	billboardToEye.Normalize();

	// 카메라의 오른쪽, Up 벡터 계산
	FVector upVector = camera->GetActorTransform().GetLocalUp();
	FVector rightVector = FVector::CrossProduct(upVector, billboardToEye);
	upVector.Normalize();
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


	// 상수 버퍼 업데이트
	ConstantData.MVP = MVP;
	ConstantData.Color = UpdateInfo.Color;
	ConstantData.bUseVertexColor = UpdateInfo.bUseVertexColor;

	Renderer->UpdateBuffer(ConstantData, RenderResource.VertexConstantIndex);
}

// 박녕준 천재 bb

void UWorldGridComp::UpdateConstantData(URenderer*& Renderer)
{
	FVector4 indexColor = APicker::EncodeUUID(this->GetUUID());
	indexColor /= 255.0f;
	ConstantUpdateInfo UpdateInfo{
		this->GetComponentTransformMatrix(),
		this->GetCustomColor(),
		(uint32)this->IsUseVertexColor(),
		FEditorManager::Get().GetCamera()->GetActorTransform().GetPosition(),
		indexColor
	};

	// 업데이트할 자료형들
	FMatrix MVP = FMatrix::Transpose(Renderer->GetProjectionMatrix())
		* FMatrix::Transpose(Renderer->GetViewMatrix())
		* FMatrix::Transpose(UpdateInfo.WorldPosition);


	ConstantData = {
		MVP, UpdateInfo.Color,
		UpdateInfo.bUseVertexColor,
		UpdateInfo.eyeWorldPos,
		UpdateInfo.indexColor,
	};

	Renderer->UpdateBuffer(ConstantData, RenderResource.VertexConstantIndex);
}
void UCharComp::UpdateConstantData(URenderer*& Renderer)
{
	FVector4 SzOffset;
	SzOffset = UTextAtlasManager::GetCharUV(this->GetChar(), AtlasName);
	FAtlasConstants UpdateInfo{
		this->GetComponentTransformMatrix(),
		SzOffset,
	};

	FMatrix MVP;

	if (bUseBillboardUtil == false) 
	{
		FMatrix WorldPosition = this->GetComponentTransformMatrix();
		// 업데이트할 자료형들
		MVP = FMatrix::Transpose(Renderer->GetProjectionMatrix())
			* FMatrix::Transpose(Renderer->GetViewMatrix())
			* FMatrix::Transpose(WorldPosition);
	}
	else {
		MVP = UBillboardUtilComponent::GetBillboardMVPMatForText(Renderer, Parent, this->RelativeTransform.GetPosition().Y);

	}

	AtlasConstantData = { MVP, UpdateInfo.AtlasSzOffset };
	Renderer->UpdateBuffer(AtlasConstantData, RenderResource.VertexConstantIndex);
}

void USubUVComponent::UpdateConstantData(URenderer*& Renderer)   // 빌보드도 추가
{
	FVector4 SzOffset;
	SzOffset = USubUVManager::GetFrameUV(this->GetFrame(), AtlasName);
	FAtlasConstants UpdateInfo{
		this->GetComponentTransformMatrix(),
		SzOffset,
	};
	// 업데이트할 자료형들
	FMatrix MVP = FMatrix::Transpose(Renderer->GetProjectionMatrix())
		* FMatrix::Transpose(Renderer->GetViewMatrix())
		* FMatrix::Transpose(this->GetComponentTransformMatrix());

	AtlasConstantData = { MVP, UpdateInfo.AtlasSzOffset };
	Renderer->UpdateBuffer(AtlasConstantData, RenderResource.VertexConstantIndex);
}


