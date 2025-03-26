#include "ObjectHash.h"
#include <cassert>
#include "Object/UObject.h"
#include "Object/UClass.h"
#include "Core/Container/Map.h"
#include "Core/Container/Set.h"

/**
 * 모든 UObject의 정보를 담고 있는 HashTable
 */
struct FUObjectHashTables
{
    static FUObjectHashTables& Get()
    {
        static FUObjectHashTables Singleton;
        return Singleton;
    }

    TMap<UClass*, TSet<UClass*>> ClassToChildListMap;
    TMap<UClass*, TSet<UObject*>> ClassToObjectListMap;
};

/** Helper function that returns all the children of the specified class recursively */
template <typename ClassType, typename ArrayAllocator>
static void RecursivelyPopulateDerivedClasses(FUObjectHashTables& ThreadHash, const UClass* ParentClass, TArray<ClassType, ArrayAllocator>& OutAllDerivedClass)
{
    // Start search with the parent class at virtual index Num-1, then continue searching from index Num as things are added
    int32 SearchIndex = OutAllDerivedClass.Num() - 1;
    const UClass* SearchClass = ParentClass;

    while (true)
    {
        if (TSet<UClass*>* ChildSet = ThreadHash.ClassToChildListMap.Find(const_cast<UClass*>(SearchClass)))
        {
            OutAllDerivedClass.Reserve(OutAllDerivedClass.Num() + ChildSet->Num());
            for (UClass* ChildClass : *ChildSet)
            {
                OutAllDerivedClass.Add(ChildClass);
            }
        }

        // Now search at next index, if it has been filled in by code above
        ++SearchIndex;

        if (SearchIndex < OutAllDerivedClass.Num())
        {
            SearchClass = OutAllDerivedClass[SearchIndex];
        }
        else
        {
            return;
        }
    }
}

void AddToClassMap(UObject* Object)
{
    assert(Object->GetClass());
    FUObjectHashTables& HashTable = FUObjectHashTables::Get();

    UClass* Class = Object->GetClass();
    HashTable.ClassToObjectListMap.FindOrAdd(Class).Add(Object);

    for (UClass* SuperClass = Class->GetParentClass(); SuperClass;)
    {
        HashTable.ClassToChildListMap.FindOrAdd(SuperClass).Add(Class);

        Class = SuperClass;
        SuperClass = SuperClass->GetParentClass();
    }
}

void RemoveFromClassMap(UObject* Object)
{
    assert(Object->GetClass());
    FUObjectHashTables& HashTable = FUObjectHashTables::Get();

    TSet<UObject*>& ObjectSet = HashTable.ClassToObjectListMap.FindOrAdd(Object->GetClass());
    int32 NumRemoved = ObjectSet.Remove(Object);

    if (NumRemoved == 0)
    {
        HashTable.ClassToObjectListMap.Remove(Object->GetClass());
    }
}

void GetObjectsOfClass(const UClass* ClassToLookFor, TArray<UObject*>& Results, bool bIncludeDerivedClasses)
{
    // Most classes searched for have around 10 subclasses, some have hundreds
    TArray<const UClass*> ClassesToSearch;
    ClassesToSearch.Add(ClassToLookFor);

    FUObjectHashTables& ThreadHash = FUObjectHashTables::Get();

    if (bIncludeDerivedClasses)
    {
        RecursivelyPopulateDerivedClasses(ThreadHash, ClassToLookFor, ClassesToSearch);
    }


    for (const UClass* SearchClass : ClassesToSearch)
    {
        if (TSet<UObject*>* List = ThreadHash.ClassToObjectListMap.Find(const_cast<UClass*>(SearchClass)))
        {
            for (const auto& Object : *List)
            {
                Results.Add(Object);
            }
        }
    }
}
