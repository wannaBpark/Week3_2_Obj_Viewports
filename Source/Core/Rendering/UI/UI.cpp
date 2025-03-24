#include "UI.h"

#include <algorithm>

#include "Object/Actor/Camera.h"
#include "Core/Rendering/URenderer.h"
#include "Core/HAL/PlatformMemory.h"
#include "ImGui/imgui_impl_dx11.h"
#include "ImGui/imgui_impl_win32.h"
#include "Debug/DebugConsole.h"
#include "ImGui/imgui_internal.h"
#include "Object/Actor/Actor.h"
#include "Object/PrimitiveComponent/UPrimitiveComponent.h"
#include "Object/Actor/Sphere.h"
#include "Object/Actor/Cube.h"
#include "Object/Actor/Arrow.h"
#include "Object/Actor/Cone.h"
#include "Object/Actor/Cylinder.h"
#include "Object/Actor/Circle.h"
#include "Object/Actor/Triangle.h"
#include "Object/Actor/WorldGrid.h"
#include "Object/Actor/BillBoard.h"
#include "Object/Actor/WorldText.h"
#include "Object/Actor/SubUV.h"
#include "Object/Actor/ATarzan.h"
#include "Object/UtilComponent/UStringComponent.h"
#include "Static/FEditorManager.h"
#include "Object/World/World.h"
#include "Object/Gizmo/GizmoHandle.h"
#include "Object/Gizmo/WorldGizmo.h"
#include "Core/FSceneManager.h"
#include "Object/Gizmo/Axis.h"
#include "Object/Material/Material.h"
#include "JsonSaveHelper.h"
#include <Object/StaticMeshComponent/StaticMeshComponent.h>
#include "StaticMeshInspector.h"

#define INI_PATH "./editor.ini" // grid scale 저장할 ini 파일 경로

void UI::Initialize(HWND hWnd, const URenderer& Renderer, UINT ScreenWidth, UINT ScreenHeight)
{
    // ImGui 초기화
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    //io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    // 기본 폰트 크기 설정
    io.FontGlobalScale = 1.0f;
    io.DisplaySize = ScreenSize;
    //io.WantSetMousePos = true;
    // ImGui Backend 초기화
    ImGui_ImplWin32_Init(hWnd);
    ImGui_ImplDX11_Init(Renderer.GetDevice(), Renderer.GetDeviceContext());

	ScreenSize = ImVec2(static_cast<float>(ScreenWidth), static_cast<float>(ScreenHeight));
    InitialScreenSize = ScreenSize;
    bIsInitialized = true;
    
    io.DisplaySize = ScreenSize;

    PreRatio = GetRatio();
    CurRatio = GetRatio();

    GetCameraStartSpeed();
}

void UI::Update()
{
    POINT mousePos;
    if (GetCursorPos(&mousePos)) {
        HWND hwnd = GetActiveWindow();
        ScreenToClient(hwnd, &mousePos);

        ImVec2 CalculatedMousePos = ResizeToScreenByCurrentRatio(ImVec2(mousePos.x, mousePos.y));
        ImGui::GetIO().MousePos = CalculatedMousePos;
        //UE_LOG("MousePos: (%.1f, %.1f), DisplaySize: (%.1f, %.1f)\n",CalculatedMousePos.x, CalculatedMousePos.y, GetRatio().x, GetRatio().y);
    }

    
    // ImGui Frame 생성
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    if (bWasWindowSizeUpdated)
    {
        PreRatio = CurRatio;
        CurRatio = GetRatio();
        //UE_LOG("Current Ratio: %f, %f", CurRatio.x, CurRatio.y);
    }
    
    RenderGameView();
    RenderControlPanel();
    RenderPropertyWindow();
    RenderSceneManager();
    Debug::ShowConsole(bWasWindowSizeUpdated, PreRatio, CurRatio);

    // ImGui 렌더링
    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    bWasWindowSizeUpdated = false;
}


void UI::Shutdown()
{
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    if (StaticMeshInspector)
        delete StaticMeshInspector;
}

void UI::OnUpdateWindowSize(UINT InScreenWidth, UINT InScreenHeight)
{
    // ImGUI 리소스 다시 생성
    ImGui_ImplDX11_InvalidateDeviceObjects();
    ImGui_ImplDX11_CreateDeviceObjects();
   // ImGui 창 크기 업데이트

    ImGuiIO& io = ImGui::GetIO();
	io.DisplaySize = ImVec2(static_cast<float>(InScreenWidth), static_cast<float>(InScreenHeight));

    bWasWindowSizeUpdated = true;
}

void UI::RenderGameView()
{
    //ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport());


}

void UI::RenderControlPanel()
{
    ImGui::Begin("Jungle Control Panel");

    if (bWasWindowSizeUpdated)
    {
        auto* Window = ImGui::GetCurrentWindow();

        ImGui::SetWindowPos(ResizeToScreen(Window->Pos));
        ImGui::SetWindowSize(ResizeToScreen(Window->Size));
    }
    
    ImGui::Text("Hello, Jungle World!");
    ImGui::Text("FPS: %.3f (what is that ms)", ImGui::GetIO().Framerate);

    RenderMemoryUsage();
    RenderPrimitiveSelection();
    RenderCameraSettings();

    RenderAtlasData();
    
    ImGui::End();
}

void UI::RenderMemoryUsage()
{
    const uint64 ContainerAllocByte = FPlatformMemory::GetAllocationBytes<EAT_Container>();
    const uint64 ContainerAllocCount = FPlatformMemory::GetAllocationCount<EAT_Container>();
    const uint64 ObjectAllocByte = FPlatformMemory::GetAllocationBytes<EAT_Object>();
    const uint64 ObjectAllocCount = FPlatformMemory::GetAllocationCount<EAT_Object>();
    ImGui::Text(
        "Container Memory Uses: %llubyte, Count: %llu",
        ContainerAllocByte,
        ContainerAllocCount
    );
    ImGui::Text(
        "Object Memory Uses: %llubyte, Count: %llu Objects",
        ObjectAllocByte,
        ObjectAllocCount
    );
    ImGui::Text(
        "Total Memory Uses: %llubyte, Count: %llu",
        ContainerAllocByte + ObjectAllocByte,
        ContainerAllocCount + ObjectAllocCount
    );

    ImGui::Separator();
}

void UI::RenderPrimitiveSelection()
{
    const char* items[] = { "Sphere", "Cube", "Cylinder", "Cone","Triangle","Circle", "BillBoard", "WorldText", "SubUV", "SubUV2", "Tarzan"};

    ImGui::Combo("Primitive", &currentItem, items, IM_ARRAYSIZE(items));

    if (ImGui::Button("Spawn"))
    {
        UWorld* World = UEngine::Get().GetWorld();
        for (int i = 0 ;  i < NumOfSpawn; i++)
        {
            if (strcmp(items[currentItem], "Sphere") == 0)
            {
                World->SpawnActor<ASphere>();
            }
            else if (strcmp(items[currentItem], "Cube") == 0)
            {
                World->SpawnActor<ACube>();
            }
            else if (strcmp(items[currentItem], "Cylinder") == 0)
            {
                World->SpawnActor<ACylinder>();
            }
            else if (strcmp(items[currentItem], "Cone") == 0)
            {
                World->SpawnActor<ACone>();
            }
            else if (strcmp(items[currentItem], "Circle") == 0)
            {
                World->SpawnActor<ACircle>();
			}
            else if (strcmp(items[currentItem], "Triangle") == 0)
            {
                World->SpawnActor<ATriangle>();
            }
			else if (strcmp(items[currentItem], "BillBoard") == 0)
			{
				World->SpawnActor<ABillBoard>();
			}
			else if (strcmp(items[currentItem], "WorldText") == 0)
			{
				AWorldText* wT = World->SpawnActor<AWorldText>();
                std::string xx = "hello world!";
                wT->SetLetterSpacing(-0.5f);
                wT->SetCharComps(xx, "koverwatchBlack.png");
				wT->SetActorTransform(FTransform(FVector(0, 0, 2), FQuat(0, 0, 0, 1), FVector(1, 1, 1)));
			}
            else if (strcmp(items[currentItem], "SubUV") == 0)
            {
                ASubUV* SubUV = World->SpawnActor<ASubUV>();
                SubUV->SetAtlas("Faker.png");
            }
            else if (strcmp(items[currentItem], "SubUV2") == 0)
            {
                ASubUV* SubUV = World->SpawnActor<ASubUV>();
                SubUV->SetAtlas("RollingChanhui.png");
            }
            else if (strcmp(items[currentItem], "Tarzan") == 0)
            {
                World->SpawnActor<ATarzan>();
            }
        }
    }
    ImGui::SameLine();
    ImGui::InputInt("Number of spawn", &NumOfSpawn, 0);

    ImGui::Separator();

    UWorld* World = UEngine::Get().GetWorld();
    uint32 bufferSize = 100;
    char* SceneNameInput = new char[bufferSize];
    strcpy_s(SceneNameInput, bufferSize, *World->SceneName);
    
	if (ImGui::InputText("Scene Name", SceneNameInput, bufferSize))
	{
		World->SceneName = SceneNameInput;
	}
    
    if (ImGui::Button("New Scene"))
    {
        World->ClearWorld();
        //FEditorManager::Get().SetCamera(World->SpawnActor<ACamera>());
        //World->SpawnActor<AAxis>();
        //FEditorManager::Get().SetBoundingBox(Picker->GetBoundingBoxComp());
        //FEditorManager::Get().SetStringComp(Picker->GetStringComponent());

		//UEngine::Get().GetWorld()->SpawnActor<AWorldGrid>();
        //UEngine::Get().GetWorld()->SpawnActor<AWorldGizmo>();
        GetGridScaleFromIni();
        GetCameraStartSpeed();
		
    }
    if (ImGui::Button("Save Scene"))
    {
        World->SaveWorld();   
        
        char gridScaleStr[32];
        sprintf_s(gridScaleStr, "%.2f", World->GetGridScale());

        if (!WritePrivateProfileStringA("EditorSettings", "GridScale", gridScaleStr, INI_PATH)) {
            MessageBoxA(NULL, "Failed to save GridScale!", "Error", MB_OK | MB_ICONERROR);
        }

        ACamera* Camera = FEditorManager::Get().GetCamera();
        char cameraSpeedStr[32];
        sprintf_s(cameraSpeedStr, "%.2f", Camera->CameraSpeed);

        if (!WritePrivateProfileStringA("EditorSettings", "CameraStartSpeed", cameraSpeedStr, INI_PATH)) {
            MessageBoxA(NULL, "Failed to save CameraSpeed!", "Error", MB_OK | MB_ICONERROR);
        }

    }
    if (ImGui::Button("Load Scene"))
    {
        World->LoadWorld(SceneNameInput);
    }
	float GridScale = World->GetGridScale();
    if (ImGui::SliderFloat("Grid Scale", &GridScale, 0.1f, 100.0f)) {
		World->SetGridScale(GridScale);
    }
    ImGui::Separator();

    const char* VMI_items[] = { "LIT", "UNLIT", "WIREFRAME"};
    if (ImGui::Combo("View Mode", &currentVMI, VMI_items, IM_ARRAYSIZE(VMI_items)))
    {
		World->SetViewMode(currentVMI);
    }

}

void UI::RenderCameraSettings()
{
    ImGui::Text("Camera");

    ACamera* Camera = FEditorManager::Get().GetCamera();

    bool IsOrthogonal;
    if (Camera->ProjectionMode == ECameraProjectionMode::Orthographic)
    {
        IsOrthogonal = true;
    }
    else if (Camera->ProjectionMode == ECameraProjectionMode::Perspective)
    {
        IsOrthogonal = false;
    }

    if (ImGui::Checkbox("Orthogonal", &IsOrthogonal))
    {
        if (IsOrthogonal)
        {
            Camera->ProjectionMode = ECameraProjectionMode::Orthographic;
        }
        else
        {
            Camera->ProjectionMode = ECameraProjectionMode::Perspective;
        }
    }

    float ViewportSize = Camera->GetViewportSize();

    if (IsOrthogonal) {
        ImGui::PushItemWidth(168);
        ImGui::SameLine();
        ImGui::DragFloat("ViewportSize", &ViewportSize, 0.1f);
        ImGui::PopItemWidth();
        Camera->SetViewportSize(ViewportSize);
    }

    float FOV = Camera->GetFieldOfView();
    if (ImGui::DragFloat("FOV", &FOV, 0.1f))
    {
        FOV = std::clamp(FOV, 0.01f, 179.99f);
        Camera->SetFieldOfVew(FOV);
    }

    float NearFar[2] = { Camera->GetNear(), Camera->GetFar() };
    if (ImGui::DragFloat2("Near, Far", NearFar, 0.1f))
    {
        NearFar[0] = FMath::Max(0.01f, NearFar[0]);
        NearFar[1] = FMath::Max(0.01f, NearFar[1]);

        if (NearFar[0] < NearFar[1])
        {
            Camera->SetNear(NearFar[0]);
            Camera->SetFar(NearFar[1]);
        }
        else
        {
            if (abs(NearFar[0] - Camera->GetNear()) < 0.00001f)
            {
                Camera->SetFar(NearFar[0] + 0.01f);
            }
            else if (abs(NearFar[1] - Camera->GetFar()) < 0.00001f)
            {
                Camera->SetNear(NearFar[1] - 0.01f);
            }
        }
    }
    
    FVector CameraPosition = Camera->GetActorTransform().GetPosition();
    if (ImGui::DragFloat3("Camera Location", reinterpret_cast<float*>(&CameraPosition), 0.1f))
    {
        FTransform Trans = Camera->GetActorTransform();
        Trans.SetPosition(CameraPosition);
        Camera->SetActorTransform(Trans);
    }

    FVector PrevEulerAngle = Camera->GetActorTransform().GetRotation().GetEuler();
    FVector UIEulerAngle = { PrevEulerAngle.X, PrevEulerAngle.Y, PrevEulerAngle.Z };
    if (ImGui::DragFloat3("Camera Rotation", reinterpret_cast<float*>(&UIEulerAngle), 0.1f))
    {
        FTransform Transform = Camera->GetActorTransform();

        //FVector DeltaEulerAngle = UIEulerAngle - PrevEulerAngle;
        //Transform.Rotate(DeltaEulerAngle);
        
        UIEulerAngle.Y = FMath::Clamp(UIEulerAngle.Y, -Camera->MaxYDegree, Camera->MaxYDegree);
        Transform.SetRotation(UIEulerAngle);
        Camera->SetActorTransform(Transform);
    }
    ImGui::DragFloat("Camera Speed", &Camera->CameraSpeed, 0.1f);

    FVector Forward = Camera->GetActorTransform().GetForward();
    FVector Up = Camera->GetActorTransform().GetUp();
    FVector Right = Camera->GetActorTransform().GetRight();

    ImGui::Text("Camera GetForward(): (%.2f %.2f %.2f)", Forward.X, Forward.Y, Forward.Z);
    ImGui::Text("Camera GetUp(): (%.2f %.2f %.2f)", Up.X, Up.Y, Up.Z);
    ImGui::Text("Camera GetRight(): (%.2f %.2f %.2f)", Right.X, Right.Y, Right.Z);
}

void UI::RenderAtlasData() 
{
    ImGui::Separator();
    ImGui::Text("Atlas Data");
    ImGui::Text("Add Atlas Data");

    ImGui::InputText("AtlasName", AtlasName, sizeof(AtlasName));
    
    ImGui::PushItemWidth(75);
    ImGui::DragInt("ColNum", &ColNum);
    ImGui::SameLine(0, 37);
    ImGui::DragInt("RowNum", &RowNum);
    ImGui::DragInt("AtlasWidth", &AtlasWidth);
    ImGui::SameLine();
    ImGui::DragInt("AtlasHeight", &AtlasHeight);
    ImGui::DragInt("TextureIndex", &TextureIndex);
    ImGui::DragInt("TotalFrame", &TotalFrame);

    if (ImGui::Button("Make New Atlas Data")) 
    {
        UAtlasInfo AtlasInfo = UAtlasInfo();
        AtlasInfo.AtlasName = AtlasName;
        AtlasInfo.ColNum = ColNum;
        AtlasInfo.RowNum = RowNum;
        AtlasInfo.AtlasWidth = AtlasWidth;
        AtlasInfo.AtlasHeight = AtlasHeight;
        AtlasInfo.TextureIndex = TextureIndex;
        AtlasInfo.TotalFrame = TotalFrame;

        if (JsonSaveHelper::SaveAtlasInfo(AtlasInfo)) 
        {
            AtlasSaveCondition = 1;
        }
        else 
        {
            AtlasSaveCondition = -1;
        }
        
    }

    if (AtlasSaveCondition == 1)
    {
        ImGui::SameLine();
        ImGui::Text("Save Atlas Data Success!");
    }
    else if (AtlasSaveCondition == -1)
    {
        ImGui::SameLine();
        ImGui::Text("Save Atlas Data Fail! Check AtlasName is Not Null");
    }
}

void UI::RenderPropertyWindow()
{

    ImGui::Begin("Properties");

    if (bWasWindowSizeUpdated)
    {
        auto* Window = ImGui::GetCurrentWindow();

        ImGui::SetWindowPos(ResizeToScreen(Window->Pos));
        ImGui::SetWindowSize(ResizeToScreen(Window->Size));
    }
    
    AActor* selectedActor = FEditorManager::Get().GetSelectedActor();
    USceneComponent* selectedComp = FEditorManager::Get().GetSelectedComponent();
    if (selectedComp != nullptr)
    {
        if (selectedComp->IsA<UStaticMeshComponent>())
        {
			UStaticMeshComponent* StaticMeshComp = static_cast<UStaticMeshComponent*>(selectedComp);
            if (StaticMeshInspector == nullptr)
            {
                StaticMeshInspector = new FStaticMeshInspector();
                StaticMeshInspector->Init(StaticMeshComp);
            }
            else
            {
                if (StaticMeshInspector->GetCurrentStaticMeshComponent() != StaticMeshComp)
                {
                    StaticMeshInspector->Init(StaticMeshComp);
                }
            }
            StaticMeshInspector->Update();
        }
    }
    else if (selectedActor != nullptr)
    {
        FTransform selectedTransform = selectedActor->GetActorTransform();
        float position[] = { selectedTransform.GetPosition().X, selectedTransform.GetPosition().Y, selectedTransform.GetPosition().Z };
        float scale[] = { selectedTransform.GetScale().X, selectedTransform.GetScale().Y, selectedTransform.GetScale().Z };

        if (ImGui::DragFloat3("Translation", position, 0.1f))
        {
            selectedTransform.SetPosition(position[0], position[1], position[2]);
            selectedActor->SetActorTransform(selectedTransform);
        }

        FVector PrevEulerAngle = selectedTransform.GetRotation().GetEuler();
        FVector UIEulerAngle = PrevEulerAngle;
        if (ImGui::DragFloat3("Rotation", reinterpret_cast<float*>(&UIEulerAngle), 0.1f))
        {
            FVector DeltaEulerAngle = UIEulerAngle - PrevEulerAngle;

            selectedTransform.Rotate(DeltaEulerAngle);
			UE_LOG("Rotation: %.2f, %.2f, %.2f", DeltaEulerAngle.X, DeltaEulerAngle.Y, DeltaEulerAngle.Z);
            selectedActor->SetActorTransform(selectedTransform);
        }
        if (ImGui::DragFloat3("Scale", scale, 0.1f))
        {
            selectedTransform.SetScale(scale[0], scale[1], scale[2]);
            selectedActor->SetActorTransform(selectedTransform);
        }
        TSet<UActorComponent*> components = selectedActor->GetComponents();
        
        for (auto component : components) { // TODO: Update에서 dynamic_cast 하는 거 너무 낭비인듯, FName 만들어지면 FName으로 비교
            auto comp = dynamic_cast<UPrimitiveComponent*>(component);
            if (comp != nullptr) {
                bool IsUseTexture = comp->IsUseVertexColor();
                if (ImGui::Checkbox("Use Texture", &IsUseTexture)) {
                    comp->SetUseVertexColor(IsUseTexture);
                }
                
                ImGui::SameLine();

                bool IsUseBillboard = comp->IsUseBillboardUtil();
				if (ImGui::Checkbox("Use Billboard", &IsUseBillboard))
				{
					comp->SetUseBillboardUtil(IsUseBillboard);
				}

            }
        }
		if (FEditorManager::Get().GetGizmoHandle() != nullptr)
		{
			AGizmoHandle* Gizmo = FEditorManager::Get().GetGizmoHandle();
            if(Gizmo->GetGizmoType() == EGizmoType::Translate)
			{
				ImGui::Text("GizmoType: Translate");
			}
			else if (Gizmo->GetGizmoType() == EGizmoType::Rotate)
			{
				ImGui::Text("GizmoType: Rotate");
			}
			else if (Gizmo->GetGizmoType() == EGizmoType::Scale)
			{
				ImGui::Text("GizmoType: Scale");
			}
		}
        if (FEditorManager::Get().GetStringComp() != nullptr) 
        {
            // WorldText의 경우 Char Component를 자체적으로 관리하므로 특수 처리
            // Letter Spacing을 조절할 수 있도록
            UStringComponent* StringComp = FEditorManager::Get().GetStringComp();
            bool IsUseBillboard = StringComp->IsUseBillboardUtil();
            if (ImGui::Checkbox("Use Text Billboard", &IsUseBillboard))
            {
                StringComp->SetUseBillboardUtil(IsUseBillboard);
            }

            float LetterSpacing = StringComp->GetLetterSpacing();
            if (ImGui::DragFloat("Letter Spacing", &LetterSpacing, 0.1f))
            {
                StringComp->SetLetterSpacing(LetterSpacing);
            }
        }

        if (selectedActor->GetClass() == AWorldText::StaticClass()) 
        {
            AWorldText* TextActor = dynamic_cast<AWorldText*>(selectedActor);
            if (selectedActor) {
                ImGui::Separator();
                ImGui::Text("Text Actor String");
                ImGui::SameLine();

                std::string textStr = TextActor->GetString();

                char buffer[256];

                strncpy_s(buffer, sizeof(buffer), textStr.c_str(), _TRUNCATE);

                buffer[sizeof(buffer) - 1] = '\0';

                float LetterSpacing = TextActor->GetLetterSpacing();

                if (ImGui::InputText("WorldTextStr", buffer, sizeof(buffer))) {
                    TextActor->SetCharComps(buffer, "koverwatch.png");
                }

                if (ImGui::DragFloat("Text Letter Spacing", &LetterSpacing, 0.1f))
                {
                    TextActor->SetLetterSpacing(LetterSpacing);
                }
            }

            
        }
    }
    ImGui::End();
}

void UI::RenderSceneManager()
{
    ImGui::Begin("SceneManager");

    if (bWasWindowSizeUpdated)
    {
        auto* Window = ImGui::GetCurrentWindow();

        ImGui::SetWindowPos(ResizeToScreen(Window->Pos));
        ImGui::SetWindowSize(ResizeToScreen(Window->Size));
    }

    if (ImGui::TreeNode("Primitives")) {
        TArray<AActor*> actors;
        if (FSceneManager::Get().GetScene(0) != nullptr)
            actors = FSceneManager::Get().GetScene(0)->GetActors();

        for (auto actor : actors) {
            /*UClass* uClass = actor->GetClass();
            if (uClass == AAxis::StaticClass() || uClass == AWorldGrid::StaticClass() || uClass == AWorldGizmo::StaticClass() ||
                uClass == ACamera::StaticClass() || uClass == APicker::StaticClass() || uClass == AGizmoHandle::StaticClass())
                continue;*/

            if (actor->IsGizmoActor())
                continue;

            char buffer[64];
            sprintf_s(buffer, "%s(UUID: %d)", *actor->GetFName().ToString(), actor->GetUUID());
            bool nodeOpen = ImGui::TreeNodeEx(buffer, ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth);

            // 항목이 클릭되었는지 확인
            if (ImGui::IsItemClicked()) {
                APicker::SetSelectActor(actor->GetRootComponent());
            }

            if (nodeOpen) {
                ImGui::Indent();
                for (auto component : actor->GetComponents()) {
                    ImGui::Text(*component->GetFName().ToString());
					if (ImGui::IsItemClicked()) {
                        if (component->IsA<USceneComponent>())
                        {
    						APicker::SetSelectedComponent(actor->GetRootComponent());
                        }
					}
                }
                ImGui::Unindent();
                ImGui::TreePop();
            }
        }
        ImGui::TreePop();
    }

    FSceneManager& SceneManager = FSceneManager::Get();
    uint32 showFlagMask = SceneManager.GetShowFlagMask();

    static const std::pair<const char*, EShowFlag> showFlagOptions[] = {
        {"Show Grid", EShowFlag::Grid},
        {"Show Primitive", EShowFlag::Primitive},
        {"Show Text", EShowFlag::Text},
        {"Show Bounding Box", EShowFlag::BoundingBox},
    };

    if (ImGui::TreeNode("ShowFlag"))
    {

        for (const auto& [label, flag] : showFlagOptions)
        {
            bool isEnabled = (showFlagMask & flag) != 0;
            if (ImGui::Checkbox(label, &isEnabled))
            {
                SceneManager.ToggleShowFlag(flag);
            }
        }
        ImGui::TreePop();
    }

    ImGui::End();
}

void PrintChildPrimitiveButton(AActor& actor) {
    actor.GetRootComponent();
}

void UI::GetGridScaleFromIni()
{
    // Grid Scale 값을 editor.ini 파일에서 읽어옴
    std::vector<char> buffer(256);
    GetPrivateProfileStringA("EditorSettings", "GridScale", "1.0", buffer.data(), buffer.size(), INI_PATH);
    float SavedGridScale = std::stof(buffer.data());
    UEngine::Get().GetWorld()->SetGridScale(SavedGridScale);
}


// Camera Speed 값 저장은 Save Scene 할 때 같이 해줌
void UI::GetCameraStartSpeed()
{
    // Camera Start Speed 값을 editor.ini 파일에서 읽어옴
    std::vector<char> buffer(256);
    GetPrivateProfileStringA("EditorSettings", "CameraStartSpeed", "1.0", buffer.data(), buffer.size(), INI_PATH);
    ACamera* Camera = FEditorManager::Get().GetCamera();
    Camera->CameraSpeed = std::stof(buffer.data());
}


