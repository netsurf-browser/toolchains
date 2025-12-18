#include <ctype.h>
#include "utils.h"

/**
 * Case insensitive strcmp() while ignoring underscores.
 * \param String1 Left string of comparison
 * \param String2 Right string of comparison
 * \return Same as strcmp() but case insensitive and ignoring any underscore characters.
 */
int
ustricmp (const char *String1, const char *String2)
{
  int iStatus;

  do
    {
      while (*String1 == '_')
	String1++;
      while (*String2 == '_')
	String2++;

      iStatus = toupper (*String1) - toupper (*String2);
    }
  while (iStatus == 0 && *String1++ != '\0' && *String2++ != '\0');

  return iStatus;
}

