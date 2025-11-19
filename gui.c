#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <stdbool.h>
#include <time.h>
#include <string.h>

#include "include/gui.h"
#include "include/utils.h"

#define KBD_MAX_KEYS 350
#define M_MAX_BUTTONS 5

static SDL_Window *window;
static SDL_Renderer *renderer;
static int keyboard[KBD_MAX_KEYS];

int m_x, m_y;
SDL_Rect m_rect;
SDL_Event ev;

bool button_status = false;
// local functions
static void mouse_button_up(SDL_MouseButtonEvent *ev, mouse_t *_m)
{
	if (ev->button < M_MAX_BUTTONS) _m->button[ev->button] = false;
}

static void mouse_button_down(SDL_MouseButtonEvent *ev, mouse_t *_m)
{
	if (ev->button < M_MAX_BUTTONS) _m->button[ev->button] = true;
}

static void keyboard_down(SDL_KeyboardEvent *ev)
{
	if (ev->keysym.scancode < KBD_MAX_KEYS) keyboard[ev->keysym.scancode] = 1;
}

static void keyboard_up(SDL_KeyboardEvent *ev)
{
	if (ev->keysym.scancode < KBD_MAX_KEYS) keyboard[ev->keysym.scancode] = 0;
}

static bool get_mouse_position(SDL_Rect *rect, int x, int y)
{
	return (x > rect->x && x < rect->x + rect->w &&
		y > rect->y && y < rect->y + rect->h);
}

//char text_input[256] = "";

void gui_initialize(const char *title, int w, int h)
{
	ERROR_EXIT(!SDL_Init(SDL_INIT_VIDEO), "GUI ERROR: Initialization failed\n", SDL_GetError());	
	ERROR_EXIT(IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG), "GUI ERROR: Texture initialization failed\n", SDL_GetError());
	ERROR_EXIT(!TTF_Init(), "GUI ERROR: Text renderer initialization failed\n", SDL_GetError());
	ERROR_EXIT( (window = SDL_CreateWindow(
							title,
							SDL_WINDOWPOS_UNDEFINED,
							SDL_WINDOWPOS_UNDEFINED,
							w,
							h,
							SDL_WINDOW_RESIZABLE
									)), 
		    "GUI ERROR: Window creation failed\n", 
		    SDL_GetError());

	ERROR_EXIT( (renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED)), 
		    "GUI ERROR: Renderer creation failed\n", 
		    SDL_GetError());
}

void gui_main_loop(gui_t *gui, void(*event_function)(void), void(*loop_function)(void))
{
	gui->running = true;

	while (gui->running)
	{
		loop_function();
		while (SDL_PollEvent(&gui->ev))
		{
			if (gui->ev.type == SDL_QUIT)
			{
				gui->running = false;
				printf("Quiting...\n");
			}		
			
			event_function();

		}

		
		gui_present();
	}

	
}

// event functions
void gui_event_mouse_down(gui_t *gui)
{
	if (gui->ev.type == SDL_MOUSEBUTTONDOWN) mouse_button_down(&gui->ev.button, &gui->mouse);
}

void gui_event_mouse_up(gui_t *gui)
{
	if (gui->ev.type == SDL_MOUSEBUTTONUP) mouse_button_up(&gui->ev.button, &gui->mouse);
}

void gui_event_mouse_motion(gui_t *gui)
{
	if (gui->ev.type == SDL_MOUSEMOTION) 
	{
		gui->mouse.pos.x = gui->ev.motion.x;
		gui->mouse.pos.y = gui->ev.motion.y;
	}
}

void gui_event_text_input(gui_t *gui, bool activate, bool *state)
{
	if (gui->ev.type == SDL_TEXTINPUT && activate == true)
	{
		if (strlen(gui->buf) + strlen(gui->ev.text.text) < sizeof(gui->buf) - 1) strcat(gui->buf, gui->ev.text.text);
	}
            else if (gui->ev.type == SDL_KEYDOWN && activate == true)
	    {
	    	if (gui->ev.key.keysym.sym == SDLK_BACKSPACE && strlen(gui->buf) > 0)
	    	{ 
			gui->buf[strlen(gui->buf) - 1] = '\0';	
		}
		    else if (keyboard[SDL_SCANCODE_RETURN] && strlen(gui->buf) > 0)
		    {
			*state = true;	
		        //gui->buf[0] = '\0';
			
		    }

	    }
}

void gui_event_scroll(gui_t *gui, int *scroll_offset)
{
	if (gui->ev.type == SDL_MOUSEWHEEL)
	{
		*scroll_offset = gui->ev.wheel.y; 
	}		
}

void gui_event_keyboard_down(gui_t *gui)
{
	if (gui->ev.type == SDL_KEYDOWN) keyboard_down(&gui->ev.key);
}

void gui_event_keyboard_up(gui_t *gui)
{
	if (gui->ev.type == SDL_KEYUP) keyboard_up(&gui->ev.key);
}

// texture functions

guitex *gui_load_texture(char *file_name)
{
	guitex *texture;
	SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, "Loading %s", file_name);
	texture = IMG_LoadTexture(renderer, file_name);

	return texture;
}

void blit(guitex *texture, int x, int y)
{
	SDL_Rect rect;
	rect.x = x;
	rect.y = y;
	SDL_QueryTexture(texture, NULL, NULL, &rect.w, &rect.h);
	SDL_RenderCopy(renderer, texture, NULL, &rect);
}

// widget functions
void gui_draw_rect(bool activate, int r, int g, int b, int a, int x, int y, int w, int h)
{
	SDL_Rect rect;
	rect.x = x;
	rect.y = y;
	rect.w = w;
	rect.h = h;

	if (activate)
	{
		SDL_SetRenderDrawColor(renderer, r, g, b, a);
		SDL_RenderFillRect(renderer, &rect);
	}
}


text_box_t gui_create_text(const char *text, char *font_location, int font_size, int r, int g, int b, int a, int x, int y)
{
	text_box_t _t;

	_t.str = text;
	_t.loc = font_location;
	_t.size = font_size;
	_t.color = (SDL_Color){ r, g, b, a };
	
	ERROR_EXIT((_t.font = TTF_OpenFont(_t.loc, _t.size)), "Text renderer error: failed to load text font. %s\n", SDL_GetError());
	ERROR_EXIT((_t.surf = TTF_RenderUTF8_Blended_Wrapped(_t.font, _t.str, _t.color, 0)), "Text renderer error: text surface creation failed. %s\n", SDL_GetError());
	_t.rect = (SDL_Rect){ x, y, _t.surf->w, _t.surf->h };
	ERROR_EXIT((_t.texture = SDL_CreateTextureFromSurface(renderer, _t.surf)), "Text renderer error: text render failed. %s\n", SDL_GetError());
	return _t;
}

void gui_render_text(text_box_t _t)
{
	
	SDL_RenderCopy(renderer, _t.texture, NULL, &_t.rect);
}

button_t gui_create_button(int r, int g, int b, int a, int x, int y, int w, int h, char *str, void (*function)(void))
{
	button_t _b;
	
	_b.rect.x = x;
	_b.rect.y = y;
	_b.rect.w = w;
	_b.rect.h = h;
	_b.main_color = (SDL_Color){ r, g, b, a };
	_b.hover_color = (SDL_Color){ r, g - 40, b, a };
	_b.clicked_color = (SDL_Color){ 0, 70, 20, a };
	_b.str = str;
	_b.function = function;
	_b.clicked = false;
	
	
	_b.text_box = gui_create_text(_b.str, DEFAULT_FONT, _b.rect.h *  0.5, 0, 0, 0, 255, 0, 0);
	_b.text_box.rect.x = _b.rect.x + ( _b.rect.w - _b.text_box.rect.w ) / 2;
	_b.text_box.rect.y = _b.rect.y + ( _b.rect.h - _b.text_box.rect.h) / 2;

	return _b;
	
}

void gui_render_button(gui_t gui, button_t *_b)
{
	
	SDL_Color current_color;	
	_b->hover = get_mouse_position(&_b->rect, gui.mouse.pos.x, gui.mouse.pos.y);
	if (_b->hover == true)  
	{
		if (gui.mouse.button[SDL_BUTTON_LEFT] == true) 
		{ 
			current_color = _b->clicked_color; 
			_b->clicked = true; 
		}
		
		else
		{
			current_color = _b->hover_color;
			if (_b->clicked)
			{
				_b->function();
				_b->clicked = false;
			}

		}
	}

	else 
	{ 
		current_color = _b->main_color;
		_b->clicked = false;
	}
	SDL_SetRenderDrawColor(renderer, current_color.r, current_color.g, current_color.b, current_color.a);
	SDL_RenderFillRect(renderer, &_b->rect);

	gui_render_text(_b->text_box);


}

input_text_t gui_create_input_text(char *label, char *font_loc, int font_size, int r, int g, int b, int a, int x, int y, int w, int h)
{
	input_text_t _p;
	text_box_t _t;
	_p.rect.x = x;
	_p.rect.y = y;
	_p.rect.w = w;
	_p.rect.h = h;
	_p.color = (SDL_Color){ r, g, b, a };
	_p.font_loc = font_loc;	
	_p.font_size = font_size;

	_p.label = gui_create_text(label, _p.font_loc, _p.font_size, r, g, b, a, x, y); 

	ERROR_EXIT((_p.font = TTF_OpenFont(_p.font_loc, _p.font_size)), "Text renderer error: failed to load font %s\n", SDL_GetError()); 
	SDL_StartTextInput();

	return _p;

}

	
	//gui_draw_rect(_p.color.r, _p.color.g, _p.color.b, _p.color.a, _p.rect.x, _p.rect.y, _p.rect.w, _p.rect.h);
void gui_render_input_text(gui_t *gui, input_text_t *p, bool activate)
{
	
	strcpy(p->str, gui->buf);
	gui_draw_rect(activate, 20, 20, 20, 100, p->rect.x, p->rect.y, p->rect.w, p->rect.h);
	gui_render_text(p->label);
	if (strlen(p->str) > 0)
	{
		
		ERROR_EXIT((p->surf = TTF_RenderText_Blended(p->font, p->str, p->color)), "Text renderer error: failed to create surface %s\n", SDL_GetError()); 
		ERROR_EXIT((p->texture = SDL_CreateTextureFromSurface(renderer, p->surf)), "Text renderer error: failed to create texture %s\n", SDL_GetError());
		p->text_rect = (SDL_Rect){ p->rect.x + 5, p->rect.y + 10, p->surf->w, p->surf->h };
		SDL_RenderCopy(renderer, p->texture, NULL, &p->text_rect);
		SDL_FreeSurface(p->surf);
		SDL_DestroyTexture(p->texture);
	}
}

void gui_stop_input_text(void)
{
	SDL_StopTextInput();
}

void gui_clear(int r, int g, int b, int a)
{
	SDL_SetRenderDrawColor(renderer, r, g, b, a);	
	SDL_RenderClear(renderer);
}

void gui_present(void)
{
	SDL_RenderPresent(renderer);
}

void gui_cleanup(void)
{
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}
