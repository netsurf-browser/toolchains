/*
 *	Librairie PopUp (exemple pour LDG)
 *	fichier en tete a inclure par le client
 */

#include <ldg.h>

void init_ldg_popup( LDG *ldgpopup);
void popupinfo( void);

extern char*  __CDECL (*_informe) ( void);
extern void   __CDECL (*_get_bkgr)( long handle, long of_x, long of_y, long of_w, long of_h, MFDB *img);
extern void   __CDECL (*_put_bkgr)( long handle, long of_x, long of_y, long of_w, long of_h, MFDB *img);
extern short  __CDECL (*_popup)   ( OBJECT *menu, long x, long y, long item, short *global);


