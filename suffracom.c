#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "include/suffracom.h"
#include "include/gui.h"

#define WIN_TITLE "SUFFRACOM"
#define WIN_WIDTH 1280 
#define WIN_HEIGHT 724 
#define VERSION "version: 1.0"

#define MAX_CANDIDATE 20
#define MAX_POSITION 20

const char *welcome_str = "Secure and enhanced voting system.";

static int button_state;
static input_text_t ipaddr;
static char ip_string[256];

gui_t gui_state;

static text_box_t title, mini_title, wstrtxt;	
static text_box_t desc[2];

static bool ip_prompt_active = false;

static int selected_pos = -1;

static button_t start_button;
static button_t create_server_button;
static button_t quit_button;

bool text_input_state;

// voting buttons
static button_t candidate_button;

// structs
typedef struct _server_browser
{
	text_box_t title;
	button_t button;
	
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
	saved_files_t saved_files;
	server_browser_t server_browser;
	button_t buttons[3];	
	text_box_t texts[5];
	input_text_t ipaddr;
	text_box_t candidate_name[MAX_CANDIDATE];
	text_box_t position[MAX_POSITION];
} menu_t;

static menu_t m;

typedef struct _admin
{
	input_text_t input[3]; 
	text_box_t label[3];	
	button_t create[2];
	button_t remove[2];
	button_t modify[2];
	button_t add_photo;
	button_t file, edit, help;
	button_t file_buttons[5];
	bool tool_state[4];
	bool input_state[3];
	bool pos_state;
	int in;
	int count;
	
} admin_t;

static admin_t a;

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

typedef struct _user
{
	char *username;
	char *password;
	int id;
	input_text_t *username_input, *password_input; 
	bool input_state[2];
} user_t;

typedef struct confirmation
{
	button_t buttons[5]
} confirmation_t;

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
static void free_menu(void);
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
static void close_admin(void);
static void add_position_button(void);
static void add_position(void);
static void enable_add_position(void);
static void add_candidate_button(void);
static void add_candidate(void);
static void select_position(void);
static void file_state_button(void);
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
	memset(&gui_state, 0, sizeof(gui_state));
	gui_state.buf[0] = '\0';
	// initialization
	gui_initialize(WIN_TITLE, WIN_WIDTH, WIN_HEIGHT);

	atexit(gui_cleanup);
	screen = MAIN;
	mini_title = gui_create_text(WIN_TITLE, LIBERATIONSANS_BOLD_ITALIC_FONT, 20, 255, 255, 255, 255, (WIN_WIDTH / 2) - 90, 10);
	menu_init();	
	
//	vote_screen_init();

	gui_main_loop(&gui_state, event_func, main_frame);

	gui_stop_input_text();
	exit(EXIT_SUCCESS);
}

static void 
generate_identity_code(void)
{
	srand(time(NULL));		

	//int id_code = (rand() % 999999999999999) + 100000000000000;
	const char *id_characters = "QWERTYUIOPASDFGHJKLZXCVBNM1234567890";
	int length = strlen(id_characters);
	
	char id_buffer[length + 1]; 

	for (int i = 0; i < length; i++)
	{
		int rand_select_index = rand() % length;
		id_buffer[i] = id_characters[rand_select_index];
	}

	id_buffer[length] = '\0';
	
	printf("%s\n", id_buffer);
}	

static void 
menu_init(void)
{
	m.texts[0] = gui_create_text(WIN_TITLE, LIBERATIONSANS_BOLD_ITALIC_FONT, 50, 0, 150, 0, 255, (WIN_WIDTH / 2) - 150, 120);
	m.texts[1] = gui_create_text(welcome_str, DEFAULT_FONT, 20, 0, 0, 0, 255, (WIN_WIDTH / 2) - 170, 200); 
	m.texts[2] = gui_create_text(VERSION, DEFAULT_FONT, 15, 0, 0, 0, 255, 10, 700);
	m.buttons[0] = gui_create_button(0, 155, 0, 255, (WIN_WIDTH / 2) - 205, 500, 500, 30, "ENTER SERVER", ip_address_prompt); 
	m.buttons[1] = gui_create_button(0, 155, 0, 255, (WIN_WIDTH / 2) - 205, 550, 500, 30, "CREATE SERVER", admin_init);
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
		gui_render_button(&gui_state, &m.buttons[i]);
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
	
	if (screen == MAIN)
	{
		gui_event_text_input(&gui_state, ip_prompt_active, &text_input_state);
	}
	
	if (screen == ADMIN)
	{
		gui_event_text_input(&gui_state, a.input_state[0], &text_input_state);
	}
}

static bool status;

static void 
top_and_bottom_bars(void)
{
	gui_draw_rect(true, 0, 100, 0, 200, 0, WIN_HEIGHT - 30, WIN_WIDTH, 70);
	gui_draw_rect(true, 0, 100, 0, 200, 0, WIN_HEIGHT - 750, WIN_WIDTH, 70);
}
	
static void 
ip_address_prompt(void)
{
	ip_prompt_active = true;
	text_input_state = false;		
	gui_state.buf[0] = '\0';
}

int outer_button_gap = 100;

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
				gui_render_input_text(&gui_state, &m.ipaddr, ip_prompt_active);		
			}
			
			if (ip_prompt_active && text_input_state)
			{
				if ((strcmp(gui_state.buf, "TESTME")) == 0) 
				{
					vote_screen_init();
					screen = VOTING;
					ip_prompt_active = false;
					text_input_state = false;
				}		

				else
				{
					text_input_state = false;	
				}
			}

			break;
		case VOTING:
			vote_screen();
			
			break;
		case ADMIN:
			admin();
			
			if (a.input_state[0] == true && text_input_state == true)
			{
				add_position(); 
				a.input_state[0] = false;
				text_input_state = false;
			}


			break;
		case DATA:
			data();

			
			break;

		default:
			break;
	}
}



struct position 
create_position(char *title)
{
	struct position res;
	res.gap = 100;

	if (title)
	{
		size_t n = strlen(title) + 1;
		res.title = malloc(n);
		if (res.title)
			memcpy(res.title, title, n);
		else
		{
			static char empty[] = "";
			res.title = empty;
		}
	}

	else
	{
		static char empty[] = "";
		res.title = empty;
	}

	res.current = false;
	res.queue[0].name = NULL;
	res.queue[1].name = NULL;

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
							select_position	);
	outer_button_gap += 45;

	return res;
}

struct candidate 
create_candidate(	struct position *_pos,	
			char *name,
			char *desc,
			char *image	)
{
	struct candidate res;
	res.name = name;

	if (image)
	{
		res.image = gui_load_texture(image);
	}

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


char *bezos_desc = "Bezos was born in Albuquerque and raised in\nHouston and Miami. He Graduated from Princeton\nUniversity in 1986 to early 1994. Bezos founded Amazon \nin mid-1994 on a road trip from \nNew York City to Seattle\0";

char *elon_desc = "Founder of Space X and owns Tesla and X.\nHe was born in South Africa and developed a game as\na child and co founded PayPal\n";

char *chester_desc = "asdnfk;anwgfinaweiognS;NSI;NDS;ONS;OGSN;OGN;";
char *obama_desc = "Obama is a president of the United\nStates for 8 years and was the first black president\nof the country.";
char *vance_desc = "JD Vance is the current Vice\nPresident of the United States\n";
char *baldes_desc = "carlos";
char *kim_desc = "FIRE IN THE HOLE";
struct position pos[MAX_POSITION];

static void 
vote_screen_init(void)
{	
	generate_identity_code();

	v.in = 0, v.count = 0;
	
	v.ok = gui_create_button(0, 155, 0, 255, (WIN_WIDTH / 2) + 250, (WIN_HEIGHT / 2) + 250, 300, 30, "OK", confirm_vote);

	v.texts[0] = gui_create_text("SELECT CANDIDATE", DEFAULT_FONT, 20, 0, 0, 0, 255, (WIN_WIDTH / 2) + 300, (WIN_HEIGHT / 2) - 300);
	v.texts[1] = gui_create_text("Please enter a candidate before proceeding", DEFAULT_FONT, 20, 255, 0, 0, 255, (WIN_WIDTH / 2) + 300, (WIN_HEIGHT / 2) - 399);

	pos[v.count] = create_position("President");
	pos[v.count] = create_position("Vice President");
	pos[v.count] = create_position("Secretary");
	pos[v.count] = create_position("Health Officer");

	pos[0].queue[0] = create_candidate(&pos[0], "Jeff Bezos", bezos_desc, "res/gfx/bezos.jpg");	
	pos[0].queue[1] = create_candidate(&pos[0], "Elon Musk", elon_desc, "res/gfx/Elon-Musk.jpg");	
	
	pos[1].queue[0] = create_candidate(&pos[1], "Chester", elon_desc, "res/gfx/chester.jpg");	
	pos[1].queue[1] = create_candidate(&pos[1], "Obamna", obama_desc, "res/gfx/obamna.jpg");

	pos[2].queue[0] = create_candidate(&pos[2], "Chester", chester_desc, "res/gfx/chester.jpg");
	pos[2].queue[1] = create_candidate(&pos[2], "JD Vance", vance_desc, "res/gfx/vance.jpg");

	pos[3].queue[0] = create_candidate(&pos[3], "Carlos", baldes_desc, "res/gfx/baldes.jpg");
	pos[3].queue[1] = create_candidate(&pos[3], "Kim", kim_desc, "res/gfx/kim.jpg");

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
	gui_render_button(&gui_state, &v.ok);
	gui_draw_rect(true, 0, 100, 0, 200, (WIN_WIDTH / 2) - 300, (WIN_HEIGHT / 2) - 320, (WIN_WIDTH / 2) - 100, WIN_HEIGHT);

	for (int j = 0; j < v.count; j++)
	{
		gui_render_button(&gui_state, &pos[j].button);
	}	

	gui_render_text(v.texts[0]);

	if (v.in < v.count)
	{
		pos[v.in].button.main_color = (SDL_Color){100, 100, 0, 255};
		 
		for (int i = 0; i < 2; i++)
		{
			gui_render_button(&gui_state, &pos[v.in].queue[i].button);

			if (pos[v.in].queue[i].button.hover)
			{
				gui_render_text(pos[v.in].queue[i].desc_box);
				if (pos[v.in].queue[i].image)
				{	
					blit(pos[v.in].queue[i].image, (WIN_WIDTH / 2) - 250, 50);
				}
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
	// sets the data inside the buffer into 0 to clear it out
	memset(buffer, 0, sizeof(char) * 20);

	if (v.in < v.count)
	{
		for (int i = 0; i < 2; i++)
		{
	
			d.candidate_name[i] = gui_create_text(pos[v.in].queue[i].name, DEFAULT_FONT, 30, 0, 0, 0, 255, (WIN_WIDTH / 2) - 500, y); 
			// stores the integers in a string
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

	gui_render_button(&gui_state, &d.next);
	gui_render_button(&gui_state, &d.back);
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

static user_t
user_init(void)
{
	
}

static void
enter_username(void)
{
	
}

static void
enter_pass(void)
{
					
}

static void
confirmation_screen(void)
{
	gui_render_text(mini_title);
	

}

text_box_t saved_files_text;
button_t saved_files_button;
button_t delete_files_button;

static void
admin_init(void)
{
	char *add = "ADD +";
	char *remove = "REMOVE -";
	char *modify = "MODIFY";

	a.in = -1;
	//pos[0].gap = 100;
	//outer_button_gap = 100;
	v.count = 0;

	a.label[0] = gui_create_text("POSITION" , DEFAULT_FONT, 20, 0, 0, 0, 255, (WIN_WIDTH / 2) - 550, (WIN_HEIGHT / 2) - 300);
	a.label[1] = gui_create_text("CANDIDATE" , DEFAULT_FONT, 20, 0, 0, 0, 255, (WIN_WIDTH / 2) - 100, (WIN_HEIGHT / 2) - 300);

	a.create[0] = gui_create_button(0, 155, 0, 255, (WIN_WIDTH / 2) - 640, (WIN_HEIGHT / 2) + 302, 110, 30, add, add_position_button);
	a.create[1] = gui_create_button(0, 155, 0, 255, (WIN_WIDTH / 2) - 300, (WIN_HEIGHT / 2) + 302, 200, 30, add, add_candidate_button);

	a.remove[0] = gui_create_button(155, 0, 0, 255, ((WIN_WIDTH / 2) - 640) + 110, (WIN_HEIGHT / 2) + 302, 110, 30, remove, NULL);
	a.remove[1] = gui_create_button(155, 0, 0, 255, ((WIN_WIDTH / 2) - 300) + 200, (WIN_HEIGHT / 2) + 302, 200, 30, remove, NULL);
	
	a.modify[0] = gui_create_button(255, 165, 0, 255, (((WIN_WIDTH / 2) - 640) + 110) + 110, (WIN_HEIGHT / 2) + 302, 120, 30, modify, NULL);
	a.modify[1] = gui_create_button(255, 165, 0, 255, (((WIN_WIDTH / 2) - 300) + 200) + 200, (WIN_HEIGHT / 2) + 302, 140, 30, modify, NULL);

	a.file = gui_create_button(0, 222, 0, 255, (WIN_WIDTH / 2) - 650, (WIN_HEIGHT / 2) - 360, 120, 42, "FILE", file_state_button);
	a.edit = gui_create_button(0, 222, 0, 255, ((WIN_WIDTH / 2) - 650) + 122, (WIN_HEIGHT / 2) - 360, 120, 42, "EDIT", NULL);
	a.help = gui_create_button(0, 222, 0, 255, (((WIN_WIDTH / 2) - 650) + 122) + 122, (WIN_HEIGHT / 2) - 360, 120, 42, "HELP", NULL);

	a.file_buttons[0] = gui_create_button(255, 165, 0, 255, (WIN_WIDTH / 2) - 622, (WIN_HEIGHT / 2) - 320, 140, 30, "Open", NULL);
	a.file_buttons[1] = gui_create_button(255, 165, 0, 255, (WIN_WIDTH / 2) - 622, ((WIN_HEIGHT / 2) - 320) + 45, 140, 30, "Save", NULL);
	a.file_buttons[2] = gui_create_button(255, 165, 0, 255, (WIN_WIDTH / 2) - 622, (((WIN_HEIGHT / 2) - 320) + 45) + 45, 140, 30, "Save as", NULL); 
	a.file_buttons[3] = gui_create_button(255, 165, 0, 255, (WIN_WIDTH / 2) - 622, ((((WIN_HEIGHT / 2) - 320) + 45) + 45) + 45, 140, 30, "Close", close_admin);

	screen = ADMIN;

	a.input[0] = gui_create_input_text("Enter position title", DEFAULT_FONT, 13, 255, 255, 255, 255, (WIN_WIDTH / 2) - 250, WIN_HEIGHT / 2, 500, 30);
}

static void
admin(void)
{

	gui_render_text(mini_title);
	gui_draw_rect(true, 0, 100, 0, 200, (WIN_WIDTH / 2) - 300, (WIN_HEIGHT / 2) - 320, (WIN_WIDTH / 2) - 100, WIN_HEIGHT);
	//gui_render_text(mini_title);
	gui_render_text(m.texts[2]);
	
	for (int i = 0; i < 2; i++)
	{
		gui_render_text(a.label[i]);
		gui_render_button(&gui_state, &a.create[i]);
		gui_render_button(&gui_state, &a.remove[i]);
		gui_render_button(&gui_state, &a.modify[i]);
	}

	gui_render_button(&gui_state, &a.file);
	gui_render_button(&gui_state, &a.edit);
	gui_render_button(&gui_state, &a.help);

	for (int i = 0; i < 7; i++)
	{
		gui_render_button(&gui_state, &pos[i].button);
	}

	if (a.tool_state[0] == true)
	{

		gui_draw_rect(a.tool_state[0], 20, 20, 20, 150, (WIN_WIDTH / 2) - 655, (WIN_HEIGHT / 2) - 320, 200, (WIN_HEIGHT / 2) - 100);
		for (int i = 0; i < 4; i++)
		{
			gui_render_button(&gui_state, &a.file_buttons[i]);
		}
	} 

	if (gui_state.mouse.button[SDL_BUTTON_RIGHT])
	{
		a.tool_state[0] = false;
	}

	if (a.input_state[0])
	{
		gui_render_input_text(&gui_state, &a.input[0], a.input_state[0]);
	}	
}

static void
close_admin(void)
{
	menu_init();
	screen = MAIN;
}

static void
select_position(void)
{
	for (int i = 0; i < v.count; i++)
	{
		button_t *btn = &pos[i].button;
		if (btn->clicked)
		{
			selected_pos = i;
			for (int k = 0; k < v.count; k++)
			{
				pos[k].current = false;
				pos[k].button.hover_color = (SDL_Color){0, 155, 0, 255};
				pos[k].button.main_color = (SDL_Color){0, 100, 0, 255};
			}	

			pos[selected_pos].current = true;

			if (pos[selected_pos].current)
			{
				pos[selected_pos].button.main_color = (SDL_Color){0, 155, 0, 255};
				a.pos_state = true;
				printf("selected position index: %d\n", selected_pos);
				if (pos[selected_pos].title)
					printf("Selected position %s\n", pos[selected_pos].title);
				else
					printf("Selected position has no name\n");
			}
		
		}	
	}
}

static void
add_position_button(void) 
{
	a.input_state[0] = true;
	text_input_state = false;
	gui_state.buf[0] = '\0';
}

static void
add_candidate_button(void) 
{
	a.input_state[1] = true;
	text_input_state = false;
	gui_state.buf[0] = '\0';
}

/*
static void
enable_add_position(void)
{
	pos_state = true;
}
*/

static void
add_position(void)
{
	if (v.count >= MAX_POSITION)
	{
		printf("Cannot add position: maximum reached (%d)\n", MAX_POSITION);		
	}

	pos[v.count] = create_position(gui_state.buf);
}

static void
free_position(void)
{

}

/*
static void
add_candidate_button(void)
{
	if (a.pos_state == true)
	{
		
		a.pos_state = false;
	}
}
*/

static void
file_state_button(void)
{
	a.tool_state[0] = true;	
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
