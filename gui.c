#include "gui.h"

void create_newwin(win_t *wind, int height, int width, 
		int startx, int starty, const char *string) {
	wind->_height = height;
	wind->_width = width;
	wind->_startx = startx;
	wind->_starty = starty;

	wind->_wind = newwin (height, width, starty, startx);
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
	clear();
	cbreak();
	noecho();
	keypad(stdscr, TRUE);

	refresh();

	for (i=0; i<WIN_NUMB; i++)
		init_win (allwin[i]);

	/* Display Window Title Intialization */
	starty = TOP_MARGIN; startx = LEF_MARGIN;
	height = TITLE_HEIGHT; width = COLS * 1 / 2;
	create_newwin (allwin[DISP_WIN_TIT], height, width,
			startx, starty, "Display");

	/* Display Window Initialization */
	starty += height;
	height = LINES - TOP_MARGIN - BOT_MARGIN - INT_MARGIN - 2 * TITLE_HEIGHT - OPTS_HEIGHT;
	create_newwin(allwin[DISP_WIN], height, width, startx,
			starty, NULL);

	/* Option Window Initialisation */
	starty += height;
	height = TITLE_HEIGHT;
	width = COLS - LEF_MARGIN - RIG_MARGIN;
	create_newwin(allwin[OPTS_WIN_TIT], height, width,
			startx, starty, "Options");

	starty += TITLE_HEIGHT;
	height = OPTS_HEIGHT;
	create_newwin(allwin[OPTS_WIN], height, width,
			startx, starty, NULL);

	/* Command Window Title Initialization */
	starty = TOP_MARGIN; startx += INT_MARGIN + allwin[DISP_WIN]->_width;
	width = COLS - startx - RIG_MARGIN; height = TITLE_HEIGHT;
	create_newwin(allwin[KEYB_WIN_TIT], height, width,
			startx, starty, "Command");

	/* Command Window Initialization */
	starty += height;
	height = COMMD_HEIGHT;
	create_newwin(allwin[KEYB_WIN], height, width, startx,
			starty, NULL );

	/* Info Window Title Initialization */
	starty += COMMD_HEIGHT;
	height = TITLE_HEIGHT;
	create_newwin(allwin[INFO_WIN_TIT], height, width, 
			startx, starty, "Info");

	/* Info Window Initialization */
	starty += TITLE_HEIGHT;
	height = LINES - TOP_MARGIN - 3 * TITLE_HEIGHT - INT_MARGIN - BOT_MARGIN - COMMD_HEIGHT - OPTS_HEIGHT;
	create_newwin(allwin[INFO_WIN], height, width, startx,
			starty, NULL);

	mvwprintw (allwin[KEYB_WIN]->_wind, 1, 2, "c : demande controle camera");
	mvwprintw (allwin[KEYB_WIN]->_wind, 2, 2, "q : abandonne controle camera");
	mvwprintw (allwin[KEYB_WIN]->_wind, 3, 2, "ctrl+c : quitte");
	mvwprintw (allwin[KEYB_WIN]->_wind, 4, 2, "fleches : deplace camera");

	for (i=0; i<WIN_NUMB; i++)
		wrefresh(allwin[i]->_wind);

	return allwin;
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
	local_win->_posx = 2;
	local_win->_posy = 2;
	local_win->_wind = NULL;
}

void print_window (win_t *local_win, const char *texte, int posx, int posy){
	int x = posx != 0 ? posx : local_win->_posx;
	int y = posy != 0 ? posy : local_win->_posy;
	char c;

	x += local_win->_startx;
	y += local_win->_starty;
	int xinit = x;
	
	while ((c = *texte++) != '\0') {
		if (c == '\n') {
			if (y + 1 != local_win->_height)
				y++; 
			x = xinit;
		}
		else {
			if (x+2 != local_win->_width)
				mvaddch(y, x++, c);
			else {
				x = xinit;
				y++;
				*texte--;
			}
		}
	}

	local_win->_posx = 2;
	local_win->_posy = y + 1 - local_win->_starty;
}

void fill_opt_wind(win_t **allwin) {
	print_window(allwin[OPTS_WIN], "-h,          affiche l'aide et quitte",2,1);
	print_window(allwin[OPTS_WIN], "-a cli_adr,  specifie l'adresse du client",0,0);
	print_window(allwin[OPTS_WIN], "-A serv_adr, specifie l'adresse du serveur",0,0);
	//print_window(allwin[OPTS_WIN], "-n hostname, specifie le nom d'hote du client",0,0);
	//print_window(allwin[OPTS_WIN], "-N hostname, specifie le nom d'hote du serveur",0,0);
	print_window(allwin[OPTS_WIN], "-P port,     specifie le port de la composante d'emission du serveur",0,0);
	print_window(allwin[OPTS_WIN], "-S port,     specifie le port de la composante de reception du serveur",0,0);
}

