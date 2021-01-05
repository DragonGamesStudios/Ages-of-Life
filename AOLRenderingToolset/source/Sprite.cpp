#include "..\include\art\Sprite.h"

namespace art
{
	Sprite::Sprite(std::vector<SpriteLayer> layers_)
	{
		layers = layers_;
	}

	Sprite::~Sprite()
	{
		for (auto& layer : layers)
		{
			if (layer.type == ART_SPRITE_ANIMATION)
				delete layer.animation;
			else if (layer.type == ART_SPRITE_IMAGE)
				delete layer.image;
		}
	}

	void Sprite::add_layer(const SpriteLayer& layer)
	{
		layers.push_back(layer);
	}

	void Sprite::add_layer(Animation* layer, const DrawData& d)
	{
		DrawData d_ = d;
		d_.ox = (float)layer->get_width() / 2;
		d_.oy = (float)layer->get_height() / 2;

		layers.push_back({ .type = ART_SPRITE_ANIMATION, .animation = layer, .d = d });
	}

	void Sprite::add_layer(ArtImage* layer, const DrawData& d)
	{
		DrawData d_ = d;
		d_.ox = (float)layer->width / 2;
		d_.oy = (float)layer->height / 2;

		layers.push_back({ .type = ART_SPRITE_IMAGE, .image = layer, .d = d });
	}

	SpriteLayer* Sprite::get_layer(int layer)
	{
		return &layers[layer];
	}

	void Sprite::update(double dt)
	{
		for (const auto& layer : layers)
			if (layer.type == ART_SPRITE_ANIMATION)
				layer.animation->update(dt);
	}

	void Sprite::draw(const DrawData& d)
	{
		for (const auto& layer : layers)
		{
			Color merged_tint = {
				(unsigned char)(((int)layer.d.tint.r * (int)d.tint.r) / 255),
				(unsigned char)(((int)layer.d.tint.g * (int)d.tint.g) / 255),
				(unsigned char)(((int)layer.d.tint.b * (int)d.tint.b) / 255),
				(unsigned char)(((int)layer.d.tint.a * (int)d.tint.a) / 255)
			};

			DrawData d_{
				layer.d.dx + d.dx,
				layer.d.dy + d.dy,
				layer.d.sx * d.sx,
				layer.d.sy * d.sy,
				layer.d.r + d.r,
				layer.d.ox + d.ox,
				layer.d.oy + d.oy,
				merged_tint
			};

			if (layer.type == ART_SPRITE_ANIMATION)
				layer.animation->draw(d_);
			else if (layer.type == ART_SPRITE_IMAGE)
				layer.image->draw(d_);
		}
	}

}