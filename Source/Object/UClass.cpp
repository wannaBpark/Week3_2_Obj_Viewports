#include "UClass.h"
#include <cassert>

UClass::UClass(const char* InName, UClass* InParentClass)
	: ParentClass(InParentClass) 
{ 
	Name = InName;
}

UClass::~UClass() 
{

}

bool UClass::IsChildOf(const UClass* OtherClass) const
{
	assert(this);
	if (!OtherClass) return false;

	if (this == OtherClass)
	{
		return true;
	}

	if (ParentClass != nullptr) 
	{
		return ParentClass->IsChildOf(OtherClass);
	}
	return false;
}