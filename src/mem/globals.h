/*
 * LDG : Gem Dynamical Libraries
 * Copyright (c) 1997-2004 Olivier Landemarre, Dominique Bereziat & Arnaud Bercegeay
 *
 * Global definitions
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
 * $Id: globals.h 9 2005-07-25 19:37:25Z bercegeay $
 */

struct mem_chunk 
	{
	long valid;
#define VAL_FREE  0xf4ee0abcL
#define VAL_ALLOC 0xa11c0abcL
#define VAL_BORDER 0xb04d0abcL

	struct mem_chunk *next;
	unsigned long size;
	};
#define ALLOC_SIZE(ch) (*(long *) ((char *) (ch) + sizeof (*(ch))))
#define BORDER_EXTRA ((sizeof (struct mem_chunk) + sizeof (long) + 7) & ~7)


#define	MX_STRAM	 0
#define MX_TTRAM	 1
#define MX_PREFSTRAM 2
#define MX_PREFTTRAM 3

#define BIT(a) (1<<a)

#define MX_PREFTTRAM 3
#define MX_MINTMP	0x8
#define MX_NOFREE	BIT(14)
/* possible modes if bit MX_MINTMP is set */
#define MX_HEADER	
#define MX_PRIVATE
#define MX_GLOBAL		0x20
#define MX_SUPERVISOR
#define MX_READABLE

#define LDG_MAXCLIENT 100

