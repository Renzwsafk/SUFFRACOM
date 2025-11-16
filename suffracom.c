#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "include/suffracom.h"
#include "include/gui.h"

#define WIN_TITLE "SUFFRACOM"
#define WIN_WIDTH 1280 
#define WIN_HEIGHT 724 
#define VERSION "version: 1.0-beta"

#define MAX_CANDIDATE 50
#define MAX_POSITION 50

const char *welcome_str = "Secure and enhanced voting system.";

static int button_state;
static input_text_t ipaddr;
static char ip_string[256];

gui_t gui_state;

static text_box_t title, mini_title, wstrtxt;	
static text_box_t desc[2];


/*
static text_box_t start_button_text;
static text_box_t create_server_button_text;
tatic text_box_t quit_button_text;
static text_box_t version;
*/
// voting texts
//static text_box_t candidate_button_text;

static bool ip_prompt_active = false;

static button_t start_button;
static button_t create_server_button;
static button_t quit_button;

//static candidate_queue_t candq;
bool text_input_state;

// voting buttons
static button_t candidate_button;

// structs
typedef struct _server_browser
{
	button_t button;
	text_box_t title;
//	input_box_t ipaddr;
//	input_box_t pass;
	bool state;
} server_browser_t;

typedef struct _saved_files
{
	text_box_t desc;
	button_t buttons[2];
	bool state;
} saved_files_t;

typedef struct _menu
{
	text_box_t texts[5];
	button_t buttons[3];	
	input_text_t ipaddr;
	saved_files_t saved_files;
	server_browser_t server_browser;
} menu_t;

static menu_t m;

typedef struct _admin
{
	
} admin_t;

typedef struct _vote
{
	int count;	
	int in;
	button_t ok;
	bool finish;
	text_box_t texts[2];
} vote_t;

static vote_t v;
typedef struct _data
{
	text_box_t candidate_name[MAX_CANDIDATE];
	text_box_t vote_count[MAX_CANDIDATE];
	text_box_t label[MAX_POSITION];
	button_t next, back;
	int y;
} data_t;

static data_t d;

typedef enum _screen_enum
{
	MAIN = 0,
	ADMIN = 1,
	VOTING = 2,
	DATA = 3,
	SURVEY = 4,
} screen_enum;


// function declarations
static void ip_address_prompt(void);
static void menu_init(void); 
static void menu(void);
static void main_frame(void);
static void event_func(void);
static void text_func(void);
static void exit_pointer(void);
static void top_and_bottom_bars(void);
//static void vote_screen_init(char candidate_names[12][50], int n, int m);
static void vote_screen_init(void);
static void vote_screen(void);
//static void print_outer_list(struct outer_node *);
static void voting_init(void);
static void voting(void);
static void activate_cand(void);
static void candidate_add_vote(void);
static void admin_init(void);
static void admin(void);
static void generate_identity_code(void);
static void confirm_vote(void);
static void pos_votes(void);
static void data_init(void);
static void data(void);
static void data_next(void);
static void data_back(void);

static screen_enum screen;

void 
suffracom_main(		int argc, 
			char **argv	)
{
	// initialization
	gui_initialize(WIN_TITLE, WIN_WIDTH, WIN_HEIGHT);
	atexit(gui_cleanup);
	screen = MAIN;
	menu_init();	
	
	vote_screen_init();


	gui_main_loop(&gui_state, event_func, main_frame);


/*
	vote_screen_init();
	gui_main_loop(&gui_state, vote_screen);

*/
	admin_init();
	gui_stop_input_text();
	exit(EXIT_SUCCESS);
}

static void 
generate_identity_code(void)
{
	int identity_code;

	srand(time(0));	
}

static void 
menu_init(void)
{

	m.texts[0] = gui_create_text(WIN_TITLE, LIBERATIONSANS_BOLD_ITALIC_FONT, 50, 0, 150, 0, 255, (WIN_WIDTH / 2) - 150, 120);
	m.texts[1] = gui_create_text(welcome_str, DEFAULT_FONT, 20, 0, 0, 0, 255, (WIN_WIDTH / 2) - 200, 170); 
	//desc[0] = gui_create_text(main_desc, DEFAULT_FONT, 20, 0, 0, 0, 255, (WIN_WIDTH / 2) - 250, 190);
	//desc[1] = gui_create_text(second_desc, DEFAULT_FONT, 20, 0, 0, 0, 255, (WIN_WIDTH / 2) - 280, 210);
	m.texts[2] = gui_create_text(VERSION, DEFAULT_FONT, 15, 0, 0, 0, 255, 10, 700);

	m.buttons[0] = gui_create_button(0, 155, 0, 255, (WIN_WIDTH / 2) - 205, 500, 500, 30, "ENTER SERVER", ip_address_prompt); 

	m.buttons[1] = gui_create_button(0, 155, 0, 255, (WIN_WIDTH / 2) - 205, 550, 500, 30, "CREATE SERVER", NULL);

	m.buttons[2]  = gui_create_button(155, 0, 0, 255, (WIN_WIDTH / 2) - 205, 600, 500, 30, "QUIT", exit_pointer);
	m.ipaddr = gui_create_input_text("Enter IP Address", DEFAULT_FONT, 13, 255, 255, 255, 255, (WIN_WIDTH - 400) / 2, (WIN_HEIGHT - 120) / 2, 500, 30);
}

static void 
menu(void)
{
	// texts
	for (int i = 0; i < 5; i++)
	{ 
		gui_render_text(m.texts[i]);
	}

	// statrt button
	for (int i = 0; i < 3; i++)
	{ 
		gui_render_button(gui_state, &m.buttons[i]);
	}
}
static void 
event_func(void)
{
	gui_event_mouse_up(&gui_state);
	gui_event_mouse_down(&gui_state);
	gui_event_mouse_motion(&gui_state);
	gui_event_keyboard_down(&gui_state);
	gui_event_keyboard_up(&gui_state);
	gui_event_text_input(&gui_state, ip_prompt_active, &text_input_state);
}

static bool status;

static void 
top_and_bottom_bars(void)
{
	gui_draw_rect(true, 0, 100, 0, 200, 0, WIN_HEIGHT - 30, WIN_WIDTH, 70);
	gui_draw_rect(true, 0, 100, 0, 200, 0, WIN_HEIGHT - 750, WIN_WIDTH, 70);
}
	
int _i = 0;
static void 
ip_address_prompt(void)
{
	ip_prompt_active = true;
	text_input_state = false;		
	gui_state.buf[0] = '\0';
}

static void 
main_frame(void)
{
	// event functions goes here
	gui_clear(155, 155, 155, 255);		
	top_and_bottom_bars();	

	switch (screen)
	{
		case MAIN:
			menu();	
			if (ip_prompt_active)
			{
				gui_render_input_text(&gui_state, m.ipaddr, ip_prompt_active);		
			}
			
			if (ip_prompt_active && text_input_state)
			{
				if ((strcmp(gui_state.buf, "TESTME")) == 0) 
				{
					screen = VOTING;
					ip_prompt_active = false;
					text_input_state = false;
				}		
			}

			break;
		case VOTING:
			vote_screen();
			
			break;
		case ADMIN:
			break;
		case DATA:
			
			data();
			
			break;

		default:
			break;
	}
}

int outer_button_gap = 100;

struct position 
create_position(char *title)
{
	struct position res;
	res.gap = 100;
	res.title = title;
	v.count++;
	res.button = gui_create_button(			0, 
							155, 
							0, 
							255, 
							(WIN_WIDTH / 2) - 650, 
							outer_button_gap, 
							340, 
							30, 
							res.title, 
							pos_votes	);
	outer_button_gap += 45; 
	return res;
}

struct candidate 
create_candidate(	struct position *_pos,	
			char *name,
			char *desc,
			guitex *image	)
{
	struct candidate res;
	res.name = name;
	res.image = image;
	res.votes = 0;
	res.desc_box = gui_create_text(desc, DEFAULT_FONT, 13, 0, 0, 0, 255, (WIN_WIDTH / 2) - 250, 350);

	res.button = gui_create_button(			0, 
							155, 
							0, 
							255, 
							(WIN_WIDTH / 2) + 250, 
							_pos->gap, 
							340, 
							30, 
							res.name, 
							candidate_add_vote	);

	_pos->gap += 45;
	return res;
}


char *bezos_desc = "CEO, ENTREPRENEUR, BORN IN 1964, JEFFREY, JEFFREY BEZOS";
char *elon_desc = "THIS IS ELON MUSK";
char *chester_desc = "asdnfk;anwgfinaweiognS;NSI;NDS;ONS;OGSN;OGN;";
char *obama_desc = "obamna pyramid";
char *vance_desc = "go cry liberals";
char *baldes_desc = "carlos nakalbo";
char *kim_desc = "FIRE IN THE HOLE";
struct position pos[4];

static void 
vote_screen_init(void)
{	
	v.in = 0, v.count = 0;
	mini_title = gui_create_text(WIN_TITLE, LIBERATIONSANS_BOLD_ITALIC_FONT, 20, 255, 255, 255, 255, (WIN_WIDTH / 2) - 90, 10);
	v.ok = gui_create_button(0, 155, 0, 255, (WIN_WIDTH / 2) + 250, (WIN_HEIGHT / 2) + 250, 300, 30, "OK", confirm_vote);

	v.texts[0] = gui_create_text("SELECT CANDIDATE", DEFAULT_FONT, 20, 0, 0, 0, 255, (WIN_WIDTH / 2) + 300, (WIN_HEIGHT / 2) - 300);
	v.texts[1] = gui_create_text("Please enter a candidate before proceeding", DEFAULT_FONT, 20, 255, 0, 0, 255, (WIN_WIDTH / 2) + 300, (WIN_HEIGHT / 2) - 399);

	pos[0] = create_position("President");
	pos[1] = create_position("Vice President");
	pos[2] = create_position("Secretary");
	pos[3] = create_position("Health Officer");

	pos[0].queue[0] = create_candidate(&pos[0], "Jeff Bezos", bezos_desc, gui_load_texture("res/gfx/bezos.jpg"));	
	pos[0].queue[1] = create_candidate(&pos[0], "Elon Musk", elon_desc, gui_load_texture("res/gfx/Elon-Musk.jpg"));	
	
	pos[1].queue[0] = create_candidate(&pos[1], "Chester", elon_desc, gui_load_texture("res/gfx/chester.jpg"));	
	pos[1].queue[1] = create_candidate(&pos[1], "Obamna", obama_desc, gui_load_texture("res/gfx/obamna.jpg"));

	pos[2].queue[0] = create_candidate(&pos[2], "Chester nanaman", chester_desc, gui_load_texture("res/gfx/chester.jpg"));
	pos[2].queue[1] = create_candidate(&pos[2], "JD Vance", vance_desc, gui_load_texture("res/gfx/vance.jpg"));

	pos[3].queue[0] = create_candidate(&pos[3], "Carlota Baldes", baldes_desc, gui_load_texture("res/gfx/baldes.jpg"));
	pos[3].queue[1] = create_candidate(&pos[3], "Green kim", kim_desc, gui_load_texture("res/gfx/kim.jpg"));

	if (v.finish) data_init();
}

bool confirmed = false;

static void 
vote_screen(void)
{
	text_input_state = false;
	top_and_bottom_bars();
	gui_render_text(m.texts[2]);
	gui_render_text(mini_title);
	gui_render_button(gui_state, &v.ok);
	gui_draw_rect(true, 0, 100, 0, 200, (WIN_WIDTH / 2) - 300, (WIN_HEIGHT / 2) - 320, (WIN_WIDTH / 2) - 100, WIN_HEIGHT);

	for (int j = 0; j < v.count; j++)
	{
		gui_render_button(gui_state, &pos[j].button);
	}	

	gui_render_text(v.texts[0]);

	if (v.in < v.count)
	{
		pos[v.in].button.main_color = (SDL_Color){100, 100, 0, 255};
		 
		for (int i = 0; i < 2; i++)
		{
			gui_render_button(gui_state, &pos[v.in].queue[i].button);

			if (pos[v.in].queue[i].button.hover)
			{
				gui_render_text(pos[v.in].queue[i].desc_box);
				blit(pos[v.in].queue[i].image, (WIN_WIDTH / 2) - 250, 50);
			}		
		}
	} 
}

static void 
candidate_add_vote(void)
{
	for (int p = 0; p < v.count; p++)
	{
		for (int c = 0; c < 2; c++)
		{
			button_t *btn = &pos[p].queue[c].button;

			if (btn->clicked == true)
			{
				for (int k = 0; k < 2; k++)
				{
					pos[p].queue[k].selected = false;
					pos[p].queue[k].button.hover_color = (SDL_Color){0, 155, 0, 255};
					pos[p].queue[k].button.main_color  = (SDL_Color){0, 100, 0, 255};
				
					if (pos[p].queue[k].votes >= 1) 
					{
						pos[v.in].queue[k].votes--;
						printf("%s: %d\n", pos[p].queue[k].name, pos[p].queue[k].votes);
					}
	
				}
			
				pos[p].queue[c].selected = true;

				if (pos[p].queue[c].selected == true)
				{	
					printf("%s is selected\n", pos[p].queue[c].name);	
					pos[v.in].queue[c].votes++;
					printf("%s: %d\n", pos[p].queue[c].name, pos[p].queue[c].votes);
					pos[v.in].button.main_color = (SDL_Color){0, 155, 0, 255};
					confirmed = true;
				}

				btn->main_color = (SDL_Color){0, 0, 155, 255};
				btn->hover_color = (SDL_Color){0, 0, 50, 255}; 	
			}
		}
	}
}

static void
confirm_vote(void)
{
	if (confirmed)
	{
		v.in++; 
		printf("%d\n", v.in); 
		printf("Vote confirmed\n");
		confirmed = false;		
	}
	
	if (v.in == v.count)
	{
		for (int c = 0; c < v.count; c++)
		{
			for (int i = 0; i < 2; i++)
			{
				data_init(); 
				screen = DATA;
				printf("%s: %d\n", pos[c].queue[i].name, pos[c].queue[i].votes);
				v.in = 0;
			}
		}
	}
}

int y = 0;
char buffer[20];


static void
data_init(void)
{
	
	y = 170;	
	memset(buffer, 0, sizeof(char) * 20);

	if (v.in < v.count)
	{
		for (int i = 0; i < 2; i++)
		{
	
			d.candidate_name[i] = gui_create_text(pos[v.in].queue[i].name, DEFAULT_FONT, 30, 0, 0, 0, 255, (WIN_WIDTH / 2) - 500, y); 
			snprintf(buffer, sizeof(buffer), "%d", pos[v.in].queue[i].votes);
			d.vote_count[i] = gui_create_text(buffer, DEFAULT_FONT, 30, 0, 0, 155, 255, (WIN_WIDTH / 2) + 412, y);
			y += 50;
		}

		for (int c = 0; c < 7; c++)
		{
			d.label[v.count] = gui_create_text(pos[v.in].title, LIBERATIONSANS_BOLD_ITALIC_FONT, 50, 0, 0, 0, 255, (WIN_WIDTH / 2) - 120, (WIN_HEIGHT / 2) - 300);
		}
	}

	d.next = gui_create_button(0, 155, 0, 255, (WIN_WIDTH / 2) + 50, (WIN_HEIGHT / 2) + 300, 300, 30, "NEXT >", data_next); 
	d.back = gui_create_button(0, 155, 0, 255, (WIN_WIDTH / 2) - 350, (WIN_HEIGHT / 2) + 300, 300, 30, "< BACK", data_back);
}

static void
data(void)
{
	gui_render_text(mini_title);

	for (int c = 0; c < v.count; c++)
	{
		gui_render_text(d.candidate_name[c]);
		gui_render_text(d.vote_count[c]);
	}

	for (int k = 0; k < 7; k++)
	{
		gui_render_text(d.label[k]);
	}

	gui_render_button(gui_state, &d.next);
	gui_render_button(gui_state, &d.back);
}

static void
data_next(void)
{
	if (v.in < v.count)
	{
		v.in++;	
		data_init();
	}
}

static void
data_back(void)
{
	if (v.in > 0)
	{
		v.in--;	
		data_init();
	}	
}
static void
server_browser_init(void)
{
	m.server_browser.button = gui_create_button(0, 155, 0, 255, (WIN_WIDTH / 2) - 205, 550, 500, 30, "TEST SERVER", NULL);
	
}

static void
server_browser(void)
{
	gui_draw_rect(m.buttons[0].clicked, 0, 100, 0, 50, WIN_WIDTH / 2, WIN_HEIGHT / 2, 400, 400);
}

static void
enter_pass(void)
{
		
}

text_box_t saved_files_text;
button_t saved_files_button;
button_t delete_files_button;

static void
admin_init(void)
{
	// admin variables
}

static void
admin(void)
{
		
}

static void
pos_votes(void)
{
	for (int k = 0; k < 2; k++)
	{
		for (int c = 0; c < 2; c++)
		{
			printf("%s", pos[k].queue[c].name);
		}
	}
}

static void exit_pointer(void)
{
	printf("Quiting...\n");
	exit(EXIT_SUCCESS);
}

