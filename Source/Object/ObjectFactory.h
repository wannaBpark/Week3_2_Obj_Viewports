#pragma once
#include "Core/Engine.h"
#include "Core/EngineStatics.h"
#include "Core/HAL/PlatformMemory.h"
#include "Debug/DebugConsole.h"
#include "Core/HAL//StackAllocator.h"
#include "Object/UClass.h"
#include "Core/Container/String.h"
#include "Core/Container/ObjectHash.h"

class UObject;

class FObjectFactory
{
public:
    /**
     * UObject를 생성합니다.
     * @tparam T UObject를 상속받은 클래스
     * @return 캐스팅된 UObject*
     */
    template<typename T>
        requires std::derived_from<T, UObject>
    static T* ConstructObject()
    {
        UE_LOG("DEBUG: Construct %s Object", typeid(T).name());
        constexpr size_t ObjectSize = sizeof(T);
        //void* RawMemory = FPlatformMemory::Malloc<EAT_Object>(ObjectSize);            // 기존 할당 방식 : 생성시마다 malloc
        //void* RawMemory = (void*)FPlatformMemory::Malloc<T, EAT_Object>(ObjectSize);    // StackAllocator : 이미 malloc 완료, 일부분을 떼어줌

        //T* ObjectPtr = new (RawMemory) T();
        T* ObjectPtr = FPlatformMemory::Malloc<T, EAT_Object>(ObjectSize);
        std::shared_ptr<T> NewObject(ObjectPtr, [ObjectSize](T* Obj)
        {
            Obj->~T();
            //FPlatformMemory::Free<EAT_Object>(Obj, ObjectSize);
			FPlatformMemory::DecrementObjectStats(ObjectSize);          // 스탯 감소 함수 호출
			StackAllocator::GetInstance().deleteNode(Obj);              // 삭제 시 Stack Allocator 해당 메모리도 해제
        });
        static uint32 instanceID = 0;
        NewObject->UUID = UEngineStatics::GenUUID();
        UClass* ClassInfo = T::StaticClass();
        NewObject->Name = FName(ClassInfo->GetName() + "_" + FString::FromInt(instanceID++));
        auto tmp = NewObject->Name.ToString();
        UE_LOG("%s ", *tmp);
        NewObject->ClassType = ClassInfo;
        ///////////////////////////////////////////////////////////////////////////!!!!!!!!!!!!!!!!!!!!!!! TODO : FName = 오브젝트 이름 + id 붙여 반영
        // s 
        // NewObject->Name = #Object + " " + FString::FromInt(NewObject->UUID);

        // Object 제거시 Index가 달라지기 때문에 임시 주석처리 <- RemoveSwap으로 해결 가능
        // NewObject->InternalIndex = UEngine::Get().GObjects.Add(NewObject);
        UEngine::Get().GObjects.Add(NewObject->GetUUID(), NewObject);
        AddToHashMap(NewObject);

        return NewObject.get();
    }
};
