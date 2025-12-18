/* 
 * ViewText : just display an ascii file in a WinDom window
 * Dominique B‚r‚ziat (c) 2001
 */

#define WTXT 0x57545854UL /* 'WTXT' */

WINDOW *ViewTextCreate	( char *file, int attrib);
void    ViewTextOpen	( WINDOW *win, int x, int y, int w, int h);
void    ViewTextReCreate( WINDOW *win, char *file);
int     ViewTextSearch	( WINDOW *win, char *target, int lastpos, int capsens);
int 	ViewTextGet		( WINDOW *win, int mode, int *val);

