#pragma once
#include "../Block.h"

namespace agl::builtins
{
    class Layout : public Block
    {
    protected:
        virtual void layout_children() = 0;
    public:
        virtual void add(Block* child);
    };
}