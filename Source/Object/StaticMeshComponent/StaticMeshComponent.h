#pragma once
#include "MeshComponent.h"
#include "Object/ObjectMacro.h"

/*
* 특정 Actor에 부착되어 메시 렌더링을 담당하는 컴포넌트
* @UStaticMesh : 에셋 참조 - FObjManager를 통해 에셋 로드
*/

class UStaticMeshComponent :
    public UMeshComponent
{
	DECLARE_CLASS(UStaticMeshComponent, UMeshComponent)

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void Render() override;


};

