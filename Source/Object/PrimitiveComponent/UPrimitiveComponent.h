#pragma once

#include "Core/Engine.h"
#include "Object/USceneComponent.h"
#include "Primitive/PrimitiveVertices.h"
#include "Core/Math/Plane.h"
#include "Core/Rendering/RenderResource.h"
#include "Object/ObjectMacro.h"

/*
* NOTE : 모든 PrimitiveComponent를 상속받는 개체는 shaderidx, constantbuffer index를 개별 지정해줘야 합니다
*/

class UBillboardUtilComponent;

class UPrimitiveComponent : public USceneComponent, public FRenderResource
{
	DECLARE_CLASS(UPrimitiveComponent, USceneComponent)
	using Super = USceneComponent;
public:
	UPrimitiveComponent() {};
	virtual ~UPrimitiveComponent() = default;

public:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	void UpdateConstantPicking(const URenderer& Renderer, FVector4 UUIDColor) const;
	void UpdateConstantDepth(const URenderer& Renderer, int Depth) const;
	virtual void Render();
	virtual void UpdateConstantData(URenderer*& Renderer);
	virtual void UpdateLightConstantData(URenderer*& Renderer);

	virtual EPrimitiveType GetType() { return EPrimitiveType::EPT_None; }

	bool IsUseVertexColor() const { return bUseVertexColor; }
	bool IsPicked() const { return bIsPicked; }

	void SetIsPicked(bool bPicked)  {
		bIsPicked = (uint32)bPicked;
	}
	void SetCustomColor(const FVector4& InColor)
	{
		CustomColor = InColor;
		bUseVertexColor = false;
	}

	void SetUseVertexColor(bool bUse)
	{
		bUseVertexColor = bUse;
	}
	const FVector4& GetCustomColor() const { return CustomColor; }

public:
	virtual void RegisterComponentWithWorld(class UWorld* World);

public:
	void SetCanBeRendered(bool bRender) { bCanBeRendered = bRender; }

	void SetIsOrthoGraphic(bool IsOrtho) { bIsOrthoGraphic = IsOrtho; }
	bool GetIsOrthoGraphic() { return bIsOrthoGraphic; }

protected:
	bool bCanBeRendered = false;
	bool bUseVertexColor = true;
	bool bIsOrthoGraphic = false;
	FVector4 CustomColor = FVector4(1.0f, 1.0f, 1.0f, 1.0f);
	bool bIsPicked = true;
public:
	FRenderResource RenderResource;
	FConstants ConstantData;
	FLightConstants LightConstantData;
public:
	bool IsUseBillboardUtil() const { return bUseBillboardUtil; }
	virtual void SetUseBillboardUtil(bool bUse);

protected:
	bool bUseBillboardUtil = false;
	UBillboardUtilComponent* BillboardUtil = nullptr;
};

class UCubeComp : public UPrimitiveComponent
{
	DECLARE_CLASS(UCubeComp, UPrimitiveComponent)
	using Super = UPrimitiveComponent;
public:
	UCubeComp()
	{
		bCanBeRendered = true;
		RenderResource.PrimitiveType = GetType();
		RenderResource.Stride = sizeof(FPosColorNormalTex);
		RenderResource.InputLayoutType = InputLayoutType::POSCOLORNORMALTEX;
		RenderResource.VertexShaderIndex = 1;
		RenderResource.PixelShaderIndex = 1;
		RenderResource.bUseIndexBuffer = true;
		/*RenderResource.VertexShaderIndex = 4;
		RenderResource.PixelShaderIndex = 4;
		RenderResource.VertexConstantIndex = 4;
		RenderResource.PixelConstantIndex = 4;
		RenderResource.bUseIndexBuffer = true;*/
		RenderResource.ShaderResourceViewIndices.emplace().push_back(0);		// 0번째 Texture 사용 : box2.png [값이 없으면 초기화]
	}
	virtual ~UCubeComp() = default;
	EPrimitiveType GetType() override
	{
		//return EPrimitiveType::EPT_Cube;
		return EPrimitiveType::EPT_CubeTex;
	}
};

class USphereComp : public UPrimitiveComponent
{
	DECLARE_CLASS(USphereComp, UPrimitiveComponent)
	using Super = UPrimitiveComponent;
public:
	USphereComp()
	{
		bCanBeRendered = true;
		RenderResource.PrimitiveType = GetType();
		RenderResource.Stride = sizeof(FPosColorNormalTex);
		RenderResource.InputLayoutType = InputLayoutType::POSCOLORNORMALTEX;
		/*RenderResource.VertexShaderIndex = 1;
		RenderResource.PixelShaderIndex = 1;
		RenderResource.bUseIndexBuffer = true;*/
		RenderResource.VertexShaderIndex = 4;
		RenderResource.PixelShaderIndex = 4;
		RenderResource.VertexConstantIndex = 4;
		RenderResource.PixelConstantIndex = 4;
		RenderResource.bUseIndexBuffer = true;
		RenderResource.ShaderResourceViewIndices.emplace().push_back(6);		// 0번째 Texture 사용 : box2.png [값이 없으면 초기화]
		// shader 관련 index 지정 필요
		
	}
	virtual ~USphereComp() = default;
	EPrimitiveType GetType() override
	{
		return EPrimitiveType::EPT_SphereTex;
	}
	void UpdateConstantData(URenderer*& Renderer) override
	{
		Super::UpdateLightConstantData(Renderer);
	}
};

class UTriangleComp : public UPrimitiveComponent
{
	DECLARE_CLASS(UTriangleComp, UPrimitiveComponent)
	using Super = UPrimitiveComponent;
public:
	UTriangleComp()
	{
		bCanBeRendered = true;
		RenderResource.PrimitiveType = GetType();
		RenderResource.Stride = sizeof(FPosColorNormalTex);
		RenderResource.InputLayoutType = InputLayoutType::POSCOLORNORMALTEX;
		/*RenderResource.VertexShaderIndex = 1;
		RenderResource.PixelShaderIndex = 1;
		RenderResource.bUseIndexBuffer = true;*/
		RenderResource.VertexShaderIndex = 4;
		RenderResource.PixelShaderIndex = 4;
		RenderResource.VertexConstantIndex = 4;
		RenderResource.PixelConstantIndex = 4;
		RenderResource.bUseIndexBuffer = true;
		//RenderResource.ShaderResourceViewIndices.emplace().push_back(0);		// 0번째 Texture 사용 : box2.png [값이 없으면 초기화]
	}
	virtual ~UTriangleComp() = default;
	EPrimitiveType GetType() override
	{
		return EPrimitiveType::EPT_TriangleTex;
	}
	void UpdateConstantData(URenderer*& Renderer) override
	{
		Super::UpdateLightConstantData(Renderer);
	}
};

class ULineComp : public UPrimitiveComponent
{
	DECLARE_CLASS(ULineComp, UPrimitiveComponent)
	using Super = UPrimitiveComponent;

public:
	ULineComp()
	{
		bCanBeRendered = true;
		RenderResource.PrimitiveType = GetType();
		RenderResource.Stride = sizeof(FPosColor);
		// shader 관련 index 지정 필요
		// 
	}
	virtual ~ULineComp() = default;
	EPrimitiveType GetType() override
	{
		return EPrimitiveType::EPT_Line;
	}
};

class UCylinderComp : public UPrimitiveComponent
{
	DECLARE_CLASS(UCylinderComp, UPrimitiveComponent)
	using Super = UPrimitiveComponent;

public:
	UCylinderComp()
	{
		bCanBeRendered = true;
		RenderResource.PrimitiveType = GetType();
		RenderResource.Stride = sizeof(FPosColorNormalTex);
		RenderResource.InputLayoutType = InputLayoutType::POSCOLORNORMALTEX;
		RenderResource.VertexShaderIndex = 1;
		RenderResource.PixelShaderIndex = 1;
		RenderResource.bUseIndexBuffer = true;
		/*RenderResource.VertexShaderIndex = 4;
		RenderResource.PixelShaderIndex = 4;
		RenderResource.VertexConstantIndex = 4;
		RenderResource.PixelConstantIndex = 4;
		RenderResource.bUseIndexBuffer = true;*/
		RenderResource.ShaderResourceViewIndices.emplace().push_back(0);		// 0번째 Texture 사용 : box2.png [값이 없으면 초기화]
		// shader 관련 index 지정 필요
	}
	virtual ~UCylinderComp() = default;
	EPrimitiveType GetType() override
	{
		return EPrimitiveType::EPT_CylinderTex;
	}
	/*void UpdateConstantData(URenderer*& Renderer) override
	{
		Super::UpdateLightConstantData(Renderer);
	}*/
};

class UConeComp : public UPrimitiveComponent
{
	DECLARE_CLASS(UConeComp, UPrimitiveComponent)
	using Super = UPrimitiveComponent;
public:
	UConeComp()
	{
		bCanBeRendered = true;
		RenderResource.PrimitiveType = GetType();
		RenderResource.Stride = sizeof(FPosColorNormalTex);
		RenderResource.InputLayoutType = InputLayoutType::POSCOLORNORMALTEX;
		/*RenderResource.VertexShaderIndex = 1;
		RenderResource.PixelShaderIndex = 1;
		RenderResource.bUseIndexBuffer = true;*/
		RenderResource.VertexShaderIndex = 4;
		RenderResource.PixelShaderIndex = 4;
		RenderResource.VertexConstantIndex = 4;
		RenderResource.PixelConstantIndex = 4;
		RenderResource.bUseIndexBuffer = true;
		RenderResource.ShaderResourceViewIndices.emplace().push_back(8);		// 0번째 Texture 사용 : box2.png [값이 없으면 초기화]
		// shader 관련 index 지정 필요
	}
	virtual ~UConeComp() = default;
	EPrimitiveType GetType() override
	{
		return EPrimitiveType::EPT_ConeTex;
	}
	void UpdateConstantData(URenderer*& Renderer) override
	{
		Super::UpdateLightConstantData(Renderer);
	}
};

class UCircleComp : public UPrimitiveComponent
{
	DECLARE_CLASS(UCircleComp, UPrimitiveComponent)
	using Super = UPrimitiveComponent;
public:
	UCircleComp()
	{
		bCanBeRendered = true;
		RenderResource.PrimitiveType = GetType();
		RenderResource.Stride = sizeof(FPosColorNormalTex);
		RenderResource.InputLayoutType = InputLayoutType::POSCOLORNORMALTEX;
		/*RenderResource.VertexShaderIndex = 1;
		RenderResource.PixelShaderIndex = 1;
		RenderResource.bUseIndexBuffer = true;*/
		RenderResource.VertexShaderIndex = 4;
		RenderResource.PixelShaderIndex = 4;
		RenderResource.VertexConstantIndex = 4;
		RenderResource.PixelConstantIndex = 4;
		RenderResource.bUseIndexBuffer = true;

		RenderResource.ShaderResourceViewIndices.emplace().push_back(0);		// 0번째 Texture 사용 : box2.png [값이 없으면 초기화]
		// shader 관련 index 지정 필요
	}
	virtual ~UCircleComp() = default;
	EPrimitiveType GetType() override
	{
		return EPrimitiveType::EPT_CircleTex;
	}
	void UpdateConstantData(URenderer*& Renderer) override
	{
		Super::UpdateLightConstantData(Renderer);
	}
};

class UBillBoardComp : public UPrimitiveComponent
{
	DECLARE_CLASS(UBillBoardComp, UPrimitiveComponent)
	using Super = UPrimitiveComponent;
public:
	UBillBoardComp()
	{
		bCanBeRendered = true;
		RenderResource.PrimitiveType = GetType();
		RenderResource.Stride = sizeof(FPosColorNormalTex);
		RenderResource.InputLayoutType = InputLayoutType::POSCOLORNORMALTEX;
		RenderResource.VertexShaderIndex = 1;
		RenderResource.PixelShaderIndex = 1;
		RenderResource.bUseIndexBuffer = true;
		RenderResource.ShaderResourceViewIndices.emplace().push_back(7);		// 1번째 Texture 사용 : tree.png [값이 없으면 초기화]
		// shader 관련 index 지정 필요
		//
	}
	virtual ~UBillBoardComp() = default;
	EPrimitiveType GetType() override
	{
		return EPrimitiveType::EPT_BillBoard;
	}

	virtual void SetUseBillboardUtil(bool bUse) override;

	void UpdateConstantData(URenderer*& Renderer) override;
};



class UCharComp : public UPrimitiveComponent 
{
	DECLARE_CLASS(UCharComp, UPrimitiveComponent)
	using Super = UPrimitiveComponent;
public:
	UCharComp() 
	{
		bCanBeRendered = true;
		RenderResource.PrimitiveType = GetType();
		RenderResource.Stride = sizeof(FPosColorNormalTex);
		RenderResource.InputLayoutType = InputLayoutType::POSCOLORNORMALTEX;
		RenderResource.VertexShaderIndex = 2;				// 2 : Atlas Vertex Shader
		RenderResource.PixelShaderIndex = 2;				// 2 : Atlas Pixel Shader		
		RenderResource.VertexConstantIndex = 3;				// 3 : Atlas Vertex Shader Constant Buffer		
		RenderResource.PixelConstantIndex = -1;				// -1 : [No] PS CBuffer		
		RenderResource.bUseIndexBuffer = true;						
		RenderResource.ShaderResourceViewIndices.emplace().push_back(1);	// TextAtlas 추가 필요
	}

	virtual ~UCharComp() = default;
	EPrimitiveType GetType() override
	{
		return EPrimitiveType::EPT_WorldText;
	}

private:
	char Character = 'a';
	std::string AtlasName = "";

public:
	void SetChar(char InCharacter, std::string InAtlasName) { Character = InCharacter; AtlasName = InAtlasName; }
	char GetChar() const { return Character; }
	FAtlasConstants AtlasConstantData;

	void UpdateConstantData(URenderer*& Renderer) override;
};

class UBoundingBoxComp : public UPrimitiveComponent {
	DECLARE_CLASS(UBoundingBoxComp, UPrimitiveComponent)
	using Super = UPrimitiveComponent;

public:
	UBoundingBoxComp()
	{
		bCanBeRendered = true;
		RenderResource.PrimitiveType = GetType();
		RenderResource.Stride = sizeof(FPosColor);
		RenderResource.InputLayoutType = InputLayoutType::POSCOLOR;
		RenderResource.VertexShaderIndex = 1;
		RenderResource.PixelShaderIndex = 1;
		RenderResource.bUseIndexBuffer = true;
	}

	virtual ~UBoundingBoxComp() = default;
	EPrimitiveType GetType() override
	{
		return EPrimitiveType::EPT_BoundingBox;
	}

	//void UpdateConstantData(URenderer*& Renderer) override;
	//void Render() override;
};

class UWorldGridComp : public UPrimitiveComponent
{
	DECLARE_CLASS(UWorldGridComp, UPrimitiveComponent)
	using Super = UPrimitiveComponent;
public:
	UWorldGridComp()
	{
		bCanBeRendered = true;
		RenderResource.PrimitiveType = GetType();
		RenderResource.Stride = sizeof(FVertexSimple);
		RenderResource.InputLayoutType = InputLayoutType::POSCOLOR;
		RenderResource.VertexShaderIndex = 0;
		RenderResource.PixelShaderIndex = 0;
		RenderResource.VertexConstantIndex = 0;
		RenderResource.PixelConstantIndex = -1;
		RenderResource.bUseIndexBuffer = false;
		//RenderResource.ShaderResourceViewIndices.emplace().push_back(0);	// TextAtlas 추가 필요
	}

	virtual ~UWorldGridComp() = default;
	EPrimitiveType GetType() override
	{
		return EPrimitiveType::EPT_WORLDGRID;
	}

	void UpdateConstantData(URenderer*& Renderer) override;
};

class USubUVComponent : public UPrimitiveComponent
{
	DECLARE_CLASS(USubUVComponent, UPrimitiveComponent)
	using Super = UPrimitiveComponent;
public:
	USubUVComponent()
	{
		bCanBeRendered = true;
		RenderResource.PrimitiveType = GetType();
		RenderResource.Stride = sizeof(FPosColorNormalTex);
		RenderResource.InputLayoutType = InputLayoutType::POSCOLORNORMALTEX;
		RenderResource.VertexShaderIndex = 2;				// 2 : Atlas Vertex Shader
		RenderResource.PixelShaderIndex = 5;				// 2 : Atlas Pixel Shader		
		RenderResource.VertexConstantIndex = 3;				// 3 : Atlas Vertex Shader Constant Buffer		
		RenderResource.PixelConstantIndex = -1;				// -1 : [No] PS CBuffer		
		RenderResource.bUseIndexBuffer = true;
	}

	virtual ~USubUVComponent() = default;
	EPrimitiveType GetType() override
	{
		return EPrimitiveType::EPT_SubUV;
	}

	void AddTextureID(uint32 TextureID) {
		RenderResource.ShaderResourceViewIndices.emplace().push_back(TextureID);	// 
	}

private:
	char Frame = 0;
	std::string AtlasName = "";

public:
	void SetFrame(int InFrame, std::string InAtlasName) { Frame = InFrame; AtlasName = InAtlasName; }
	int32 GetFrame() const { return Frame; }
	FAtlasConstants AtlasConstantData;

	void UpdateConstantData(URenderer*& Renderer) override;
};