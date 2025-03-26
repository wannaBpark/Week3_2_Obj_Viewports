#pragma once
#include "functional"
#include "Core/Container/String.h"
#include "Core/Container/CString.h"
#include "Object/NameTypes.h"
#include "Core/Container/Array.h"
#include "Core/Container/Map.h"
#include "Core/Container/Set.h"
#include "Core/HAL/PlatformType.h"
#include "Core/EngineTypes.h"

class UClass;
class UObject;

/** FUObjectHashTables에 Object의 정보를 저장합니다. */
void AddToHashMap(UObject* Object);

/** FUObjectHashTables에 저장된 Object정보를 제거합니다. */
void RemoveFromHashMap(UObject* Object);

/**
특정 클래스의 객체 배열을 반환합니다. 선택적으로, 결과에 파생 클래스의 객체도 포함될 수 있습니다.
@param ClassToLookFor 반환할 객체들의 클래스.
@param Results 지정된 클래스의 객체들을 담을 출력 리스트.
@param bIncludeDerivedClasses true인 경우, 자식 클래스의 객체들도 결과에 포함됩니다.
@param AdditionalExcludeFlags 이 플래그들 중 하나라도 가진 객체는 결과에서 제외됩니다.
@param ExclusiveInternalFlags 반환할 객체를 필터링하는 데 사용할 내부 플래그들을 지정합니다.
*/
void GetObjectsOfClass(const UClass* ClassToLookFor, TArray<UObject*>& Results, bool bIncludeDerivedClasses = true, EObjectFlags ExcludeFlags = RF_ClassDefaultObject, EInternalObjectFlags ExclusionInternalFlags = EInternalObjectFlags::None);

/**
 * 지정된 클래스의 객체들을 순회하며 각 객체에 대해 Operation을 실행합니다.
 *
 * @tparam Func 호출 가능한 객체(람다, 함수 포인터 등)의 타입.
 * @param ClassToLookFor 찾고자 하는 객체의 기준 클래스.
 * @param Operation 각 객체에 대해 실행할 호출 가능한 객체.
 * @param bIncludeDerivedClasses true이면, 기준 클래스의 파생 클래스 객체들도 순회합니다.
 * @param ExcludeFlags 추가로 제외할 UObject 플래그.
 * @param ExclusionInternalFlags 제외에 사용할 내부 객체 플래그.
 */
template<typename Func>
void ForEachObjectOfClass(const UClass* ClassToLookFor, Func Operation, bool bIncludeDerivedClasses = true, EObjectFlags ExcludeFlags = RF_ClassDefaultObject, EInternalObjectFlags ExclusionInternalFlags = EInternalObjectFlags::None);

/**
 * 지정된 여러 클래스의 객체들을 순회하며 각 객체에 대해 Operation을 실행합니다.
 *
 * @tparam Func 호출 가능한 객체(람다, 함수 포인터 등)의 타입.
 * @param ClassesToLookFor 찾고자 하는 객체들의 기준 클래스 배열.
 * @param Operation 각 객체에 대해 실행할 호출 가능한 객체.
 * @param ExcludeFlags 추가로 제외할 UObject 플래그.
 * @param ExclusionInternalFlags 제외에 사용할 내부 객체 플래그.
 */
template<typename Func>
void ForEachObjectOfClasses(TArray<const UClass* const> ClassesToLookFor, Func Operation, EObjectFlags ExcludeFlags = RF_ClassDefaultObject, EInternalObjectFlags ExclusionInternalFlags = EInternalObjectFlags::None);

/**
 * 지정된 클래스의 파생 클래스들을 검색하여 Results 배열에 저장합니다.
 *
 * @param ClassToLookFor 검색 기준이 되는 클래스.
 * @param Results 검색된 파생 클래스들을 담을 출력 배열.
 * @param bRecursive true이면, 재귀적으로 모든 파생 클래스들을 검색합니다.
 */
void GetDerivedClasses(const UClass* ClassToLookFor, TArray<UClass*>& Results, bool bRecursive = true);

/**
 * 모든 클래스의 파생 클래스 관계를 분석하여, 각 클래스별 파생 클래스 집합을 반환합니다.
 *
 * @return 각 UClass*를 키로, 해당 클래스의 파생 클래스들을 담은 TSet<UClass*>를 값으로 갖는 TMap.
 */
TMap<UClass*, TSet<UClass*>> GetAllDerivedClasses();

/**
 * UObject 해시 테이블들의 메모리 사용량을 최적화하기 위해 압축(메모리 축소) 작업을 수행합니다.
 */
void ShrinkUObjectHashTables();
