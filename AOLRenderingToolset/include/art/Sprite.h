#pragma once

#include <vector>
#include <string>

#include "Animation.h"

#define ART_SPRITE_ANIMATION 1
#define ART_SPRITE_IMAGE 2

namespace art
{
	struct SpriteLayer
	{
		char type = 0;
		Animation* animation = 0;
		ArtImage* image = 0;
		DrawData d;
	};

	class Sprite
	{
	private:
		std::vector<SpriteLayer> layers;

	public:
		Sprite(std::vector<SpriteLayer> layers_);
		~Sprite();

		void add_layer(const SpriteLayer& layer);
		void add_layer(Animation* layer, const DrawData& d = {});
		void add_layer(ArtImage* layer, const DrawData& d = {});

		SpriteLayer* get_layer(int layer);

		void update(double dt);
		void draw(const DrawData& d = {});
	};
}