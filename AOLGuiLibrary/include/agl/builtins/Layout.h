#pragma once
#include "agl/Block.h"

namespace agl::builtins
{
    class Flow : public Block
    {
    protected:
        virtual void layout_children() = 0;
    };
}