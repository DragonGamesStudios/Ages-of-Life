#pragma once

#include "ArtImage.h"

#include <vector>

namespace art
{
	class Atlas
	{
	private:
		const ArtImage* image;

	public:
		Atlas();
		~Atlas();

		void set_image(const ArtImage* image_);

		template<class ArtImageType, std::enable_if_t<std::is_base_of_v<ArtImage, ArtImageType> && !std::is_same_v<ArtImage, ArtImageType>, int> = 0>
		ArtImage* get_image(float x, float y, float w, float h);

		template<class ArtImageType, std::enable_if_t<std::is_base_of_v<ArtImage, ArtImageType> && !std::is_same_v<ArtImage, ArtImageType>, int> = 0>
		std::vector<ArtImage*> get_image_set(float x, float y, float frame_w, float frame_h, int row_size, float advance_x, float advance_y, int count);
	};

	template<class ArtImageType, std::enable_if_t<std::is_base_of_v<ArtImage, ArtImageType> && !std::is_same_v<ArtImage, ArtImageType>, int>>
	inline ArtImage* Atlas::get_image(float x, float y, float w, float h)
	{
		return new ArtImageType(image, x, y, w, h);
	}

	template<class ArtImageType, std::enable_if_t<std::is_base_of_v<ArtImage, ArtImageType> && !std::is_same_v<ArtImage, ArtImageType>, int>>
	inline std::vector<ArtImage*> Atlas::get_image_set(float x, float y, float frame_w, float frame_h, int row_size, float advance_x, float advance_y, int count)
	{
		std::vector<ArtImage*> images;

		float cx = x;
		float cy = y;

		int img = 0;
		
		for (int i = 0; i < count; i++)
		{
			images.push_back(get_image<ArtImageType>(cx, cy, frame_w, frame_h));
			
			cx += frame_w + advance_x;
			img++;

			if (img == row_size)
			{
				img = 0;
				cx = x;
				cy += frame_h + advance_y;
			}
		}

		return images;
	}
}