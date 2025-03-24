#pragma once

#define _TCHAR_DEFINED
#include <d3d11.h>

#include <wrl/client.h>
#include <unordered_map>
#include <tuple>
#include "Primitive/PrimitiveVertices.h"
#include "Core/Container/Array.h"

#include "Core/Math/Vector.h"
#include "Core/Container/String.h"

struct FPosColorNormalTex;
struct BufferInfo
{
public:
	BufferInfo() = default;
	BufferInfo(ID3D11Buffer* InBuffer, int BufferSize, D3D_PRIMITIVE_TOPOLOGY InTopology)
	{
		Buffer = InBuffer;
		//InBuffer.CopyTo(Buffer.GetAddressOf());
		Size = BufferSize;
		Topology = InTopology;
	}

	ID3D11Buffer* GetBuffer() const { return Buffer.Get(); }
	int GetSize() const { return Size; }
	D3D_PRIMITIVE_TOPOLOGY GetTopology() const { return Topology; }

private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> Buffer;
	D3D_PRIMITIVE_TOPOLOGY Topology;
	int Size;
};

class FBufferCache
{
private:
	std::unordered_map <EPrimitiveType, BufferInfo> Cache;
	std::unordered_map <FString, BufferInfo> StaticMeshCache;

public:
	FBufferCache();
	~FBufferCache();

	void Init();
	BufferInfo GetBufferInfo(EPrimitiveType Type);

public:
	TArray<FVertexSimple> CreateArrowVertices();
	TArray<FVertexSimple> CreateConeVertices();
	TArray<FVertexSimple> CreateCylinderVertices();
	std::tuple<TArray<FPosColorNormalTex>, std::vector<uint32>> CreateCubeTexVertices();
	std::tuple<TArray<FPosColorNormalTex>, std::vector<uint32>> CreateCylinderTexVertices();
	std::tuple<TArray<FPosColorNormalTex>, std::vector<uint32>> CreateTriangleTexVertices();
	std::tuple<TArray<FPosColorNormalTex>, std::vector<uint32>> CreateCircleTexVertices();
	std::tuple<TArray<FPosColorNormalTex>, std::vector<uint32>> CreateSphereTexVertices();
	std::tuple<TArray<FPosColorNormalTex>, std::vector<uint32>> CreateBillBoardVertices();
	std::tuple<TArray<FPosColorNormalTex>, std::vector<uint32>> CreateConeTexVertices();
	std::tuple<TArray<FVertexSimple>, std::vector<uint32>> CreateBoundingBoxVertices();
	std::tuple<TArray<FVertexSimple>, std::vector<uint32>> CreateWorldGridVertices(float cellSize, float gridSize);

	static std::tuple<TArray<FVertexSimple>, std::vector<uint32>> CreateWorldGridVertices(float cellSize, float gridSize, const FVector cameraPos);


	std::tuple<TArray<FPosColorNormalTex>, std::vector<uint32>> CreateTextTexVertices();


private :
	BufferInfo CreateVertexBufferInfo(EPrimitiveType Type);
};


