/* Bindings of POPUP.LDG
 * Compile this module and link it with the application client
 */

#include <gem.h>
#include <ldg.h>

char* __CDECL (*_informe) ( void);
void  __CDECL (*_get_bkgr)( long handle, long of_x, long of_y, long of_w, long of_h, void* img);
void  __CDECL (*_put_bkgr)( long handle, long of_x, long of_y, long of_w, long of_h, void* img);
short __CDECL (*_popup)   ( OBJECT *menu, long x, long y, long item, short *global);

void init_ldg_popup( LDG *ldg_popup) {
	_informe  = ldg_find("__infos", ldg_popup);
	_get_bkgr = ldg_find("__get_bkgr", ldg_popup);
	_put_bkgr = ldg_find("__put_bkgr", ldg_popup);
	_popup    = ldg_find("__popup", ldg_popup);
}

void popupinfo(void) {
	(void)form_alert(1,_informe());
}
