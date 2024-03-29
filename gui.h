#ifndef GUI_H
#define GUI_H

#include <ncurses.h>
#include <string.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <term.h>

#define DISP_WIN 0
#define KEYB_WIN 2
#define INFO_WIN 4
#define OPTS_WIN 6

#define DISP_WIN_TIT 1
#define KEYB_WIN_TIT 3
#define INFO_WIN_TIT 5
#define OPTS_WIN_TIT 7

#define TOP_MARGIN 2
#define BOT_MARGIN 2
#define LEF_MARGIN 3
#define RIG_MARGIN 3
#define INT_MARGIN 2

#define TITLE_HEIGHT  3
#define COMMD_HEIGHT  6
#define OPTS_HEIGHT   7

/* Count of windows which will be displayed on the screen */
#define WIN_NUMB 8

typedef struct {
	int _startx, _starty, _height, _width;
	int _posx, _posy;
	WINDOW *_wind;
} win_t;


void create_newwin(win_t *wind, int height, int width, 
		int startx, int starty, const char *string);

void update_win (win_t *local_win, int height, int width, 
		int startx, int starty, const char *string);

void destroy_win(win_t *local_win);

void destroy_win_tab (win_t **all_win);

win_t **init_screen ();
void init_win (win_t *local_win);

void clean_ncurse (win_t **win);

void print_window (win_t *local_win, const char *texte, int posx, int posy);

void fill_opt_wind(win_t **);

#endif
