#include "gui.h"

void create_newwin(win_t *wind, int height, int width, 
		int startx, int starty, const char *string) {
	wind->_height = height;
	wind->_width = width;
	wind->_startx = startx;
	wind->_starty = starty;

	wind->_wind = newwin (height, width, startx, starty);
	wborder(wind->_wind, '|', '|', '-', '-', '*', '*', '*', '*');

	if (string) 
		mvwprintw (wind->_wind, 1, 2, "%s", string);

	wrefresh(wind->_wind);
}

void update_win (win_t *mywin, int height, int width, 
		int startx, int starty, const char *string) {
	destroy_win(mywin);

	create_newwin (mywin, height, width, 
			startx, starty, string);
	wborder (mywin->_wind, '|', '|', '-', '-', '*', '*', '*', '*');

	wrefresh(mywin->_wind);
}

void destroy_win(win_t *local_win) {
	wborder (local_win->_wind, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');
	wrefresh (local_win->_wind);
	delwin(local_win->_wind);
}

void destroy_win_tab (win_t **all_win) {
	int i=0;

	for (i=0; i<WIN_NUMB; i++)
		if (all_win[i])
			destroy_win(all_win[i]);
}

win_t **init_screen () {
	int height, width, startx, starty;
	int i;

	/* Tester le retour */
	int size_struct = sizeof (WINDOW *) + 6*sizeof(int);
	win_t **allwin = (win_t **) malloc (WIN_NUMB * sizeof (win_t *));
	for (i = 0; i < WIN_NUMB; i++)
		allwin[i] = (win_t *) malloc (size_struct);

	initscr();
	cbreak();
	noecho();
	keypad(stdscr, TRUE);

	refresh();

	for (i=0; i<WIN_NUMB; i++)
		init_win (all_win[i]);

	/* Display Window Title Intialization */
	startx = TOP_MARGIN; starty = LEF_MARGIN;
	height = TITLE_HEIGHT; width = COLS * 2 / 3;
	create_newwin (all_win[DISP_WIN_TIT], height, width,
			startx, starty, "Display");

	/* Display Window Initialization */
	startx += height;
	height = LINES - TOP_MARGIN - BOT_MARGIN - INT_MARGIN - TITLE_HEIGHT;
	create_newwin(all_win[DISP_WIN], height, width, startx,
			starty, NULL);

	/* Command Window Title Initialization */
	startx = TOP_MARGIN; starty += INT_MARGIN + width;
	width = COLS - starty - RIG_MARGIN; height = TITLE_HEIGHT;
	create_newwin(all_win[KEYB_WIN_TIT], height, width,
			startx, starty, "Command");

	/* Command Window Initialization */
	startx += height;
	height = COMMD_HEIGHT;
	create_newwin(all_win[KEYB_WIN], height, width, startx,
			starty, NULL );

	/* Info Window Title Initialization */
	startx += COMMD_HEIGHT;
	height = TITLE_HEIGHT;
	create_newwin(all_win[INFO_WIN_TIT], height, width, 
			startx, starty, "Info");

	/* Info Window Initialization */
	startx += TITLE_HEIGHT;
	height = LINES - TOP_MARGIN - 2 * TITLE_HEIGHT - INT_MARGIN - BOT_MARGIN - COMMD_HEIGHT;
	create_newwin(all_win[INFO_WIN], height, width, startx,
			starty, NULL);

	mvwprintw (all_win[KEYB_WIN]->_wind, 1, 2, "Input : ");

	int i=0;
	for (i=0; i<WIN_NUMB; i++)
		wrefresh(all_win[i]->_wind);

	return all_win;
}

void clean_ncurse (win_t **win) {
	destroy_win_tab(win);
	free(win);
	endwin();
}

void init_win (win_t *local_win) {
	local_win->_startx = 0;
	local_win->_starty = 0;
	local_win->_height = 0;
	local_win->_width = 0;
	local_win->_posx = 1;
	local_win->_posy = 2;
	local_win->_wind = NULL;
}

void print_window (win_t *local_win, const char *texte, int posx, int posy){
	int x = posx ? posx : local_win->_posx;
	int y = posy ? posy : local_win->_posy;

	if (texte) {
		mvprintw (local_win, x, y, texte);
		wrefresh(local_win);
	}
	
	getyx (local_win, x, y);
	local_win->_posx = x + 1;
	local_win->_posy = 2;
}

