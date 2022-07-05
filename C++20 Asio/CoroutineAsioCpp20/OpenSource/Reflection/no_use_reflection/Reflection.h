#pragma once

// Ό³Έν
// https://www.slideshare.net/xtozero/c20-251161090
// https://github.com/xtozero/SSR/tree/multi-thread/Source/Core/Public/Reflection
// https://github.com/xtozero/SSR/tree/multi-thread/Source/Core/Private/Reflection


#include "MethodMacros.h"
#include "PropertyMacros.h"
#include "TypeInfoMacros.h"

template <typename To, typename From>
To* Cast(From* src)
{
	return src && src->GetTypeInfo().IsChildOf<To>() ? reinterpret_cast<To*>(src) : nullptr;
}