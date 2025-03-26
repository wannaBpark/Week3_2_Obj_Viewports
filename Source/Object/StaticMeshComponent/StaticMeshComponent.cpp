#include "StaticMeshComponent.h"
#include <cfloat>
#include "Object/Actor/Actor.h"
#include "Object/Mesh/ObjManager.h"
#include "Object/Mesh/UStaticMesh.h"
#include "Object/Material/Material.h"

void UStaticMeshComponent::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void UStaticMeshComponent::Render()
{
	UEngine::Get().GetRenderer()->RenderMesh(this);
}

EPrimitiveType UStaticMeshComponent::GetType()
{
	return EPrimitiveType::EPT_Mesh;
}

void UStaticMeshComponent::SetStaticMesh(const FString& staticMeshPath)
{
	StaticMesh = FObjManager::LoadObjStaticMesh(staticMeshPath);

	if (StaticMesh == nullptr)
		return;
	// 버텍스 버퍼 생성
	CreateVertexBuffer();
	// 인덱스 버퍼 생성 
	CreateIndexBuffer();

	bCanBeRendered = true;
	RenderResource.PrimitiveType = GetType();
	RenderResource.Stride = sizeof(FNormalVertex);
	RenderResource.InputLayoutType = InputLayoutType::POSNORMALTANGENTCOLORTEX;
	RenderResource.VertexShaderIndex = 5;
	RenderResource.PixelShaderIndex = 6;
	RenderResource.bUseIndexBuffer = true;
	RenderResource.VertexConstantIndex = 5;

	// 머티리얼 로드
	Materials.Empty();
	for (auto& Kvp : StaticMesh->GetStaticMeshAsset()->SubMeshes)
	{
		auto Material = FObjManager::LoadMaterial(Kvp.second.MaterialName);
		Materials.Add(Material);
	};
}

void UStaticMeshComponent::CreateVertexBuffer()
{
	if (StaticMesh == nullptr)
		return;

	auto Vertices = StaticMesh->GetStaticMeshAsset()->Vertices;
	int Size = Vertices.Num();

	FVector VertexMin(FLT_MAX, FLT_MAX, FLT_MAX);
	FVector VertexMax(-FLT_MAX, -FLT_MAX, -FLT_MAX);

	for (int vertexIndex = 0; vertexIndex < Size; ++vertexIndex)
	{
		const FVector& Position = Vertices[vertexIndex].Position;

		VertexMin.X = FMath::Min(VertexMin.X, Position.X);
		VertexMax.X = FMath::Max(VertexMax.X, Position.X);

		VertexMin.Y = FMath::Min(VertexMin.Y, Position.Y);
		VertexMax.Y = FMath::Max(VertexMax.Y, Position.Y);

		VertexMin.Z = FMath::Min(VertexMin.Z, Position.Z);
		VertexMax.Z = FMath::Max(VertexMax.Z, Position.Z);
	}
	AActor* owner = GetOwner();
	USceneComponent* Root = owner->GetRootComponent();

	Root->SetMinMax(VertexMin, VertexMax);
	
	VertexBuffer = UEngine::Get().GetRenderer()->CreateVertexBuffer(Vertices.GetData(), sizeof(FNormalVertex) * Size);
}

void UStaticMeshComponent::CreateIndexBuffer()
{
	if (StaticMesh == nullptr)
		return;

	auto Indices = StaticMesh->GetStaticMeshAsset()->Indices;
	RenderResource.numVertices = Indices.Num();
	IndexBuffer = UEngine::Get().GetRenderer()->CreateIndexBuffer(Indices);
}

const UMaterial* UStaticMeshComponent::GetMaterial(uint32 Index)
{
	if (Index >= Materials.Num())
	{
		UE_LOG("UStaticMeshComp : Index is out of range");
		return nullptr;
	}
	return Materials[Index];
}

void UStaticMeshComponent::SetMaterial(uint32 Index, UMaterial* InMaterial)
{
	if (Index >= Materials.Num())
	{
		UE_LOG("UStaticMeshComp : Index is out of range");
		return;
	}
	Materials[Index] = InMaterial;
}

void UStaticMeshComponent::LoadAndConstruct(const FActorComponentInfo Info)
{
	Super::LoadAndConstruct(Info);
	
	// Info의 Args의 0번은 StaticMeshPath
	// 이후의 Arg들은 적용되어있는 머티리얼들의 이름
	// 1번 arg는 0번째 머티리얼, 2번 arg는 1번째 머티리얼, ...

	SetStaticMesh(Info.Args[0]);
	for (int i = 1; i < Info.Args.Num(); ++i)
	{
		auto Material = FObjManager::LoadMaterial(FName(Info.Args[i]));
		SetMaterial(i - 1, Material);
	}
}

FActorComponentInfo UStaticMeshComponent::GetActorComponentInfo()
{
	FActorComponentInfo Info = Super::GetActorComponentInfo();

	Info.Name = "UStaticMeshComponent";

	// Info의 Args의 0번은 StaticMeshPath
	// 이후의 Arg들은 적용되어있는 머티리얼들의 이름
	// 1번 arg는 0번째 머티리얼, 2번 arg는 1번째 머티리얼, ...

	Info.Args.Add(StaticMesh->GetStaticMeshAsset()->Path);
	for (int i = 0; i < Materials.Num(); ++i)
	{
		Info.Args.Add(Materials[i]->GetMaterialName().ToString());
	}

	return Info;
}
