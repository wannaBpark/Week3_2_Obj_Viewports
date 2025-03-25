#include "StaticMeshComponent.h"
#include "Object/Mesh/ObjManager.h"
#include "Object/Mesh/UStaticMesh.h"

void UStaticMeshComponent::BeginPlay()
{
	Super::BeginPlay();
}

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
	auto Vertices = StaticMesh->GetStaticMeshAsset()->Vertices;
	int Size = Vertices.Num();
	VertexBuffer = UEngine::Get().GetRenderer()->CreateVertexBuffer(Vertices.GetData(), sizeof(FNormalVertex) * Size);
}

void UStaticMeshComponent::CreateIndexBuffer()
{
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
