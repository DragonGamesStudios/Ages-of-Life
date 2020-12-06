
#include "../Block.h"

namespace agl::builtins
{

    class Flow : public Block
    {
    private:
        int main_axis_spacing;
        int second_axis_spacing;
        Point resize_point_topleft;
        Point resize_point_bottomright;
        char main_axis_align;
        char second_axis_align;
        bool single;

        char main_axis_direction;

        void layout_children();

        void on_children_changed(Event e);
    public:
        void resize_to_content();
        void set_main_axis_spacing(int spacing);
        void set_second_axis_spacing(int spacing);
        void set_main_axis_align(char align);
        void set_second_axis_align(char align);

        void set_single_subflow(bool value);
        void set_main_axis(char axis);

        virtual void add(Block* child);
        virtual void apply(Style* style);
        virtual void set_size(int width, int height);

        Flow();
    };

}