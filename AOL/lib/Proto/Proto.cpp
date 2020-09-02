#include "Proto.h"
#include <iostream>
#include <assert.h>
#include <sstream>
#include <fstream>
#include <stdarg.h>
#include <fmt/format.h>
#include <algorithm>
#include <filesystem>
#include <Windows.h>

namespace fs = std::filesystem;

Proto::Proto()
{
	display = NULL;
	queue = NULL;
	timer = NULL;
	al_get_mouse_state(&mouse);

	lastTime = al_get_time();

	loaded_images = {};
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

	al_start_timer(this->timer);
}

void Proto::registerImage(Image* image)
{

	this->loaded_images.push_back(image);
}

void Proto::updateButton(Button* btn)
{
	btn->setMousePos(this->mouse.x, this->mouse.y);
	btn->mreleased = this->mousereleased;
}

void Proto::loadDictionary(const char* path)
{
	std::ifstream i(path);
	i >> this->dictionary;
	i.close();
}

std::string Proto::dict(std::string value)
{
	return this->dictionary[value];
}

std::pair<float, float> Proto::getScale(Image img, int w, int h)
{
	return std::make_pair((float)w/(float)img.width, (float)h/(float)img.height);
}

void Proto::quit()
{
	for (int i=0; i<this->loaded_images.size();i++) if (this->loaded_images[i]->image) al_destroy_bitmap(this->loaded_images[i]->image);

	if (this->display) al_destroy_display(this->display);
	if (this->queue) al_destroy_event_queue(this->queue);
}

std::pair <bool, bool> Proto::update()
{
	ALLEGRO_EVENT event;
	bool ticked = false;
	al_wait_for_event(this->queue, &event);
	if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
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
	char* path;
	size_t len;
	_dupenv_s(&path, &len, "APPDATA");
	this->AppDataPath = path;
	this->AppDataPath += "\\"+name;

	this->createDir(this->AppDataPath.c_str());
}

void Proto::createDir(const char* path)
{
	assert(CreateDirectoryA(path, NULL) || ERROR_ALREADY_EXISTS == GetLastError());
}


Image::Image()
{
	this->image = NULL;
	this->data = DrawData{};
	this->tint = al_map_rgb(255, 255, 255);
	this->width = 0;
	this->height = 0;
}

Image::Image(const char* filepath, DrawData dData)
{
	this->image = NULL;
	this->image = al_load_bitmap(filepath);

	assert(this->image);

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


Button::Button(int x, int y, int height, int width, Image image, void(*onclick)(), void (*onhover)())
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

void Button::setHoverFunction(void(*onhover)())
{
	this->onhover = onhover;
}

void Button::setClickFunction(void(*onclick)())
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

void Button::update()
{
	if (this->hovermap.collides(mx - this->x, this->my - this->y)) {
		this->hover = true;
		if (this->onhover) this->onhover();
		if (this->mreleased) {
			if (this->onclick) this->onclick();
			this->mreleased = false;
		}
	}
	else this->hover = false;
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

Font::Font(const char* filepath, int sizes[], int sizes_size)
{
	this->fonts = {};

	for (int i = 0; i < sizes_size; i++) {
		this->fonts.insert({ sizes[i], al_load_ttf_font(filepath, sizes[i], 0) });
		assert(this->fonts[sizes[i]]);
	}
}

int Font::getHeight(int size)
{
	return al_get_font_line_height(this->fonts[size]);
}

int Font::getWidth(int size, const char* str)
{
	return al_get_text_width(this->fonts[size], str);
}

Label::Label(std::string text, DrawData dData, std::map<std::string, ALLEGRO_COLOR> colormap, std::map<std::string, Font*> fontmap, std::string color, std::string font, int fontsize)
{
	this->data = dData;
	this->chunks = {};
	this->fontmap = fontmap;
	this->colormap = colormap;
	this->text = text;
	std::string chunk;
	std::string::iterator c;

	bool next_unimportant = false;
	bool reading_tag = false;
	std::vector<std::string> tagargs = {};
	std::string textcolor = color;
	std::string textfont = font;
	int textsize = fontsize;
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
}

Label::Label(std::string text, DrawData dData, ALLEGRO_COLOR color, Font* font, int fontsize)
{
	this->data = dData;
	this->chunks = {};
	this->fontmap = { {"default", font} };
	this->colormap = { {"default", color} };
	this->text = text;

	this->chunks.push_back(TextChunk{ color, font->fonts[fontsize], text, 0, 0 });
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

	this->bgcolor = bgcolor;

	this->width = width;
	this->height = height;
	this->calculated_height = 0;
	this->calculated_width = 0;
	al_identity_transform(&deftrans);

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
	al_identity_transform(&deftrans);
}

void GUIPanel::setPosition(int x, int y)
{
	for (std::vector<GUIElement>::iterator elem = this->elements.begin(); elem != this->elements.end(); elem++) {
		if (elem->type == PROTO_GUI_BUTTON) {
			this->buttons[elem->index].data.x += x;
			this->buttons[elem->index].data.y += y;
			this->buttons[elem->index].data.create_transform();
		}
		else if (elem->type == PROTO_GUI_LABEL) {
			this->labels[elem->index].data.x += x;
			this->labels[elem->index].data.y += y;
			this->labels[elem->index].data.create_transform();
		}
		else if (elem->type == PROTO_GUI_IMAGE) {
			this->images[elem->index].data.x += x;
			this->images[elem->index].data.y += y;
			this->images[elem->index].data.create_transform();
			//std::cout << "Image coords set to "
		}
	}
	this->x = x;
	this->y = y;
}

void GUIPanel::draw()
{
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
}

void GUIPanel::update()
{
	for (btnvec::iterator btn = this->buttons.begin(); btn != this->buttons.end(); btn++) {
		btn->update();
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

	this->calculatePanels();
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



const char* serializeData(DataCell cell)
{
	return nullptr;
}
