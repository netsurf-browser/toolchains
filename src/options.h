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
 * $Source: /cvsroot/windom/windom/src/options.h,v $
 * Module : WinDom compilation options
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/01/03 13:52:34 $
 *   $Revision: 1.6 $
 */


/*
 *	WINDOMDEBUG   : write trace in a log file.
 */

/* #define WINDOMDEBUG */

/* size of the array __iconlist */

#define MAX_ICON 	100

/* buffer size */
#define FILE_SIZE	255		/* filenames */
#define LINE_SIZE	150		/* line len for fget() */
#define PIPE_SIZE	256		/* global memory for data exchange between applications */

/* Memory Protection Friendly
 * comment this if you don't care about memory protection, and accept that
 * your application jump to memory it doesn't own (icfs cookie design for instance)
 */
#define MP_FRIENDLY 1
