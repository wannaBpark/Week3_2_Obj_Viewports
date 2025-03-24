#pragma once
#include "Actor.h"
#include "Object/ObjectMacro.h"

class ATarzan :
    public AActor
{
	DECLARE_CLASS(ATarzan, AActor)
	using Super = AActor;
public:
    ATarzan();
    virtual ~ATarzan() = default;
};

