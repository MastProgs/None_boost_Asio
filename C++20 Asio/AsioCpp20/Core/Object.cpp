#include "stdafx.h"
#include "Object.h"

bool Object::DoPreInit()
{
    if (true == isPreInited)
    {
        return true;
    }

    return false;
}

bool Object::DoInit()
{
    if (true == isInited)
    {
        return true;
    }

    return false;
}

bool Object::DoPostInit()
{
    if (true == isPostInited)
    {
        return true;
    }

    return false;
}
