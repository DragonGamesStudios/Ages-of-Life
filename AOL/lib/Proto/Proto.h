#pragma once
#define _USE_MATH_DEFINES

#include<allegro5/allegro.h>
#include<allegro5/allegro_audio.h>
#include<allegro5/allegro_font.h>
#include<allegro5/allegro_acodec.h>
#include<allegro5/allegro_ttf.h>
#include<allegro5/allegro_image.h>
#include<allegro5/allegro_primitives.h>

#include <nlohmann/json.hpp>

#include <fmt/format.h>

#include <utility>
#include <vector>
#include <math.h>
#include <string>
#include <iostream>
#include <variant>
#include <map>
#include <Windows.h>
#include <fstream>


#define PROTO_WINDOW_FULLSCREEN 0b1
#define PROTO_OFFSET_TOPLEFT 0
#define PROTO_OFFSET_CENTER 1
#define PROTO_GUI_BUTTON 0
#define PROTO_GUI_LABEL 1
#define PROTO_GUI_IMAGE 2
#define PROTO_GUI_LAYOUT_PANELS 0
#define PROTO_GUIPANEL_PERCENTDIMS 0b1
#define PROTO_GUIPANEL_BGCOLOR 0b10
#define PROTO_LABEL_USES_DICT 0b1

using json = nlohmann::json;

struct DrawData {
	int x = 0;
	int y = 0;
	float r = 0;
	float sx = 1;
	float sy = 1;
	int ox = 1;
	int oy = 1;
	bool transformCreated = false;
	ALLEGRO_TRANSFORM transform;
	void create_transform();
};

struct TransformAnimationFrame {
	DrawData data;
	float duration;
};

struct Quad {
	int x;
	int y;
	int width;
	int height;
};

struct HoverShape {
	const char* type;
	int shouldCollide;
	int x1;
	int y1;
	int x2;
	int y2;
	int x3 = NULL;
	int y3 = NULL;
	bool collides(int x, int y);
};

struct TextChunk {
	ALLEGRO_COLOR color;
	ALLEGRO_FONT* font;
	std::string text;
	int x;
	int y;
};




class Drawable {
public:
	DrawData data;
	void draw();
};

class Hovermap
{
public:
	Hovermap();
	Hovermap(std::vector<HoverShape> shapes);

	std::vector<HoverShape> shapesets;
	int shapeset_size;

	bool collides(int x, int y);
};

class Font
{
public:
	Font();
	Font(const char* filepath, int sizes[], int size_amount);

	std::map<int, ALLEGRO_FONT*> fonts;

	int getHeight(int size);
	int getWidth(int size, const char* str);
};


class Image : public Drawable
{
public:
	Image();
	Image(const char* filepath, DrawData dData);

	ALLEGRO_BITMAP* image;
	
	int width, height;
	DrawData data;
	ALLEGRO_COLOR tint;

	void setOrigin(int ox, int oy);
	void setPosition(int x, int y);
	void setRotation(float r);
	void setScale(float sx, float sy);
	void setTint(unsigned char r, unsigned char g, unsigned char b);
	void setTint(unsigned char r, unsigned char g, unsigned char b, unsigned char a);
	void setTint(ALLEGRO_COLOR color);

	void draw();
	void draw(DrawData dData);
	void draw(Quad quad);
	void draw(Quad quad, DrawData dData);

};

class Button : public Drawable
{
public:
	int x;
	int y;
	int width;
	int height;
	Hovermap hovermap;
	int transposeX;
	int transposeY;
	Image image;
	Image hoverImage;
	ALLEGRO_COLOR hoverTint;

	int mx;
	int my;
	bool mreleased;

	DrawData imgdata;

	bool hover;

	void (*onclick)();
	void (*onhover)();

	Button(int x, int y, int width, int height, Image image, void (*onclick)()=NULL, void (*onhover)()=NULL);

	void setHoverData(int x, int y, int width, int height);
	void setHoverData(Hovermap hovermap);

	void setHoverEffect(int transposeX, int transposeY);
	void setHoverEffect(Image hoverImage);
	void setHoverEffect(ALLEGRO_COLOR tint);

	void setHoverFunction(void (*onhover)());
	void setClickFunction(void (*onclick)());

	void setImageDisplayParameters(DrawData dData);

	void setMousePos(int mx, int my);
	void setPosition(int x, int y);
	void update();
	void draw();
};

class Label : public Drawable
{
public:
	Label(std::string text, DrawData dData, std::map<std::string, ALLEGRO_COLOR> colormap, std::map<std::string, Font*> fontmap, std::string color, std::string font, int fontsize, int offset);
	Label(std::string text, DrawData dData, ALLEGRO_COLOR color, Font* font, int fontsize, int offset);

	std::string text;
	DrawData data;
	std::vector <TextChunk> chunks;
	std::map <std::string, ALLEGRO_COLOR> colormap;
	std::map <std::string, Font*> fontmap;
	int offset;
	bool is_dict;
	std::string dictkey;

	void draw();
	void setKey(std::string key);

private:

};

typedef std::vector<Image> imgvec;
typedef std::vector<Button> btnvec;
typedef std::vector<Label> lblvec;

class Animation {
public:
	Animation(Image animationImage, int frameCount, float animationDuration, std::vector<Quad> frames);
	Animation(Image animationImage, float animationDuration, int rowlength, int quadwidth, int quadheight, int offsetX, int offsetY, int interval);

	double animationTime;
	float animationDuration;
	int animationFrameCount;
	std::vector<Quad> animationFrames;
	Image animationSheet;

	void draw();
	void update(double dt);
};

class TransformAnimation {
public:
	TransformAnimation(DrawData startData, std::vector<TransformAnimationFrame> frames, bool loop=false);

	std::vector <TransformAnimationFrame> frames;

	DrawData data;
	DrawData saved;
	DrawData startData;

	int currentFrame;
	bool loop;
	float animationTime;
	bool playing;

	void update(double dt);
};


class ElementSet
{
public:
	ElementSet(std::vector <Label> labels, std::vector <Button> buttons, std::vector<Image> images);

	std::vector <Label> labels;
	std::vector <Button> buttons;
	std::vector <Image> images;

	void draw();
	void update();
};


struct GUIElement {
	int type;
	int index;
	int z_index = 0;
};



class DataCell;
class DataCell : public std::variant <bool, int, float, double, char, std::string, std::vector <DataCell>> {
public:
	using base = std::variant <bool, int, float, double, char, std::string, std::vector <DataCell>>;
	using base::base;
	using base::operator=;
};

void serializeData(DataCell cell, std::ofstream* ofs);
DataCell deserializeData(std::ifstream* ifs);

typedef std::vector<DataCell> datvec;

class Proto
{
public:
	Proto();
	~Proto();

	ALLEGRO_DISPLAY* display;
	ALLEGRO_EVENT_QUEUE* queue;
	ALLEGRO_TIMER* timer;
	ALLEGRO_MOUSE_STATE mouse;

	double lastTime;
	bool mousereleased;

	TCHAR AppDataPath[MAX_PATH+2];

	std::vector<Image*> loaded_images;
	std::vector<Button*> registered_buttons;
	std::vector<Label*> registered_labels;

	json dictionary;

	static std::vector<std::vector<Quad>> loadSpritesheet(int rows, int rowlength, int quadwidth, int quadheight, int offsetX, int offsetY, int intervalX, int intervalY);
	static std::vector<Quad> loadSpritesheet(int rowlength, int quadwidth, int quadheight, int offsetX, int offsetY, int interval);

	void createWindow(int width, int height, ALLEGRO_BITMAP* icon, const char* title, int flags);
	void registerImage(Image* image);
	void registerButton(Button* btn);
	void registerLabel(Label* lbl);
	void updateButton(Button* btn);
	void loadDictionary(const char* path);
	std::string dict(std::string value);
	template <std::size_t ... Is>
	inline std::string dict(const std::string& format, const std::vector<std::string>& v, std::index_sequence<Is...>);
	template <std::size_t N>
	inline std::string dict(const std::string& format, const std::vector<std::string>& v);
	std::pair <float, float> getScale(Image img, int w, int h);
	std::pair <int, int> getWindowDimensions();

	void quit();
	std::pair <bool, bool> update();
	void draw();
	double step();
	void finish_frame();

	void setAppDataDir(LPCWSTR name);
	void createDir(LPCWSTR path);
	void openAppDataFile(LPCWSTR filepath, std::ofstream* ofs);
	void openAppDataFile(LPCWSTR filepath, std::ifstream* ifs);

	template <typename ...T>
	void log(const T& ... args);

};

class GUIPanel {
public:
	GUIPanel(std::vector<GUIElement> elements, imgvec images, lblvec labels, btnvec buttons, float width, float height, bool attached, ALLEGRO_COLOR bgcolor, int flags);
	GUIPanel();

	std::vector<GUIElement> elements;
	imgvec images;
	btnvec buttons;
	lblvec labels;
	int x, y, flags;
	float width, height;
	float calculated_width, calculated_height;

	bool attach, percentDims, usebgcol;

	ALLEGRO_COLOR bgcolor;

	ALLEGRO_TRANSFORM deftrans;

	void setPosition(int x, int y);
	void draw();
	void update();
};

class GUI
{
public:
	GUI(int layout, std::vector <GUIPanel>panels, int x, int y, int width, int height);
	GUI();

	int layout;
	int x, y, width, height;
	std::vector<GUIPanel> panels;

	void calculatePanels();

	void draw();
	void update();
};

class Timeout
{
public:
	Timeout(int duration, void(*ontimeoutended)(), bool startnow=true);

	double currentTime;
	double endTime;
	float duration;
	bool started;

	void (*ontimeoutended)();

	void start();
	void clear();
	void setNewDuration(int duration);
	void postpone(float value);

	void update(double dt);
};

template <std::size_t ... Is>
inline std::string Proto::dict(const std::string& value, const std::vector<std::string>& v, std::index_sequence<Is...>)
{
	const std::string& val = this->dictionary[value];
	return fmt::format(val, v[Is]...);
}


template <std::size_t N>
inline std::string Proto::dict(const std::string& value, const std::vector<std::string>& v)
{
	return this->dict(value, v, std::make_index_sequence<N>());
}

template <typename ...T>
inline void Proto::log(const T& ... args)
{
	(std::cout << ... << args) << '\n';
}