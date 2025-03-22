#pragma once

#define _TCHAR_DEFINED
#include <d3d11.h>
#include <memory>

#include <unordered_map> // ShaderMap

#include "UI.h"
#include "Core/Math/Vector.h"
// #include "Object/Actor/Camera.h"
#include "Core/Rendering/BufferCache.h"
#include "Core/Math/Matrix.h"
#include "Core/Engine.h"
#include "Primitive/PrimitiveVertices.h"
#include "Core/Math/Plane.h"
#include "Core/Rendering/ShaderParameterMacros.h" // InputlayoutType
#include "../EEnum.h"

using namespace Microsoft::WRL;
struct FVertexSimple;
struct FVector4;
struct FRenderResource;

class ACamera;

class URenderer
{
private:
public:
    /** Renderer를 초기화 합니다. */
    void Create(HWND hWindow);

    /** Renderer에 사용된 모든 리소스를 해제합니다. */
    void Release();

    void CreateShader();

    void ReleaseShader();

    void CreateConstantBuffer();

    void ReleaseConstantBuffer();

    void CreateTexturesSamplers();
    void ReleaseTexturesSamplers();


    /** 스왑 체인의 백 버퍼와 프론트 버퍼를 교체하여 화면에 출력 */
    void SwapBuffer() const;

    /** 렌더링 파이프라인을 준비 합니다. */
    void Prepare() const;

    /** 셰이더를 준비 합니다. */
    void PrepareShader() const;

	void RenderPrimitive(class UPrimitiveComponent* PrimitiveComp);
    void RenderPrimitive(class UPrimitiveComponent* PrimitiveComp, struct FRenderResource& RenderResource);

    /**
     * Buffer에 있는 Vertex를 그립니다.
     * @param pBuffer 렌더링에 사용할 버텍스 버퍼에 대한 포인터
     * @param numVertices 버텍스 버퍼에 저장된 버텍스의 총 개수
     */
    void RenderPrimitiveInternal(ID3D11Buffer* pBuffer, UINT numVertices) const;
    void RenderPrimitiveIndexed(ID3D11Buffer* pVertexBuffer, ID3D11Buffer* pIndexBuffer, UINT numIndices) const;

    /**
     * 정점 데이터로 Vertex Buffer를 생성합니다.
     * @param Vertices 버퍼로 변환할 정점 데이터 배열의 포인터
     * @param ByteWidth 버퍼의 총 크기 (바이트 단위)
     * @return 생성된 버텍스 버퍼에 대한 ID3D11Buffer 포인터, 실패 시 nullptr
     *
     * @note 이 함수는 D3D11_USAGE_IMMUTABLE 사용법으로 버퍼를 생성합니다.
     */
     /*ID3D11Buffer* CreateVertexBuffer(const FVertexSimple* Vertices, UINT ByteWidth) const
    {
        D3D11_BUFFER_DESC VertexBufferDesc = {};
        VertexBufferDesc.ByteWidth = ByteWidth;
        VertexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
        VertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

        D3D11_SUBRESOURCE_DATA VertexBufferSRD = {};
        VertexBufferSRD.pSysMem = Vertices;

        ID3D11Buffer* VertexBuffer;
        const HRESULT Result = Device->CreateBuffer(&VertexBufferDesc, &VertexBufferSRD, &VertexBuffer);
        if (FAILED(Result))
        {
            return nullptr;
        }
        return VertexBuffer;
    }*/

    /** Buffer를 해제합니다. */
    void ReleaseVertexBuffer(ID3D11Buffer* pBuffer) const;

    /** Constant Data를 업데이트 합니다. */
    void UpdateConstant(const ConstantUpdateInfo& UpdateInfo) const;

    ID3D11Device* GetDevice() const;
    ID3D11DeviceContext* GetDeviceContext() const;

    /** View 변환 Matrix를 업데이트 합니다. */
    void UpdateViewMatrix(const FTransform& CameraTransform);

    /** Projection 변환 Matrix를 업데이트 합니다. */
    void UpdateProjectionMatrix(ACamera* Camera);

	void OnUpdateWindowSize(int Width, int Height);

    void OnResizeComplete();

    void RenderMesh(class UStaticMeshComponent* MeshComp);

protected:
    /** Direct3D Device 및 SwapChain을 생성합니다. */
    void CreateDeviceAndSwapChain(HWND hWindow);

    /** Direct3D Device 및 SwapChain을 해제합니다.  */
    void ReleaseDeviceAndSwapChain();

    /** 프레임 버퍼를 생성합니다. */
    void CreateFrameBuffer();

    /** 뎁스 스텐실 버퍼를 생성합니다. */
	void CreateDepthStencilBuffer();

    /** 뎁스 스텐실 상태를 생성합니다. */
	void CreateDepthStencilState();
	
    /** 프레임 버퍼를 해제합니다. */
    void ReleaseFrameBuffer();

	/** 뎁스 스텐실 버퍼를 해제합니다. */
	void ReleaseDepthStencilBuffer();
	
    /** 레스터라이즈 상태를 생성합니다. */
    void CreateRasterizerState();

    /** 레스터라이저 상태를 해제합니다. */
    void ReleaseRasterizerState();

    void CreateBufferCache();

    void InitMatrix();

protected:
    // Direct3D 11 장치(Device)와 장치 컨텍스트(Device Context) 및 스왑 체인(Swap Chain)을 관리하기 위한 포인터들
    ID3D11Device* Device = nullptr;                         // GPU와 통신하기 위한 Direct3D 장치
    ID3D11DeviceContext* DeviceContext = nullptr;           // GPU 명령 실행을 담당하는 컨텍스트
    IDXGISwapChain* SwapChain = nullptr;                    // 프레임 버퍼를 교체하는 데 사용되는 스왑 체인

    // 렌더링에 필요한 리소스 및 상태를 관리하기 위한 변수들
    ID3D11RenderTargetView* RTVs[2];
    ID3D11RasterizerState* RasterizerStates[3] = { nullptr, nullptr, nullptr }; // 래스터라이저 상태 (Lit, Unlit, Wireframe)
    ID3D11Texture2D* FrameBuffer = nullptr;                 // 화면 출력용 텍스처
    ID3D11RenderTargetView* FrameBufferRTV = nullptr;       // 텍스처를 렌더 타겟으로 사용하는 뷰
    ID3D11Buffer* ConstantBuffer = nullptr;                 // 쉐이더에 데이터를 전달하기 위한 상수 버퍼

    FLOAT ClearColor[4] = { 0.025f, 0.025f, 0.025f, 1.0f }; // 화면을 초기화(clear)할 때 사용할 색상 (RGBA)
    D3D11_VIEWPORT ViewportInfo = {};                       // 렌더링 영역을 정의하는 뷰포트 정보

    // Shader를 렌더링할 때 사용되는 변수들
    ID3D11VertexShader* SimpleVertexShader = nullptr;       // Vertex 데이터를 처리하는 Vertex 셰이더
    ID3D11PixelShader* SimplePixelShader = nullptr;         // Pixel의 색상을 결정하는 Pixel 셰이더

    ID3D11InputLayout* SimpleInputLayout = nullptr;         // Vertex 셰이더 입력 레이아웃 정의
    unsigned int Stride = 0;                                // Vertex 버퍼의 각 요소 크기

    // Depth Stenil Buffer
	ID3D11Texture2D* DepthStencilBuffer = nullptr;          // DepthStencil버퍼 역할을 하는 텍스쳐
	ID3D11DepthStencilView* DepthStencilView = nullptr;     // DepthStencil버퍼를 렌더 타겟으로 사용하는 뷰
	ID3D11DepthStencilState* DepthStencilState = nullptr;   // DepthStencil 상태(깊이 테스트, 스텐실 테스트 등 정의)
    ID3D11DepthStencilState* GizmoDepthStencilState = nullptr; // 기즈모용 스텐실 스테이트. Z버퍼 테스트 하지않고 항상 앞에렌더


public:
    ID3D11Buffer* LineVertexBuffer = nullptr;
	uint32 MaxLineSize = 0;
    std::unordered_map<EPrimitiveType, ComPtr<ID3D11Buffer>> VertexBufferMap;
    std::unordered_map<EPrimitiveType, ComPtr<ID3D11Buffer>> IndexBufferMap;
    std::unordered_map<InputLayoutType, ComPtr<ID3D11InputLayout>> InputLayoutMap;

    std::unordered_map<EPrimitiveType, uint32> VertexCountMap;
    std::unordered_map<EPrimitiveType, D3D11_PRIMITIVE_TOPOLOGY> TopologyMap;

    std::unordered_map<EPrimitiveType, FHitColliderInfo> HitColliderInfoMap;

protected:
    std::unordered_map<uint32, ComPtr<ID3D11Buffer>> ConstantBufferMap;
    std::unordered_map<uint32, ComPtr<ID3D11SamplerState>> SamplerMap;
    std::unordered_map<uint32, ComPtr<ID3D11ShaderResourceView>> ShaderResourceViewMap;
    std::unordered_map<uint32, ComPtr<ID3D11VertexShader>> ShaderMapVS;
    std::unordered_map<uint32, ComPtr<ID3D11PixelShader>> ShaderMapPS;
    std::unordered_map<uint32, ComPtr<ID3D11GeometryShader>> ShaderMapGS;

    // Tessellation 테스트
    ID3D11VertexShader* TessVertexShader = nullptr;
    ID3D11PixelShader* TessPixelShader = nullptr;
    ID3D11HullShader* HullShader = nullptr;
    ID3D11DomainShader* DomainShader = nullptr;


	
	// Buffer Cache

	std::unique_ptr<FBufferCache> BufferCache;

	FMatrix WorldMatrix;
    FMatrix ViewMatrix;
	FMatrix ProjectionMatrix;

	D3D_PRIMITIVE_TOPOLOGY CurrentTopology = D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED;

#pragma region Util_ConstantBuffer
public:
    template <typename T>
    ID3D11Buffer* CreateVertexBuffer(const T* Vertices, UINT ByteWidth) const
    {
        D3D11_BUFFER_DESC VertexBufferDesc = {};
        VertexBufferDesc.ByteWidth = ByteWidth;
        VertexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
        VertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

        D3D11_SUBRESOURCE_DATA VertexBufferSRD = {};
        VertexBufferSRD.pSysMem = Vertices;

        ID3D11Buffer* VertexBuffer;
        const HRESULT Result = Device->CreateBuffer(&VertexBufferDesc, &VertexBufferSRD, &VertexBuffer);
        if (FAILED(Result))
        {
            return nullptr;
        }
        return VertexBuffer;
    }

	// 월드 그리드 렌더링 버텍스 버퍼 생성
    template <typename T>
    ID3D11Buffer* CreateLineVertexBuffer(const T* Vertices, UINT ByteWidth)
    {
        MaxLineSize = max(ByteWidth, MaxLineSize);
        D3D11_BUFFER_DESC VertexBufferDesc = {};
        VertexBufferDesc.ByteWidth = MaxLineSize;
        VertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
        VertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        VertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

        D3D11_SUBRESOURCE_DATA VertexBufferSRD = {};
        VertexBufferSRD.pSysMem = Vertices;

        HRESULT hr = Device->CreateBuffer(&VertexBufferDesc, &VertexBufferSRD, &LineVertexBuffer);
        if (FAILED(hr)) {
            return nullptr;
        }
        return LineVertexBuffer;
    }

    // 월드 그리드 렌더링 버텍스 버퍼 업데이트
	void UpdateLineVertexBuffer(const FVertexSimple* Vertices, uint32 NewSize)
	{
        if (MaxLineSize < NewSize) 
        {
			MaxLineSize = NewSize;
            if (LineVertexBuffer) { LineVertexBuffer->Release();  LineVertexBuffer = nullptr; }
			CreateLineVertexBuffer(Vertices, MaxLineSize);
        } 
        D3D11_MAPPED_SUBRESOURCE ms;
        if (SUCCEEDED(DeviceContext->Map(LineVertexBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms))) {
			memcpy(ms.pData, Vertices, MaxLineSize > NewSize ? NewSize : MaxLineSize); // 현재 사이즈보다 작은 경우에는 현재 사이즈만큼만 복사
            DeviceContext->Unmap(LineVertexBuffer, 0);
        }
	}
    
    ID3D11Buffer* CreateIndexBuffer(const std::vector<uint32>& indices)
    {
        ID3D11Buffer* IndexBuffer;
        D3D11_BUFFER_DESC bufferDesc = {};
        bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;                       // 초기화 후 변경X
        bufferDesc.ByteWidth = UINT(sizeof(uint32) * indices.size());
        bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
        bufferDesc.CPUAccessFlags = 0;                                  // 0 if no CPU access is necessary.
        bufferDesc.StructureByteStride = sizeof(uint32);

        D3D11_SUBRESOURCE_DATA indexBufferData = { 0 };
        indexBufferData.pSysMem = indices.data();
        indexBufferData.SysMemPitch = 0;
        indexBufferData.SysMemSlicePitch = 0;

        Device->CreateBuffer(&bufferDesc, &indexBufferData, &IndexBuffer);
        return IndexBuffer;
    }

    ID3D11Buffer* CreateIndexBuffer(const TArray<uint32>& indices)
    {
        ID3D11Buffer* IndexBuffer;
        D3D11_BUFFER_DESC bufferDesc = {};
        bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;                       // 초기화 후 변경X
        bufferDesc.ByteWidth = UINT(sizeof(uint32) * indices.Num());
        bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
        bufferDesc.CPUAccessFlags = 0;                                  // 0 if no CPU access is necessary.
        bufferDesc.StructureByteStride = sizeof(uint32);

        D3D11_SUBRESOURCE_DATA indexBufferData = { 0 };
        indexBufferData.pSysMem = indices.GetData();
        indexBufferData.SysMemPitch = 0;
        indexBufferData.SysMemSlicePitch = 0;

        Device->CreateBuffer(&bufferDesc, &indexBufferData, &IndexBuffer);
        return IndexBuffer;
    }

    template <typename T>
    uint32 CreateConstantBuffer()           // 각 PrimitiveComponent에서 호출됨 : 각자가 쓸 상수 버퍼의 인덱스를 소유토록 return
    {
        ComPtr<ID3D11Buffer> ConstantBuffer;
        D3D11_BUFFER_DESC cbDesc;
        cbDesc.ByteWidth = (sizeof(T) + 0xf) & 0xfffffff0;
        cbDesc.Usage = D3D11_USAGE_DYNAMIC;
        cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        cbDesc.MiscFlags = 0;
        cbDesc.StructureByteStride = 0;
        ;
        if (FAILED(Device->CreateBuffer(&cbDesc, nullptr, ConstantBuffer.GetAddressOf()))) {
            std::cout << "CreateConstantBuffer() CreateBuffer failed()." << std::endl;
        }

        uint32 idx = ConstantBufferMap.size();
        ConstantBufferMap.insert({idx, ConstantBuffer});

        return idx;
    }
    /**
     * (상수) 버퍼를 새로운 bufferData 값으로 갱신합니다
     * @param bufferData 갱신할 값
     * @param pBuffer 갱신 대상 (상수) 버퍼 포인터
     */
    template <typename T_DATA>
    void UpdateBuffer(const T_DATA& bufferData, uint32 idx) {
        
        if (ConstantBufferMap.find(idx) == ConstantBufferMap.end()) {               // 해당 idx에 상수버퍼가 존재하지 않을 때
            std::cout << "UpdateBuffer() buffer was not initialized." << std::endl;
        }

        ComPtr<ID3D11Buffer> pBuffer = ConstantBufferMap[idx];
        D3D11_MAPPED_SUBRESOURCE ms;
        
        DeviceContext->Map(pBuffer.Get(), NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);
        memcpy(ms.pData, &bufferData, sizeof(bufferData));
        DeviceContext->Unmap(pBuffer.Get(), NULL);
    }

    void CreateTextureSRV(const std::string& filename);
    void CreateTextureSRV(const WIDECHAR* filename);
    void CreateTextureSRVW(const WIDECHAR* filename);
#pragma endregion

#pragma region picking
protected:
	// 피킹용 버퍼들
	ID3D11Texture2D* PickingFrameBuffer = nullptr;                 // 화면 출력용 텍스처
	ID3D11RenderTargetView* PickingFrameBufferRTV = nullptr;       // 텍스처를 렌더 타겟으로 사용하는 뷰
	ID3D11Buffer* ConstantPickingBuffer = nullptr;                 // 뷰 상수 버퍼
	FLOAT PickingClearColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f }; //
	//ID3D11PixelShader* PickingPixelShader = nullptr;         // Pixel의 색상을 결정하는 Pixel 셰이더
	ID3D11Buffer* ConstantsDepthBuffer = nullptr;

	ID3D11DepthStencilState* IgnoreDepthStencilState = nullptr;   // DepthStencil 상태(깊이 테스트, 스텐실 테스트 등 정의)

public:
	//피킹용 함수들	
    void ReleasePickingFrameBuffer();
    void CreatePickingTexture(HWND hWnd);
    void PrepareZIgnore();
	void UpdateConstantPicking(FVector4 UUIDColor) const;
    void UpdateConstantDepth(int Depth) const;
    void PrepareMain();
	void PrepareMainShader();

    FVector GetRayDirectionFromClick(FVector MPos);
	FVector4 GetPixel(FVector MPos);

	void RenderPickingTexture();
    FMatrix GetProjectionMatrix() const { return ProjectionMatrix; }
    FMatrix GetViewMatrix() const { return ViewMatrix; }
#pragma endregion picking
};
