#pragma once
#include "../Block.h"

namespace agl::builtins
{
    class Flow : public Block
    {
    protected:
        virtual void layout_children() = 0;
    };
}