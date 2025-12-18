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
 * $Source: /cvsroot/windom/windom/src/toserror.h,v $
 * Module : definition of TOS error (based on Atari Compendium)
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/01/03 13:52:35 $
 *   $Revision: 1.6 $
 */

#ifndef __TOSERROR
#define __TOSERROR

/* error codes unified for all libc 
 * (except sozobonx that already has included xdlibs/errno.h
 *  which is similar to this toserror.h)
 */

#ifndef __SOZOBONX__

#define E_OK	0		/* No error */

/* BIOS errors (-1 to -31) */ 

#define ERROR	-1		/* Generic error */
#define EDRVNR	-2		/* Drive not ready */
#define EUNCMD	-3		/* Unknown command */
#define E_CRC	-4		/* CRC error */
#define EBADRQ	-5		/* Bad request */
#define E_SEEK	-6 		/* Seek error */
#define EMEDIA	-7 		/* Unknown media */
#define ESECNF	-8		/* Sector not found */
#define EPAPER	-9		/* Out of paper */
#define EWRITF	-10 	/* Write fault */
#define EREADF	-11 	/* Read fault */
#define EWRPRO	-12 	/* Device is write protected */
#define E_CHNG	-14 	/* Media change detected */
#define EUNDEV	-15		/* Unknown device */
#define EBADSF	-16		/* Bad sectors on format */
#define EOTHER	-17		/* Insert other disk (request) */

/* GEMDOS errors (from -32) */
 
#define EINVFN	-32		/* Invalid function */
#define ENOSYS	-32		/* Invalid function */
#define EFILNF	-33		/* File not found */
#define EPTHNF	-34		/* Path not found */
#define ENHNDL	-35		/* No more handles */
#define EACCDN	-36 	/* Acces denied */
#define EIHNDL	-37 	/* Invalid handle */
#define ENSMEM	-39		/* Insufficient memory */
#define EIMBA	-40 	/* Invalid memory block address */
#define EDRIVE	-46		/* Invalid drive specification */
#define ENSAME	-47		/* Cross device rename */
#define ENMFIL	-49		/* No more files */
#define ELOCKED -58		/* Record is already locked */
#define ENSLOCK -59		/* Invalid lock removal request */
#define ERANGE	-64		/* Range error */
#define ENAMETOOLONG	ERANGE
#define EINTRN	-65		/* Internal error */
#define EPLFMT	-66		/* Invalid program load format */
#define EGSBF	-67		/* Memory block growth failure */
#define ELOOP 	-80 	/* Too many symbolic links */
#define EMOUNT	-200	/* Mount point crossed */

#else  /* __SOZOBONX__ */

/* constants needed by windom, but missing in xdlibs */
#define ENOSYS	-32		/* Invalid function */

#endif /* __SOZOBONX__ */

#endif
