#pragma once

#include "Core/HAL/PlatformMemory.h"

/**
 * TRemoveReference<type> will remove any references from a type.
 */
template <typename T> struct TRemoveReference { typedef T Type; };
template <typename T> struct TRemoveReference<T& > { typedef T Type; };
template <typename T> struct TRemoveReference<T&&> { typedef T Type; };
