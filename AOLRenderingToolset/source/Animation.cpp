#include "..\include\art\Animation.h"

namespace art
{
	Animation::Animation(const std::vector<ArtImage*>& frames_, int frame_count_, int speed, bool start_immediately)
	{
		frames = frames_;
		playing = start_immediately;

		fps = speed;
		frame_count = frame_count_;

		repetitions = 0;
		played_repetitions = 0;
		current_frame = 0;
	}

	void Animation::reset()
	{
		played_repetitions = 0;
		current_frame = 0;
	}

	void Animation::set_playing_state(bool state)
	{
		playing = state;
	}

	void Animation::set_repetitions(int repetitions_)
	{
		repetitions = repetitions_;
	}

	void Animation::update(double dt)
	{
		if (playing)
		{
			current_frame += dt * fps;

			if (int(current_frame) >= frame_count)
			{
				played_repetitions++;
				current_frame = 0;
			}

			if (repetitions && played_repetitions == repetitions)
				playing = false;
		}
	}

	void Animation::draw(const DrawData& d)
	{
		frames[int(current_frame)]->draw(d);
	}

	int Animation::get_width() const
	{
		return frames[0]->width;
	}

	int Animation::get_height() const
	{
		return frames[0]->width;
	}

}