/*
 * WinDom: a high level GEM library
 * Copyright (c) 1997-2006 windom authors (see AUTHORS file)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 *
 * $Source: /cvsroot/windom/windom/examples/component/cal.c,v $
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/01/03 14:39:25 $
 *   $Revision: 1.7 $
 */

#include <mt_wndm.h>
#include <scancode.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


/* data identifiers defines */
#define CDT_TMDATA 0x434f4bUL
#define CDT_TM(app,c) mt_CompDataSearch(app, c, CDT_TMDATA)

#define CDT_COLOR 0x434f4cUL
#define CDT_STRING 0x434f4dUL 
#define CDT_SELECTED 0x434f4eUL 


static void cal_construct( APPvar *app, COMPONENT *cc );


static
void tm_add_duration( struct tm *t, int m ) {
	int mon = t->tm_mon+m;
	if ( mon >= 0 ) {
		t->tm_year += mon / 12;
		t->tm_mon = mon % 12;
	} else {
		t->tm_year += (mon - 11) / 12;
		t->tm_mon = 12 + (mon % 12);
	}
}

/* FIXME: move this to the WinDom instead - we use it all along the COMPONENT sources */
static
void mt_comp_reflow( APPvar *app, WINDOW *win ) {
	short wbuff[8];
	mt_WindGet( app, win, WF_CURRXYWH,
			&wbuff[4], &wbuff[5], &wbuff[6], &wbuff[7] );
	wbuff[0] = CM_REFLOW; /* recompute the nested component sizes */
	mt_EvntExec(app, win, wbuff);
}


static
void __CDECL color_box_redraw( COMPONENT *c, long buff[8], APPvar *app) {
	INT16 xy[4];
	LGRECT work;

	// get the work rectangle...
	mt_CompGetLGrect(app, c, WF_WORKXYWH, &work);

	// fill in the array for v_bar (leave clipping to upper layer)
	xy[0] = work.g_x;
	xy[1] = work.g_y;
	xy[2] = xy[0] + work.g_w - 1;
	xy[3] = xy[1] + work.g_h - 1;

	// set the color
	vsf_color( c->graf->handle, (long)mt_CompDataSearch(app, c, CDT_COLOR) );

	// draw the contents
	vswr_mode( c->graf->handle, MD_REPLACE );
	v_bar( c->graf->handle, xy);
	vswr_mode( c->graf->handle, MD_TRANS );
}


static
void __CDECL string_redraw( COMPONENT *c, long buff[8], APPvar *app) {
	INT16 xy[4];
	LGRECT work;

	// get the work rectangle...
	mt_CompGetLGrect(app, c, WF_WORKXYWH, &work);

	// fill in the array for v_bar (leave clipping to upper layer)
	xy[0] = work.g_x;
	xy[1] = work.g_y;
	xy[2] = xy[0] + work.g_w - 3;
	xy[3] = xy[1] + work.g_h - 3;

	// set the color
	if ( mt_CompDataSearch(app, c, CDT_SELECTED) ) {;
			vsf_color( c->graf->handle, LCYAN);
	} else {
			vsf_color( c->graf->handle, LBLACK);
	}
	v_bar( c->graf->handle, xy);

	v_gtext ( c->graf->handle, work.g_x + 2, work.g_y + 16,
			(char *)mt_CompDataSearch(app, c, CDT_STRING) );
}


static
void __CDECL string_dstry( COMPONENT *c, long buff[8], APPvar *app) {
	free( (char *)mt_CompDataSearch(app, c, CDT_STRING) );
}


static
void __CDECL headline_string_redraw( COMPONENT *c, long buff[8], APPvar *app) {
	LGRECT work;
	char *s = (char *)mt_CompDataSearch(app, c, CDT_STRING);

	// get the work rectangle...
	mt_CompGetLGrect(app, c, WF_WORKXYWH, &work);

	v_gtext ( c->graf->handle, work.g_x + work.g_w/2 - strlen(s)*4, work.g_y + 16, s);
}


static
void cal_update( APPvar *app, COMPONENT *c ) {
	WINDOW *win = mt_CompGetPtr( app, c, CF_WINDOW);

	/* remove all children COMPONENTs */
	COMPONENT *ch = (COMPONENT*)listFirst(c->children);
	while ( ch ) {
		COMPONENT *trash = ch;
		ch = (COMPONENT*)listNext(ch); /* get the next list item */
		listRemove( (LINKABLE*)trash);
		mt_CompDelete( app, trash);
	}

	/* rebuild the calendar structure according to the struct tm* */
	cal_construct( app, c);

	/* reflow the components and redraw the WINDOW */
	if ( win ) {
		mt_comp_reflow( app, win);
		mt_EvntRedraw( app, win);
	}
}


static
void __CDECL cal_next_month( COMPONENT *c, long buff[8], APPvar *app) {
	tm_add_duration( CDT_TM(app,c->parent->parent), 1 );
	cal_update( app, c->parent->parent );
}

static
void __CDECL cal_prev_month( COMPONENT *c, long buff[8], APPvar *app) {
	tm_add_duration( CDT_TM(app,c->parent->parent), -1 );
	cal_update( app, c->parent->parent );
}

static
void __CDECL win_keybrd( WINDOW *win, long buff[8], APPvar *app) {
	switch (app->evnt.keybd) {
		case 0x1615:	/* C-U */
			mt_ApplWrite( app, mt_AppAESapid(app), WM_CLOSED, w_get_hndl(app, win), 0, 0, 0, 0);
			break;
	}
}

static
void __CDECL cal_keybrd( COMPONENT *c, long buff[8], APPvar *app) {

	switch (app->evnt.keybd>>8) {
	case SC_HOME:
		{ /* to current datetime */
			time_t tv = time(0L);
			localtime_r(&tv, CDT_TM(app,c));
			cal_update( app, c);
		}
		break;
	case SC_PGUP:
	case SC_UPARW:
	case SC_LFARW:
		tm_add_duration( CDT_TM(app,c), -1 );
		cal_update( app, c);
		break;		
	case SC_PGDN:
	case SC_DWARW:
	case SC_RTARW:
		tm_add_duration( CDT_TM(app,c), 1 );
		cal_update( app, c);
		break;		
	default:
		{ /* propagate the rest to the WINDOW we are in */
			WINDOW *win = mt_CompGetPtr( app, c, CF_WINDOW);
			win_keybrd( win, buff, app);
		}
	}
}

static
void cal_construct( APPvar *app, COMPONENT *cc ) {
	struct tm *t = CDT_TM( app, cc);
	int m = t->tm_mon+1;
	int y = t->tm_year+1900;
    	static int daysofmonth[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
	int mdays = daysofmonth[m-1] + (m == 2 && !(y % 100 ? y % 4 : y % 400) ? 1 : 0);
	time_t tv = time(0L); /* current datetime */
	struct tm currtime;
	char daystring[20];

	COMPONENT *dc, *rc = NULL;

	int di;
	int wday;

	/* transform the current datetime into 'struct tm' */
	localtime_r(&tv, &currtime);

	{ /* get the weekday of the first day of the month */
		struct tm ct;
		memset( &ct, 0, sizeof(struct tm));
		ct.tm_mday = 1;
		ct.tm_mon = m-1;
		ct.tm_year = y-1900;
		tv = mktime( &ct );
		localtime_r(&tv, &ct);
		wday = ct.tm_wday;
	}

	{ /* header COMPONENTs */
		static char* monname[12] = {
			"Jan", "Feb", "Mar", "Apr", "May", "Jun",
		       	"Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
		};
		static char* dayname[7] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};

		sprintf( daystring, "%s %d", monname[m-1], y );
		rc = mt_CompCreate( app, CLT_HORIZONTAL, 25, 0 );
		mt_CompAttach( app, cc, rc);
		{ /* month & year */
			dc = mt_CompCreate( app, CLT_VERTICAL, 0, 1 );
			mt_CompEvntAttach( app, dc, WM_XBUTTON, cal_prev_month );
			mt_CompAttach( app, rc, dc);

			dc = mt_CompCreate( app, CLT_VERTICAL, 0, 1 );
			mt_CompEvntAttach( app, dc, WM_REDRAW, headline_string_redraw );
			mt_CompEvntAttach( app, dc, WM_DESTROY, string_dstry );
			mt_CompDataAttach( app, dc, CDT_STRING, strdup( daystring) );
			mt_CompAttach( app, rc, dc);

			dc = mt_CompCreate( app, CLT_VERTICAL, 0, 1 );
			mt_CompEvntAttach( app, dc, WM_XBUTTON, cal_next_month );
			mt_CompAttach( app, rc, dc);
		}

		/* day name line */
		rc = mt_CompCreate( app, CLT_HORIZONTAL, 25, 0 );
		mt_CompAttach( app, cc, rc);
		for( di=0; di<7; di++ ) {
			dc = mt_CompCreate( app, CLT_VERTICAL, 0, 1 );
			mt_CompEvntAttach( app, dc, WM_REDRAW, string_redraw );
			mt_CompEvntAttach( app, dc, WM_DESTROY, string_dstry );
			mt_CompDataAttach( app, dc, CDT_STRING, strdup( dayname[di]) );
			mt_CompAttach( app, rc, dc );
		}

		/* split */
		rc = mt_CompCreate( app, CLT_HORIZONTAL, 3, 0 );
		mt_CompEvntAttach( app, rc, WM_REDRAW, color_box_redraw );
		mt_CompDataAttach( app, rc, CDT_COLOR, (void*)BLACK );
		mt_CompAttach( app, cc, rc);
		/* gap */
		rc = mt_CompCreate( app, CLT_HORIZONTAL, 2, 0 );
		mt_CompAttach( app, cc, rc);
	}

	/* month doesn't start on Sunday -> create the week row + indentation */
	if ( wday != 0 ) {
		rc = mt_CompCreate( app, CLT_HORIZONTAL, 0, 1 );
		mt_CompAttach( app, cc, rc);

		dc = mt_CompCreate( app, CLT_VERTICAL, 0, wday );
		mt_CompAttach( app, rc, dc );
	}

	/* all month days in several week rows */
	for( di=1; di<=mdays; di++ ) {
		sprintf( daystring, "%d", di);

		if ( (wday % 7) == 0 ) {
			/* another week row */
			rc = mt_CompCreate( app, CLT_HORIZONTAL, 0, 1 );
			mt_CompAttach( app, cc, rc);
		}

		dc = mt_CompCreate( app, CLT_VERTICAL, 0, 1 );
		mt_CompEvntAttach( app, dc, WM_REDRAW, string_redraw );
		mt_CompEvntAttach( app, dc, WM_DESTROY, string_dstry );
		mt_CompDataAttach( app, dc, CDT_STRING, strdup( daystring) );
		if (	di == currtime.tm_mday &&
				y-1900 == currtime.tm_year &&
				m-1 == currtime.tm_mon )
		{
			mt_CompDataAttach( app, dc, CDT_SELECTED, (void*)1);
		}
		dc->bounds.min_width = 30;
		dc->bounds.min_height = 25;
		mt_CompAttach( app, rc, dc );

		/* increase the day of week (keep it in the range) */
		wday++;
		wday %= 7;
	}

	/* last day of the month isn't Saturday -> pad the rest of the row */
	if ( wday != 0 ) {
		dc = mt_CompCreate( app, CLT_VERTICAL, 0, 7 - wday );
		mt_CompAttach( app, rc, dc );
	}
}

static
void __CDECL cal_dstry( COMPONENT *c, long buff[8], APPvar *app) {
	free( CDT_TM(app,c));
}

static
COMPONENT *cal_create( APPvar *app, struct tm* tdata ) {
	COMPONENT *c = mt_CompCreate( app, CLT_VERTICAL, 0, 1 );
	struct tm *t = malloc(sizeof(struct tm));
	*t = *tdata; /* heap allocated copy */
	mt_CompDataAttach( app, c, CDT_TMDATA, t );
	mt_CompEvntAttach( app, c, WM_DESTROY, cal_dstry );

	cal_construct( app, c);

	mt_CompEvntAttach( app, c, WM_XKEYBD, cal_keybrd );
	mt_CompEvntAttach( app, c, WM_REDRAW, color_box_redraw );
	mt_CompDataAttach( app, c, CDT_COLOR, (void*)LWHITE );

	/* set focus to the itself so that the keyboard events
	 * are sent to this COMPONENT */
	mt_CompSetPtr( app, c, CF_FOCUS, c, NULL);

	return c;
}


int main( void) {
	WINDOW *win;
	APPvar*	app = mt_ApplInit();
	if (!app)
		return -1;

	win = mt_WindCreate( app, NAME|SIZER|MOVER|FULLER|SMALLER|CLOSER|HSLIDE|VSLIDE,
						 app->x, app->y, app->w, app->h);
	{
		/* get the current date struct tm representation */
		struct tm currtime;
		time_t tv = time(0L);
		localtime_r(&tv, &currtime);

		/* attach the calendar component */
		mt_WindSetPtr( app, win, WF_COMPONENT, cal_create( app, &currtime), NULL);
	}

	mt_WindSetStr( app, win, WF_NAME, "Calendar Component Example");

	mt_WindOpen( app, win, 50, 50, 350, 250);
	mt_comp_reflow( app, win); /* assure the contents fits the window position and size */

	for(;;) mt_EvntWindom( app, MU_MESAG|MU_BUTTON|MU_KEYBD);
	return 0;
}
