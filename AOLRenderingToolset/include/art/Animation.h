#pragma once

#include "ArtImage.h"

#include <vector>

namespace art
{
	class Animation
	{
	private:
		std::vector<ArtImage*> frames;

		int fps;

		int played_repetitions;
		int repetitions;
		bool playing;
		int frame_count;

		double current_frame;
		
	public:
		Animation(const std::vector<ArtImage*>& frames_, int frame_count_, int speed, bool start_immediately = false);

		void reset();
		void set_playing_state(bool state);

		void set_repetitions(int repetitions_);

		void update(double dt);
		void draw(const DrawData& d = {});

		int get_width() const;
		int get_height() const;
	};
}