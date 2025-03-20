// stb_image.h 헤더 오류 픽싱용 매크로
#define _CRT_SECURE_NO_WARNINGS // stb_image_write compile error fix

#include "URenderer.h"
#include <d3dcompiler.h>
#include "Core/Rendering/BufferCache.h"
#include "Core/Math/Transform.h"
#include <Object/Actor/Camera.h>
#include "Object/PrimitiveComponent/UPrimitiveComponent.h"
#include "Static/FEditorManager.h"
#include "Core/Rendering/BufferCache.h" // 그리드 동적 렌더링

// 아래는 Texture에 쓸 이미지 로딩용
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image.h";
#include "stb_image_write.h";

#include <directxtk/DDSTextureLoader.h> // Create DDS Texture Loader
#include <directxtk/WICTextureLoader.h>

#include "../Source/Object/World/World.h" // World로부터 GridScale을 가져옴

#include "Core/Rendering/TextAtlasManager.h";
#include "Core/Rendering/SubUVManager.h";

#define SAFE_RELEASE(p)       { if (p) { (p)->Release();  (p) = nullptr; } }

void URenderer::Create(HWND hWindow)
{
    CreateDeviceAndSwapChain(hWindow);
    CreateViewportInfos(); // 뷰포트 정보 설정
    CreateFrameBuffer();
    CreateRasterizerState();
    CreateBufferCache();
    CreateTexturesSamplers();
    CreateDepthStencilBuffer();
    CreateDepthStencilState();

    CreatePickingTexture(hWindow);

    RTVs[0] = FrameBufferRTV;
    RTVs[1] = PickingFrameBufferRTV;
    
    InitMatrix();
}

void URenderer::Release()
{
    ReleaseRasterizerState();

    // 렌더 타겟을 초기화
    DeviceContext->OMSetRenderTargets(0, nullptr, DepthStencilView);

    ReleaseFrameBuffer();
    ReleasePickingFrameBuffer();
    ReleaseDepthStencilBuffer();
    ReleaseDeviceAndSwapChain();
}

// 필요한 모든 셰이더, Input Layout을 생성합니다
void URenderer::CreateShader()
{
    /**
         * 컴파일된 셰이더의 바이트코드를 저장할 변수 (ID3DBlob)
         *
         * 범용 메모리 버퍼를 나타내는 형식
         *   - 여기서는 shader object bytecode를 담기위해 쓰임
         * 다음 두 메서드를 제공한다.
         *   - LPVOID GetBufferPointer
         *     - 버퍼를 가리키는 void* 포인터를 돌려준다.
         *   - SIZE_T GetBufferSize
         *     - 버퍼의 크기(바이트 갯수)를 돌려준다
         */
    ID3D11VertexShader* LightPosTexVertexShader;
    ID3D11PixelShader* LightPosTexPixelShader;
    ID3D11VertexShader* PosTexVertexShader;
    ID3D11PixelShader* PosTexPixelShader;
    ID3D11InputLayout* PosTexInputLayout;
    ID3D11VertexShader* AtlasVertexShader;
    ID3D11PixelShader* AtlasPixelShader;
    ID3D11PixelShader* AtlasNoClipPixelShader;
    ID3DBlob* VertexShaderCSO;
    ID3DBlob* PosTexVertexShaderCSO;
    ID3DBlob* PixelShaderCSO;
    ID3DBlob* PosTexPixelShaderCSO;
    ID3DBlob* AtlasVertexShaderCSO;
    ID3DBlob* AtlasPixelShaderCSO;

	ID3DBlob* TessVertexShaderCSO;
	ID3DBlob* TessHullShaderCSO;
	ID3DBlob* TessDomainShaderCSO;
	ID3DBlob* TessPixelShaderCSO;

    //ID3DBlob* PickingShaderCSO;
    
	ID3DBlob* ErrorMsg = nullptr;
    // 셰이더 컴파일 및 생성
    D3DCompileFromFile(L"Shaders/ShaderW0.hlsl", nullptr, nullptr, "mainVS", "vs_5_0", 0, 0, &VertexShaderCSO, &ErrorMsg);
    Device->CreateVertexShader(VertexShaderCSO->GetBufferPointer(), VertexShaderCSO->GetBufferSize(), nullptr, &SimpleVertexShader);

    D3DCompileFromFile(L"Shaders/ShaderW0.hlsl", nullptr, nullptr, "mainPS", "ps_5_0", 0, 0, &PixelShaderCSO, &ErrorMsg);
    Device->CreatePixelShader(PixelShaderCSO->GetBufferPointer(), PixelShaderCSO->GetBufferSize(), nullptr, &SimplePixelShader);
    
    if (ErrorMsg)
    {
        std::cout << (char*)ErrorMsg->GetBufferPointer() << std::endl;
        SAFE_RELEASE(ErrorMsg);
    }
    auto it = InputLayouts.find(InputLayoutType::POSCOLOR);
    Device->CreateInputLayout(it->second.data(), it->second.size(), VertexShaderCSO->GetBufferPointer(), VertexShaderCSO->GetBufferSize(), &SimpleInputLayout);


    /* Position Color Normal Texcoord Inputlayout, shader */
    D3DCompileFromFile(L"Shaders/PosTexVertexShader.hlsl", nullptr, nullptr, "mainVS", "vs_5_0", 0, 0, &PosTexVertexShaderCSO, &ErrorMsg);
    //UE_LOG("%s ", (char*)ErrorMsg->GetBufferPointer());
    if (FAILED(Device->CreateVertexShader(PosTexVertexShaderCSO->GetBufferPointer(), PosTexVertexShaderCSO->GetBufferSize(), nullptr, &PosTexVertexShader))) {
        std::cout << (char*)ErrorMsg->GetBufferPointer() << std::endl;
        SAFE_RELEASE(ErrorMsg);
    }

    D3DCompileFromFile(L"Shaders/PosTexPixelShader.hlsl", nullptr, nullptr, "mainPS", "ps_5_0", 0, 0, &PosTexPixelShaderCSO, &ErrorMsg);
    if (FAILED(Device->CreatePixelShader(PosTexPixelShaderCSO->GetBufferPointer(), PosTexPixelShaderCSO->GetBufferSize(), nullptr, &PosTexPixelShader))) {
        std::cout << (char*)ErrorMsg->GetBufferPointer() << std::endl;
        SAFE_RELEASE(ErrorMsg);
    }
   
    it = InputLayouts.find(InputLayoutType::POSCOLORNORMALTEX);
    Device->CreateInputLayout(it->second.data(), it->second.size(), PosTexVertexShaderCSO->GetBufferPointer(), PosTexVertexShaderCSO->GetBufferSize(), &PosTexInputLayout);
    
    
    /* Atlas Texture Shader : VS computes coordinates */
    D3DCompileFromFile(L"Shaders/AtlasVertexShader.hlsl", nullptr, nullptr, "mainVS", "vs_5_0", 0, 0, &AtlasVertexShaderCSO, &ErrorMsg);
    //UE_LOG("%s ", (char*)ErrorMsg->GetBufferPointer());
    if (FAILED(Device->CreateVertexShader(AtlasVertexShaderCSO->GetBufferPointer(), AtlasVertexShaderCSO->GetBufferSize(), nullptr, &AtlasVertexShader))) {
        std::cout << (char*)ErrorMsg->GetBufferPointer() << std::endl;
        SAFE_RELEASE(ErrorMsg);
    }

    D3DCompileFromFile(L"Shaders/AtlasPixelShader.hlsl", nullptr, nullptr, "mainPS", "ps_5_0", 0, 0, &AtlasPixelShaderCSO, &ErrorMsg);
    if (FAILED(Device->CreatePixelShader(AtlasPixelShaderCSO->GetBufferPointer(), AtlasPixelShaderCSO->GetBufferSize(), nullptr, &AtlasPixelShader))) {
        std::cout << (char*)ErrorMsg->GetBufferPointer() << std::endl;
        SAFE_RELEASE(ErrorMsg);
    }

    SAFE_RELEASE(AtlasPixelShaderCSO);
    D3DCompileFromFile(L"Shaders/AtlasPixelShader.hlsl", nullptr, nullptr, "mainPSNoClip", "ps_5_0", 0, 0, &AtlasPixelShaderCSO, &ErrorMsg);
    if (FAILED(Device->CreatePixelShader(AtlasPixelShaderCSO->GetBufferPointer(), AtlasPixelShaderCSO->GetBufferSize(), nullptr, &AtlasNoClipPixelShader))) {
        std::cout << (char*)ErrorMsg->GetBufferPointer() << std::endl;
        SAFE_RELEASE(ErrorMsg);
    }

    SAFE_RELEASE(VertexShaderCSO);  SAFE_RELEASE(PixelShaderCSO);
    D3DCompileFromFile(L"Shaders/GridVertexShader.hlsl", nullptr, nullptr, "mainVS", "vs_5_0", 0, 0, &TessVertexShaderCSO, &ErrorMsg);
    Device->CreateVertexShader(TessVertexShaderCSO->GetBufferPointer(), TessVertexShaderCSO->GetBufferSize(), nullptr, &TessVertexShader);

    D3DCompileFromFile(L"Shaders/GridPixelShader.hlsl", nullptr, nullptr, "mainPS", "ps_5_0", 0, 0, &TessPixelShaderCSO, &ErrorMsg);
    Device->CreatePixelShader(TessPixelShaderCSO->GetBufferPointer(), TessPixelShaderCSO->GetBufferSize(), nullptr, &TessPixelShader);

    SAFE_RELEASE(VertexShaderCSO);  SAFE_RELEASE(PixelShaderCSO);
    D3DCompileFromFile(L"Shaders/LightPosTexVertexShader.hlsl", nullptr, nullptr, "mainVS", "vs_5_0", 0, 0, &VertexShaderCSO, &ErrorMsg);
    Device->CreateVertexShader(VertexShaderCSO->GetBufferPointer(), VertexShaderCSO->GetBufferSize(), nullptr, &LightPosTexVertexShader);

    D3DCompileFromFile(L"Shaders/LightPosTexPixelShader.hlsl", nullptr, nullptr, "mainPS", "ps_5_0", 0, 0, &PixelShaderCSO, &ErrorMsg);
    Device->CreatePixelShader(PixelShaderCSO->GetBufferPointer(), PixelShaderCSO->GetBufferSize(), nullptr, &LightPosTexPixelShader);

    ShaderMapVS.insert({ 0, SimpleVertexShader});                               // 여기서 Vertex Shader, Pixel Shader, InputLayout 추가
    ShaderMapVS.insert({ 1, PosTexVertexShader});
    ShaderMapVS.insert({ 2, AtlasVertexShader });
    ShaderMapVS.insert({ 3, TessVertexShader });
    ShaderMapVS.insert({ 4, LightPosTexVertexShader });

    ShaderMapPS.insert({ 0, SimplePixelShader });
    ShaderMapPS.insert({ 1, PosTexPixelShader });
    ShaderMapPS.insert({ 2, AtlasPixelShader });
    ShaderMapPS.insert({ 3, TessPixelShader });
    ShaderMapPS.insert({ 4, LightPosTexPixelShader });
    ShaderMapPS.insert({ 5, AtlasNoClipPixelShader });

    InputLayoutMap.insert({ InputLayoutType::POSCOLOR, SimpleInputLayout });
    InputLayoutMap.insert({ InputLayoutType::POSCOLORNORMALTEX, PosTexInputLayout });

    

    // 정점 하나의 크기를 설정 (바이트 단위)
    Stride = sizeof(FVertexSimple);

    


}

void URenderer::ReleaseShader()
{
    SAFE_RELEASE(SimpleInputLayout);
    SAFE_RELEASE(SimplePixelShader);
    SAFE_RELEASE(SimpleVertexShader);
}

// 필요한 모든 상수버퍼를 생성합니다
void URenderer::CreateConstantBuffer()
{
    uint32 idx;
    idx = CreateConstantBuffer<FConstants>();           // Fconstants : 0
    idx = CreateConstantBuffer<FPickingConstants>();    // Picking CBuffer : 1
    idx = CreateConstantBuffer<FDepthConstants>();      // DepthConstants : 2
    idx = CreateConstantBuffer<FAtlasConstants>();           // Atlas CBuffer : 3
    idx = CreateConstantBuffer<FLightConstants>();           // Lighting 테스트용 CBuffer : 4
    UE_LOG("constantbuffer size : %d", idx);
}

void URenderer::ReleaseConstantBuffer()
{
    SAFE_RELEASE(ConstantBuffer);
    SAFE_RELEASE(ConstantPickingBuffer);
    SAFE_RELEASE(ConstantsDepthBuffer);
}

void URenderer::CreateTexturesSamplers()
{
    ComPtr<ID3D11SamplerState> SamplerState;
    D3D11_SAMPLER_DESC sampDesc;
    ZeroMemory(&sampDesc, sizeof(sampDesc));
    sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    sampDesc.MinLOD = 0;
    sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

    Device->CreateSamplerState(&sampDesc, SamplerState.GetAddressOf());

    SamplerMap.insert({ 0, SamplerState });

    CreateTextureSRVW(L"Textures/box.jpg");
    CreateTextureSRVW(L"Textures/koverwatch.png");
    UAtlasInfo KoverwatchText = JsonSaveHelper::LoadAtlasInfo("koverwatch.png");
    UTextAtlasManager::AddAtlasInfo(KoverwatchText);

    CreateTextureSRVW(L"Textures/Faker.png");
    UAtlasInfo FakerSubUV = JsonSaveHelper::LoadAtlasInfo("Faker.png");
    USubUVManager::AddAtlasInfo(FakerSubUV);

    CreateTextureSRVW(L"Textures/koverwatchBlack.png");
    UAtlasInfo KoverwatchBlackText = JsonSaveHelper::LoadAtlasInfo("koverwatchBlack.png");
    UTextAtlasManager::AddAtlasInfo(KoverwatchBlackText);

    CreateTextureSRVW(L"Textures/FakerLightOff.png");
    UAtlasInfo FakerLightOffSubUV = JsonSaveHelper::LoadAtlasInfo("FakerLightOff.png");
    USubUVManager::AddAtlasInfo(FakerLightOffSubUV);

    CreateTextureSRVW(L"Textures/RollingChanhui.png");
    UAtlasInfo RollingSubUV = JsonSaveHelper::LoadAtlasInfo("RollingChanhui.png");
    USubUVManager::AddAtlasInfo(RollingSubUV);

    CreateTextureSRVW(L"Textures/earth.jpg");

    CreateTextureSRVW(L"Textures/tree.png");

    CreateTextureSRVW(L"Textures/waffle.jpg");

    //CreateTextureSRV(L"Textures/box.dds");
    //CreateTextureSRV(L"../../../Textures/bg5.dds");
    /*CreateTextureSRVW(L"Textures/box.jpg");*/
    //CreateTextureSRV("bg5.png");
    //CreateTextureSRV("earth.jpg");
    //CreateTextureSRV("cat0.png");
}

void URenderer::ReleaseTexturesSamplers()
{
}

void URenderer::SwapBuffer() const
{
    SwapChain->Present(1, 0); // SyncInterval: VSync 활성화 여부
}

void URenderer::Prepare() const
{
    // 화면 지우기
    DeviceContext->ClearRenderTargetView(FrameBufferRTV, ClearColor);
    DeviceContext->ClearRenderTargetView(PickingFrameBufferRTV, ClearColor);
    DeviceContext->ClearDepthStencilView(DepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    
    // InputAssembler의 Vertex 해석 방식을 설정 / deprecated : 각 오브젝트마다 topology 가지는 것으로 설정 - RenderPrimitive() 참고
    //DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // Rasterization할 Viewport를 설정 
    DeviceContext->RSSetViewports(1, &ViewportInfo);
    DeviceContext->RSSetState(RasterizerStates[UEngine::Get().GetWorld()->GetViewMode()]);

    /**
     * OutputMerger 설정
     * 렌더링 파이프라인의 최종 단계로써, 어디에 그릴지(렌더 타겟)와 어떻게 그릴지(블렌딩)를 지정
     */
	DeviceContext->OMSetRenderTargets(2, RTVs, DepthStencilView);    // DepthStencil 뷰 설정
    DeviceContext->OMSetBlendState(nullptr, nullptr, 0xffffffff);
    DeviceContext->OMSetDepthStencilState(DepthStencilState, 0);                // DepthStencil 상태 설정. StencilRef: 스텐실 테스트 결과의 레퍼런스
}

void URenderer::PrepareShader() const
{
    // 기본 셰이더랑 InputLayout을 설정
    DeviceContext->VSSetShader(SimpleVertexShader, nullptr, 0);
    DeviceContext->PSSetShader(SimplePixelShader, nullptr, 0);
    DeviceContext->IASetInputLayout(SimpleInputLayout);

    // 버텍스 쉐이더에 상수 버퍼를 설정
    if (ConstantBuffer)
    {
        DeviceContext->VSSetConstantBuffers(0, 1, &ConstantBuffer);
    }
    if (ConstantPickingBuffer)
    {
        DeviceContext->PSSetConstantBuffers(1, 1, &ConstantPickingBuffer);
    }
    if (ConstantsDepthBuffer)
    {
        DeviceContext->PSSetConstantBuffers(2, 1, &ConstantsDepthBuffer);
    }
}


void URenderer::RenderPrimitive(UPrimitiveComponent* PrimitiveComp, FRenderResource& RenderResource)
{
    BufferInfo Info = BufferCache->GetBufferInfo(RenderResource.PrimitiveType);
    auto& [Type, ILType, Topology, numVertices, stride, VS, PS, VC, PC, GS, bUseIndexBuffer, SRVs] = RenderResource;


    if (Type == EPrimitiveType::EPT_WORLDGRID) {
        float GridScale = UEngine::Get().GetWorld()->GetGridScale();
        auto [Vertices, Indices] = BufferCache->CreateWorldGridVertices(GridScale, 1000.0f * GridScale, 
            FEditorManager::Get().GetCamera()->GetActorTransform().GetPosition());
        auto Size = Vertices.Num();
        this->UpdateLineVertexBuffer(Vertices.GetData(), Size * sizeof(FVertexSimple));
        Topology = D3D_PRIMITIVE_TOPOLOGY_LINELIST;
        VertexBufferMap[Type] = LineVertexBuffer;
        VertexCountMap[Type] = Size;
        TopologyMap[Type] = Topology;
    }
    
    RenderResource.Topology = TopologyMap[Type];
    RenderResource.numVertices = VertexCountMap[Type];                      // indexbuffer를 사용하는 primitive는 numIndices가 numVertices에 저장된다 (union 사용못해서 이렇게 함)


    assert(ShaderMapVS[VS].Get() != nullptr); assert(ShaderMapPS[PS].Get() != nullptr);
    assert(InputLayoutMap[ILType] != nullptr); assert(TopologyMap.find(Type) != TopologyMap.end());
    assert(VertexCountMap.find(Type) != VertexCountMap.end());
    assert(VertexBufferMap.find(Type) != VertexBufferMap.end());
    if (bUseIndexBuffer == true ) assert(IndexBufferMap.find(Type) != IndexBufferMap.end());

    if (CurrentTopology != Topology)
    {
        DeviceContext->IASetPrimitiveTopology(Topology);
        CurrentTopology = Topology;
    }
    DeviceContext->VSSetShader(ShaderMapVS[VS].Get(), nullptr, 0);
    DeviceContext->PSSetShader(ShaderMapPS[PS].Get(), nullptr, 0);

    /* Vertex Shader의 상수 버퍼 */   
    if (ConstantBufferMap.find(VC) != ConstantBufferMap.end())
    {
        DeviceContext->VSSetConstantBuffers(0, 1, ConstantBufferMap[VC].GetAddressOf());
    }
    /* Pixel Shader의 상수 버퍼 */
    if (ConstantBufferMap.find(PC) != ConstantBufferMap.end())
    {
        DeviceContext->PSSetConstantBuffers(0, 1, ConstantBufferMap[PC].GetAddressOf());
    }
    /* Pixel Shader의 ShaderResourceView */
    if (SRVs.has_value())
    {
        std::vector<ID3D11ShaderResourceView*> SRVArray;
        for (auto SRVIndex : *SRVs)
        {
            if (ShaderResourceViewMap.find(SRVIndex) != ShaderResourceViewMap.end())
            {
                SRVArray.push_back(ShaderResourceViewMap[SRVIndex].Get());
            }
        }
        DeviceContext->PSSetShaderResources(0, static_cast<UINT>(SRVArray.size()), SRVArray.data());
        DeviceContext->PSSetSamplers(0, 1, SamplerMap[0].GetAddressOf());                                           // TODO : 샘플러 기본 1개로 설정되있는 것 각자 여러개 접근토록 바꿔야 함
    }
	

    this->Stride = stride;
    DeviceContext->IASetInputLayout(InputLayoutMap[ILType].Get());
    DeviceContext->IASetPrimitiveTopology(Topology);                                    // 실제 토폴로지 세팅

    for (uint32 i{ 0 }; i < NumViewports; ++i)
    {
        DeviceContext->RSSetViewports(1, &ViewportInfos[i]);
        if (bUseIndexBuffer == true) {
            RenderPrimitiveIndexed(VertexBufferMap[Type].Get(), IndexBufferMap[Type].Get(), numVertices);
        }
        else {
            RenderPrimitiveInternal(VertexBufferMap[Type].Get(), numVertices);                  // info에 담긴 실제 vertexbuffer, numVertices 전달 및 렌더
        }
    }

}

void URenderer::RenderPrimitiveInternal(ID3D11Buffer* pBuffer, UINT numVertices) const
{
    UINT Offset = 0;
    DeviceContext->IASetVertexBuffers(0, 1, &pBuffer, &Stride, &Offset);

    DeviceContext->Draw(numVertices, 0);
}

void URenderer::RenderPrimitiveIndexed(ID3D11Buffer* pVertexBuffer, ID3D11Buffer* pIndexBuffer, UINT numIndices) const
{
    UINT Offset = 0;
    DeviceContext->IASetVertexBuffers(0, 1, &pVertexBuffer, &Stride, &Offset);
    DeviceContext->IASetIndexBuffer(pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
    DeviceContext->DrawIndexed(numIndices, 0, 0);
}

void URenderer::ReleaseVertexBuffer(ID3D11Buffer* pBuffer) const
{
    SAFE_RELEASE(pBuffer);
}


ID3D11Device* URenderer::GetDevice() const { return Device; }

ID3D11DeviceContext* URenderer::GetDeviceContext() const { return DeviceContext; }

void URenderer::CreateDeviceAndSwapChain(HWND hWindow)
{
    // 지원하는 Direct3D 기능 레벨을 정의
    D3D_FEATURE_LEVEL FeatureLevels[] = { D3D_FEATURE_LEVEL_11_0 };
    
    // SwapChain 구조체 초기화
    DXGI_SWAP_CHAIN_DESC SwapChainDesc = {};
    SwapChainDesc.BufferDesc.Width = 0;                            // 창 크기에 맞게 자동으로 설정
    SwapChainDesc.BufferDesc.Height = 0;                           // 창 크기에 맞게 자동으로 설정
    SwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;  // 색상 포멧
    SwapChainDesc.SampleDesc.Count = 1;                            // 멀티 샘플링 비활성화
    SwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;   // 렌더 타겟으로 설정
    SwapChainDesc.BufferCount = 2;                                 // 더블 버퍼링
    SwapChainDesc.OutputWindow = hWindow;                          // 렌더링할 창 핸들
    SwapChainDesc.Windowed = TRUE;                                 // 창 모드
    SwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;      // 스왑 방식
    
    // Direct3D Device와 SwapChain을 생성
    D3D11CreateDeviceAndSwapChain(
        // 입력 매개변수
        nullptr,                                                       // 디바이스를 만들 때 사용할 비디오 어댑터에 대한 포인터
        D3D_DRIVER_TYPE_HARDWARE,                                      // 만들 드라이버 유형을 나타내는 D3D_DRIVER_TYPE 열거형 값
        nullptr,                                                       // 소프트웨어 래스터라이저를 구현하는 DLL에 대한 핸들
        D3D11_CREATE_DEVICE_BGRA_SUPPORT | D3D11_CREATE_DEVICE_DEBUG,  // 사용할 런타임 계층을 지정하는 D3D11_CREATE_DEVICE_FLAG 열거형 값들의 조합
        FeatureLevels,                                                 // 만들려는 기능 수준의 순서를 결정하는 D3D_FEATURE_LEVEL 배열에 대한 포인터
        ARRAYSIZE(FeatureLevels),                                      // pFeatureLevels 배열의 요소 수
        D3D11_SDK_VERSION,                                             // SDK 버전. 주로 D3D11_SDK_VERSION을 사용
        &SwapChainDesc,                                                // SwapChain 설정과 관련된 DXGI_SWAP_CHAIN_DESC 구조체에 대한 포인터
    
        // 출력 매개변수
        &SwapChain,                                                    // 생성된 IDXGISwapChain 인터페이스에 대한 포인터
        &Device,                                                       // 생성된 ID3D11Device 인터페이스에 대한 포인터
        nullptr,                                                       // 선택된 기능 수준을 나타내는 D3D_FEATURE_LEVEL 값을 반환
        &DeviceContext                                                 // 생성된 ID3D11DeviceContext 인터페이스에 대한 포인터
    );
    
    // 생성된 SwapChain의 정보 가져오기
    SwapChain->GetDesc(&SwapChainDesc);
    
    // 뷰포트 정보 설정
    ViewportInfo = {
        0.0f, 0.0f,
        static_cast<float>(SwapChainDesc.BufferDesc.Width), static_cast<float>(SwapChainDesc.BufferDesc.Height),
        0.0f, 1.0f
    };
}

void URenderer::CreateViewportInfos()
{
    uint32 Width = UEngine::Get().GetScreenWidth();
    uint32 Height = UEngine::Get().GetScreenHeight();

    // 각 뷰포트의 너비와 높이는 화면 크기의 절반
    float HalfWidth = static_cast<float>(Width) / 2.0f;
    float HalfHeight = static_cast<float>(Height) / 2.0f;

    // 테스트용 : 뷰포트 두개를 수평으로 나누어 생성
    for (int i = 0; i < NumViewports; ++i)
    {
        D3D11_VIEWPORT Viewport;
        Viewport = {
            .TopLeftX = (i % 2) * HalfWidth,  // 짝수 인덱스는 왼쪽, 홀수 인덱스는 오른쪽
            .TopLeftY = (i / 2) * HalfHeight, // 첫 두 뷰포트는 위쪽, 나머지는 아래쪽
            .Width = HalfWidth,
            .Height = HalfHeight,
            .MinDepth = 0.0f,
            .MaxDepth = 1.0f
        };
        ViewportInfos.Add(Viewport);
    }
}

void URenderer::ReleaseDeviceAndSwapChain()
{
    if (DeviceContext)
    {
        DeviceContext->Flush(); // 남이있는 GPU 명령 실행
    }
    SAFE_RELEASE(SwapChain);
    SAFE_RELEASE(Device);
    SAFE_RELEASE(DeviceContext);
}

void URenderer::CreateFrameBuffer()
{
    // 스왑 체인으로부터 백 버퍼 텍스처 가져오기
    SwapChain->GetBuffer(0, IID_PPV_ARGS(&FrameBuffer));

    // 렌더 타겟 뷰 생성
    D3D11_RENDER_TARGET_VIEW_DESC FrameBufferRTVDesc = {};
    FrameBufferRTVDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;      // 색상 포맷
    FrameBufferRTVDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D; // 2D 텍스처

    Device->CreateRenderTargetView(FrameBuffer, &FrameBufferRTVDesc, &FrameBufferRTV);
}

void URenderer::CreateDepthStencilBuffer()
{
    D3D11_TEXTURE2D_DESC DepthBufferDesc = {};
    DepthBufferDesc.Width = static_cast<UINT>(ViewportInfo.Width);
    DepthBufferDesc.Height = static_cast<UINT>(ViewportInfo.Height);
    DepthBufferDesc.MipLevels = 1;
    DepthBufferDesc.ArraySize = 1;
	DepthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;            // 32비트 중 24비트는 깊이, 8비트는 스텐실
    DepthBufferDesc.SampleDesc.Count = 1;
    DepthBufferDesc.SampleDesc.Quality = 0;
    DepthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	DepthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;              // 텍스쳐 바인딩 플래그를 DepthStencil로 설정
    DepthBufferDesc.CPUAccessFlags = 0;
    DepthBufferDesc.MiscFlags = 0;

    HRESULT result = Device->CreateTexture2D(&DepthBufferDesc, nullptr, &DepthStencilBuffer);

    D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
    dsvDesc.Format = DepthBufferDesc.Format;
    dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    dsvDesc.Texture2D.MipSlice = 0;
    
    result = Device->CreateDepthStencilView(DepthStencilBuffer, &dsvDesc, &DepthStencilView);
}

void URenderer::CreateDepthStencilState()
{
    D3D11_DEPTH_STENCIL_DESC DepthStencilDesc = {};
    DepthStencilDesc.DepthEnable = TRUE;
    DepthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    DepthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;                     // 더 작은 깊이값이 왔을 때 픽셀을 갱신함

    Device->CreateDepthStencilState(&DepthStencilDesc, &DepthStencilState);
    
    D3D11_DEPTH_STENCIL_DESC IgnoreDepthStencilDesc = {};
    DepthStencilDesc.DepthEnable = TRUE;
    DepthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    DepthStencilDesc.DepthFunc = D3D11_COMPARISON_ALWAYS;                     
    Device->CreateDepthStencilState(&IgnoreDepthStencilDesc ,&IgnoreDepthStencilState);
}

void URenderer::ReleaseFrameBuffer()
{
    SAFE_RELEASE(FrameBuffer);
    SAFE_RELEASE(FrameBufferRTV);
    //SAFE_RELEASE(PickingFrameBuffer);
    //SAFE_RELEASE(PickingFrameBufferRTV);
}

void URenderer::ReleaseDepthStencilBuffer()
{
    SAFE_RELEASE(DepthStencilBuffer);
    SAFE_RELEASE(DepthStencilView);
    SAFE_RELEASE(DepthStencilState);
    SAFE_RELEASE(IgnoreDepthStencilState);
}

void URenderer::CreateRasterizerState()
{
    using enum EViewModeIndex;
    D3D11_RASTERIZER_DESC RasterizerDesc = {};
    RasterizerDesc.FillMode = D3D11_FILL_SOLID; // 채우기 모드
    //RasterizerDesc.FillMode = D3D11_FILL_WIREFRAME;
    RasterizerDesc.CullMode = D3D11_CULL_BACK;  // 백 페이스 컬링
    //RasterizerDesc.CullMode = D3D11_CULL_FRONT;  // 프론트 페이스 컬링

    Device->CreateRasterizerState(&RasterizerDesc, &RasterizerStates[static_cast<uint32>(VMI_Lit)]);
    Device->CreateRasterizerState(&RasterizerDesc, &RasterizerStates[static_cast<uint32>(VMI_Unlit)]);
    
RasterizerDesc.FillMode = D3D11_FILL_WIREFRAME;
    RasterizerDesc.CullMode = D3D11_CULL_NONE;
    Device->CreateRasterizerState(&RasterizerDesc, &RasterizerStates[static_cast<uint32>(VMI_Wireframe)]);
}

void URenderer::ReleaseRasterizerState()
{
	for (auto& RS : RasterizerStates)
	{
		SAFE_RELEASE(RS);
	}
}

void URenderer::CreateBufferCache()
{
    BufferCache = std::make_unique<FBufferCache>();
}

void URenderer::InitMatrix()
{
	WorldMatrix = FMatrix::Identity();
	ViewMatrix = FMatrix::Identity();
	ProjectionMatrix = FMatrix::Identity();
}


void URenderer::ReleasePickingFrameBuffer()
{
    SAFE_RELEASE(PickingFrameBuffer);
    SAFE_RELEASE(PickingFrameBufferRTV);
}

void URenderer::CreatePickingTexture(HWND hWnd)
{
    RECT Rect;
    int Width , Height;

    Width = ViewportInfo.Width;
	Height = ViewportInfo.Height;

    D3D11_TEXTURE2D_DESC textureDesc = {};
    textureDesc.Width = Width;
    textureDesc.Height = Height;
    textureDesc.MipLevels = 1;
    textureDesc.ArraySize = 1;
    textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    textureDesc.SampleDesc.Count = 1;
    textureDesc.Usage = D3D11_USAGE_DEFAULT;
    textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

    Device->CreateTexture2D(&textureDesc, nullptr, &PickingFrameBuffer);

    D3D11_RENDER_TARGET_VIEW_DESC PickingFrameBufferRTVDesc = {};
    PickingFrameBufferRTVDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;      // 색상 포맷
    PickingFrameBufferRTVDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D; // 2D 텍스처
    
    Device->CreateRenderTargetView(PickingFrameBuffer, &PickingFrameBufferRTVDesc, &PickingFrameBufferRTV);
}

void URenderer::PrepareZIgnore()
{
    DeviceContext->OMSetDepthStencilState(IgnoreDepthStencilState, 0);
}

void URenderer::UpdateConstantPicking(FVector4 UUIDColor) const
{
    if (!ConstantPickingBuffer) return;

    D3D11_MAPPED_SUBRESOURCE ConstantBufferMSR;

    UUIDColor = FVector4(UUIDColor.X/255.0f, UUIDColor.Y/255.0f, UUIDColor.Z/255.0f, UUIDColor.W/255.0f);
    
    DeviceContext->Map(ConstantPickingBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &ConstantBufferMSR);
    {
        FPickingConstants* Constants = static_cast<FPickingConstants*>(ConstantBufferMSR.pData);
        Constants->UUIDColor = UUIDColor;
    }
    DeviceContext->Unmap(ConstantPickingBuffer, 0);
}

void URenderer::UpdateConstantDepth(int Depth) const
{
    if (!ConstantsDepthBuffer) return;

    ACamera* Cam = FEditorManager::Get().GetCamera();
    
    D3D11_MAPPED_SUBRESOURCE ConstantBufferMSR;
    
    DeviceContext->Map(ConstantsDepthBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &ConstantBufferMSR);
    {
        FDepthConstants* Constants = static_cast<FDepthConstants*>(ConstantBufferMSR.pData);
        Constants->DepthOffset = Depth;
        Constants->nearPlane = Cam->GetNear();
        Constants->farPlane = Cam->GetFar();
    }
    DeviceContext->Unmap(ConstantsDepthBuffer, 0);
}

void URenderer::PrepareMain()
{
	DeviceContext->OMSetDepthStencilState(DepthStencilState, 0);                // DepthStencil 상태 설정. StencilRef: 스텐실 테스트 결과의 레퍼런스
    DeviceContext->OMSetRenderTargets(2, RTVs, DepthStencilView);
    DeviceContext->OMSetBlendState(nullptr, nullptr, 0xFFFFFFFF);
}

void URenderer::PrepareMainShader()
{
    DeviceContext->PSSetShader(SimplePixelShader, nullptr, 0);
}

FVector URenderer::GetRayDirectionFromClick(FVector MPos)
{

    // 1. 화면 좌표를 NDC 좌표로 변환 (-1 ~ 1 범위)
    float ndcX = (2.0f * MPos.X / ViewportInfo.Width) - 1.0f;
    float ndcY = 1.0f - (2.0f * MPos.Y / ViewportInfo.Height); // 화면 좌표계는 아래로 증가하므로 반전

    // 2. NDC 좌표로 근거리 및 원거리 클립 공간의 점을 생성
    FVector4 nearPoint = FVector4(ndcX, ndcY, 0.0f, 1.0f);
    FVector4 farPoint = FVector4(ndcX, ndcY, 1.0f, 1.0f);

    // 3. 투영 및 뷰 변환 역행렬 계산
    FMatrix invProjection = ProjectionMatrix.Inverse();
    FMatrix invView = ViewMatrix.Inverse();

    // 4. NDC 좌표를 뷰 공간으로 변환
    FVector4 nearPointView = invProjection.TransformVector(nearPoint);
    FVector4 farPointView = invProjection.TransformVector(farPoint);

    // 원근 나눗셈(w로 나누기)
    nearPointView /= nearPointView.W;
    farPointView /= farPointView.W;

    // 5. 뷰 공간의 점을 월드 공간으로 변환
    FVector4 nearPointWorld = invView.TransformVector(nearPointView);
    FVector4 farPointWorld = invView.TransformVector(farPointView);

    // 6. 카메라 위치 추출 (뷰 행렬의 역행렬의 마지막 행은 월드 공간에서의 카메라 위치)
    FVector cameraPosition;
    invView.GetTranslateMatrix(cameraPosition);

    // 7. 카메라 위치에서 원거리 지점으로의 방향 벡터 계산 및 정규화
    FVector rayDirection = farPointWorld - cameraPosition;
    rayDirection.Normalize();

    return rayDirection;
}

FVector4 URenderer::GetPixel(FVector MPos)
{
    MPos.X = FMath::Clamp(MPos.X, 0.0f, ViewportInfo.Width);
    MPos.Y = FMath::Clamp(MPos.Y, 0.0f, ViewportInfo.Height);
    // 1. Staging 텍스처 생성 (1x1 픽셀)
    D3D11_TEXTURE2D_DESC stagingDesc = {};
    stagingDesc.Width = 1; // 픽셀 1개만 복사
    stagingDesc.Height = 1;
    stagingDesc.MipLevels = 1;
    stagingDesc.ArraySize = 1;
    stagingDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // 원본 텍스처 포맷과 동일
    stagingDesc.SampleDesc.Count = 1;
    stagingDesc.Usage = D3D11_USAGE_STAGING;
    stagingDesc.BindFlags = 0;
    stagingDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;

    ID3D11Texture2D* stagingTexture = nullptr;
    Device->CreateTexture2D(&stagingDesc, nullptr, &stagingTexture);

    // 2. 복사할 영역 정의 (D3D11_BOX)
    D3D11_BOX srcBox = {};
    srcBox.left = static_cast<UINT>(MPos.X);
    srcBox.right = srcBox.left + 1; // 1픽셀 너비
    srcBox.top = static_cast<UINT>(MPos.Y);
    srcBox.bottom = srcBox.top + 1; // 1픽셀 높이
    srcBox.front = 0;
    srcBox.back = 1;
    FVector4 uuid {1, 1, 1, 1};

    if (stagingTexture == nullptr)
        return uuid;

    // 3. 특정 좌표만 복사
    DeviceContext->CopySubresourceRegion(
        stagingTexture, // 대상 텍스처
        0,              // 대상 서브리소스
        0, 0, 0,        // 대상 좌표 (x, y, z)
        PickingFrameBuffer, // 원본 텍스처
        0,              // 원본 서브리소스
        &srcBox         // 복사 영역
    );

    // 4. 데이터 매핑
    D3D11_MAPPED_SUBRESOURCE mapped = {};
    DeviceContext->Map(stagingTexture, 0, D3D11_MAP_READ, 0, &mapped);

    // 5. 픽셀 데이터 추출 (1x1 텍스처이므로 offset = 0)
    const BYTE* pixelData = static_cast<const BYTE*>(mapped.pData);

    if (pixelData)
    {
        uuid.X = static_cast<float>(pixelData[0]); // R
        uuid.Y = static_cast<float>(pixelData[1]); // G
        uuid.Z = static_cast<float>(pixelData[2]); // B
        uuid.W = static_cast<float>(pixelData[3]); // A
    }
    std::cout << "X: " << (int)uuid.X << " Y: " << (int)uuid.Y
        << " Z: " << uuid.Z << " A: " << uuid.W << "\n";

    // 6. 매핑 해제 및 정리
    DeviceContext->Unmap(stagingTexture, 0);
    SAFE_RELEASE(stagingTexture);

    return uuid;
}

void URenderer::UpdateViewMatrix(const FTransform& CameraTransform)
{
    ViewMatrix = CameraTransform.GetViewMatrix();
}

void URenderer::UpdateProjectionMatrix(ACamera* Camera)
{
    float AspectRatio = UEngine::Get().GetScreenRatio();

    float FOV = FMath::DegreesToRadians(Camera->GetFieldOfView());
    float ViewportSize = Camera->GetViewportSize();
    float Near = Camera->GetNear();
    float Far = Camera->GetFar();

    if (Camera->ProjectionMode == ECameraProjectionMode::Perspective)
    {
        ProjectionMatrix = FMatrix::PerspectiveFovLH(FOV, AspectRatio, Near, Far);
    }
    else if (Camera->ProjectionMode == ECameraProjectionMode::Orthographic)
    {
        ProjectionMatrix = FMatrix::OrthoForLH(ViewportSize, ViewportSize, Near, Far);
    }
}

void URenderer::OnUpdateWindowSize(int Width, int Height)
{
    ReleaseFrameBuffer();
    ReleasePickingFrameBuffer();
    ReleaseDepthStencilBuffer();

    if (SwapChain)
    {
        SwapChain->ResizeBuffers(0, Width, Height, DXGI_FORMAT_UNKNOWN, 0);

        DXGI_SWAP_CHAIN_DESC SwapChainDesc;
        SwapChain->GetDesc(&SwapChainDesc);
        // 뷰포트 정보 갱신
        ViewportInfo = {
            0.0f, 0.0f,
            static_cast<float>(SwapChainDesc.BufferDesc.Width), static_cast<float>(SwapChainDesc.BufferDesc.Height),
            0.0f, 1.0f
        };
    }
}

void URenderer::OnResizeComplete()
{
    // 프레임 버퍼를 다시 생성
    CreateFrameBuffer();
    CreatePickingTexture(UEngine::Get().GetWindowHandle());
    // 뎁스 스텐실 버퍼를 다시 생성
    CreateDepthStencilBuffer();

    RTVs[0] = FrameBufferRTV;
    RTVs[1] = PickingFrameBufferRTV;
}

void URenderer::RenderPickingTexture()
{
    // 백버퍼로 복사
    ID3D11Texture2D* backBuffer;
    SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBuffer));
    DeviceContext->CopyResource(backBuffer, PickingFrameBuffer);
    SAFE_RELEASE(backBuffer);
}

void URenderer::CreateTextureSRV(const std::string& filename)
{
    // 지역 변수로 우선 선언
    ID3D11Texture2D* Texture;
    ID3D11ShaderResourceView* SRV;
    int Width, Height, Channels;


    std::string path = "./Textures/" + filename;
    unsigned char* img = stbi_load(path.c_str(), &Width, &Height, &Channels, 0); // 이미지 데이터 읽어옴

    assert(Channels == 4);

    std::vector<uint8_t> image;

    image.resize(Width * Height * Channels);
    memcpy(image.data(), img, image.size() * sizeof(uint8_t));
    stbi_image_free(img);

    // Create texture.
    D3D11_TEXTURE2D_DESC txtDesc = {};
    txtDesc.Width = Width;
    txtDesc.Height = Height;
    txtDesc.MipLevels = txtDesc.ArraySize = 1;
    txtDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;        // 8비트씩 unsigned norm
    txtDesc.SampleDesc.Count = 1;
    txtDesc.Usage = D3D11_USAGE_IMMUTABLE;              // 한 번 읽고 수정 X
    txtDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;     // Shader Resource 플래그

    // Fill in the subresource data.
    D3D11_SUBRESOURCE_DATA InitData;
    InitData.pSysMem = image.data();
    InitData.SysMemPitch = txtDesc.Width * sizeof(uint8_t) * Channels;     // SysMemPitch : 한 행의 바이트 크기
    // InitData.SysMemSlicePitch = 0;

    Device->CreateTexture2D(&txtDesc, &InitData, &Texture);                     // 텍스처, 텍스처 리소스뷰 생성
    Device->CreateShaderResourceView(Texture, nullptr, &SRV);

    // 실제로 접근 가능한 map으로 삽입
    uint32 idx = ShaderResourceViewMap.size();
    ShaderResourceViewMap.insert({ idx, SRV });
}

void URenderer::CreateTextureSRV(const wchar_t* filename)
{
    using namespace DirectX;

    ComPtr<ID3D11ShaderResourceView> SRV;
    ComPtr<ID3D11Texture2D> Texture; // Texture도 받아야 함

    auto hr = CreateDDSTextureFromFile(Device, DeviceContext, filename, (ID3D11Resource**)Texture.GetAddressOf(), SRV.GetAddressOf());

    if (FAILED(hr))
    {
        UE_LOG("Failed to load texture");
        return;
    }
    assert(SRV.Get() != nullptr);
    // ShaderResourceViewMap에 추가
    uint32_t idx = ShaderResourceViewMap.size();
    ShaderResourceViewMap.insert({ idx, SRV });

    UE_LOG("Successfully loaded texture");
}


void URenderer::CreateTextureSRVW(const WIDECHAR* filename)
{
    using namespace DirectX;

    ComPtr<ID3D11ShaderResourceView> SRV;
    ComPtr<ID3D11Texture2D> Texture; // Texture도 받아야 함


    auto hr = CreateWICTextureFromFile(Device, DeviceContext, filename, (ID3D11Resource**)Texture.GetAddressOf(), SRV.GetAddressOf());


    if (FAILED(hr))
    {
        UE_LOG("Failed to load texture");
        return;
    }
    assert(SRV.Get() != nullptr);
    // ShaderResourceViewMap에 추가
    uint32_t idx = ShaderResourceViewMap.size();
    ShaderResourceViewMap.insert({ idx, SRV });

    UE_LOG("Successfully loaded texture");
}