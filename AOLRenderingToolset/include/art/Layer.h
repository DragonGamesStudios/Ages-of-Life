#pragma once
#include <agl/GuiGroup.h>

namespace art
{
	class Layer
	{
	private:
		agl::GuiGroup* guigroup;
		unsigned char index;
		bool visible;

	public:
		Layer(unsigned char index_);
		~Layer();

		unsigned char get_index() const;

		void connect_guigroup(agl::GuiGroup* group);
		void draw();
		void update(double dt);

		void set_visibility(bool vis);
		bool is_visible() const;
	};
}