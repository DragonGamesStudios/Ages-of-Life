#include "..\include\art\Renderer.h"

namespace art
{
	bool LayerIndexComparator::operator()(const Layer* l1, const Layer* l2) const
	{
		return l1->get_index() < l2->get_index();
	}

	Renderer::Renderer()
	{
	}

	Renderer::~Renderer()
	{
		for (auto layer : layers)
		{
			delete layer;
		}
	}

	void Renderer::add_layer(Layer* layer)
	{
		layers.insert(layer);
	}

	void Renderer::update(double dt)
	{
		for (const auto& layer : layers)
		{
			layer->update(dt);
		}
	}

	void Renderer::draw()
	{
		for (const auto& layer : layers)
		{
			layer->draw();
		}
	}

}