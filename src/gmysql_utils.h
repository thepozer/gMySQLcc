#ifndef _GMYSQL_UTILS_H
#define _GMYSQL_UTILS_H

#include <glib.h>
#include <errno.h>

#include "gTableUtils.h"

#ifndef __VAR_ARFIELDTYPES__

extern p_fieldTypeCapability * arFieldTypes;
extern int szarFieldTypes;

#endif /* __VAR_ARFIELDTYPES__ */


p_fieldTypeCapability createFieldTypeCapability (char * name, int value, gboolean unsign, gboolean zerofill, gboolean binary, gboolean autoincr);
void initFieldTypeCapabilities ();

int gmysql_count_noascii_character (const unsigned char * string);
gchar * gmysql_alloc_iconv(GIConv icv, const char * source);

#endif /* _GMYSQL_UTILS_H */
