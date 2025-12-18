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
 * $Source: /cvsroot/windom/windom/examples/tutorial/text.h,v $
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/01/03 14:39:25 $
 *   $Revision: 1.2 $
 */

/**
 * \file text.h
 * Interface to text.c module.
 */

typedef struct {
	char filename[256];  /*> Filename */
	char *buffer; /*> address of text */
	char **line;  /*> table of each lines */
	int maxline;  /*> number of line */
	int wchar,hchar; /*> size of a character */
} TEXT;

TEXT *load_text( char *filename);
void  free_text( TEXT *text);

