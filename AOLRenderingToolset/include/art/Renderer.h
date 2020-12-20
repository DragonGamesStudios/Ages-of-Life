#pragma once
#include "Layer.h"

namespace art
{
	struct LayerIndexComparator
	{
		bool operator() (const Layer* l1, const Layer* l2) const;
	};

	class Renderer
	{
	private:
		std::set<Layer*, LayerIndexComparator> layers;

	public:
		Renderer();
		~Renderer();

		void add_layer(Layer* layer);
		void update(double dt);
		void draw();
	};
}