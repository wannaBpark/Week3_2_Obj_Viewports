#pragma once
#include "Actor.h"
#include "Core/Container/Array.h"
#include <string>
#include "Object/ObjectMacro.h"
#include "Object/UtilComponent/UStringComponent.h"

class UStringComponent;

class AWorldText : public AActor
{
    DECLARE_CLASS(AWorldText, AActor)
    using Super = AActor;
    
public:
    AWorldText();
    virtual ~AWorldText() = default;
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;
    virtual const char* GetTypeName() override;

    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    void ClearCharComps();
    void SetCharComps(std::string InText, std::string InAtlasName);
    
    std::string GetString();

    float GetLetterSpacing();
    void SetLetterSpacing(float InLetterSpacing);

    void SetActive(bool bActive);
private:
    UStringComponent* StringComponent;
    bool bIsActive = false;

public:
	void SetUseBillboardUtil(bool bUse);
	bool IsUseBillboardUtil() const { return StringComponent->IsUseBillboardUtil(); }

private:

};
