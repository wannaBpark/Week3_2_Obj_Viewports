#include "Engine.h"

#include <iostream>
#include "Object/ObjectFactory.h"
#include "Object/World/World.h"
#include "Debug/DebugConsole.h"
#include "Object/Gizmo/Axis.h"
#include "Core/Input/PlayerInput.h"
#include "Core/Input/PlayerController.h"
#include "Object/Actor/Camera.h"
#include "Object/Actor/Sphere.h"
#include "Object/Actor/WorldGrid.h"
#include "Static/FEditorManager.h"
#include "Object/Gizmo/WorldGizmo.h"
#include "Core/FSceneManager.h"
#include "EngineConfig.h"
#include "Core/Container/ObjectIterator.h"
#include "Object/Mesh/ObjManager.h"
#include "Object/Actor/ATarzan.h"



class AArrow;
class APicker;
// ImGui WndProc 정의
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);


LRESULT UEngine::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    // ImGui의 메시지를 처리
    if (ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
    {
        return true;
    }
    
    switch (uMsg)
    {
        // 창이 제거될 때 (창 닫기, Alt+F4 등)
    case WM_DESTROY:
        PostQuitMessage(0); // 프로그램 종료
        break;
    case WM_KEYDOWN:
        APlayerInput::Get().KeyDown(static_cast<EKeyCode>(wParam));
        if ((lParam>>30)%2 != 0)
        {
            APlayerInput::Get().KeyOnceUp(static_cast<EKeyCode>( wParam ));
        }
        break;
    case WM_KEYUP:
        APlayerInput::Get().KeyUp(static_cast<EKeyCode>( wParam ));
        break;
    case WM_LBUTTONDOWN:
        APlayerInput::Get().HandleMouseInput(hWnd, lParam, true, false);
        break;
    case WM_LBUTTONUP:
        APlayerInput::Get().HandleMouseInput(hWnd, lParam, false, false);
        break;
    case WM_RBUTTONDOWN:
        APlayerInput::Get().HandleMouseInput(hWnd, lParam, true, true);
        break;
    case WM_RBUTTONUP:
        APlayerInput::Get().HandleMouseInput(hWnd, lParam, false, true);
        break;
    case WM_SIZE:
		UEngine::Get().UpdateWindowSize(LOWORD(lParam), HIWORD(lParam));
		break;
    default:
        return DefWindowProc(hWnd, uMsg, wParam, lParam);
    }

    return 0;
}

void UEngine::Initialize(
    HINSTANCE hInstance, const WCHAR* InWindowTitle, const WCHAR* InWindowClassName, int InScreenWidth,
    int InScreenHeight,
    EScreenMode InScreenMode
)
{
    EngineConfig = new FEngineConfig();
    EngineConfig->LoadEngineConfig();

	int StoredWidth = EngineConfig->GetEngineConfigValue<int>(EEngineConfigValueType::EEC_ScreenWidth);
	int StoredHeight = EngineConfig->GetEngineConfigValue<int>(EEngineConfigValueType::EEC_ScreenHeight);

    WindowInstance = hInstance;
    WindowTitle = InWindowTitle;
    WindowClassName = InWindowClassName;
    ScreenMode = InScreenMode;
    ScreenWidth = StoredWidth;
    ScreenHeight = StoredHeight;

    InitWindow(ScreenWidth, ScreenHeight);

	EngineConfig->SaveEngineConfig<int>(EEngineConfigValueType::EEC_ScreenWidth, ScreenWidth);
	EngineConfig->SaveEngineConfig<int>(EEngineConfigValueType::EEC_ScreenHeight, ScreenHeight);

    InitRenderer();

    InitWorld();

    InitializedScreenWidth = ScreenWidth;
    InitializedScreenHeight = ScreenHeight;
    
    ui.Initialize(WindowHandle, *Renderer, ScreenWidth, ScreenHeight);
    
	UE_LOG("Engine Initialized!");
}

void UEngine::Run()
{
    // FPS 제한
    constexpr int TargetFPS = 750;
    constexpr double TargetDeltaTime = 1000.0f / TargetFPS; // 1 FPS의 목표 시간 (ms)

    // 고성능 타이머 초기화
    LARGE_INTEGER Frequency;
    QueryPerformanceFrequency(&Frequency);

    LARGE_INTEGER StartTime;
    QueryPerformanceCounter(&StartTime);


    IsRunning = true;
    while (IsRunning)
    {
        // DeltaTime 계산 (초 단위)
        const LARGE_INTEGER EndTime = StartTime;
        QueryPerformanceCounter(&StartTime);

        const float DeltaTime =
            static_cast<float>(StartTime.QuadPart - EndTime.QuadPart) / static_cast<float>(Frequency.QuadPart);

        APlayerInput::Get().PreProcessInput();
        
        // 메시지(이벤트) 처리
        MSG Msg;
        while (PeekMessage(&Msg, nullptr, 0, 0, PM_REMOVE))
        {
            // 키 입력 메시지를 번역
            TranslateMessage(&Msg);

            // 메시지를 등록한 Proc에 전달
            DispatchMessage(&Msg);

            if (Msg.message == WM_QUIT)
            {
                IsRunning = false;
                break;
            }

        }
		// Renderer Update
        Renderer->Prepare();          
        Renderer->PrepareShader();    // 각 rendercomponent에서 호출

        for (TObjectIterator<UPrimitiveComponent> It(UEngine::Get().GObjects.begin(), UEngine::Get().GObjects.end());
            It;
            ++It)
        {
            UPrimitiveComponent* prim = *It;

            //if (prim)
            //{
            //    UE_LOG(TEXT("Found PrimitiveCompnent : %s"), *prim->GetName());
            //}
        }

		// World Update
		if (World)
		{
			World->Tick(DeltaTime);
			World->Render();
		    World->LateTick(DeltaTime);
		}

        //각 Actor에서 TickActor() -> PlayerTick() -> TickPlayerInput() 호출하는데 지금은 Message에서 처리하고 있다.
        APlayerInput::Get().TickPlayerInput(); //잘못된 위치. 위에 달린 주석대로 처리해야 정상 플레이어 액터 내에서만 처리해야할것같다.
        
        // TickPlayerInput
        APlayerController::Get().ProcessPlayerInput(DeltaTime);
        
		// ui Update
        ui.Update();

        Renderer->SwapBuffer();

        // FPS 제한
        double ElapsedTime;
        do
        {
            Sleep(0);

            LARGE_INTEGER CurrentTime;
            QueryPerformanceCounter(&CurrentTime);

            ElapsedTime = static_cast<double>(CurrentTime.QuadPart - StartTime.QuadPart) * 1000.0 / static_cast<double>(Frequency.QuadPart);
        } while (ElapsedTime < TargetDeltaTime);
    }
}


void UEngine::Shutdown()
{
    World->ClearWorld();
    ShutdownWindow();
}


void UEngine::InitWindow(int InScreenWidth, int InScreenHeight)
{
    // 윈도우 클래스 등록
    WNDCLASSW wnd_class{};
    wnd_class.lpfnWndProc = WndProc;
    wnd_class.hInstance = WindowInstance;
    wnd_class.lpszClassName = WindowClassName;
    RegisterClassW(&wnd_class);

    // Window Handle 생성
    WindowHandle = CreateWindowExW(
        0, WindowClassName, WindowTitle,
        WS_POPUP | WS_VISIBLE | WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        InScreenWidth, InScreenHeight,
        nullptr, nullptr, WindowInstance, nullptr
    );

    // TODO: 전체화면 구현
    if (ScreenMode != EScreenMode::Windowed)
    {
        std::cout << "not implement Fullscreen and Borderless mode." << '\n';
    }

    // 윈도우 포커싱
    ShowWindow(WindowHandle, SW_SHOW);
    SetForegroundWindow(WindowHandle);
    SetFocus(WindowHandle);

    //AllocConsole(); // 콘솔 창 생성

    //// 표준 출력 및 입력을 콘솔과 연결
    //freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
    //freopen_s((FILE**)stdin, "CONIN$", "r", stdin);

    //std::cout << "Debug Console Opened!" << '\n';
}

void UEngine::InitRenderer()
{
	// 렌더러 초기화
	Renderer = std::make_unique<URenderer>();
	Renderer->Create(WindowHandle);
	Renderer->CreateShader();
	Renderer->CreateConstantBuffer();
}

void UEngine::InitWorld()
{
    World = FObjectFactory::ConstructObject<UWorld>();
    World->SceneName = "MainScene";
    FSceneManager::Get().AddScene(World);

	ACamera* Camera = World->SpawnActor<ACamera>();
    FEditorManager::Get().SetCamera(Camera);
	FTransform CameraTransform = Camera->GetActorTransform();
    // Camera ini 세팅
    float CamPosX = EngineConfig->GetEngineConfigValue<float>(EEngineConfigValueType::EEC_EditorCameraPosX);
	float CamPosY = EngineConfig->GetEngineConfigValue<float>(EEngineConfigValueType::EEC_EditorCameraPosY);
	float CamPosZ = EngineConfig->GetEngineConfigValue<float>(EEngineConfigValueType::EEC_EditorCameraPosZ);
	CameraTransform.SetPosition(FVector(CamPosX, CamPosY, CamPosZ));
    
	float CamRotX = EngineConfig->GetEngineConfigValue<float>(EEngineConfigValueType::EEC_EditorCameraRotX);
	float CamRotY = EngineConfig->GetEngineConfigValue<float>(EEngineConfigValueType::EEC_EditorCameraRotY);
	float CamRotZ = EngineConfig->GetEngineConfigValue<float>(EEngineConfigValueType::EEC_EditorCameraRotZ);
	float CamRotW = EngineConfig->GetEngineConfigValue<float>(EEngineConfigValueType::EEC_EditorCameraRotW);
    CameraTransform.SetRotation(FQuat(CamRotX, CamRotY, CamRotZ, CamRotW));

	Camera->SetActorTransform(CameraTransform);

    World->SpawnActor<AAxis>();
    APicker* Picker = World->SpawnActor<APicker>();
    FEditorManager::Get().SetBoundingBox(Picker->GetBoundingBoxComp());
    FEditorManager::Get().SetStringComp(Picker->GetStringComponent());

    World->SpawnActor<AWorldGrid>();
    AWorldGizmo* WorldGizmo = World->SpawnActor<AWorldGizmo>();
	/*World->BeginPlay();*/

	FString DefaultSceneName = "MainScene";

    // preload
    PreloadResources();

	World->LoadWorld(*DefaultSceneName);
}

void UEngine::ShutdownWindow()
{
    DestroyWindow(WindowHandle);
    WindowHandle = nullptr;

    UnregisterClassW(WindowClassName, WindowInstance);
    WindowInstance = nullptr;

	ui.Shutdown();

    EngineConfig->SaveAllConfig();
	delete EngineConfig;

    // 프리로드 리소스 해제
	FObjManager::ReleaseResources();
}

void UEngine::UpdateWindowSize(UINT InScreenWidth, UINT InScreenHeight)
{
	ScreenWidth = InScreenWidth;
	ScreenHeight = InScreenHeight;

    if(Renderer)
    {
        Renderer->OnUpdateWindowSize(ScreenWidth, ScreenHeight);
    }

	if (ui.bIsInitialized)
	{
		ui.OnUpdateWindowSize(ScreenWidth, ScreenHeight);
	}

	if (Renderer)
	{
		Renderer->OnResizeComplete();
	}

	// 전체 윈도우 영역 가져와서 ini에 저장
	RECT WindowRect;

	GetWindowRect(WindowHandle, &WindowRect);

	UINT TotalWidth = WindowRect.right - WindowRect.left;
	UINT TotalHeignt = WindowRect.bottom - WindowRect.top;

	EngineConfig->SaveEngineConfig<int>(EEngineConfigValueType::EEC_ScreenWidth, TotalWidth);
	EngineConfig->SaveEngineConfig<int>(EEngineConfigValueType::EEC_ScreenHeight, TotalHeignt);

}

void UEngine::PreloadResources()
{
	FObjManager::LoadObjStaticMeshAsset(TEXT("Assets/2PX7U16XARLGHIM3W48FS86MJ.obj"));
	FObjManager::LoadObjStaticMeshAsset(TEXT("Assets/Dice.obj"));
}

UObject* UEngine::GetObjectByUUID(uint32 InUUID) const
{
    if (const auto Obj = GObjects.Find(InUUID))
    {
        return Obj->get();
    }
    return nullptr;
}
