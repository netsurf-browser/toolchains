/*
 * OS Lib very limited port
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "support/unix/kernel.h"
#include "oslib/messagetrans.h"
#include "oslib/osfile.h"

/* --- types ------------------------------------------------------------- */

typedef struct s__msgtrans
{
	struct s__msgtrans	*next;
	char				*token;
	char				*value;
} s_msgtrans;

/* --- gloabl data ------------------------------------------------------- */

extern os_error oserr;

/* ------------------------------------------------------------------------
 * Function:      messagetrans_file_info()
 *
 * Description:   Gives information about a message file
 *
 * Input:         file_name - value of R1 on entry
 *
 * Output:        flags - value of R0 on exit
 *                size - value of R2 on exit
 *
 * Other notes:   Calls SWI 0x41500.
 */

extern os_error *xmessagetrans_file_info (char const *file_name,
      messagetrans_file_flags *flags,
      int *size)

{
	os_error		*perr = NULL;
    	fileswitch_object_type	obj_type;
    	bits			load_addr;
    	bits			exec_addr;
    	fileswitch_attr		attr;
    	bits			file_type;

	if((perr = xosfile_read_stamped( file_name,
		                        &obj_type,
					&load_addr,
					&exec_addr,
					size,
					&attr,
					&file_type
					) )==NULL)
	{
		if(obj_type==osfile_NOT_FOUND)
		{
			perr         = &oserr;
			oserr.errnum = error_MESSAGE_TRANS_FILE_OPEN;
			strcpy(oserr.errmess, "Unable to find Messagetrans file");
		}
		else
		{
			/* request 1 byte larger to allow 0 termination */
			(*size)++;
		}
	}

	if (flags) *flags=messagetrans_DIRECT_ACCESS;
	return ERROR(perr);
}

extern void messagetrans_file_info (char const *file_name,
      messagetrans_file_flags *flags,
      int *size)
{
	xmessagetrans_file_info (file_name, flags, size);
}

/* ------------------------------------------------------------------------
 * Function:      messagetrans_open_file()
 *
 * Description:   Opens a message file
 *
 * Input:         cb - value of R0 on entry
 *                file_name - value of R1 on entry
 *                buffer - value of R2 on entry
 *
 * Other notes:   Calls SWI 0x41501.
 */

extern os_error *xmessagetrans_open_file (messagetrans_control_block *cb,
      char const *file_name,
      char *buffer)
{
	os_error               *perr = NULL;
    fileswitch_object_type obj_type;
    bits                   load_addr;
    bits                   exec_addr;
	int                    size;
    fileswitch_attr        attr;

	if((perr = xosfile_load_stamped (file_name,
									 (byte*)buffer,
		                             &obj_type,
									 &load_addr,
									 &exec_addr,
									 &size,
									 &attr))==NULL)
	{
		if(obj_type==osfile_NOT_FOUND)
		{
			perr         = &oserr;
			oserr.errnum = error_MESSAGE_TRANS_FILE_OPEN;
			strcpy(oserr.errmess, "Unable to read Messagetrans file");
		}
		else
		{
			s_msgtrans *m    = NULL;
			s_msgtrans *om   = NULL;
			char       *s    = buffer;
			int			line = 1;
		
			/*
			 * zero terminate buffer - size should be 1 bigger than file
		     * as return from messagetrans_file_info
			 */
			buffer[size] = 0;
			
			cb->cb[0] = (int)buffer;
			cb->cb[1] = 0;

			do
			{
				/* skip comment or blank lines */
				if(*s=='#' || *s==' ' || *s=='\r' || *s=='\n')
				{
					if(*s!='\n')
						s = strchr(s+1, '\n');

					if(s)
					{
						s++;
						line++;

						if(*s=='\r')
							s++;
					}
				}
				else
				{
					/* make list entry */
					if((m = malloc(sizeof(*m)))==NULL)
					{
						perr         = &oserr;
						oserr.errnum = error_MESSAGE_TRANS_FILE_OPEN;
						strcpy(oserr.errmess, "No memory for Messagetrans list");
						s = NULL;
					}
					else
					{
						m->next  = NULL;
						m->token = s;

						/* find token value seperator */
						s = strchr(s, ':');

						if(s)
						{
							/* terminate token */
							*s = 0;

							/* add to tail, or set head of list */
							if(om)
								om->next = m;
							else
								cb->cb[1] = (int)m;

							om = m;

							m->value = ++s;

							if((s=strchr(s, '\n'))!=NULL)
							{
								/* terminate value string on NL/LF */
								if(*(s-1)=='\r')
									*(s-1) = 0;

								*(s++) = 0;
							}
							else
							{
								/* end of file, so advance s to end */
								s = buffer+size;
							}
						}
						else
						{
							free(m);
						}
					} /* endif if malloc */
				} /* endelse '#' '\r' '\n' */
			} while(s!=NULL && s<(buffer+size));

			/* report and tidy up after invalid file */
			if(s==NULL)
			{
				perr         = &oserr;
				oserr.errnum = error_MESSAGE_TRANS_SYNTAX;
				sprintf(oserr.errmess, "Messagetrans syntax error at line %d", line);

				m = (s_msgtrans*)cb->cb[1];

				while(m)
				{
					om = m->next;
					free(m);
					m = om;
				}

				cb->cb[1]=0;
			}
		} /* endelse osfile_NOT_FOUND */
	} /* endif perr */

	return ERROR(perr);
}

void messagetrans_open_file (messagetrans_control_block *cb,
      char const *file_name,
      char *buffer)
{
	xmessagetrans_open_file (cb, file_name, buffer);
}

/* ------------------------------------------------------------------------
 * Function:      messagetrans_lookup()
 *
 * Description:   Translates a message token into a string
 *
 * Input:         cb - value of R0 on entry
 *                token - value of R1 on entry
 *                buffer - value of R2 on entry
 *                size - value of R3 on entry
 *                arg0 - value of R4 on entry
 *                arg1 - value of R5 on entry
 *                arg2 - value of R6 on entry
 *                arg3 - value of R7 on entry
 *
 * Output:        result - value of R2 on exit (X version only)
 *                used - value of R3 on exit
 *
 * Returns:       R2 (non-X version only)
 *
 * Other notes:   Calls SWI 0x41502.
 */

extern os_error *xmessagetrans_lookup (messagetrans_control_block const *cb,
      char const *token,
      char *buffer,
      int size,
      char const *arg0,
      char const *arg1,
      char const *arg2,
      char const *arg3,
      char **result,
      int *used)
{
	osbool found = FALSE;

	NOT_USED(buffer);
	NOT_USED(arg0);
	NOT_USED(arg1);
	NOT_USED(arg2);
	NOT_USED(arg3);

	if(cb!=NULL && cb->cb[0]!=0 && cb->cb[1]!=0)
	{
		/*
		 * #### non zero token termination, wild card and args not supported,
		 *      lookup horribly inefficent
		 */

		s_msgtrans *m = (s_msgtrans*)cb->cb[1];

		while(m && !found)
		{
			if(strcmp(m->token, token)==0)
			{
				found   = TRUE;
				*result = m->value;

				if(buffer)
				{
					*used = strlen(*result);

					if((*used) >= size)
						*used = size-1;

					strncpy(buffer, m->value, *used);
					buffer[*used] = 0;
				}
				else
				{
					*used = 0;
				}
			}
			else
			{
				m = m->next;
			}
		}
	}

	if(!found)
	{
		oserr.errnum = error_MESSAGE_TRANS_TOKEN_NOT_FOUND;
		sprintf(oserr.errmess, "Token '%s' not found", token);
		return ERROR(&oserr);
	}
	else
	{
		return NULL;
	}
}

extern char *messagetrans_lookup (messagetrans_control_block const *cb,
      char const *token,
      char *buffer,
      int size,
      char const *arg0,
      char const *arg1,
      char const *arg2,
      char const *arg3,
      int *used)
{
	char *result;
	if(xmessagetrans_lookup (cb, token, buffer, size, arg0, arg1, arg2, arg3, &result, used))
		return NULL;
	else
		return result;
}

/* ------------------------------------------------------------------------
 * Function:      messagetrans_error_lookup()
 *
 * Description:   Translates a message token within an error block
 *
 * Input:         error - value of R0 on entry
 *                cb - value of R1 on entry
 *                buffer - value of R2 on entry
 *                size - value of R3 on entry
 *                arg0 - value of R4 on entry
 *                arg1 - value of R5 on entry
 *                arg2 - value of R6 on entry
 *                arg3 - value of R7 on entry
 *
 * Other notes:   Calls SWI 0x41506.
 */


/* ------------------------------------------------------------------------
 * Function:      messagetrans_close_file()
 *
 * Description:   Closes a message file
 *
 * Input:         cb - value of R0 on entry
 *
 * Other notes:   Calls SWI 0x41504.
 */

extern os_error *xmessagetrans_close_file (messagetrans_control_block const *cb)
{
	s_msgtrans *m = (s_msgtrans*)cb->cb[1];
	s_msgtrans *om;

	while(m)
	{
		om = m->next;
		free(m);
		m = om;
	}

	return NULL;
}

void messagetrans_close_file (messagetrans_control_block const *cb)
{
	xmessagetrans_close_file(cb);
}

