/*
cookiec.c C version

olivier Landemarre
11 November 2011
Pas de reset handler!

Tir‚ de:

;--------------------------------------------------------------------
;	Module:		COOKIE.S
;
;	Abstract:	Everything you need for accessing the 
;				cookie jar on Atari ST/TT computers.
;
;	Author:		Arnd Beissner, SciLab GmbH - Germany
;				Parts of the code are derived from Atari's 
;				TOS 1.06 Release Notes.
;
;	Copyright:	COOKIE.S is in the public domain.
;
;	Compatibility:
;				Any ST/TT-compatible machine, any TOS version.
;
;	Language:	MAS68K by Borland
;
;	Version:	1.03
;
;	Date:		15.08.1991
;	
;	History:
;
;	05.01.1991	AB	- fixed a bug in CK_ReadJar
;
;	23.06.1991  AB  - added a copyright notice in the header
;
;	15.08.1991  AB  - Fixed an ugly bug in CK_ResizeJar. The bug
;					  was most disastrous when no cookie jar was
;					  present at all.
;
;--------------------------------------------------------------------
;	Comments:
;
;	Most functions in this module must be called in supervisor mode.
;	No registers except D0 (for the return value) are modified.
;------------------------------------------------------------------
*/

#ifdef __PUREC__
#include <tos.h>
#else
#include <osbind.h>
#endif

#define NULL 0L

#define p_cookies 0x5A0


typedef struct
{
    long id;             /* Identification code */
    long value;          /* Value of the cookie */
} COOKJAR;

static int cookieJarIncrement = 20;


/*
;--------------------------------------------------------------------
; CK_JarInstalled
;
; See if the cookie jar is installed.
;
; Return value:
;		pointer to the cookie jar (0 = not installed)
;--------------------------------------------------------------------
*/
long *CK_JarInstalled(void) /* See if the cookie jar is installed */
{
	long *cookie;
	cookie=(long *)p_cookies;
	return (long *) *cookie;
}

/*
;--------------------------------------------------------------------
; CK_UsedEntries
;
; Inquire the number of used cookie jar entries. The number includes
; the null cookie, so a return value of 0 means that there is no
; cookie jar at all.
;
; Return value:
;		number of used cookie jar entries
;--------------------------------------------------------------------
*/

int CK_UsedEntries(void)
{ int nb=0; 
	COOKJAR *cookiejar;

	cookiejar =(COOKJAR *)CK_JarInstalled();
	if(cookiejar)
	{
		while (cookiejar->id)
	  {
			nb++;
			cookiejar++;
		}
	}
	return nb;
}

/*
;--------------------------------------------------------------------
; CK_JarSize
;
; Inquire the total number of cookie jar entries.
;
; Return value:
;		total number of cookie jar entries
;--------------------------------------------------------------------
*/

int CK_JarSize(void)
{ 
	COOKJAR *cookiejar;
	cookiejar =(COOKJAR *)CK_JarInstalled();
  if(cookiejar)
	{
		while (cookiejar->id)
	  {
			cookiejar++;
		}
	} else return 0L;

	return (int)cookiejar->value; /* nombre d'entree dans le conteneur */

}

/*
;--------------------------------------------------------------------
; CK_ResizeJar
;
; Resize the cookie jar to the desired size.
;
; Input arguments:
;		desired cookie jar size, number of entries
;
; Return value:
;		state (0=FALSE, 1=TRUE)
;--------------------------------------------------------------------
*/

int CK_ResizeJar(int nbcookies)
{	COOKJAR *new_jar, *old_jar;
	int i=0; int sizecookies;
	long *cookie;
	cookie=(long *)p_cookies;
	sizecookies=(nbcookies+1)<<3; /* 8 octets par cookie */
	new_jar= (COOKJAR *) Malloc(sizecookies);
	if(new_jar==NULL) return 0;
	old_jar=(COOKJAR *) CK_JarInstalled();
	new_jar->id=NULL;
	new_jar->value=nbcookies;
  if(old_jar)
	{
		while(old_jar->id)
		{
			new_jar[i].id=old_jar->id;
			new_jar[i].value=old_jar->id;
			old_jar++;
			i++;
		}
		new_jar[i].id=NULL;
		new_jar[i].value=nbcookies;
	}
	*cookie= (long) new_jar; /* installation du nouveau conteneur */
	return 1;
}

/*
;--------------------------------------------------------------------
; _searchJar			**** internal ****
;
; search the position of the specified cookie
;
; Input arguments:
;		cookie name
;
; Return value:
;		pointer to cookie
;--------------------------------------------------------------------
*/
static COOKJAR* searchJar(long cookie_name)
{	COOKJAR *cookiejar;
	cookiejar = (COOKJAR *) CK_JarInstalled();
	if(cookiejar)
	{
		while (cookiejar->id)
	  {
			if (cookiejar->id==cookie_name) return cookiejar;
			cookiejar++;
		}
	}
	return NULL;
}

/*
;--------------------------------------------------------------------
; CK_ReadJar
;
; Read the value of the specified cookie.
;
; Input arguments:
;		cookie name
;		pointer to cookie value
;
; Return value:
;		state (0=FALSE, 1=TRUE)
;--------------------------------------------------------------------
*/
int CK_ReadJar(long cookie_name, long *cookie_value)
{	COOKJAR *cookiejar;
	cookiejar = searchJar(cookie_name);
	if(cookiejar == NULL) 
	{
		*cookie_value=0L;
		return 0;
	}
	*cookie_value = cookiejar->value;
	return 1;
}

/*
;--------------------------------------------------------------------
; CK_WriteJar
;
; Insert a new entry into the cookie jar. If no cookie jar exists
; or the current cookie jar is full, a new, bigger cookie jar is
; installed. The increment in size can be set using CK_SetOptions.
;
; Input arguments:
;		cookie name
;		cookie value
;
; Return value:
;		state (0=FALSE, 1=TRUE)
;--------------------------------------------------------------------
*/

int CK_WriteJar(long cookie_name, long cookie_value)
{	COOKJAR *cookiejar;
	long jarsize, jarusedentries;
	cookiejar = searchJar(cookie_name);
	if(cookiejar)	/* cookie d‚j… existant on r‚‚crit dessus */
	{
		cookiejar->value = cookie_value;
		return 1;
	}
	jarsize = CK_JarSize();
	jarusedentries = CK_UsedEntries();
	if(jarsize<=jarusedentries)
	{
		if(CK_ResizeJar(jarsize+cookieJarIncrement)==0) return 0;
	}
	cookiejar = (COOKJAR *) CK_JarInstalled();
	cookiejar[jarusedentries+1].value = cookiejar[jarusedentries].value;
	cookiejar[jarusedentries+1].id = 0;
	cookiejar[jarusedentries].id = cookie_name;
	cookiejar[jarusedentries].value = cookie_value;
	return 1;
}

/*
;--------------------------------------------------------------------
; CK_SetOptions
;
; Set cookie jar options.
;
; Input arguments:
;		cookie jar increment when allocating a new buffer
;		xbra id for reset handler (not avaible on C version)
;--------------------------------------------------------------------
*/

void CK_SetOptions(int increment, long dummy)
{
	cookieJarIncrement = increment;
}

