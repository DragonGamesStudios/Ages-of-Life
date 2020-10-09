#include "Proto.h"
#include <iostream>
#include <assert.h>
#include <sstream>
#include <fstream>
#include <stdarg.h>
#include <fmt/format.h>
#include <algorithm>
#include <filesystem>
#include <ShlObj.h>
#include <Shlwapi.h>

namespace fs = std::filesystem;

#ifndef max
#define max std::max;
#endif // !max


std::vector<ALLEGRO_BITMAP*> loaded_bitmaps = {};
std::vector<ALLEGRO_FONT*> loaded_fonts = {};

Proto::Proto()
{
	display = NULL;
	queue = NULL;
	timer = NULL;
	al_get_mouse_state(&mouse);
	al_get_keyboard_state(&keyboard);

	lastTime = al_get_time();

	loaded_images = {};
	registered_buttons = {};
	registered_labels = {};
	shouldClose = false;

	this->keyboard_callback = NULL;
}

Proto::~Proto()
{
	//this->wyjdz();
}

std::vector<std::vector<Quad>> Proto::loadSpritesheet(int rows, int rowlength, int quadwidth, int quadheight, int offsetX, int offsetY, int intervalX, int intervalY)
{
	std::vector<std::vector<Quad>> result;
	int y = offsetY;
	for (int row = 0; row < rows; row++) {
		result.push_back(Proto::loadSpritesheet(rowlength, quadwidth, quadheight, offsetX, y, intervalX));
		y += intervalY + quadheight;
	}

	return result;
}

std::vector<Quad> Proto::loadSpritesheet(int rowlength, int quadwidth, int quadheight, int offsetX, int offsetY, int interval)
{
	std::vector<Quad> result;

	int x = offsetX;
	
	for (int cell = 0; cell < rowlength; cell++) {
		result.push_back(Quad{ x, offsetY, quadwidth, quadheight });

		x += interval + quadwidth;
	}

	return result;
}

void Proto::createWindow(int width, int height, ALLEGRO_BITMAP* icon, const char* title, int flags)
{
	if ((flags & PROTO_WINDOW_FULLSCREEN) == PROTO_WINDOW_FULLSCREEN) {
		al_set_new_display_flags(ALLEGRO_FULLSCREEN);
		ALLEGRO_MONITOR_INFO info;
		int display_width, display_height;
		int i = 0;
		do {
			al_get_monitor_info(i++, &info);
		} while (!(info.x1 == 0 && info.y1 == 0));
		display_width = info.x2 - info.x1;
		display_height = info.y2 - info.y1;
		width = display_width;
		height = display_height;
	}
	this->display = al_create_display(width, height);
	al_set_window_title(this->display, title);

	if (icon != NULL) {
		al_set_display_icon(this->display, icon);
	}

	this->timer = al_create_timer(1 / 60.f);

	this->queue = al_create_event_queue();

	al_get_mouse_state(&this->mouse);

	al_register_event_source(this->queue, al_get_display_event_source(this->display));
	al_register_event_source(this->queue, al_get_timer_event_source(this->timer));
	al_register_event_source(this->queue, al_get_mouse_event_source());
	al_register_event_source(this->queue, al_get_keyboard_event_source());

	al_start_timer(this->timer);
}

void Proto::updateButton(Button* btn)
{
	btn->setMousePos(this->mouse.x, this->mouse.y);
	btn->mreleased = this->mousereleased;
}

void Proto::loadDictionary(fs::path path)
{
	std::ifstream i(path);
	i >> this->dictionary;
	i.close();
}

std::string Proto::dict(std::string value)
{
	std::string res = "Unknown key: " + value;
	if (this->dictionary.find(value) != this->dictionary.end())
		res = this->dictionary[value];

	return res;
}

std::pair<float, float> Proto::getScale(Image img, int w, int h)
{
	return std::make_pair((float)w/(float)img.width, (float)h/(float)img.height);
}

std::pair<float, float> Proto::getScale(Image* img, int w, int h)
{
	return std::make_pair((float)w / (float)img->width, (float)h / (float)img->height);
}

std::pair<int, int> Proto::getWindowDimensions()
{
	int w = al_get_display_width(this->display);
	int h = al_get_display_height(this->display);

	return std::make_pair(w, h);
}

void Proto::quit()
{
	//for (int i=0; i<this->loaded_images.size();i++) if (this->loaded_images[i]->image) al_destroy_bitmap(this->loaded_images[i]->image);
	for (auto bitmap = loaded_bitmaps.begin(); bitmap != loaded_bitmaps.end(); bitmap++) al_destroy_bitmap(*bitmap);
	for (auto font = loaded_fonts.begin(); font != loaded_fonts.end(); font++) al_destroy_font(*font);
	if (this->display) al_destroy_display(this->display);
	if (this->queue) al_destroy_event_queue(this->queue);
}

void Proto::close() {
	this->shouldClose = true;
}

std::pair <bool, bool> Proto::update()
{
	ALLEGRO_EVENT event;
	bool ticked = false;
	al_wait_for_event(this->queue, &event);
	if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE || this->shouldClose) {
		this->quit();
		return std::make_pair(false, false);
	}
	else if (event.type == ALLEGRO_EVENT_TIMER) {
		ticked = true;
	}
	else if (event.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP) {
		this->mousereleased = true;
	}
		
	if (event.type == ALLEGRO_EVENT_MOUSE_AXES || event.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP || event.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
		al_get_mouse_state(&mouse);
	}

	if (event.type == ALLEGRO_EVENT_KEY_UP || event.type == ALLEGRO_EVENT_KEY_DOWN) {
		if (this->keyboard_callback) this->keyboard_callback(event.keyboard.keycode, event.type == ALLEGRO_EVENT_KEY_DOWN);
		al_get_keyboard_state(&this->keyboard);
	}

	if (event.type == ALLEGRO_EVENT_KEY_CHAR) {
		if (this->input_callback) this->input_callback(event.keyboard.keycode, event.keyboard.unichar);
	}

	return std::make_pair(true, ticked);
}

void Proto::draw()
{
}

double Proto::step()
{
	double time = al_get_time();
	double dt = time - this->lastTime;
	this->lastTime = time;


	return dt;
}

void Proto::finish_frame()
{
	this->mousereleased = false;
}

void Proto::setAppDataDir(std::string name)
{
	/*char* path;
	size_t len;
	_dupenv_s(&path, &len, "APPDATA");
	this->AppDataPath = path;
	this->AppDataPath += "\\"+name;*/

	/*PWSTR path;
	SHGetKnownFolderPath(FOLDERID_RoamingAppData, 0, NULL, &path);

	PathCombineW(this->AppDataPath, path, name);*/

	char* appbuff;
	size_t len;
	_dupenv_s(&appbuff, &len, "APPDATA");

	this->AppDataPath = (fs::path)appbuff;
	this->AppDataPath /= name;

	this->createDir(this->AppDataPath);
}

void Proto::createDir(std::string path)
{
	assert(al_make_directory(path.c_str()));
}

void Proto::createDir(fs::path path)
{
	assert(al_make_directory(path.string().c_str()));
}

void Proto::openAppDataFile(std::string filepath, std::ofstream* ofs)
{
	std::string target_path = (this->AppDataPath / filepath).string();

	ofs->open(target_path);

	assert(ofs->is_open());
}

bool Proto::openAppDataFile(std::string filepath, std::ifstream* ifs)
{
	std::string target_path = (this->AppDataPath / filepath).string();

	ifs->open(target_path);

	return ifs->is_open();
}

void Proto::destroyRecursively(fs::path path)
{
	int mode;
	std::string name;
	ALLEGRO_FS_ENTRY* dir = al_create_fs_entry(path.string().c_str());
	if (al_open_directory(dir)) {
		ALLEGRO_FS_ENTRY* file;
		while (file = al_read_directory(dir)) {
			mode = al_get_fs_entry_mode(file);
			name = al_get_fs_entry_name(file);
			if (mode & ALLEGRO_FILEMODE_ISDIR) {
				//std::cout << "Calling myself\n";
				destroyRecursively(path / name);
			}
			else {
				//std::cout << "Removing file " << path / name << "\n";
				al_remove_filename((path / name).string().c_str());
			}
			al_destroy_fs_entry(file);
		}
	}
	//std::cout << "Removing directory " << path / name << "\n";
	al_remove_filename(path.string().c_str());
	al_destroy_fs_entry(dir);

}

void Proto::setKeyboardCallback(std::function<void(int, bool)> callback)
{
	this->keyboard_callback = callback;
}

void Proto::setInputCallback(std::function<void(int, int)> callback)
{
	this->input_callback = callback;
}


Image::Image()
{
	this->image = NULL;
	this->data = DrawData{};
	this->tint = al_map_rgb(255, 255, 255);
	this->width = 0;
	this->height = 0;
}

Image::Image(std::string filepath, DrawData dData, int flags)
{
	this->image = NULL;
	this->image = al_load_bitmap(filepath.c_str());

	assert((this->image != NULL) || (flags & PROTO_IMAGE_IGNORE_ERRORS));

	loaded_bitmaps.push_back(this->image);

	if (!dData.transformCreated) dData.create_transform();
	this->data = dData;
	this->tint = al_map_rgb(255, 255, 255);

	this->width = al_get_bitmap_width(this->image);
	this->height = al_get_bitmap_height(this->image);
}

void Image::setOrigin(int ox, int oy)
{
	this->data.ox = ox;
	this->data.oy = oy;
	this->data.create_transform();
}

void Image::setPosition(int x, int y)
{
	this->data.x = x;
	this->data.y = y;
	this->data.create_transform();
}

void Image::setRotation(float r)
{
	this->data.r = r;
	this->data.create_transform();
}

void Image::setScale(float sx, float sy=NULL)
{
	this->data.sx = sx;
	if (sy)
		this->data.sy = sy;
	else
		this->data.sy = sx;
	this->data.create_transform();
}

void Image::setTint(unsigned char r, unsigned char g, unsigned char b)
{
	this->tint = al_map_rgb(r, g, b);
}

void Image::setTint(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
	this->tint = al_map_rgba(r, g, b, a);
}

void Image::setTint(ALLEGRO_COLOR color)
{
	this->tint = color;
}

void Image::reload(const char* filepath, DrawData dData)
{
	this->image = NULL;
	this->image = al_load_bitmap(filepath);

	assert(this->image);

	loaded_bitmaps.push_back(this->image);

	if (!dData.transformCreated) dData.create_transform();
	this->data = dData;
	this->tint = al_map_rgb(255, 255, 255);

	this->width = al_get_bitmap_width(this->image);
	this->height = al_get_bitmap_height(this->image);
}

void Image::draw()
{
	if (!this->data.transformCreated) this->data.create_transform();
	al_use_transform(&this->data.transform);
	al_draw_tinted_bitmap(this->image, this->tint, 0, 0, 0);
}

void Image::draw(DrawData dData)
{
	if (!dData.transformCreated) dData.create_transform();
	al_use_transform(&dData.transform);
	al_draw_tinted_bitmap(this->image, this->tint, 0, 0, 0);
}

void Image::draw(Quad quad)
{
	if (!this->data.transformCreated) this->data.create_transform();
	al_use_transform(&this->data.transform);
	al_draw_tinted_bitmap_region(this->image, this->tint, quad.x, quad.y, quad.width, quad.height, 0, 0, 0);
}

void Image::draw(Quad quad, DrawData dData)
{
	if (!dData.transformCreated) dData.create_transform();
	al_use_transform(&dData.transform);
	al_draw_tinted_bitmap_region(this->image, this->tint, quad.x, quad.y, quad.width, quad.height, 0, 0, 0);
}



Hovermap::Hovermap()
{
	this->shapesets = {};
	this->shapeset_size = 0;
}

Hovermap::Hovermap(std::vector<HoverShape> shapes)
{
	this->shapeset_size = shapes.size();

	this->shapesets = shapes;
}

bool Hovermap::collides(int x, int y)
{
	for (int j = 0; j < this->shapeset_size; j++) {
		if ( (this->shapesets[j].shouldCollide && this->shapesets[j].collides(x, y)) || (!this->shapesets[j].shouldCollide && !this->shapesets[j].collides(x, y)) ) {
			return true;
		}
	}
	return false;
}

bool HoverShape::collides(int x, int y)
{
	if (this->type == "rectangle") {
		if (this->x1 <= x && x <= this->x2 && this->y1 <= y && y <= this->y2) {
			return true;
		}
	} else if(this->type == "triangle") {
		float A = 0.5 * (-this->y2 * this->x3 + this->y1 * (this->x3 - this->x2) + this->x1 * (this->y2 - this->y3) + this->x2 * this->y3);
		int sign = A < 0 ? -1 : 1;
		float s = (this->y1 * this->x3 - this->x1 * this->y3 + (this->y3 - this->y1) * x + (this->x1 - this->x3) * y) * sign;
		float t = (this->x1 * this->y2 - this->y1 * this->x2 + (this->y1 - this->y2) * x + (this->x2 - this->x1) * y) * sign;

		return s > 0 && t > 0 && (s + t) < 2 * A * sign;
	}
	return false;
}


Button::Button(int x, int y, int width, int height, Image image, std::function<void()>onclick, std::function<void()>onhover)
{
	this->x = x;
	this->y = y;
	this->width = width;
	this->height = height;
	this->mx = 0;
	this->my = 0;
	this->mreleased = false;

	this->imgdata = DrawData{ x, y };
	this->imgdata.create_transform();

	this->hover = false;

	this->onclick = onclick;
	this->onhover = onhover;

	this->hovermap = Hovermap({ HoverShape {"rectangle", true, 0, 0, width, height} });
	this->image = image;

	this->transposeX = 0;
	this->transposeY = 0;
	this->hoverImage = Image(image);
	this->hoverTint = al_map_rgb(255, 255, 255);
}

void Button::setHoverData(int x, int y, int width, int height)
{
	this->hovermap = Hovermap({ HoverShape {"rectangle", true, x, y, width, height} });
}

void Button::setHoverData(Hovermap hovermap)
{
	this->hovermap = hovermap;
}

void Button::setHoverFunction(std::function<void()> onhover)
{
	this->onhover = onhover;
}

void Button::setClickFunction(std::function<void()> onclick)
{
	this->onclick = onclick;
}

void Button::setImageDisplayParameters(DrawData dData)
{
	this->imgdata = dData;
	this->imgdata.x += this->x;
	this->imgdata.y += this->y;
	this->imgdata.create_transform();
}

void Button::setMousePos(int mx, int my)
{
	this->mx = mx;
	this->my = my;
}

void Button::setPosition(int x, int y)
{
	this->x = x;
	this->y = y;
	this->imgdata.x = x;
	this->imgdata.y = y;
}

void Button::update()
{
	al_get_mouse_state(&this->mouse);
	this->mx = this->mouse.x;
	this->my = this->mouse.y;
	int btns = this->mouse.buttons;

	if (this->hovermap.collides(mx - this->x, this->my - this->y)) {
		this->hover = true;
		if (btns & 1)
		{
			this->mreleased = true;
		}
		else if (!(btns & 1) && this->mreleased) {
			if (this->onclick) this->onclick();
			this->mreleased = false;
		}
		if (this->onhover) this->onhover();
	}
	else { this->hover = false; this->mreleased = false; }
}

void Button::draw()
{
	if (!this->hover) {
		this->image.draw(this->imgdata);
	}
	else {
		ALLEGRO_COLOR tintbuffer = this->hoverImage.tint;
		this->hoverImage.setTint(this->hoverTint);
		this->imgdata.x += this->transposeX;
		this->imgdata.y += this->transposeY;
		this->hoverImage.draw(this->imgdata);
		this->imgdata.x -= this->transposeX;
		this->imgdata.y -= this->transposeY;
		this->hoverImage.setTint(tintbuffer);
	}
}

void Button::setHoverEffect(int transposeX, int transposeY)
{
	this->transposeX = transposeX;
	this->transposeY = transposeY;
}

void Button::setHoverEffect(Image hoverImage)
{
	this->hoverImage = hoverImage;
}

void Button::setHoverEffect(ALLEGRO_COLOR tint)
{
	this->hoverTint = tint;
}

Animation::Animation(Image animationImage, int frameCount, float animationDuration, std::vector<Quad> frames)
{
	this->animationTime = 0;
	this->animationFrameCount = frameCount;
	this->animationDuration = animationDuration;

	this->animationFrames = frames;
	this->animationSheet = animationImage;
}

Animation::Animation(Image animationImage, float animationDuration, int rowlength, int quadwidth, int quadheight, int offsetX, int offsetY, int interval)
{
	this->animationTime = 0;
	this->animationFrameCount = rowlength;
	this->animationDuration = animationDuration;

	this->animationFrames = Proto::loadSpritesheet(rowlength, quadwidth, quadheight, offsetX, offsetY, interval);
	this->animationSheet = animationImage;
}

void Animation::draw()
{
	int frame = this->animationFrameCount*this->animationTime/this->animationDuration;
	this->animationSheet.draw(this->animationFrames[frame]);
}

void Animation::update(double dt)
{
	this->animationTime += dt;
	if (this->animationTime >= this->animationDuration) this->animationTime = 0;
}



Timeout::Timeout(int duration, void(*ontimeoutended)(), bool startnow)
{
	this->started = false;
	this->duration = duration;
	this->currentTime = (double)0;
	this->endTime = (double)0;

	this->ontimeoutended = ontimeoutended;
	
	if (startnow) this->start();
}

void Timeout::start()
{
	this->started = true;
	this->currentTime = al_get_time();
	this->endTime = this->currentTime + this->duration;
}

void Timeout::clear()
{
	this->started = false;
}

void Timeout::setNewDuration(int duration)
{
	this->duration = duration;
}

void Timeout::postpone(float value)
{
	this->endTime += value;
}

void Timeout::update(double dt)
{
	this->currentTime += dt;
	if (this->started && this->currentTime >= this->endTime) {
		this->started = false;
		this->ontimeoutended();
	}
}



TransformAnimation::TransformAnimation(DrawData startData, std::vector<TransformAnimationFrame> frames, bool loop)
{
	this->data = startData;
	this->currentFrame = 0;
	this->loop = loop;
	this->saved = startData;
	this->animationTime = 0;
	this->frames = frames;
	this->playing = true;
	this->startData = startData;
}

void TransformAnimation::update(double dt)
{
	if (this->playing) {
		this->animationTime += dt;

		if (this->animationTime < this->frames[this->currentFrame].duration) {
			float percent = this->animationTime / this->frames[this->currentFrame].duration;
			this->data.x = this->saved.x + percent * (this->frames[this->currentFrame].data.x - this->saved.x);
			this->data.y = this->saved.y + percent * (this->frames[this->currentFrame].data.y - this->saved.y);
			this->data.r = this->saved.r + percent * (this->frames[this->currentFrame].data.r - this->saved.r);
			this->data.sx = this->saved.sx + percent * (this->frames[this->currentFrame].data.sx - this->saved.sx);
			this->data.sy = this->saved.sy + percent * (this->frames[this->currentFrame].data.sy - this->saved.sy);
			this->data.ox = this->saved.ox + percent * (this->frames[this->currentFrame].data.ox - this->saved.ox);
			this->data.oy = this->saved.oy + percent * (this->frames[this->currentFrame].data.oy - this->saved.oy);
		}
		else {
			this->data = this->frames[this->currentFrame].data;
			this->saved = this->data;
			this->currentFrame++;
			this->animationTime = 0;

			if (this->currentFrame >= this->frames.size()) {
				if (this->loop) {
					this->currentFrame = 0;
				}
				else {
					this->playing = false;
					this->saved = this->data = this->startData;
				}
			}
		}
	}
}

Font::Font()
{
	this->fonts = {};
}

Font::Font(const char* filepath, int sizes[], int sizes_size)
{
	this->fonts = {};

	for (int i = 0; i < sizes_size; i++) {
		ALLEGRO_FONT* newfont = al_load_ttf_font(filepath, sizes[i], 0);
		this->fonts.insert({ sizes[i],  newfont });
		assert(this->fonts[sizes[i]]);
		loaded_fonts.push_back(newfont);
	}
}

int Font::getHeight(int size)
{
	return al_get_font_line_height(this->fonts[size]);
}

int Font::getWidth(int size, std::string str)
{
	return al_get_text_width(this->fonts[size], str.c_str());
}

void Font::loadSizes(const char* filepath, int sizes[], int sizes_size)
{
	for (int i = 0; i < sizes_size; i++) {
		ALLEGRO_FONT* newfont = al_load_ttf_font(filepath, sizes[i], 0);
		this->fonts.insert({ sizes[i],  newfont });
		assert(this->fonts[sizes[i]]);
		loaded_fonts.push_back(newfont);
	}
}

Label::Label()
{
	this->defsize = 0;
	this->height = 0;
	this->is_dict = false;
	this->offset = 0;
	this->width = 0;
}

Label::Label(std::string text, DrawData dData, std::map<std::string, ALLEGRO_COLOR> colormap, std::map<std::string, Font*> fontmap, std::string color, std::string font, int fontsize, int offset)
{
	this->data = dData;
	this->chunks = {};
	this->fontmap = fontmap;
	this->colormap = colormap;
	this->text = text;
	this->is_dict = false;
	this->defsize = fontsize;

	std::string chunk;
	std::string::iterator c;

	bool next_unimportant = false;
	bool reading_tag = false;
	std::vector<std::string> tagargs = {};
	std::string textcolor = color;
	std::string textfont = font;
	int textsize = fontsize;
	int maxlineheight = 0;
	int chunkX = 0;
	for (c = text.begin(); c != text.end(); c++) {
		char ch = *c;

		if (!next_unimportant) {
			if (ch == '/') next_unimportant = true;
			else if (ch == '<') {
				//fontmap[textfont.c_str()];
				//fontmap[textfont.c_str()]->fonts[textsize];
				//colormap[textcolor.c_str()];
				this->chunks.push_back(TextChunk{ colormap[textcolor], fontmap[textfont]->fonts[textsize], chunk, chunkX, 0 });
				chunkX += al_get_text_width(fontmap[textfont]->fonts[textsize], chunk.c_str());
				maxlineheight = max(maxlineheight, al_get_font_line_height(fontmap[textfont]->fonts[textsize]));
				chunk = "";
				reading_tag = true;
			}
			else if (ch == '>') {
				tagargs.push_back(chunk);
				chunk = "";

				if (tagargs[0] == "style") {
					assert(tagargs.size() == 4);
					textcolor = tagargs[1];
					textfont = tagargs[2];
					textsize = std::stoi(tagargs[3]);
				}

				reading_tag = false;
				tagargs.clear();
			}
			else if (ch == ':' && reading_tag) {
				tagargs.push_back(chunk);
				chunk = "";
			}
			else chunk.push_back(ch);
		}
		else {
			chunk.push_back(ch);
		}
	}

	this->chunks.push_back(TextChunk{colormap[textcolor], fontmap[textfont]->fonts[textsize], chunk, chunkX, 0});
	chunkX += al_get_text_width(fontmap[textfont]->fonts[textsize], chunk.c_str());
	maxlineheight = max(maxlineheight, al_get_font_line_height(fontmap[textfont]->fonts[textsize]));
	this->height = maxlineheight;
	this->width = chunkX;

	if (offset == PROTO_OFFSET_CENTER) {
		this->data.x -= chunkX / 2;
		this->data.y -= maxlineheight / 2;
		this->data.create_transform();
	}
}

Label::Label(std::string text, DrawData dData, ALLEGRO_COLOR color, Font* font, int fontsize, int offset)
{
	this->data = dData;
	this->chunks = {};
	this->fontmap = { {"default", font} };
	this->colormap = { {"default", color} };
	this->text = text;
	this->defsize = fontsize;
	this->height = al_get_font_line_height(font->fonts[fontsize]);
	this->width = font->getWidth(fontsize, text.c_str());

	this->chunks.push_back(TextChunk{ color, font->fonts[fontsize], text, 0, 0 });

	if (offset == PROTO_OFFSET_CENTER) {
		this->data.x -= al_get_text_width(font->fonts[fontsize], text.c_str()) / 2;
		this->data.y -= al_get_font_line_height(font->fonts[fontsize])/2;
		this->data.create_transform();
	}
}

Label::Label(std::string text, DrawData dData, std::map<std::string, ALLEGRO_COLOR> colormap, std::map<std::string, Font*> fontmap, std::string color, std::string font, int fontsize, int offset, int maxwidth, int line_interval, int adjust_type)
{
	this->data = dData;
	this->chunks = {};
	this->fontmap = fontmap;
	this->colormap = colormap;
	this->text = text;
	this->is_dict = false;
	this->height = 0;
	this->width = maxwidth;
	this->defsize = fontsize;

	std::string chunk;
	std::string::iterator c;

	bool next_unimportant = false;
	bool reading_tag = false;
	std::vector<std::string> tagargs = {};
	std::string textcolor = color;
	std::string textfont = font;
	int textsize = fontsize;
	int maxlineheight = 0;
	int chunkX = 0;
	int chunkY = 0;
	for (c = text.begin(); c != text.end(); c++) {
		char ch = *c;

		if (!next_unimportant) {
			if (ch == '/') next_unimportant = true;
			else if (ch == '<') {
				this->chunks.push_back(TextChunk{ colormap[textcolor], fontmap[textfont]->fonts[textsize], chunk, chunkX, chunkY });
				chunkX += al_get_text_width(fontmap[textfont]->fonts[textsize], chunk.c_str());
				maxlineheight = max(maxlineheight, al_get_font_line_height(fontmap[textfont]->fonts[textsize]));
				chunk = "";
				reading_tag = true;
			}
			else if (ch == '>') {
				tagargs.push_back(chunk);
				chunk = "";

				if (tagargs[0] == "style") {
					assert(tagargs.size() == 4);
					textcolor = tagargs[1];
					textfont = tagargs[2];
					textsize = std::stoi(tagargs[3]);
				}
				else if (tagargs[0] == "br") {
					assert(tagargs.size() == 1);
					this->chunks.push_back(TextChunk{ colormap[textcolor], fontmap[textfont]->fonts[textsize], chunk, chunkX, chunkY });
					chunkX = 0;
					maxlineheight = max(maxlineheight, fontmap[textfont]->getHeight(textsize));
					chunkY += maxlineheight + line_interval;
					maxlineheight = 0;
					chunk = "";
				}

				reading_tag = false;
				tagargs.clear();
			}
			else if (ch == ':' && reading_tag) {
				tagargs.push_back(chunk);
				chunk = "";
			}
			else if (ch == ' ' && !reading_tag) {
				chunk.push_back(ch);
				if (chunkX + fontmap[textfont]->getWidth(textsize, chunk.c_str()) > maxwidth) {
					chunkX = 0;
					chunkY += maxlineheight + line_interval;
					maxlineheight = 0;
				}
				this->chunks.push_back(TextChunk{ colormap[textcolor], fontmap[textfont]->fonts[textsize], chunk, chunkX, chunkY });
				maxlineheight = max( maxlineheight, fontmap[textfont]->getHeight(textsize));
				chunkX += fontmap[textfont]->getWidth(textsize, chunk.c_str());
				chunk = "";
			}
			else chunk.push_back(ch);
		}
		else {
			chunk.push_back(ch);
			next_unimportant = false;
		}
	}
	if (chunkX + fontmap[textfont]->getWidth(textsize, chunk.c_str()) > maxwidth) {
		chunkX = 0;
		chunkY += maxlineheight + line_interval;
	}
	this->chunks.push_back(TextChunk{ colormap[textcolor], fontmap[textfont]->fonts[textsize], chunk, chunkX, chunkY });
	maxlineheight = max(maxlineheight, fontmap[textfont]->getHeight(textsize));
	chunkY += maxlineheight;
	this->height = chunkY;
}

Label::Label(std::string text, DrawData dData, ALLEGRO_COLOR color, Font* font, int fontsize, int offset, int maxwidth, int line_interval, int adjust_type)
{
	this->defsize = 0;
	this->height = 0;
	this->is_dict = false;
	this->offset = 0;
	this->width = 0;
}

void Label::draw()
{
	ALLEGRO_TRANSFORM T2;
	auto T1 = al_get_current_transform();

	al_identity_transform(&T2);

	al_translate_transform(&T2, -this->data.ox, -this->data.oy);

	al_scale_transform(&T2, this->data.sx, this->data.sy);
	
	al_rotate_transform(&T2, this->data.r);

	al_translate_transform(&T2, this->data.x, this->data.y);

	al_use_transform(&T2);

	std::vector<TextChunk>::iterator iter;
	for (iter = this->chunks.begin(); iter != this->chunks.end(); iter++) {
		auto chunk = *iter;
		al_draw_text(chunk.font, chunk.color, chunk.x, chunk.y, 0, chunk.text.c_str());
	}

	al_use_transform(T1);
}

void Label::setKey(std::string key)
{
	this->dictkey = key;
	this->is_dict = true;
}

void Label::setTextChunk(std::string text)
{
	this->chunks.clear();
	this->chunks.push_back(TextChunk{ this->colormap["default"], this->fontmap["default"]->fonts[this->defsize], text, 0, 0 });
	this->text = text;
	this->width = this->fontmap["default"]->getWidth(this->defsize, text.c_str());
	this->height = this->fontmap["default"]->getHeight(this->defsize);
}

void Label::setText(std::string text)
{
}

ElementSet::ElementSet(std::vector<Label> labels, std::vector<Button> buttons, std::vector<Image> images)
{
	this->labels = labels;
	this->buttons = buttons;
	this->images = images;
}

void ElementSet::draw()
{
	for (std::vector<Image>::iterator x = images.begin(); x != images.end(); x++) {
		x->draw();
	}

	for (std::vector<Label>::iterator x = labels.begin(); x != labels.end(); x++) {
		x->draw();
	}

	for (std::vector<Button>::iterator x = buttons.begin(); x != buttons.end(); x++) {
		x->draw();
	}
}

void ElementSet::update()
{
	for (std::vector<Button>::iterator x = buttons.begin(); x != buttons.end(); x++) {
		x->update();
	}
}

void DrawData::create_transform()
{
	al_identity_transform(&this->transform);
	al_translate_transform(&this->transform, -this->ox, -this->oy);
	al_scale_transform(&this->transform, this->sx, this->sy);
	al_rotate_transform(&this->transform, this->r);
	al_translate_transform(&this->transform, this->x, this->y);
}



bool element_sorter(GUIElement const& l, GUIElement const& r)
{
	return l.z_index < r.z_index;
}

GUIPanel::GUIPanel(std::vector<GUIElement> elements, imgvec images, lblvec labels, btnvec buttons, float width, float height, bool attached, ALLEGRO_COLOR bgcolor, int flags)
{
	this->elements = elements;
	this->labels = labels;
	this->buttons = buttons;
	this->images = images;
	this->attach = attached;
	this->flags = flags;
	this->percentDims = (flags & PROTO_GUIPANEL_PERCENTDIMS) == PROTO_GUIPANEL_PERCENTDIMS;
	this->usebgcol = (flags & PROTO_GUIPANEL_BGCOLOR) == PROTO_GUIPANEL_BGCOLOR;
	this->scroll = 0;
	this->btn_originals = {};
	this->lbl_originals = {};
	this->img_originals = {};
	this->scrollHighBound = 500;
	this->scrollLowBound = 0;

	for (btnvec::iterator elem = this->buttons.begin(); elem != this->buttons.end(); elem++) {
		btn_originals.push_back(DrawData{elem->x, elem->y});
	}

	for (lblvec::iterator elem = this->labels.begin(); elem != this->labels.end(); elem++) {
		lbl_originals.push_back(elem->data);
	}

	for (imgvec::iterator elem = this->images.begin(); elem != this->images.end(); elem++) {
		img_originals.push_back(elem->data);
	}

	this->bgcolor = bgcolor;

	this->width = width;
	this->height = height;
	this->calculated_height = 0;
	this->calculated_width = 0;
	al_identity_transform(&deftrans);
	this->hide_overflow = (flags & PROTO_GUIPANEL_SCROLLABLE) == PROTO_GUIPANEL_SCROLLABLE;

	std::sort(this->elements.begin(), this->elements.end(), &element_sorter);
}

GUIPanel::GUIPanel()
{
	this->width = 0;
	this->height = 0;
	this->images = {};
	this->labels = {};
	this->elements = {};
	this->buttons = {};
	this->flags = 0;
	this->bgcolor = al_map_rgba(0, 0, 0, 0);
	this->x = 0;
	this->y = 0;
	this->percentDims = false;
	this->usebgcol = false;
	this->calculated_height = 0;
	this->calculated_width = 0;
	this->hide_overflow = false;
	this->scroll = 0;
	this->scrollHighBound = 500;
	this->scrollLowBound = 0;
	al_identity_transform(&deftrans);
}

void GUIPanel::setPosition(int x, int y)
{
	for (std::vector<GUIElement>::iterator elem = this->elements.begin(); elem != this->elements.end(); elem++) {
		if (elem->type == PROTO_GUI_BUTTON) {
			this->buttons[elem->index].setPosition(this->btn_originals[elem->index].x + x, this->btn_originals[elem->index].y + y);
			this->buttons[elem->index].imgdata.create_transform();
		}
		else if (elem->type == PROTO_GUI_LABEL) {
			this->labels[elem->index].data.x = this->lbl_originals[elem->index].x + x;
			this->labels[elem->index].data.y = this->lbl_originals[elem->index].y + y;
			this->labels[elem->index].data.create_transform();
		}
		else if (elem->type == PROTO_GUI_IMAGE) {
			this->images[elem->index].data.x = this->img_originals[elem->index].x + x;
			this->images[elem->index].data.y = this->img_originals[elem->index].y + y;
			this->images[elem->index].data.create_transform();
			//std::cout << "Image coords set to "
		}
	}
	this->x = x;
	this->y = y;
}

void GUIPanel::setCalculatedSize(int width, int height)
{
	this->calculated_width = width;
	this->calculated_height = height;
}

void GUIPanel::draw()
{
	int rx, ry, rw, rh;
	al_get_clipping_rectangle(&rx, &ry, &rw, &rh);
	if (this->hide_overflow) {
		al_set_clipping_rectangle(this->x, this->y, this->calculated_width, this->calculated_height);
	}
	al_use_transform(&deftrans);
	if (this->usebgcol) al_draw_filled_rectangle(this->x, this->y, this->x + this->calculated_width, this->y + this->calculated_height, this->bgcolor);
	for (std::vector<GUIElement>::iterator elem = this->elements.begin(); elem != this->elements.end(); elem++) {
		if (elem->type == PROTO_GUI_BUTTON) {
			this->buttons[elem->index].draw();
		}
		else if (elem->type == PROTO_GUI_LABEL) {
			this->labels[elem->index].draw();
		}
		else if (elem->type == PROTO_GUI_IMAGE) {
			this->images[elem->index].draw();
		}
	}
	if (this->hide_overflow) {
		al_set_clipping_rectangle(rx, ry, rw, rh);
	}
}

void GUIPanel::setScroll(int scroll)
{
	this->scroll = max(scroll, this->scrollLowBound);
	this->scroll = min(this->scroll, this->scrollHighBound);
	for (std::vector<GUIElement>::iterator elem = this->elements.begin(); elem != this->elements.end(); elem++) {
		if (elem->type == PROTO_GUI_BUTTON) {
			this->buttons[elem->index].setPosition(this->buttons[elem->index].x, this->y + this->btn_originals[elem->index].y - this->scroll);
			this->buttons[elem->index].imgdata.create_transform();
		}
		else if (elem->type == PROTO_GUI_LABEL) {
			this->labels[elem->index].data.y = this->y + this->lbl_originals[elem->index].y - this->scroll;
			this->labels[elem->index].data.create_transform();
		}
		else if (elem->type == PROTO_GUI_IMAGE) {
			this->images[elem->index].data.y = this->y + this->img_originals[elem->index].y - this->scroll;
			this->images[elem->index].data.create_transform();
			//std::cout << "Image coords set to "
		}
	}
}

void GUIPanel::setScrollLimits(int minlim, int maxlim)
{
	this->scrollLowBound = minlim;
	this->scrollHighBound = maxlim;
}

void GUIPanel::update()
{
	for (int btn = 0; btn < this->buttons.size(); btn++) {
		this->buttons[btn].update();
	}
}

void Drawable::draw()
{
	std::cout << "Nie ta funkcja";
}

GUI::GUI(int layout, std::vector<GUIPanel> panels, int x, int y, int width, int height)
{
	this->layout = layout;
	this->panels = panels;
	this->x = x;
	this->y = y;
	this->width = width;
	this->height = height;
	this->usesBg = 0;

	this->calculatePanels();
}

GUI::GUI()
{
	this->layout = 0;
	this->panels = {};
	this->height = 0;
	this->width = 0;
	this->x = 0;
	this->y = 0;
	this->usesBg = 0;
}

void GUI::calculatePanels()
{
	int drawx = this->x, drawy = this->y;
	float maxrowheight=0;
	if (this->layout == PROTO_GUI_LAYOUT_PANELS) {
		for (std::vector<GUIPanel>::iterator panel = this->panels.begin(); panel != this->panels.end(); panel++) {
			float panelwidth = panel->width, panelheight = panel->height, attached_panelwidth=0;
			
			if (panel->percentDims) {
				panelwidth *= (float)this->width;
				panelheight *= (float)this->height;
			}

			if (panel->attach && panel + 1 != this->panels.end()) {
				attached_panelwidth = (panel + 1)->percentDims ? (panel + 1)->width * (float)this->width : (panel + 1)->width;
			}

			if (drawx + panelwidth + attached_panelwidth > this->x + this->width) {
				drawx = this->x;
				drawy += (int)maxrowheight;
				maxrowheight = panelheight;
			}
			else {
				maxrowheight = max(maxrowheight, panelheight);
			}

			panel->setPosition(drawx, drawy);
			panel->calculated_width = panelwidth;
			panel->calculated_height = panelheight;
			drawx += panelwidth;
		}
	}
}

void GUI::draw()
{
	if (this->usesBg) {
		this->bgimg.draw(this->bgdata);
	}

	if (this->layout == PROTO_GUI_LAYOUT_PANELS) {
		for (std::vector<GUIPanel>::iterator panel = this->panels.begin(); panel != this->panels.end(); panel++) {
			panel->draw();
		}
	}
}

void GUI::update()
{
	if (this->layout == PROTO_GUI_LAYOUT_PANELS) {
		for (std::vector<GUIPanel>::iterator panel = this->panels.begin(); panel != this->panels.end(); panel++) {
			panel->update();
		}
	}
}

void GUI::setBackgroundImage(Image bgimg, DrawData bgdata)
{
	this->usesBg = true;
	this->bgdata = bgdata;
	this->bgimg = bgimg;
	this->bgdata.x += this->x;
	this->bgdata.y += this->y;
	this->bgdata.create_transform();
}

template<typename T>
void writetype(DataCell cell, char type, std::ofstream* ofs)
{
	ofs->write(&type, sizeof(char));
	ofs->write(reinterpret_cast<char*>(&cell), sizeof(T));
}

void serializeData(DataCell cell, std::ofstream* ofs)
{
	std::string val;
	std::vector<DataCell> val2;
	char booltype = 0x01;
	char inttype = 0x02;
	char floattype = 0x03;
	char doubletype = 0x04;
	char chartype = 0x05;
	char strbegin = 0x06;
	char strend = 0x07;
	char vecbegin = 0x08;
	char vecend = 0x09;
	switch (cell.index()) {
	case 0:
		writetype<bool>(cell, booltype, ofs);
		break;
	case 1:
		writetype<int>(cell, inttype, ofs);
		break;
	case 2:
		writetype<float>(cell, floattype, ofs);
		break;
	case 3:
		writetype<double>(cell, doubletype, ofs);
		break;
	case 4:
		writetype<char>(cell, chartype, ofs);
		break;
	case 5:
		ofs->write(&strbegin, sizeof(char));
		val = std::get<std::string>(cell);
		for (std::string::iterator c = val.begin(); c != val.end(); c++) {
			ofs->write(reinterpret_cast<char*>(&(*c)), sizeof(char));
		}
		ofs->write(&strend, sizeof(char));
		break;
	case 6:
		ofs->write(&vecbegin, sizeof(char));
		val2 = std::get<std::vector<DataCell>>(cell);
		for (std::vector<DataCell>::iterator el = val2.begin(); el != val2.end(); el++) {
			serializeData(*el, ofs);
		}
		ofs->write(&vecend, sizeof(char));
		break;
	default:
		std::cout << "Not supported";
		throw "Unsupported type";
	}
}

DataCell deserializeData(std::ifstream* ifs)
{
	std::vector<std::vector<DataCell>> vecstack = { std::vector<DataCell>() };
	int vecsize = 1;
	int intval;
	bool boolval;
	float floatval;
	double doubleval;
	std::string strval;
	char charval;

	int byte = 0;

	bool reading_string = false;
	while (byte = ifs->get()) {
		if (byte == -1)
			break;
		if (!reading_string) {
			switch (byte) {
			case 1:
				ifs->read(reinterpret_cast<char*>(&boolval), sizeof(bool));
				vecstack[vecsize - 1].push_back(boolval);
				break;
			case 2:
				ifs->read(reinterpret_cast<char*>(&intval), sizeof(int));
				vecstack[vecsize - 1].push_back(intval);
				break;
			case 3:
				ifs->read(reinterpret_cast<char*>(&floatval), sizeof(float));
				vecstack[vecsize - 1].push_back(floatval);
				break;
			case 4:
				ifs->read(reinterpret_cast<char*>(&doubleval), sizeof(double));
				vecstack[vecsize - 1].push_back(doubleval);
				break;
			case 5:
				ifs->read(reinterpret_cast<char*>(&charval), sizeof(char));
				vecstack[vecsize - 1].push_back(charval);
				break;
			case 6:
				reading_string = true;
				break;
			case 8:
				vecstack.push_back(std::vector<DataCell>{});
				vecsize++;
				break;
			case 9:
				vecstack[vecsize - 2].push_back(vecstack[vecsize - 1]);
				vecstack.pop_back();
				vecsize--;
				break;
			default:
				throw "Unsupported";
			}
		}
		else {
			if (byte != 7) {
				strval.push_back(byte);
			}
			else {
				vecstack[vecsize - 1].push_back(strval);
				strval = "";
				reading_string = false;
			}
		}
	}

	return vecstack[vecsize-1][0];
}

std::wstring s2ws(const std::string& s)
{
	int len;
	int slength = (int)s.length() + 1;
	len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0);
	wchar_t* buf = new wchar_t[len];
	MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
	std::wstring r(buf);
	delete[] buf;
	return r;
}

