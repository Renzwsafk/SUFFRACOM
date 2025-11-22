#ifndef __SUFFRACOM_H__
#define __SUFFRACOM_H__

#include <stdbool.h>

#include "gui.h"

struct candidate
{
	bool selected;
	char *name, *desc;
	text_box_t desc_box;
	button_t button;
	guitex *image;
	int votes;
	bool add_image;
};

struct position 
{
	char *title;
	button_t button;
	struct candidate queue[20];
	bool current;
	int gap;
	int max;
};

void suffracom_main(int,char**);

/*
struct outer_node *create_outer_node(char*, void(*direct_outer_function)(void));
struct outer_node *set_outer_head(struct outer_node**, struct outer_node*);
*/

struct position create_position(char*);
struct candidate create_candidate(struct position*,char*,char*,char*);

//struct node *set_head(struct node**, struct node*);


#endif // __SUFFRACOM_H__
