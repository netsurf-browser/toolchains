/*
 * XGEM.LDG: a userdef library for windom
 * Copyright (c) 2005 Arnaud Bercegeay
 *
 * some routines are based on Eric REBOUX's XGEM library
 * Copyright (c)1997-2003 Eric Reboux
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
 * $Source: /cvsroot/windom/windom/userdef/xgem/draw.c,v $
 * WinDom header file
 *  
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2005/11/06 18:32:20 $
 *   $Revision: 1.6 $
 */

#include <stdlib.h>
#include <string.h>

#include "global.h"

#include "font_8.h"

#ifdef UDEFTOS

#define UDLIB_LOCAL static

/*  *** workaround for TOS and its small supervisor stack ****
 *
 *  gemlib function that are stack hungry are replaced by the following
 *  function. These replacement are not safe thread but we don't care
 *  because this is for the non-shareable version of the LDG (for monoTOS)
 */

static short vdi_intin[VDI_INTINMAX];
static short vdi_intout[VDI_INTOUTMAX];
static short vdi_ptsin[VDI_PTSINMAX];
static short vdi_ptsout[VDI_PTSOUTMAX];
static short vdi_control[VDI_CNTRLMAX];

static VDIPB vdi_params = {
        &vdi_control[0],
        &vdi_intin[0],
        &vdi_ptsin[0],
        &vdi_intout[0],
        &vdi_ptsout[0] 
};

static void my_v_ftext( int handle, int x, int y, char *str) {
	static short i;

	i = vdi_str2array(str, vdi_intin);
	vdi_ptsin[0] = x;
	vdi_ptsin[1] = y;

	vdi_control[0] = 241;
	vdi_control[1] = 1;
	vdi_control[3] = i;
	vdi_control[5] = 0;
	vdi_control[6] = handle;
	vdi(&vdi_params);
}

static void my_v_gtext (int handle, int x, int y, char *str) {
	vdi_ptsin[0] = x;
	vdi_ptsin[1] = y;
	
	vdi_control[0] = 8;
	vdi_control[1] = 1;
	vdi_control[3] = vdi_str2array (str, vdi_intin);
	vdi_control[5] = 0;
	vdi_control[6] = handle;

	vdi (&vdi_params);
}

static void my_vqt_extent (int handle, char *str, INT16 extent[]) {
	static int i;
	static INT16 *ptr;

	vdi_control[0] = 116;
	vdi_control[1] = vdi_control[5] = 0;
	vdi_control[3] = vdi_str2array (str, vdi_intin);
	vdi_control[6] = handle;

	vdi (&vdi_params);

	ptr = vdi_ptsout;
	i = 8;
	do
	{
		*(extent ++) = *(ptr ++);
	} while (-- i);
}


static int my_vsl_color (int handle, int idx) {
	vdi_intin[0] = idx;

	vdi_control[0] = 17;
	vdi_control[1] = vdi_control[5] = 0;
	vdi_control[3] = 1;
	vdi_control[6] = handle;

	vdi (&vdi_params);

	return ((int)vdi_intout[0]);
}

#define v_ftext my_v_ftext
#define v_gtext my_v_gtext
#define vqt_extent my_vqt_extent
#define vsl_color my_vsl_color

/* *** end of the workaround for TOS *** */

#else /* UDEFTOS */

#define UDLIB_LOCAL 

#endif /* UDEFTOS */



static void inline my_rc_set( GRECT *rect, short x, short y, short w, short h) {
        rect->g_x = x;
        rect->g_y = y;
        rect->g_w = w;
        rect->g_h = h;
}
#define rc_set my_rc_set

typedef enum {
	NO_SHADING,
	SHADING_LIGHT, 
	SHADING_LIGHT2DARK, 
	SHADING_DARK 
} SHADING_MODE;

#define SHADING_COLOR_OFFSET 100

static void shading_bar (short vdi_handle, const GRECT * area, SHADING_MODE mode)
{
	short i, j, c;
	short back, rgb[3], rgb_bak[3], sys_pxy[4];
	short px, py, pw, ph;
	
	px = area->g_x;
	py = area->g_y;
	pw = area->g_x + area->g_w - 1;
	ph = area->g_y + area->g_h - 1;

	vswr_mode( vdi_handle, MD_REPLACE);
	
	/* TODO: may be configurable */
	back = LWHITE;

	vq_color (vdi_handle, back, 0, rgb_bak);
	vsf_color (vdi_handle, back);
	vsf_interior (vdi_handle, FIS_SOLID);

	sys_pxy[0] = px;
	sys_pxy[2] = pw;
	if (((i = ph - py + 1) == 0) || ((j = (SHADING_COLOR_OFFSET + SHADING_COLOR_OFFSET) / i) == 0))
		j = 1;
	
	if ((c = rgb_bak[0] + SHADING_COLOR_OFFSET) > 1000)
		c = 1000;
	rgb[0] = c;
	if ((c = rgb_bak[1] + SHADING_COLOR_OFFSET) > 1000)
		c = 1000;
	rgb[1] = c;
	if ((c = rgb_bak[2] + SHADING_COLOR_OFFSET) > 1000)
		c = 1000;
	rgb[2] = c;

	if (mode == SHADING_LIGHT) {
		vs_color (vdi_handle, back, rgb);
		sys_pxy[1] = py;
		sys_pxy[3] = ph;
		vr_recfl (vdi_handle, sys_pxy);
	} else if (mode == SHADING_LIGHT2DARK) {
		for (i = py; i < ph; i += 2)
		{
			if ((c = rgb[0] - j) < 0)
				c = 0;
			rgb[0] = c;
			if ((c = rgb[1] - j) < 0)
				c = 0;
			rgb[1] = c;
			if ((c = rgb[2] - j) < 0)
				c = 0;
			rgb[2] = c;

			vs_color (vdi_handle, back, rgb);
			sys_pxy[1] = i;
			sys_pxy[3] = i + 1;
			vr_recfl (vdi_handle, sys_pxy);
		}
		if ((ph - py + 1) & 1)
		{
			sys_pxy[1] = sys_pxy[3] = i;
			vr_recfl (vdi_handle, sys_pxy);
		}
	} else /* if (mode == SHADING_DARK) */ {
		if ((c = rgb_bak[0] - SHADING_COLOR_OFFSET) < 0)
			c = 0;
		rgb[0] = c;
		if ((c = rgb_bak[1] - SHADING_COLOR_OFFSET) < 0)
			c = 0;
		rgb[1] = c;
		if ((c = rgb_bak[2] - SHADING_COLOR_OFFSET) < 0)
			c = 0;
		rgb[2] = c;
		vs_color (vdi_handle, back, rgb);
		sys_pxy[1] = py;
		sys_pxy[3] = ph;
		vr_recfl (vdi_handle, sys_pxy);
	}

	vs_color (vdi_handle, back, rgb_bak);
}


static void draw_text_font_8( short vdih, short xf, short yf, short style, short color, const char *str)
{
	MFDB pic;
	MFDB scr = {0};
	short fx_mem[16];
	short sys_pxy[20];
	short italic_offset;
	short c;
	short char_width;
	short tcolor[2];
	
	tcolor[0] = color;
	tcolor[1] = WHITE;
	
	pic.fd_addr = fx_mem;
	pic.fd_w = 16;
	pic.fd_h = 8;
	pic.fd_wdwidth = 1;
	pic.fd_stand = 0;
	pic.fd_nplanes = 1;
	
	sys_pxy[0] = 0;
	sys_pxy[1] = 0;
	sys_pxy[2] = 0;
	sys_pxy[3] = 7;
	
	sys_pxy[4] = xf;
	sys_pxy[5] = yf;
	sys_pxy[6] = 0;
	sys_pxy[7] = yf + 7;
	
	if (style & ULINE) /* underline */
	{
		sys_pxy[3]++;
		sys_pxy[7]++;
	}
	
	italic_offset = 0;
	
	while ( (c = *str++) != 0) {
		char *model = (char*) &my_font_8[(c - 1) * 4] ;
		char *dest = (char*)fx_mem;
		
		char_width = ofwf[c];
		
		memset(fx_mem,0,20);
		*dest = *model++; dest+=2;
		*dest = *model++; dest+=2;
		*dest = *model++; dest+=2;
		*dest = *model++; dest+=2;
		*dest = *model++; dest+=2;
		*dest = *model++; dest+=2;
		*dest = *model++; dest+=2;
		*dest = *model;
		
		if (style & ULINE)  /* underline */
		{
			static short underline_table[]={0x00,0x80,0xC0, 0xE0, 0xF0, 0xF8, 0xFC, 0xFE, 0xFF};
			fx_mem[2] = 0;
			fx_mem[2] |= underline_table[char_width] << 8;
		}
		
		if (style & BOLD) /* bold */
		{
			long *lvalue = (long*)fx_mem;
			
			char_width++;
			*lvalue++ = *lvalue | (*lvalue>>1);
			*lvalue++ = *lvalue | (*lvalue>>1);
			*lvalue++ = *lvalue | (*lvalue>>1);
			*lvalue++ = *lvalue | (*lvalue>>1);
			*lvalue   = *lvalue | (*lvalue>>1);
		}
		
		if (style & ITALIC) /* italic */
		{
			short *w = fx_mem;
			
			*w++ = (*w >> 3);
			*w++ = (*w >> 3);
			*w++ = (*w >> 2);
			*w++ = (*w >> 2);
			*w++ = (*w >> 1);
			*w++ = (*w >> 1);
			
			italic_offset = 3;
		}
		
		if (style & INVERSE)  /* inverse */
		{
			long *lvalue = (long*)fx_mem;
			
			*lvalue++ = ~(*lvalue);
			*lvalue++ = ~(*lvalue);
			*lvalue++ = ~(*lvalue);
			*lvalue++ = ~(*lvalue);
		}
		
		if (style & SHADOW) /* shadow */
		{
			/* TODO */
		}
		
		if (style & LIGHT) /* light */
		{
			long *lvalue = (long*)fx_mem;
			
			*lvalue++ = *lvalue & 0x5555AAAAL;
			*lvalue++ = *lvalue & 0x5555AAAAL;
			*lvalue++ = *lvalue & 0x5555AAAAL;
			*lvalue++ = *lvalue & 0x5555AAAAL;
			*lvalue   = *lvalue & 0x5555AAAAL;
		}
		
		if (style & MONOSPACE) /* monospaced */
		{
			char_width = 8;
		}
		
		sys_pxy[2] = char_width + italic_offset - 1;
		sys_pxy[6] = sys_pxy[4] + sys_pxy[2];
		
		/*vdi*/
		vrt_cpyfm( vdih, 2, sys_pxy, &pic, &scr, tcolor);
		
		sys_pxy[4] += char_width;
	}
	
}


/**  set the clip area inside the object borders
 *
 *  @param vdih handle of the (virtual) workstation for vdi outputs
 *  @param pblk
 *  @param border width of the (inside) border. border shall be positive.
 *
 *  @return 0 if the clipping area is empty (no need to redraw),
 *          or 1 if the clipping area has been set.
 *
 */

static int
clip_inside( short vdih, PARMBLK *pblk, short border)
{
	UDLIB_LOCAL GRECT grect, gclip;
	UDLIB_LOCAL INT16 tab[4];
	
	memcpy( &grect, &pblk->pb_x , sizeof(GRECT));
	memcpy( &gclip, &pblk->pb_xc, sizeof(GRECT));

	grect.g_x += border;
	grect.g_y += border;
	grect.g_w -= border + border;	if (grect.g_w <= 0) return 0;
	grect.g_h -= border + border;	if (grect.g_h <= 0) return 0;

	rc_intersect( &grect, &gclip);
	if (gclip.g_w <= 0 || gclip.g_h <= 0) return 0;
	
	grect_to_array( &gclip, tab);
	vs_clip( vdih, 1, tab);
	
	return 1;
}


/**  draw the background of userdef objects
 *
 *  @param vdih handle of the (virtual) workstation for vdi outputs
 *  @param pblk
 *  @param dx
 *  @param dy
 *  @param monopat
 *  @param nplanes number of planes in vdih
 *
 */

static void 
draw_background( short vdih, const GRECT *grect, int dx, int dy, SHADING_MODE mode) 
{
	UDLIB_LOCAL GRECT area;
		
	area.g_x = grect->g_x - dx;
	area.g_y = grect->g_y - dy;
	area.g_w = grect->g_w + dx + dx;
	area.g_h = grect->g_h + dy + dy;
	
	shading_bar (vdih, &area, mode);
}


/**  draw the frame of 3D objects (border)
 *
 *  @param vdih handle of the (virtual) workstation for vdi outputs
 *  @param pblk
 *  @param offset distance between the border line to draw and the outside
 *         of the pblk object (border will be drawn outside if positive; inside
 *         if negative) 
 *  @param col1 color for the left and upper border lines
 *  @param col2 color for the right and bottom border lines
 *
 *  <pre>
 *		---------.
 *		|        .     -- (col1)
 *		..........     .. (col2)
 *  </pre>
 */

static void 
draw_frame( int vdih, PARMBLK *pblk, int offset, int col1, int col2) 
{	
	UDLIB_LOCAL INT16 tab[12];
	
	tab[0] = tab[2] = pblk->pb_x - offset;
	tab[1]          = pblk->pb_y + pblk->pb_h - 1 + offset;
	tab[3] = tab[5] = pblk->pb_y - offset;
	tab[4]          = pblk->pb_x + pblk->pb_w - 1 + offset;
	
	vsl_color( vdih, col1);
	v_pline( vdih, 3, tab);
	
	tab[2] = tab[4];
	tab[3] = tab[1];
	
	vsl_color( vdih, col2);
	v_pline( vdih, 3, tab);
}


/**  compute a string for formular with shortcut
 *
 *  @param src the string stored in the OBJECT, with the "shortcut" syntax
 *         (character '[' before the shortcut character),
 *  @param dst this function filled this string with the string to display
 *         (ie the source string without the shortcut symbol)
 *  @return the position of the character which is the shortcut
 *
 */
 
static int 
find_shortcut( const char *src, char *dst) 
{
	UDLIB_LOCAL char *p;
	UDLIB_LOCAL int pos;
	
	pos = -1;

	strncpy( dst, src, 127);

	if( (p = strchr( dst, '[')) != NULL ) {
		pos = (int) (p - dst);
		*p = '\0';
		strcat( dst, p+1);
	}
	return pos;
}


/** pakage of parameters for the function draw_text
 */

typedef struct {
	/** horizontal alignement of the text */
	enum {
		halign_left,	/**< text is left aligned */
		halign_center,	/**< text is centered */
		halign_right	/**< text is right aligned */
	} halign;
	/** vertical alignment of the text */
	enum {
		valign_top,		/**< text is displayed on the top part of the area */
		valign_center,	/**< text is displayed on the middle part of the area */
		valign_bottom	/**< text is displayed on the bottom part of the area */
	} valign;
	short draw_disabled;		/**< do not use specific attribute for DISABLED text */
	short may_have_shortcut;	/**< 0 if the character [ shall not be taken as a shortcut marker */
} DRAW_TEXT_PARAMETERS;


/**  Draw a text
 *
 *  @param vdih handle of the (virtual) workstation for vdi outputs
 *  @param obstate bits 9 to 14 are checked. These bits contains the text attributes
 *         defined as following:
 *         - bit 9: bold
 *         - bit 10: underlined
 *         - bit 11: italic
 *         - bit 12: reverse video mode
 *         - bit 13: shadowed
 *         - bit 14: light (<16 colors) or grey (>=16 colors)
 *         .
 *         The DISABLED bit is also checked (similar to bit 14) if parm->draw_disabled is true.
 *  @param str
 *  @param outbox
 *  @param parm
 *  @param attrib
 *  @param draw_shortcut
 *
 */

static void
draw_text( XOBJC_DATA *uldata, int obstate, const char *str, GRECT *outbox, 
           DRAW_TEXT_PARAMETERS *parm, short *attrib, int draw_shortcut ) 
{
	UDLIB_LOCAL int effect;
	UDLIB_LOCAL int x, y;
	UDLIB_LOCAL INT16 att[12];
	UDLIB_LOCAL int shortcut;
	UDLIB_LOCAL char txt[128];
	UDLIB_LOCAL int h_char, h_cell;
	UDLIB_LOCAL char c;
	UDLIB_LOCAL INT16 tab[12];
	UDLIB_LOCAL int vdih;
	
	effect = 0;
	shortcut = -1;
	vdih = uldata->vdih;
	
	if (attrib == NULL) attrib=att;

	/* set the text effects */
	if( obstate & STATE9)  effect |= BOLD;
	if( obstate & STATE10) effect |= ULINE;
	if( obstate & STATE11) effect |= ITALIC;
	if( obstate & STATE12) effect |= INVERSE;
	if( obstate & STATE13) effect |= SHADOW;
	
	/* short cut */
	if (parm->may_have_shortcut)
		shortcut = find_shortcut( str, txt);
	else
		strcpy( txt, str);
	
	/* compute text position */
	
	vst_effects( vdih, effect);
	if( obstate & STATE14 || (obstate & DISABLED && parm->draw_disabled ))
		vst_color( vdih, LBLACK);

	vqt_attributes( vdih, att);
	h_char = att[7];
	h_cell = att[9];
	
	vqt_extent( vdih, txt, attrib);

	switch (parm->halign) {
	case halign_left:	x = outbox->g_x; break;
	case halign_right:	x = outbox->g_x + outbox->g_w - (attrib[2] - attrib[0]) - 1; break;
	case halign_center:	x =  outbox->g_x + (outbox->g_w - (attrib[2] - attrib[0])) / 2 - 1; break;
	default: return; /* error */
	}
	
	switch (parm->valign) {
	case valign_top:	y = outbox->g_y + h_char - 1; break;
	case valign_bottom:	y = outbox->g_y + outbox->g_h - (h_cell - h_char) - 1; break;
	case valign_center:	y = outbox->g_y + (outbox->g_h + h_char) / 2 - 1; break;
	default: return;/* error */
	}
	
	
	/* draw text */
	
	if( effect & SHADOW && !(effect & LIGHT)) {
		vst_effects( vdih, effect | LIGHT);
		if (uldata->has_fsmgdos)	v_ftext( vdih, x+2,  y+2, txt);
		else						v_gtext( vdih, x+2,  y+2, txt);
		vst_effects( vdih, effect);
	}
	
	if (uldata->has_fsmgdos)	v_ftext( vdih, x,  y, txt);
	else						v_gtext( vdih, x,  y, txt);

	/* draw shortcut */
	
	if (draw_shortcut && shortcut >= 0) {
		tab[1] = tab[3] = y + (h_cell - h_char) - 1;

		c = txt[shortcut+1];
		txt[shortcut+1] = '\0';
		vqt_extent( vdih, txt, att);
		txt[shortcut+1] = c;
		tab[2] = x + att[2] - att[0] - 1;

		c = txt[shortcut];
		txt[shortcut] = '\0';
		vqt_extent( vdih, txt, att);
		txt[shortcut] = c;
		tab[0] = x + att[2] - att[0] - 1;

		vsl_color( vdih, CONF(uldata)->key_color);
		v_pline( vdih, 2, tab);
	}
}

static void
draw_text_8( XOBJC_DATA *uldata, int obstate, const char *str, GRECT *outbox, 
           DRAW_TEXT_PARAMETERS *parm, short *attrib, int draw_shortcut ) 
{
	UDLIB_LOCAL int effect;
	UDLIB_LOCAL int x, y;
	UDLIB_LOCAL INT16 att[12];
	UDLIB_LOCAL int shortcut;
	UDLIB_LOCAL char txt[128];
	UDLIB_LOCAL int h_char, h_cell;
#if 0
	UDLIB_LOCAL char c;
	UDLIB_LOCAL INT16 tab[12];
#endif
	UDLIB_LOCAL int vdih;
	
	effect = 0;
	shortcut = -1;
	vdih = uldata->vdih;
	
	if (attrib == NULL) attrib=att;

	/* set the text effects */
	if( obstate & STATE9)  effect |= BOLD;
	if( obstate & STATE10) effect |= ULINE;
	if( obstate & STATE11) effect |= ITALIC;
	if( obstate & STATE12) effect |= INVERSE;
	if( obstate & STATE13) effect |= SHADOW;
	
	/* short cut */
	if (parm->may_have_shortcut)
		shortcut = find_shortcut( str, txt);
	else
		strcpy( txt, str);
	
	/* compute text position */
	
	vst_effects( vdih, effect);
	if( obstate & STATE14 || (obstate & DISABLED && parm->draw_disabled ))
		vst_color( vdih, LBLACK);

	h_char = 7;
	h_cell = 8;
	
	vqt_extent( vdih, txt, attrib); /* FIXME: calculate width using specific (todo-)function */

	switch (parm->halign) {
	case halign_left:	x = outbox->g_x; break;
	case halign_right:	x = outbox->g_x + outbox->g_w - (attrib[2] - attrib[0]) - 1; break;
	case halign_center:	x =  outbox->g_x + (outbox->g_w - (attrib[2] - attrib[0])) / 2 - 1; break;
	default: return; /* error */
	}
	
	switch (parm->valign) {
	case valign_top:	y = outbox->g_y + h_char - 1; break;
	case valign_bottom:	y = outbox->g_y + outbox->g_h - (h_cell - h_char) - 1; break;
	case valign_center:	y = outbox->g_y + (outbox->g_h + h_char) / 2 - 1; break;
	default: return;/* error */
	}
	
	
	/* draw text */
	
	y -= 8;
	draw_text_font_8( vdih, x, y, effect, BLACK, txt);
	
#if 0
	/* draw shortcut */
	
	if (draw_shortcut && shortcut >= 0) {
		tab[1] = tab[3] = y + (h_cell - h_char);

		c = txt[shortcut+1];
		txt[shortcut+1] = '\0';
		vqt_extent( vdih, txt, att);
		txt[shortcut+1] = c;
		tab[2] = x + att[2] - att[0] - 1;

		c = txt[shortcut];
		txt[shortcut] = '\0';
		vqt_extent( vdih, txt, att);
		txt[shortcut] = c;
		tab[0] = x + att[2] - att[0] - 1;

		vsl_color( vdih, CONF(uldata)->key_color);
		v_pline( vdih, 2, tab);
	}
#endif	
}

/** set the text attribute of an object
 *
 *  @param uldata the userdef library private data
 *  @param att the text attributes to set
 *  @retval same as vst_font()
 *
 */

static int 
set_attrib( XOBJC_DATA *uldata, ATTRIB *att) {
	UDLIB_LOCAL INT16 dum;
	UDLIB_LOCAL short res;
	UDLIB_LOCAL short vdih;
	
	vdih = uldata->vdih;
	
	if (uldata->has_fsmgdos)
		res = vst_font( vdih, att->font);
	else 
		res = SYSFONT;
	
	if( att->size) 
		vst_point( vdih, att->size, &dum, &dum, &dum, &dum);
	
	if( att->color >= 0) 
		vst_color( vdih, att->color);
	
	return res;
}


/** set the text attribute of an TEDINFO
 *
 *  @param uldata the userdef library private data
 *  @param attrib the text attributes to set
 *  @param smallsize size of the font if the tedinfo specifies the use of a "small font". Otherwise
 *         (normal font) the font size in attribut is used.
 *  @param hchar will be filled with the height of a character
 *  @param hcell will be filled with the height of a cell that includes characters (refer to 
 *         vst_point() for details).
 *
 *  @return same as vst_font()
 *
 */

static void 
set_attrib_xtedinfo( XOBJC_DATA *uldata, ATTRIB *attrib, int smallsize, TEDINFO *tedinfo, INT16 *hchar, INT16 *hcell) {
	UDLIB_LOCAL int fontid;
	UDLIB_LOCAL INT16 dum;
	UDLIB_LOCAL short vdih;
	
	vdih = uldata->vdih;
	
	if (uldata->has_fsmgdos)
		fontid = vst_font( vdih, attrib->font);
	else 
		fontid = SYSFONT;

	if( fontid == SYSFONT) 
		vst_point (vdih, tedinfo->te_font==5?8:10, &dum, hchar, &dum, hcell);
	else 
		vst_point (vdih, tedinfo->te_font==5?smallsize:attrib->size, &dum, hchar, &dum, hcell);
	
	if( attrib->color >= 0) 
		vst_color( vdih, attrib->color);
	else
		vst_color( vdih, (tedinfo->te_color >> 8 ) & 0x0F);
}


/** draw an I_BOX object
 *
 *  @param vdih
 *  @param nplanes
 *  @param grect
 *  @param selected
 *  @param draw3d
 *  @param border
 *  @param color
 *
 */

static void 
draw_ibox3D( int vdih, GRECT *grect, int selected, int border, int color) 
{
	UDLIB_LOCAL int b;
	UDLIB_LOCAL int x1,y1,x2,y2;
	UDLIB_LOCAL int i;
	UDLIB_LOCAL INT16 p[40]; /* border max = 10px (10*4=40) */
	UDLIB_LOCAL INT16 tab[12];
	
	grect_to_array( grect, tab);
	x1 = tab[0]; y1 = tab[1];
	x2 = tab[2]; y2 = tab[3];

	vswr_mode (vdih, MD_REPLACE);

	/* white border (upper/left border) */
	if (selected)
		vsl_color (vdih, LBLACK);
	else 
		vsl_color (vdih, WHITE);

	for( b=border>1?1:0; b<border; b++ ) {
		tab[0] = x1 + b;
		tab[1] = y2 - b - 1;
		tab[2] = x1 + b;
		tab[3] = y1 + b;
		tab[4] = x2 - b - 1;
		tab[5] = y1 + b;
		v_pline( vdih, 3, tab);
	}

	/* dark grey border (lower/right border) */
	if (selected)
		vsl_color (vdih, WHITE);
	else
		vsl_color (vdih, LBLACK);

	for( b=border>1?1:0; b<border; b++ ) {
		tab[0] = x2 - b;
		tab[1] = y1 + b + 1;
		tab[2] = x2 - b;
		tab[3] = y2 - b;
		tab[4] = x1 + b + 1;
		tab[5] = y2 - b;
		v_pline (vdih, 3, tab);
	}
	
	/* frontier between the dark grey border and the white border 
	 * (ie the upper/right pixel of the border and the lower/left pixel
	 * of the border) are always drawn using light grey colour
	 */
	i = 0;
	for( b=border>1?1:0; b<border && i<40; b++ ) {
		p[i++] = x2 - b;
		p[i++] = y1 + b;
		p[i++] = x1 + b;
		p[i++] = y2 - b;
	}
	vsm_color(vdih, LWHITE);
	vsm_type(vdih,1);
	v_pmarker(vdih,i/2,p);
	
	/* the border (if border is bold enought) using the given colour */
	if (border > 1) {
		tab[0] = tab[6] = tab[8] = x1;
		tab[1] = tab[3] = tab[9] = y1;
		tab[2] = tab[4] = x2;
		tab[5] = tab[7] = y2;
		vsl_color (vdih, color);
		v_pline (vdih, 5, tab);
	}
}


/** draw a BOX object
 *
 *  @param vdih
 *  @param nplanes
 *  @param grect
 *  @param selected
 *  @param draw3d
 *  @param border
 *  @param bordercol
 *  @param fillpattern
 *  @param interiorcol
 *  
 */

static void	
draw_box3D( int vdih, GRECT *grect, int selected, int border, int bordercol, 
            int fillpattern, int interiorcol, SHADING_MODE mode, int root_x, int root_y) 
{
	UDLIB_LOCAL INT16 tab[12];	
	
	if (mode != NO_SHADING) {
		draw_background( vdih, grect, 0, 0, mode);
	} else	{
		if (fillpattern==0 && interiorcol==0) {
			fillpattern = 7 ;  		/* 100% */
			interiorcol = LWHITE ;  /* light grey */
		}

		grect_to_array( grect, tab);
		vswr_mode (vdih, MD_REPLACE);

		/* background (pattern) */

		switch ( fillpattern ) {
		case 0:
			vsf_interior (vdih, 0);
			break;
		case 1:	case 2:	case 3:
		case 4:	case 5:	case 6:
			vsf_interior (vdih, FIS_PATTERN);
			vsf_style (vdih, fillpattern);
			break;
		case 7:
			vsf_interior (vdih, FIS_SOLID);
			break;
		}
		vsf_color (vdih, interiorcol);

		tab[0] += border;
		tab[1] += border;
		tab[2] -= border;
		tab[3] -= border;	
		vr_recfl (vdih, tab);
	}
	
	/* border */
	
	draw_ibox3D(vdih, grect, selected, border, bordercol);
}


/** add a DISABLED effect to 3D objects
 *
 *  @param vdih
 *  @param grect
 *  @param border
 *  
 */
 
static void 
draw_disabled3D( int vdih, GRECT *grect, int border) 
{
	UDLIB_LOCAL INT16 tab[12];
	
	grect_to_array( grect, tab);

	tab[0] += border;
	tab[1] += border;
	tab[2] -= border;
	tab[3] -= border;	

	vswr_mode (vdih, MD_TRANS);
	vsf_color (vdih, WHITE);
	vsf_interior (vdih, FIS_PATTERN);
	vsf_style (vdih, 4);
	vr_recfl (vdih, tab);
}





/* ************************************************************************************* *
 *									 STANDARD 3D OBJECTS                                 *
 * ************************************************************************************* */

short __CDECL ub_ibox3d( PARMBLK *pblk) 
{
	UDLIB_LOCAL short border;
	UDLIB_LOCAL BFOBSPEC obspec;
	UDLIB_LOCAL INT16 tabclip[4];
	UDLIB_LOCAL GRECT grect;
	UDLIB_LOCAL XOBJC_DATA *uldata;
	UDLIB_LOCAL short vdih;

	uldata = ((W_PARM*)(pblk->pb_parm))->wp_uldata;	
	vdih = uldata->vdih;
	memcpy( &grect, &pblk->pb_x , sizeof(GRECT));
	memcpy( &obspec, &(((W_PARM *)(pblk->pb_parm))->wp_spec), sizeof(BFOBSPEC));
	border = obspec.framesize;
	if (border < 0) border = -border; /* we need the border width (ie border without sign) */

	/* clipping area */ 
	grect_to_array( (GRECT*) &pblk->pb_xc, tabclip);
	vs_clip( vdih, 1, tabclip);
	
	/* draw empty box */
	draw_ibox3D( vdih, &grect, IS_STATE(pblk,SELECTED), border, obspec.framecol);
	
	if ( IS_STATE(pblk,DISABLED) )
		draw_disabled3D( vdih, &grect, border);
	
	vs_clip_off( vdih);

	return 0;
}


short __CDECL ub_box3d( PARMBLK *pblk) 
{
	UDLIB_LOCAL short border;
	UDLIB_LOCAL BFOBSPEC obspec;
	UDLIB_LOCAL INT16 tabclip[4];
	UDLIB_LOCAL GRECT grect;
	UDLIB_LOCAL XOBJC_DATA *uldata;
	UDLIB_LOCAL short vdih;

	uldata = ((W_PARM*)(pblk->pb_parm))->wp_uldata;	
	vdih = uldata->vdih;
	memcpy( &grect, &pblk->pb_x , sizeof(GRECT));
	memcpy( &obspec, &(((W_PARM *)(pblk->pb_parm))->wp_spec), sizeof(BFOBSPEC));
	border = obspec.framesize;
	if (border < 0) border = -border; /* we need the border width (ie border without sign) */

	/* clipping area */ 
	grect_to_array( (GRECT*) &pblk->pb_xc, tabclip);
	vs_clip( vdih, 1, tabclip);
	
	/* draw empty box */
	draw_box3D( vdih, &grect, IS_STATE(pblk,SELECTED), border, obspec.framecol, obspec.fillpattern, obspec.interiorcol,
	            NO_SHADING, pblk->pb_tree->ob_x, pblk->pb_tree->ob_y);
	
	if ( IS_STATE(pblk,DISABLED) )
		draw_disabled3D( vdih, &grect, border);
	
	vs_clip_off( vdih);

	return 0;
}


short __CDECL ub_boxchar3d( PARMBLK *pblk) 
{
	static DRAW_TEXT_PARAMETERS dtp = {halign_center, valign_center, 0, 0};
	
	UDLIB_LOCAL short border;
	UDLIB_LOCAL BFOBSPEC obspec;
	UDLIB_LOCAL char c[2];
	UDLIB_LOCAL INT16 tabclip[4];
	UDLIB_LOCAL GRECT grect;
	UDLIB_LOCAL GRECT gtext;
	UDLIB_LOCAL XOBJC_DATA *uldata;
	UDLIB_LOCAL short vdih;
	UDLIB_LOCAL ATTRIB attrib;

	uldata = ((W_PARM*)(pblk->pb_parm))->wp_uldata;	
	vdih = uldata->vdih;
	memcpy( &grect, &pblk->pb_x , sizeof(GRECT));
	memcpy( &obspec, &(((W_PARM *)(pblk->pb_parm))->wp_spec), sizeof(BFOBSPEC));
	border = obspec.framesize;
	if (border < 0) border = -border; /* we need the border width (ie border without sign) */

	/* clipping area */ 
	grect_to_array( (GRECT*) &pblk->pb_xc, tabclip);
	vs_clip( vdih, 1, tabclip);
	
	/* draw empty box */
	draw_box3D( vdih, &grect, IS_STATE(pblk,SELECTED), border, obspec.framecol, obspec.fillpattern, obspec.interiorcol, 
	            NO_SHADING, pblk->pb_tree->ob_x, pblk->pb_tree->ob_y);
	

	/* draw the character */
	
	if ( clip_inside( vdih, pblk, border) ) {	
		attrib.font  = SYSFONT;
		attrib.size  = 10;
		attrib.color = obspec.textcol;

		set_attrib( uldata, &attrib);

		c[0] = obspec.character;
		c[1] = 0;

		gtext = grect;
		if ( IS_STATE(pblk,SELECTED) ) {
			gtext.g_x += 1;
			gtext.g_y += 1;
		}

		vswr_mode( vdih, obspec.textmode? MD_REPLACE : MD_TRANS);	
		draw_text( uldata, pblk->pb_currstate, c, &gtext, &dtp, 0L, 0);
	}
	
	if ( IS_STATE(pblk,DISABLED) ) {
		vs_clip( vdih, 1, tabclip);
		draw_disabled3D( vdih, &grect, border);
	}
	
	vs_clip_off( vdih);

	return 0;
}

short __CDECL ub_boxtext3d( PARMBLK *pblk) 
{
	UDLIB_LOCAL DRAW_TEXT_PARAMETERS dtp;
	
	UDLIB_LOCAL TEDINFO *tedinfo;
	UDLIB_LOCAL INT16 tabclip[4];
	UDLIB_LOCAL int fillpattern, interiorcol, border, bordercol;
	UDLIB_LOCAL INT16 dum;
	UDLIB_LOCAL XOBJC_DATA *uldata;
	UDLIB_LOCAL short vdih;
	UDLIB_LOCAL GRECT grect;
	UDLIB_LOCAL GRECT gtext;
	
	dtp = (DRAW_TEXT_PARAMETERS){halign_center, valign_center, 0, 1};
	
	uldata = ((W_PARM*)(pblk->pb_parm))->wp_uldata;
	vdih = uldata->vdih;
	
	memcpy( &grect, &pblk->pb_x, sizeof(GRECT));
	tedinfo = (TEDINFO*)((W_PARM *)pblk->pb_parm)->wp_spec;
	bordercol = (tedinfo->te_color >> 12) & 0x0F;
	fillpattern = (tedinfo->te_color >> 4) & 0x07;
	interiorcol = tedinfo->te_color & 0x0F;
	
	border = tedinfo->te_thickness & 0x00FF;
	if (border >= 128) border = 256 - border;
	
	grect_to_array( (GRECT*) &pblk->pb_xc, tabclip);
	vs_clip( vdih, 1, tabclip);
	
	/* draw the filled box */
	
	draw_box3D( vdih, &grect, IS_STATE(pblk,SELECTED), border, bordercol, fillpattern, interiorcol,
	            NO_SHADING, pblk->pb_tree->ob_x, pblk->pb_tree->ob_y);
	
	/* draw the texte */
	
	if ( clip_inside( vdih, pblk, border) ) {
		set_attrib_xtedinfo( uldata, &CONF(uldata)->xtedinfo, CONF(uldata)->xtdinf_smlfnt, tedinfo, &dum, &dum);

		switch (tedinfo->te_just) {
		case 0 : dtp.halign = halign_left;   break; /* left   */
		case 2 : dtp.halign = halign_center; break; /* center */
		default: dtp.halign = halign_right;         /* right  */
		}

		gtext = grect;
		if( IS_STATE(pblk,SELECTED) ) {
			gtext.g_x += 1;
			gtext.g_y += 1;
		}

		vswr_mode( vdih, MD_TRANS);  /* MD_TRANS for 3D objects */
		draw_text( uldata, pblk->pb_currstate, tedinfo->te_ptext, &gtext, &dtp, 0L, 1);
	}
		
	if ( IS_STATE(pblk,DISABLED) ) {
		vs_clip( vdih, 1, tabclip);
		draw_disabled3D( vdih, &grect, border);
	}

	vs_clip_off( vdih);

	return 0;
}


short __CDECL ub_but3d( PARMBLK *pblk) 
{
	static DRAW_TEXT_PARAMETERS dtp = {halign_center, valign_center, 0, 1};
	
	UDLIB_LOCAL INT16 tabclip[4];
	UDLIB_LOCAL XOBJC_DATA *uldata;
	UDLIB_LOCAL short vdih;
	UDLIB_LOCAL short border;
	UDLIB_LOCAL GRECT grect;
	UDLIB_LOCAL GRECT gtext;
	
	uldata = ((W_PARM*)(pblk->pb_parm))->wp_uldata;
	vdih = uldata->vdih;
	
	border = 1;
	if (pblk->pb_tree[pblk->pb_obj].ob_flags & EXIT)    border++;
	if (pblk->pb_tree[pblk->pb_obj].ob_flags & DEFAULT) border++;
	
	/* for buttons, border are external border. We have to transform them
	 * to the "inside only" convention before using other 3D function of
	 * this library (which requier borders to be inside border)
	 */
	memcpy( &grect, &pblk->pb_x, sizeof(GRECT));
#if 0 /* test */
	grect.g_x += (3 - border);
	grect.g_y += (3 - border);
	grect.g_w -= (6 - border - border);
	grect.g_h -= (6 - border - border);
#endif /* test */	

	grect_to_array( (GRECT*) &pblk->pb_xc, tabclip);
	vs_clip( vdih, 1, tabclip);
	
	/* draw the filled box */
	
	draw_box3D( vdih, &grect, IS_STATE(pblk,SELECTED), border, 1, 0, 0,
	            NO_SHADING, pblk->pb_tree->ob_x, pblk->pb_tree->ob_y);
				
	/* draw the texte */
	
#if 0 /* test */
	if ( clip_inside( vdih, pblk, 3) ) {
#else /* test */	
	if ( clip_inside( vdih, pblk, 1) ) {
#endif /* test */	
		if (pblk->pb_tree[pblk->pb_obj].ob_flags & EXIT) 
			set_attrib( uldata, &CONF(uldata)->exit);
		else 
			set_attrib( uldata, &CONF(uldata)->button);

		gtext = grect;
		if( IS_STATE(pblk,SELECTED) ) {
			gtext.g_x += 1;
			gtext.g_y += 1;
		}

		vswr_mode( vdih, MD_TRANS);  /* MD_TRANS for 3D objects */
		draw_text( uldata, pblk->pb_currstate, (char*)((W_PARM *)pblk->pb_parm)->wp_spec, &gtext, &dtp, 0L, 1);
	}
		
	if ( IS_STATE(pblk,DISABLED) ) {
		vs_clip( vdih, 1, tabclip);
		draw_disabled3D( vdih, &grect, border);
	}

	vs_clip_off( vdih);

	return 0;
}


/* ************************************************************************************* *
 *									 EXTENDED OBJECTS                                    *
 * ************************************************************************************* */


/**  userdef subroutine for selectable buttons (radio, chechbox...)
 *
 *  @param pblk
 *
 *  objects with EXIT flag _and_ without RBUTTON flag  is drawn as a 
 *  standard 3d button.
 *
 *  an icon is drawn on the left of each object. The icon choosen depend
 *  on the DRAW3D flag and the STATE8 ob_state.
 *
 *
 */

short __CDECL ub_button( PARMBLK *pblk)
{
	static DRAW_TEXT_PARAMETERS dtp = {halign_left, valign_center, 1, 1};
	
	UDLIB_LOCAL GRECT grect;
	UDLIB_LOCAL INT16 tab[12];
	UDLIB_LOCAL XOBJC_DATA *uldata;
	UDLIB_LOCAL short vdih;
	
	if( IS_FLAGS(pblk,EXIT) && !IS_FLAGS(pblk,RBUTTON)) 
		return ub_but3d( pblk);
		
	uldata = ((W_PARM*)(pblk->pb_parm))->wp_uldata;
	vdih = uldata->vdih;
	memcpy( &grect, &pblk->pb_x, sizeof(GRECT));

	/* set clipping area */ 
	grect_to_array( (GRECT*) &pblk->pb_xc, tab);
	vs_clip( vdih, 1, tab);

#define WIDGET_WIDTH 13
	pblk->pb_h = WIDGET_WIDTH;

	vswr_mode (vdih, MD_REPLACE);

	if( IS_FLAGS(pblk,RBUTTON)) {
		if (pblk->pb_currstate & DISABLED)
		{
			// tracer un cercle gris fonc‚ sans effet 3D

			vsl_color (vdih, 9);
			vsl_width (vdih, 1);
			v_arc (vdih, pblk->pb_x + (WIDGET_WIDTH >> 1), pblk->pb_y + (WIDGET_WIDTH >> 1),
		    	   WIDGET_WIDTH >> 1, 0, 3600);
		}
		else
		{
			// tracer un cercle avec effet 3D

			// bordure noire

			vsl_color (vdih, 1);
			vsl_width (vdih, 1);
			v_arc (vdih, pblk->pb_x + (WIDGET_WIDTH >> 1), pblk->pb_y + (WIDGET_WIDTH >> 1),
		    	   WIDGET_WIDTH >> 1, 450, 2250);

			// bordure blanche

			vsl_color (vdih, 0);
			v_arc (vdih, pblk->pb_x + (WIDGET_WIDTH >> 1), pblk->pb_y + (WIDGET_WIDTH >> 1),
		    	   WIDGET_WIDTH >> 1, 2250, 450);
		}

		if (pblk->pb_currstate & SELECTED)
		{

			// rond noir

			vsf_color (vdih, 1);
			v_pieslice (vdih, pblk->pb_x + (WIDGET_WIDTH >> 1), pblk->pb_y + (WIDGET_WIDTH >> 1),
		    	   (WIDGET_WIDTH >> 1) - 3, 0, 3600);


		}
	} else {
		static short cross_img[] = { 0x0000, 0x6300, 0x7700, 0x3E00, 0x1C00, 0x3E00, 0x7700, 0x6300, 0x0000 };
		static MFDB cross_mfdb = { cross_img, 9, 9, 1, 1, 1, 0, 0, 0};
		static MFDB screen_mfdb = { 0L};
		
		short color[2];
		
		vsl_width (vdih, 1);

		if ( IS_STATE(pblk, DISABLED) ) {
		
			/* external border : dark grey */	
			tab[0] = tab[6] = tab[8] = pblk->pb_x;
			tab[1] = tab[3] = tab[9] = pblk->pb_y;
			tab[2] = tab[4] = pblk->pb_x + WIDGET_WIDTH - 1;
			tab[5] = tab[7] = pblk->pb_y + WIDGET_WIDTH - 1;
			vsl_color (vdih, LBLACK);
			v_pline (vdih, 5, tab);
			
			/* interior : light grey */
			tab[0] = pblk->pb_x + 1;
			tab[1] = pblk->pb_y + 1;
			tab[2] = pblk->pb_x + WIDGET_WIDTH - 2;
			tab[3] = pblk->pb_y + WIDGET_WIDTH - 2;
			vsf_color (vdih, LWHITE);
			vsf_interior (vdih, 1);
			vr_recfl (vdih, tab);
			
			/* cross : dark grey on light grey background */
			tab[0] = 0;
			tab[1] = 0;
			tab[2] = 8;
			tab[3] = 8;
			tab[4] = pblk->pb_x + 2;
			tab[5] = pblk->pb_y + 2;
			tab[6] = tab[4] + 8;
			tab[7] = tab[5] + 8;
			if (IS_STATE(pblk, SELECTED) )	color[0] = LBLACK;
			else                            color[0] = LWHITE;
			color[1] = LWHITE;
			vrt_cpyfm( vdih, MD_REPLACE, tab, &cross_mfdb, &screen_mfdb, color);
			
		} else {
			
			/* most external border (top left) : dark grey */
			tab[0] = tab[2] = pblk->pb_x;
			tab[3] = tab[5] = pblk->pb_y;
			tab[4] = pblk->pb_x + WIDGET_WIDTH - 2;
			tab[1] = pblk->pb_y + WIDGET_WIDTH - 2;
			vsl_color (vdih, LBLACK);
			v_pline (vdih, 3, tab);
			
			/* most external border (bottom right) : white */
			tab[0] = pblk->pb_x + 1;
			tab[5] = pblk->pb_y + 1;
			tab[2] = tab[4] = pblk->pb_x + WIDGET_WIDTH - 1;
			tab[1] = tab[3] = pblk->pb_y + WIDGET_WIDTH - 1;
			vsl_color (vdih, WHITE);
			v_pline (vdih, 3, tab);
			
			/* inside border: light grey */
			tab[0] = tab[6] = tab[8] = pblk->pb_x + 1;
			tab[1] = tab[3] = tab[9] = pblk->pb_y + 1;
			tab[2] = tab[4] = pblk->pb_x + WIDGET_WIDTH - 2;
			tab[5] = tab[7] = pblk->pb_y + WIDGET_WIDTH - 2;
			vsl_color (vdih, LWHITE);
			v_pline (vdih, 5, tab);
			
			/* cross : black on white background */
			tab[0] = 0;
			tab[1] = 0;
			tab[2] = 8;
			tab[3] = 8;
			tab[4] = pblk->pb_x + 2;
			tab[5] = pblk->pb_y + 2;
			tab[6] = tab[4] + 8;
			tab[7] = tab[5] + 8;
			if (IS_STATE(pblk, SELECTED) )	color[0] = BLACK;
			else                            color[0] = WHITE;
			color[1] = WHITE;
			vrt_cpyfm( vdih, MD_REPLACE, tab, &cross_mfdb, &screen_mfdb, color);
		}
		
	}

	/* draw text */	
	vswr_mode( vdih, MD_TRANS);	
	set_attrib( uldata, &CONF(uldata)->button);
	grect.g_x += WIDGET_WIDTH + 5;
	grect.g_w -= WIDGET_WIDTH + 5;
	draw_text_8( uldata, pblk->pb_currstate | STATE9, (char*)((W_PARM *)pblk->pb_parm)->wp_spec, &grect, &dtp, 0L, 1);

 	vs_clip_off( vdih);

	return 0;
}


/**  userdef subroutine for boxtitle
 *
 *  @param pblk
 *
 *
 *
 */
 
short __CDECL ub_boxtitle( PARMBLK* pblk) {
	static DRAW_TEXT_PARAMETERS dtp = {halign_left, valign_center, 1, 1};
	
	UDLIB_LOCAL int ind;
	UDLIB_LOCAL INT16 tab[12];
	UDLIB_LOCAL INT16 attrib[10];
	UDLIB_LOCAL GRECT grect;
	UDLIB_LOCAL XOBJC_DATA *uldata;
	UDLIB_LOCAL short vdih;

	uldata = ((W_PARM*)(pblk->pb_parm))->wp_uldata;
	vdih = uldata->vdih;
		
	grect_to_array( (GRECT*) &pblk->pb_xc, tab);
	vs_clip( vdih, 1, tab);

	set_attrib( uldata, &CONF(uldata)->string);
	vswr_mode( vdih, MD_TRANS);
	/* FIXME: the text is centered on the top border of the object... so it is half
	 * out of the object area !
	 */
	grect.g_x = pblk->pb_x + 9;   /* 8 + 1 = 9; 1 pixel for shadowed text */
	grect.g_y = pblk->pb_y - 9;  /* 10 pixel height for half a char should be enought ( 10-1 = 9)*/
	grect.g_w = pblk->pb_w;
	grect.g_h = 20;
	
	/* white shadow */
	vst_color(vdih,WHITE);
	draw_text( uldata, pblk->pb_currstate, (char*)((W_PARM *)pblk->pb_parm)->wp_spec, &grect, &dtp, attrib, 1);
	
	/* grey text */
	grect.g_x--; 
	grect.g_y--;
	vst_color(vdih,LBLACK);
	draw_text( uldata, pblk->pb_currstate, (char*)((W_PARM *)pblk->pb_parm)->wp_spec, &grect, &dtp, attrib, 1);
	
	/* now, draw the border of the box */
	vswr_mode( vdih, MD_REPLACE);
	
	/* white lines (shadow) */
	tab[0] = pblk -> pb_x + 5 + 1;
	tab[11] = tab[9] = tab[3] = tab[1] = pblk -> pb_y + 1;
	tab[4] = tab[2] = pblk -> pb_x + 1;
	tab[7] = tab[5] = tab[3] + pblk -> pb_h - 1 + 1;
	tab[8] = tab[6] = tab[4] + pblk -> pb_w - 1 + 1;
	tab[10] = tab[0] + attrib[2] - attrib[0] + 4 + 1;
	vsl_color( vdih, WHITE);
	v_pline( vdih, 6, tab);
	
	/* grey lines */
	for(ind=1;ind<12;tab[ind++]--);
	tab[10]++;
	vsl_color( vdih, LBLACK);
	v_pline( vdih, 6, tab);
	
	vs_clip_off( vdih);
	
	return 0;
}


/**  userdef subroutine for underlined text
 *
 *  @param pblk
 *
 *  if ob_state & STATE8, then the underline is as long as the object width.
 *  otherwise, only the text is underlined.
 *
 */

short __CDECL ub_ulinetext( PARMBLK *pblk) 
{
	static DRAW_TEXT_PARAMETERS dtp = {halign_left, valign_center, 1, 1};
	
	UDLIB_LOCAL int hcell;
	UDLIB_LOCAL INT16 tab[12];
	UDLIB_LOCAL GRECT grect;
	UDLIB_LOCAL XOBJC_DATA *uldata;
	UDLIB_LOCAL short vdih;

	uldata = ((W_PARM*)(pblk->pb_parm))->wp_uldata;
	vdih = uldata->vdih;
	memcpy( &grect, &pblk->pb_x, sizeof(GRECT));
	
	grect_to_array( (GRECT*) &pblk->pb_xc, tab);
	vs_clip( vdih, 1, tab);
	
	set_attrib( uldata, &CONF(uldata)->string);
	vqt_attributes( vdih, tab);
	hcell = tab[9];

	vswr_mode( vdih, MD_TRANS);
	draw_text( uldata, pblk->pb_currstate, (char*)((W_PARM *)pblk->pb_parm)->wp_spec, &grect, &dtp, tab, 1);
	
	if( pblk->pb_currstate & STATE8) {
		tab[0] = pblk->pb_x;
		tab[1] = pblk->pb_y + pblk->pb_h - 1;
		tab[2] = pblk->pb_x + pblk->pb_w - 1;
		tab[3] = tab[1];
	} else {
		tab[0] += pblk->pb_x;
		tab[1] += pblk->pb_y + pblk->pb_h/2 + hcell/2 - 1;
		tab[2] += pblk->pb_x;
		tab[3] = tab[1];
	}

	tab[1] ++; tab[3] ++;
	vswr_mode( vdih, MD_REPLACE);
	vsl_color( vdih, LBLACK);
	v_pline( vdih, 2, tab);
	
	if( pblk->pb_currstate & DRAW3D) {
		tab[1] --; tab[3] --;
		vsl_color( vdih, WHITE);
		v_pline( vdih, 2, tab);
	}

	vs_clip_off( vdih);
	
	return 0;
}


/**  userdef subroutine for boxcircle
 *
 *  @param pblk
 *
 *
 */

short __CDECL ub_boxcircle( PARMBLK *pblk) 
{
	UDLIB_LOCAL INT16 tab[12];
	UDLIB_LOCAL XOBJC_DATA *uldata;
	UDLIB_LOCAL short vdih;

	uldata = ((W_PARM*)(pblk->pb_parm))->wp_uldata;
		
	vdih = uldata->vdih;

	grect_to_array( (GRECT*) &pblk->pb_xc, tab);
	vs_clip( vdih, 1, tab);

	/* TODO */
	
	vs_clip_off( vdih);
	
	return 0;
}


/** userdef subroutine for thumb
 *
 *  @param pblk
 *
 */

short __CDECL ub_onglet( PARMBLK *pblk) 
{
	static DRAW_TEXT_PARAMETERS dtp = {halign_center, valign_center, 1, 1};

	UDLIB_LOCAL int sel;
	UDLIB_LOCAL INT16 tab[12];
	UDLIB_LOCAL GRECT grect;
	UDLIB_LOCAL XOBJC_DATA *uldata;
	UDLIB_LOCAL short vdih;
	
	enum { active_thumb, passive_thumb, panel } type_of_object = panel;
	
	uldata = ((W_PARM*)(pblk->pb_parm))->wp_uldata;
	vdih = uldata->vdih;
	sel = (pblk->pb_currstate & SELECTED)?1:0;
	
	if ( IS_FLAGS(pblk,RBUTTON) ) {
		if (sel) type_of_object = active_thumb;
		else     type_of_object = passive_thumb;
	} else {
		type_of_object = panel;
	}

	/* object area */
	memcpy( &grect, &pblk->pb_x, sizeof(GRECT));
	
	/* reduce width (because of 3d external borders) */
	if ( type_of_object == passive_thumb || type_of_object == active_thumb ) {
		grect.g_x += 0;
		grect.g_w -= 2;
	}
	/* reduce height if the object is not selected (background thumb) */
	if ( type_of_object == passive_thumb) {
		grect.g_y += 2;
		grect.g_h -= 4;
	}
	/* reduce height (because of 3d external borders) */
	if ( type_of_object == active_thumb) {
		if ( IS_STATE(pblk,STATE15) )
			grect.g_y += 1;
		grect.g_h -= 1;
	}
	
	/* clipping area */
	grect_to_array( (GRECT*) &pblk->pb_xc, tab);
	vs_clip( vdih, 1, tab);
	
	/* background */
	switch (type_of_object) {
	case passive_thumb:
		draw_background( vdih, &grect, 0, 0, SHADING_DARK);
		break;
	case active_thumb:
		draw_background( vdih, &grect, 0, 0, SHADING_LIGHT);
		break;
	case panel:
		draw_background( vdih, &grect, 0, 0, SHADING_LIGHT2DARK);
		break;
	}

	/* border and text */
	if (type_of_object == panel) {
		draw_ibox3D( vdih, &grect, 0, 1, 0);
	} else {
		if ( ! IS_STATE(pblk,STATE15) ) {
			/* up thumb */
			tab[0] = grect.g_x;
			tab[1] = grect.g_y + grect.g_h - 1;
			tab[2] = grect.g_x;
			tab[3] = grect.g_y;
			tab[4] = grect.g_x + grect.g_w - 2;
			tab[5] = grect.g_y;
			vsl_color( vdih, WHITE);
			v_pline( vdih, 3, tab);
			tab[0] = grect.g_x + grect.g_w - 1;
			tab[1] = grect.g_y;
			vsm_color(vdih, LWHITE);
			vsm_type(vdih,1);
			v_pmarker(vdih,1,tab);
			tab[0] = grect.g_x + grect.g_w - 1;
			tab[1] = grect.g_y + 1;
			tab[2] = grect.g_x + grect.g_w - 1;
			tab[3] = grect.g_y + grect.g_h - 1;
			vsl_color( vdih, LBLACK);
			v_pline( vdih, 2, tab);
			if (type_of_object == active_thumb) {
				tab[0] = grect.g_x + grect.g_w - 1;
				tab[1] = grect.g_y + grect.g_h - 1;
				v_pmarker(vdih,1,tab);
			}
		} else {
			/* down thumb */
			tab[0] = grect.g_x;
			tab[1] = grect.g_y + grect.g_h - 1;
			tab[2] = grect.g_x;
			tab[3] = grect.g_y;
			vsl_color( vdih, WHITE);
			v_pline( vdih, 2, tab);
			vsm_color(vdih, LWHITE);
			vsm_type(vdih,1);
			if (type_of_object == active_thumb)
				v_pmarker(vdih,2,tab);
			else
				v_pmarker(vdih,1,tab);
			tab[0] = grect.g_x + 1;
			tab[1] = grect.g_y + grect.g_h - 1;
			tab[2] = grect.g_x + grect.g_w - 1;
			tab[3] = grect.g_y + grect.g_h - 1;
			tab[4] = grect.g_x + grect.g_w - 1;
			tab[5] = grect.g_y;
			vsl_color( vdih, LBLACK);
			v_pline( vdih, 3, tab);
		}

		/* text */
		set_attrib( uldata, &CONF(uldata)->button);
		if( IS_STATE(pblk,STATE15) )	grect.g_y += sel ?  1 : -1;
		else							grect.g_y += sel ? -1 :  1;
		vswr_mode( vdih, MD_TRANS);
		draw_text( uldata, pblk->pb_currstate | (sel?STATE9:0), (char*)((W_PARM *)pblk->pb_parm)->wp_spec, &grect, &dtp, 0L, !IS_STATE(pblk,SELECTED));
		vswr_mode( vdih, MD_REPLACE);
	} 
	
	vs_clip_off( vdih); 

	return 0;
}


/** userdef subroutine for popup button
 *
 *  @param pblk
 *
 */

short __CDECL ub_popuptext( PARMBLK *pblk) 
{
	static DRAW_TEXT_PARAMETERS dtp = {halign_left, valign_center, 1, 1};
	
	UDLIB_LOCAL INT16 tab[12];
	UDLIB_LOCAL GRECT grect;
	UDLIB_LOCAL XOBJC_DATA *uldata;
	UDLIB_LOCAL short vdih;

	uldata = ((W_PARM*)(pblk->pb_parm))->wp_uldata;
	vdih = uldata->vdih;
		
	/* clipping area */
	grect_to_array( (GRECT*) &pblk->pb_xc, tab);
	vs_clip( vdih, 1, tab);
	
	if( pblk->pb_prevstate == pblk->pb_currstate) /* redraw */
	{
		if (pblk->pb_currstate & SELECTED) {
			tab[0]=pblk->pb_x-1;
			tab[1]=pblk->pb_y-1;
			tab[2]=pblk->pb_x+pblk->pb_w+1;
			tab[3]=pblk->pb_y+pblk->pb_h+1;
			vsf_color( vdih, BLACK);
			vswr_mode( vdih, MD_XOR);
			vsf_interior( vdih, FIS_SOLID );
			v_bar( vdih, tab);
		}

		/* text */
		memcpy( &grect, &pblk->pb_x, sizeof(GRECT));
		set_attrib( uldata, &CONF(uldata)->string);
		vswr_mode( vdih, MD_TRANS);
		draw_text( uldata, pblk->pb_currstate, (char*)((W_PARM *)pblk->pb_parm)->wp_spec, &grect, &dtp, 0L, !IS_STATE(pblk,SELECTED));
		vswr_mode( vdih, MD_REPLACE);

	} else {	/* only state has changed (SELECTED) */
		if( (pblk->pb_currstate & SELECTED &&
			 (pblk->pb_prevstate & SELECTED) == 0) ||
			(pblk->pb_prevstate & SELECTED &&
			 (pblk->pb_currstate & SELECTED) == 0)) {
			tab[0]=pblk->pb_x-1;
			tab[1]=pblk->pb_y-1;
			tab[2]=pblk->pb_x+pblk->pb_w+1;
			tab[3]=pblk->pb_y+pblk->pb_h+1;
			vsf_color( vdih, BLACK);
			vswr_mode( vdih, MD_XOR);
			vsf_interior( vdih, FIS_SOLID );
			v_bar( vdih, tab);
		}
	}
	
	vs_clip_off( vdih); 

	return 0;
}


short __CDECL ub_mntitle( PARMBLK *pblk) 
{
	static DRAW_TEXT_PARAMETERS dtp_lbl = {halign_left, valign_center, 1, 0};
	static DRAW_TEXT_PARAMETERS dtp_shc = {halign_right, valign_center, 1, 0};
	
	UDLIB_LOCAL char *p;
	UDLIB_LOCAL int len;
	UDLIB_LOCAL int line;
	UDLIB_LOCAL int pre, post;
	UDLIB_LOCAL int type;
	UDLIB_LOCAL GRECT r1, r2;
	UDLIB_LOCAL INT16 tab[12];
	UDLIB_LOCAL char str[128];
	UDLIB_LOCAL INT16 attrib[10];
	
	UDLIB_LOCAL int draw_normal;
	UDLIB_LOCAL int reverse_drawing;
	
	UDLIB_LOCAL XOBJC_DATA *uldata;
	UDLIB_LOCAL short vdih;
	
	line = 0;
	pre = 0;
	post = 0;
	type = (((W_PARM *)pblk->pb_parm)->wp_type & 0x00FF);
	
	draw_normal = 1;
	reverse_drawing = 0;
	
	uldata = ((W_PARM*)(pblk->pb_parm))->wp_uldata;
		
	vdih = uldata->vdih;
	
	/* clipping area given by TOS seems buggy (maybe because it's
	 * a menubar ???)
	 * => we intersect the clipping area and the object area
	 * except for for titles
	 */
	if( type == G_TITLE) {
		r1.g_x = pblk -> pb_x + 1;
		r1.g_y = pblk -> pb_y + 1;
		r1.g_w = pblk -> pb_w - 2;
		r1.g_h = pblk -> pb_h - 2;
	} else {
		r1.g_x = pblk -> pb_x - 1;
		r1.g_y = pblk -> pb_y - 1;
		r1.g_w = pblk -> pb_w + 2;
		r1.g_h = pblk -> pb_h + 2;
	}
	r2.g_x = pblk -> pb_xc;
	r2.g_y = pblk -> pb_yc;
	r2.g_w = pblk -> pb_wc;
	r2.g_h = pblk -> pb_hc;
	
	/* r2.g_x==0 means that the object is a TITLE of the desktop menubar */
	if( r2.g_x) rc_intersect( &r1, &r2);
	grect_to_array( (GRECT*) &r2, tab);
	vs_clip( vdih, 1, tab);

	/* copy the string to a working buffer */
	strcpy( str, (char*)((W_PARM *)pblk->pb_parm)->wp_spec);
	p = NULL;
	
	/* Is this a line separator ? */
	if( (*str == '-' && pblk->pb_currstate & DISABLED) || IS_STATE(pblk, STATE8)) {
		UDLIB_LOCAL char *s;
		
		line = 1;
		
		/* Is there a label within the dashed line ? */		
		for( s = str; *s; s++)
			if( *s != '-') break;
			else *s = ' ';

		if( *s) {
			pre = (int)(s - str);
			/* continue up to next '-' */
			for(  s = str; *s; s++)
				if( *s == '-') break;
			post = (int)(s - str);
			if( *s) {
				for( ; *s == '-'; s++) 
					*s = ' ';
			}
			
		}
	} else {
		/* not a line separator */	
		if( (((W_PARM *)pblk->pb_parm)->wp_type & 0x00FF) != G_TITLE) {
			/* get the shortcut (right part of the text) */
			len = (int)strlen(str);
			if( len > 2 && str[len-2] != ' ') { 
				/* there is a shortcut */
				str[len-1] = '\0';
				p = strrchr( str, ' ');
				if( p) *p = '\0';
			}
			/* str = the title name, p = the shortcut */
		}
	}
	
	if( (pblk->pb_currstate & SELECTED && (pblk->pb_prevstate & SELECTED) == 0) ||
		(pblk->pb_prevstate & SELECTED && (pblk->pb_currstate & SELECTED) == 0))
	{
		draw_normal = 0;
		reverse_drawing = 1;
	}
	
	if( draw_normal) {
		int charpos = 0;
		int xpos = 0;
		GRECT grect;
		
		memcpy( &grect, &pblk->pb_x, sizeof(GRECT));
		
		/* text */
		set_attrib( uldata, &CONF(uldata)->title);
		vqt_attributes( vdih, tab);			
		if( line && str[1] == '-' ) {
			charpos = 1;
			xpos = tab[8]*2;
		}
		vswr_mode( vdih, MD_TRANS);
		draw_text( uldata, pblk->pb_currstate, str+charpos, &grect, &dtp_lbl, 0L, 0);

		/* shortcut */
		if( p) {
			grect.g_w -= tab[6];
			draw_text( uldata, pblk->pb_currstate, p+1, &grect, &dtp_shc, 0L, 0);
		}
		vswr_mode( vdih, MD_REPLACE);

		/* separator line */
		if( line) {
			vswr_mode( vdih, MD_REPLACE);
			vsf_perimeter(vdih, 0);
			if( uldata->nplanes >= 4) {
				vsf_color	( vdih, LBLACK);
				vsf_interior( vdih, FIS_SOLID);
			} else {
				vsf_color	( vdih, BLACK);
				vsf_interior( vdih, FIS_PATTERN);
				vsf_style	( vdih, 4);
			}
			tab[0] = pblk->pb_x;
			tab[1] = pblk->pb_y + pblk->pb_h/2 - 1;
			tab[3] = pblk->pb_y + pblk->pb_h/2;
			if( pre) {
				char c = str[pre];
				str[pre] = 0;
				vqt_extent( vdih, str, attrib);
				str[pre] = c;
				tab[2] = pblk->pb_x + attrib[2]-attrib[0];
				v_bar( vdih, tab);
				c = str[post];
				str[post] = 0;
				vqt_extent( vdih, str, attrib);
				str[post] = c;
				tab[0] = pblk->pb_x + attrib[2]-attrib[0];
				tab[2] = pblk->pb_x + pblk->pb_w - 1;
				v_bar( vdih, tab);
	
			} else {
				tab[2] = pblk->pb_x + pblk->pb_w - 1;
				v_bar( vdih, tab);
			}
		}
		
		/* reverse the drawing if SELECTED */
		if( pblk->pb_currstate & SELECTED)
			reverse_drawing = 1;
	}
	
	if (reverse_drawing) {
		grect_to_array( (GRECT*)&pblk->pb_x, tab);

		vsf_color( vdih, BLACK);
		vswr_mode( vdih, MD_XOR);
		vsf_interior( vdih, FIS_SOLID );

		v_bar( vdih, tab);
	}

	vs_clip_off( vdih);

	return 0;
}

short __CDECL ub_dialmover( PARMBLK* pblk) 
{
	UDLIB_LOCAL INT16 tab[12];
	UDLIB_LOCAL XOBJC_DATA *uldata;
	UDLIB_LOCAL short vdih;
	UDLIB_LOCAL GRECT grect;
	
	uldata = ((W_PARM*)(pblk->pb_parm))->wp_uldata;
	vdih = uldata->vdih;

	/* clippage */
	grect_to_array( (GRECT*) &pblk->pb_xc, tab);
	vs_clip( vdih, 1, tab);
	
	memcpy( &grect, &pblk->pb_x, sizeof(GRECT));
	if( IS_STATE(pblk,OUTLINED)) {
		draw_background( vdih, &grect, 2, 2, SHADING_LIGHT2DARK);
		draw_frame( vdih, pblk, 3, BLACK, BLACK);
		draw_frame( vdih, pblk, 2, WHITE, LBLACK);
		draw_frame( vdih, pblk, -1, LBLACK, WHITE);
		draw_frame( vdih, pblk, -2, BLACK, BLACK);
	} else {
		draw_background( vdih, &grect, 0, 0, SHADING_LIGHT2DARK);
		draw_frame( vdih, pblk, 0, BLACK, BLACK);
		draw_frame( vdih, pblk, -1, WHITE, LBLACK);
	}
	
	vs_clip_off( vdih);

	return 0;
}


static char *
str_formater( int vdih, char *src, char *dest, int width_max, int len_max, int * width) 
{
	UDLIB_LOCAL int len, prev_len;  
	UDLIB_LOCAL unsigned char *s, *d;
	UDLIB_LOCAL INT16 textpos[8];	
	
	len = prev_len = 0 ;
	
	s = (unsigned char *) src;
	d = (unsigned char *) dest;
	
	while (1) {
		while (*s>32 && len<len_max) {
			*d++ = *s++;
			len++;
		}
		*d = 0;
		vqt_extent( vdih, dest, textpos);
		if ( (textpos[2]-textpos[0]>width_max) || (len==len_max) ) {
			/* line is full ! */
			/* we try a natural line break */
			if (prev_len) {
				dest[prev_len]=0;
				s = (unsigned char *)&src[prev_len];
				while ( (*s<=32) && (*s!=10) && (*s!=13) )
					s++;
				while (*s && (*s==10 || *s==13))
					s++;
				return (char *)s;
			}
			/* we'll have to do a dirty line break in a the middle of a word */
			while ( (textpos[2]-textpos[0]>width_max) && (d!=(unsigned char *)&dest[1]) ) {
				*--d = 0;
				s--;
				vqt_extent( vdih, dest, textpos);
			}
			*width = textpos[2]-textpos[0];
			return (char *)s;
		}
		/* we memorise this position as a natural line break */
		*width = textpos[2]-textpos[0];
		prev_len = len;
		/* There was no line break. we check if there's a carriage return */
		/* or if its the end of the string */
		if (*s==0 || *s==13 || *s==10) {
			while (*s && (*s==10 || *s==13))
				s++;
			return (char *)s;
		}
		/* go after the character "space" and continue */
		*d++ = *s++;
		len++;
	}
}

/*
 * Objet XBOXLONGTEXT
 */


short __CDECL ub_multiligne( PARMBLK *pblk) 
{
	UDLIB_LOCAL int x,y;
	UDLIB_LOCAL TEDINFO *tedinfo;
	UDLIB_LOCAL int border, b;
	UDLIB_LOCAL int x1,y1,x2,y2;
	UDLIB_LOCAL INT16 char_h, cell_h;
	UDLIB_LOCAL INT16 tabclip[4];
	UDLIB_LOCAL char *tmpptr;
	UDLIB_LOCAL int width, borderc;
	UDLIB_LOCAL char str[128];
	UDLIB_LOCAL INT16 tab[12];
	UDLIB_LOCAL XOBJC_DATA *uldata;
	UDLIB_LOCAL short vdih;
	UDLIB_LOCAL GRECT grect;
	
	uldata = ((W_PARM*)(pblk->pb_parm))->wp_uldata;
		
	vdih = uldata->vdih;
	tedinfo = (TEDINFO*)((W_PARM *)pblk->pb_parm)->wp_spec;
	memcpy( &grect, &pblk->pb_x, sizeof(GRECT));

	grect_to_array( (GRECT*) &pblk->pb_x, tab);
	x1 = tab[0] ;
	y1 = tab[1] ;
	x2 = tab[2] ;
	y2 = tab[3] ;
	
	/* clipping */ 
	grect_to_array( (GRECT*) &pblk->pb_xc, tabclip);
	if (tabclip[0]<tab[0]) tabclip[0]=tab[0];
	if (tabclip[1]<tab[1]) tabclip[1]=tab[1];
	if (tabclip[2]>tab[2]) tabclip[2]=tab[2];
	if (tabclip[3]>tab[3]) tabclip[3]=tab[3];
	if (tabclip[0]>tabclip[2]) return 0;
	if (tabclip[1]>tabclip[3]) return 0;
	vs_clip( vdih, 1, tabclip);
	
	/* background light grey (white if < 16 colors) */
	vswr_mode (vdih, MD_REPLACE);
	vsf_color (vdih, IS_STATE(pblk,DRAW3D)?(uldata->nplanes<4?0:8):WHITE);
	vsf_interior (vdih, 1);
	vr_recfl (vdih, tab);
	vsl_width (vdih, 1);

	/* get the border width (absolute value)
	 * this is an "inside" border.
	 */
	border = tedinfo->te_thickness & 0x00FF;
    if (border >= 128) border = 256 - border;
	borderc = (int)(tedinfo->te_color>>12 & 0x0FL);
	
	/* background (motif) */
	if ( tedinfo->te_color & 0x0070) {
		tab[0] += border;
		tab[1] += border;
		tab[2] -= border;
		tab[3] -= border;
		vsf_color (vdih, (int)(tedinfo->te_color & 0x0FL));
		vsf_interior (vdih, 2);
		vsf_style (vdih, (tedinfo->te_color>>4) & 0x7);
		vr_recfl (vdih, tab);
	}
	vsl_color (vdih, IS_STATE(pblk,DRAW3D)?LBLACK:borderc);
	for( b=0; b<border; b++ ) {
		tab[0] = x1 + b;
		tab[1] = y2 - b - 1;
		tab[2] = x1 + b;
		tab[3] = y1 + b;
		tab[4] = x2 - b - 1;
		tab[5] = y1 + b;
		v_pline (vdih, 3, tab);
	}

	vsl_color (vdih,IS_STATE(pblk,DRAW3D)?WHITE:borderc);
	for( b=0; b<border; b++ ) {
		tab[0] = x2 - b;
		tab[1] = y1 + b + 1;
		tab[2] = x2 - b;
		tab[3] = y2 - b;
		tab[4] = x1 + b + 1;
		tab[5] = y2 - b;
		v_pline (vdih, 3, tab);
	}

	/* black border */
	if (border > 1) {
		tab[0] = tab[6] = tab[8] = x1+border-1;
		tab[1] = tab[3] = tab[9] = y1+border-1;
		tab[2] = tab[4] = x2-border+1;
		tab[5] = tab[7] = y2-border+1;
		vsl_color (vdih,IS_STATE(pblk,DRAW3D)?LWHITE:borderc);
		v_pline (vdih, 5, tab);
	}

	vs_clip_off( vdih);

	/* and now... the text ! */
	
	/* clipping */
	if (tabclip[0] < x1+border) tabclip[0] = x1+border;
	if (tabclip[1] < y1+border) tabclip[1] = y1+border;
	if (tabclip[2] > x2-border) tabclip[2] = x2-border;
	if (tabclip[3] > y2-border) tabclip[3] = y2-border;
	if (tabclip[0]>tabclip[2]) return 0;
	if (tabclip[1]>tabclip[3]) return 0;
	vs_clip( vdih, 1, tabclip);

	vswr_mode( vdih, tedinfo->te_color & 0x0080?MD_REPLACE:MD_TRANS);
	set_attrib_xtedinfo( uldata, &CONF(uldata)->xlongedit, CONF(uldata)->xlgedt_smlfnt, tedinfo, &char_h, &cell_h);
	vst_effects (vdih, 0 /*(pblk->pb_currstate & DISABLED)?2:0*/);

	x1 += border + 1;
	y = y1 + border + char_h + 1;
	
	tmpptr = tedinfo->te_ptext;
	/*	if ( tedinfo->te_color & 0x0070) { */
	while (*tmpptr) {
		tmpptr = str_formater(vdih,tmpptr,str,x2-x1-border-border,128,&width);
		switch (tedinfo->te_just) {
			case 0 : /* left */ x=x1; break;
			case 2 : /*center*/ x=x1+((pblk->pb_w - width)>>1)-border-1; break;
			default: /* right*/ x=x1+(pblk->pb_w - width)-(border<<1)-2; break;
		}
		v_gtext( vdih, x,  y, str);
		if (y>y2) break;
		y += cell_h;
	}
	
	if (pblk->pb_currstate & DISABLED)
		draw_disabled3D( vdih, &grect, border);
	
	vs_clip_off( vdih);
	
	return 0;
}



/* ************************************************************************************* *
 *									 EDITABLE OBJECTS                                    *
 * ************************************************************************************* */


#ifdef MODIF_XEDIT

/*   compute the position position relative to the object:
 *   LABEL : Le texte|____
 *   ^---------------^ = return value
 * 
 *   cur is the position of the cursor (take into account
 *   the parameter xedit->pos).
 */
 
int 
_xobjc_width_curs( XOBJC_DATA *uldata, W_XEDIT *xedit, int curs) 
{
	UDLIB_LOCAL INT16 tab[10], width;
	UDLIB_LOCAL char c;
	UDLIB_LOCAL short vdih;
	
	vdih = uldata->vdih;
	
	/*
	 * width of the LABEL part 
	 */
	
	vst_effects( vdih, 0);
	set_attrib( uldata, &CONF(uldata)->xedit_label);
	
	if( uldata->has_fsmgdos)	vqt_f_extent( vdih, xedit->tedinfo->te_ptmplt, tab);
	else						vqt_extent  ( vdih, xedit->tedinfo->te_ptmplt, tab);
	
	width = tab[2]-tab[0];

	/*
	 * width of the editable part 
	 */
	
	vst_effects( vdih, 0);
	set_attrib( uldata, &CONF(uldata)->xedit_label);

	if( xedit->tedinfo->te_pvalid[0] == 'H') {
		if( uldata->has_fsmgdos) vqt_f_extent( vdih, "*", tab);
		else					 vqt_extent  ( vdih, "*", tab);
		width += (tab[2]-tab[0])*(curs-xedit->pos);
	} else {
		c = xedit->line[curs];
		xedit->line[curs] = '\0';
		if( uldata->has_fsmgdos) vqt_f_extent( vdih, xedit->line+xedit->pos, tab);
		else					 vqt_extent  ( vdih, xedit->line+xedit->pos, tab);
		width += tab[2]-tab[0];
		xedit->line[curs] = c;
	}
	
	return width;
}


/**  Draw the cursor
 *
 *  @param uldata
 *  @param tree
 *  @param idx
 *  @param grect absolute position and size of the object
 *
 *  This function draws the text cursor.
 *  The clipping area has to be set before calling this function.
 */

void 
_xobjc_xedit_curs( XOBJC_DATA *uldata, OBJECT *tree, int idx, GRECT * grect) 
{
	UDLIB_LOCAL W_XEDIT *xedit;
	UDLIB_LOCAL INT16 bar[4];
	UDLIB_LOCAL short vdih;
	
	xedit = (W_XEDIT*)tree[idx].ob_spec.userblk->ub_parm;
	vdih = uldata->vdih;
	
	bar[0] = grect->g_x + _xobjc_width_curs( uldata, xedit, xedit->curs);
	bar[1] = grect->g_y;
	bar[2] = bar[0] + 1;
	bar[3] = bar[1] + grect->g_h - 1;
	
	vswr_mode( vdih, MD_REPLACE);
	vsf_color( vdih, CONF(uldata)->xedit_curs_color);
	v_bar( vdih, bar);
}


/**  reverse-video of the bloc (text selected)
 *
 *  @param uldata
 *  @param tree
 *  @param idx
 *  @param grect absolute position and size of the object
 *
 *  This function draws a black filled rectangle in XOR mode over
 *  the area of the selected text.
 *  The clipping area has to be set before calling this function.
 */

void 
_xobjc_xedit_bloc( XOBJC_DATA *uldata, OBJECT *tree, int idx, GRECT * grect) 
{
	UDLIB_LOCAL W_XEDIT *xedit;
	UDLIB_LOCAL INT16 bar[4];
	UDLIB_LOCAL short vdih;
	
	xedit = (W_XEDIT*)tree[idx].ob_spec.userblk->ub_parm;
	vdih = uldata->vdih;
	
	if( xedit->blcbeg != xedit->blcend) {
		bar[0] = grect->g_x + _xobjc_width_curs( uldata, xedit, xedit->blcbeg);
		bar[1] = grect->g_y;
		bar[2] = grect->g_x + _xobjc_width_curs( uldata, xedit, xedit->blcend) + 1;
		bar[3] = bar[1] + grect->g_h - 1;

		vswr_mode( vdih, MD_XOR);
		vsf_color( vdih, BLACK);
		v_bar( vdih, bar);
	}
}


/*
 * Champ ‚ditable de longueur quelconque (XEDIT)
 *  Aspect :
 *		label avec raccourci : ___champ_encadr‚___
 *
 *	Le userdef pointe vers une structure XEDITDATA
 *	associ‚e … l'objet par la fonction init_type() (voir XRSRC.C).
 *
 *	On utilise la structure XEDITDATA.TEDINFO pour coder le texte
 *	et le label. Accessoirement, cela permet de conserver le
 *  texte saisi si on desalloue le type XEDIT.
 *	Le reste de la structure XEDITDATA sert a coder
 *	les position, curseur et bloc. La fonction ub_editable
 *	reste assez simple. Le gros du boulot est dans ObjcEdit().
 *
 *	Il faut ‚crire une routine ObjcEdit() qui fonctionne
 *	pour tous objets ‚ditables. C'est elle qui s'occupera
 *	de la gestion dynamique du texte, du curseur et aussi des blocs.
 *
 */

#define XTEXT	xedit->line;
#define XLABEL	xedit->tedinfo->te_ptmplt;

#define OPAQUE (1<<7)	/* Parametre Opaque/trans des TEDINFO */

short __CDECL ub_editable( PARMBLK *pblk) 
{
	UDLIB_LOCAL char *p, *q, *txt;
	UDLIB_LOCAL int y;
	UDLIB_LOCAL W_XEDIT *xedit;
	UDLIB_LOCAL char *pvalid;
	UDLIB_LOCAL INT16 tab[12];
	UDLIB_LOCAL char str[128];
	UDLIB_LOCAL GRECT r1, r2;
	UDLIB_LOCAL GRECT grect;
	UDLIB_LOCAL XOBJC_DATA *uldata;
	UDLIB_LOCAL short vdih;
	
	xedit  = (W_XEDIT *)pblk->pb_parm;
	uldata = xedit->wp_uldata;
	vdih = uldata->vdih;
	memcpy( &grect, &pblk->pb_x, sizeof(GRECT));
	pvalid = xedit->tedinfo->te_pvalid;
	
	grect_to_array( (GRECT*)&pblk->pb_xc, tab);
	vs_clip( vdih, 1, tab);

	/*
	 * background 
	 */
	
	draw_background( vdih, &grect, 2, 2, NO_SHADING);
	
	
	/*
	 * label 
	 */
	
	p = XLABEL;
	q = strchr( p, '_');
	if( q) *q = '\0';
	vswr_mode( vdih, (xedit->tedinfo->te_color & OPAQUE)?MD_TRANS:MD_REPLACE);
	vst_effects( vdih, 0);
	set_attrib( uldata, &CONF(uldata)->xedit_label);
	if( CONF(uldata)->xedit_label.color == -1)
		vst_color( vdih, (xedit->tedinfo->te_color>>8) & 0xF);
	vqt_attributes( vdih, tab);
	y = pblk->pb_y + (pblk->pb_h+tab[7])/2;	
		
	if( uldata->has_fsmgdos) 
		v_ftext( vdih, pblk->pb_x+2, y, p);
	else
		v_gtext( vdih, pblk->pb_x+2, y, p);
	
	vqt_extent( vdih, p, tab);
	pblk->pb_x += tab[2]-tab[0];
	pblk->pb_w -= tab[2]-tab[0];

	/*
	 *  border
	 */

	grect_to_array( (GRECT*)&pblk->pb_x, tab);
	vsf_color( vdih, WHITE);
	vsf_interior( vdih, FIS_SOLID);
	vswr_mode( vdih, MD_REPLACE);
	v_bar( vdih, tab);
	if( IS_STATE(pblk,DRAW3D)) {
		draw_frame( vdih, pblk, 2, LBLACK, WHITE);
		draw_frame( vdih, pblk, 1, BLACK, BLACK);
	} else 
		draw_frame( vdih, pblk, 1, BLACK, BLACK);
	
	/*
	 *  text 
	 */

	p = XTEXT;
	if( xedit->pos > 0) p += xedit->pos;

	if( *pvalid == 'H') {
		int i , len = MIN( strlen(p), 127);
		for( i = 0; i<len; str[i++] = '*');
		str[len] = '\0';
		txt = str;
	} else
		txt = p;
	
	vs_clip_off( vdih);

	/* clip within the border so that the text doesn't overwrite borders */
	rc_set( &r2, pblk->pb_x, pblk->pb_y, pblk->pb_w, pblk->pb_h);
	rc_set( &r1, pblk->pb_xc, pblk->pb_yc, pblk->pb_wc, pblk->pb_hc);
	rc_intersect( &r1, &r2);	 
	grect_to_array( &r2, tab);
	vs_clip( vdih, 1, tab);

	vst_effects( vdih, 0);
	set_attrib( uldata, &CONF(uldata)->xedit_text);
	if( CONF(uldata)->xedit_text.color == -1)
		vst_color( vdih, (xedit->tedinfo->te_color>>8) & 0xF);

	vqt_attributes( vdih, tab);	
	y = pblk->pb_y + (pblk->pb_h+tab[7])/2;
	
	if( uldata->has_fsmgdos) 
		v_ftext( vdih, pblk->pb_x, y, txt);
	else
		v_gtext( vdih, pblk->pb_x, y, txt);

	/*
	 * cursor 
	 */
	 
 	if( xedit->flags & XEDIT_CURS_ON )
		_xobjc_xedit_curs( uldata, pblk->pb_tree, pblk->pb_obj, &grect);

	/*
	 * bloc 
	 */
	 
	if( xedit->flags & XEDIT_BLOC_ON  )
		_xobjc_xedit_bloc( uldata, pblk->pb_tree, pblk->pb_obj, &grect);


	vs_clip_off( vdih);
	
	return 0;
}

#endif /* MODIF_XEDIT */


