#include "pch.h"
#include "Object.h"

Object::Object()
{
    DoPreInit();
    DoInit();
    DoPostInit();
}

bool Object::DoPreInit()
{
    if (true == isPreInited)
    {
        return false;
    }

    return true;
}

bool Object::DoInit()
{
    if (true == isInited)
    {
        return false;
    }

    return true;
}

bool Object::DoPostInit()
{
    if (true == isPostInited)
    {
        return false;
    }

    return true;
}
