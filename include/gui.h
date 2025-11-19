#ifndef __GUI_H__
#define __GUI_H__

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdbool.h>

#define LIBERATIONSANS_BOLD_ITALIC_FONT "res/fonts/LiberationSans-BoldItalic.ttf"
#define DEFAULT_FONT "res/fonts/LiberationMono-Regular.ttf"

typedef SDL_Texture guitex;

typedef struct _vec2_i_t
{
	int x, y;
} vec2_i_t;

typedef struct _vec2_f_t
{
	float x, y;
} vec2_f_t;

typedef struct _text_box
{
	char *loc;
	TTF_Font *font;
	int size;
	SDL_Color color;
	SDL_Surface *surf;
	SDL_Texture *texture;
	SDL_Rect rect;
	SDL_Rect text_rect;
	const char *str;
} text_box_t;

typedef struct _button
{
	SDL_Color current_color;
	SDL_Color main_color;
	SDL_Color hover_color;
	SDL_Color clicked_color;
	SDL_Texture *texture;
	SDL_Rect rect;
	text_box_t text_box;
	char *str;
	bool hover;
	bool clicked;
	bool selected;
	void (*function)(void);

} button_t;

typedef struct _mouse
{
	vec2_i_t pos;
	bool button[5];
} mouse_t;

typedef struct _input_text 
{
	SDL_Rect rect;
	SDL_Color color;
	SDL_Rect text_rect;
	button_t button[2];
	text_box_t label;
	SDL_Texture *texture;
	SDL_Surface *surf;
	char *font_loc;
	int font_size;
	TTF_Font *font;
	char str[256];
	bool status;
} input_text_t;

typedef struct _gui
{
	button_t button;
	mouse_t mouse;
	char buf[256];
	SDL_Event ev;
	bool running;
} gui_t;

typedef struct _scroll_box
{
	SDL_Color color;
	SDL_Rect scroll_view;
	SDL_Rect content_view;

	int scroll_offset;
	
	bool scrollable;
	

} scroll_box_t;

// initialization
void gui_initialize(const char *, int, int);
void gui_main_loop(gui_t *, void(*event_function)(void), void(*loop_function)(void));
// events
//void gui_event_quit(gui_t *);
void gui_event_mouse_down(gui_t *);
void gui_event_mouse_up(gui_t *);
void gui_event_mouse_motion(gui_t *);
void gui_event_text_input(gui_t *, bool, bool*);
void gui_event_keyboard_down(gui_t *);
void gui_event_keyboard_up(gui_t *);
void gui_event_scroll(gui_t*,int*);
// texture
SDL_Texture *gui_load_texture(char *);
void blit(SDL_Texture*,int,int);
// widgets
void gui_draw_rect(bool,int,int,int,int,int,int,int,int);
text_box_t gui_create_text(const char*,char *,int,int,int,int,int,int,int);
void gui_render_text(text_box_t); 
button_t gui_create_button(int,int,int,int,int,int,int,int,char*,void(*function)(void));
void gui_render_button(gui_t,button_t*);
input_text_t gui_create_input_text(char*,char*,int,int,int,int,int,int,int,int,int);
void gui_render_input_text(gui_t*,input_text_t*,bool);
void gui_stop_input_text(void);
void gui_create_scroll_box(int r, int g, int b, int a);
// end
void gui_clear(int,int,int,int);
void gui_present(void);
void gui_cleanup(void);

#endif // __GUI_H__
