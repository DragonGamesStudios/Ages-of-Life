#include "..\include\art\Layer.h"

namespace art
{
	Layer::Layer(unsigned char index_)
	{
		guigroup = 0;
		visible = false;
		index = 0;
	}

	Layer::~Layer()
	{
		if (guigroup)
			delete guigroup;
	}

	unsigned char Layer::get_index() const
	{
		return index;
	}

	void Layer::connect_guigroup(agl::GuiGroup* group)
	{
		guigroup = group;
	}

	void Layer::draw()
	{
		guigroup->draw();
	}

	void Layer::update(double dt)
	{
		guigroup->update();
	}

	void Layer::set_visibility(bool vis)
	{
		visible = vis;
	}

	bool Layer::is_visible() const
	{
		return visible;
	}

}