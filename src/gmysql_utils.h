#ifndef __GMYSQL_UTILS_H__
#define __GMYSQL_UTILS_H__

#include <glib.h>
#include <errno.h>

#define FTC_TV_NONE					0
#define FTC_TV_LENGTH				1
#define FTC_TV_LENGTH_DECI	2
#define FTC_TV_LIST_VALUE		3

typedef struct _fieldTypeCapability {
	char * name;
	int typeValue; /* 0 - none, 1 - Length, 2 - Length and decials, 3 - List of value */
	gboolean unsign;
	gboolean zerofill;
	gboolean binary;
	gboolean autoincr;
} fieldTypeCapability;

typedef fieldTypeCapability * p_fieldTypeCapability;


#ifndef __VAR_ARFIELDTYPES__

extern p_fieldTypeCapability * arFieldTypes;
extern int szarFieldTypes;

#endif /* __VAR_ARFIELDTYPES__ */

/* Charsets list functions */
GList * gmysql_charset_list_new ();
gchar * gmysql_charset_list_get_by_index (gint idx);
gint gmysql_charset_list_get_by_name (const gchar * name);

/* Field capabilities */
p_fieldTypeCapability createFieldTypeCapability (char * name, int value, gboolean unsign, gboolean zerofill, gboolean binary, gboolean autoincr);
void initFieldTypeCapabilities ();

/* Character conversion helpers */
int gmysql_count_noascii_character (const unsigned char * string);
gchar * gmysql_alloc_iconv(GIConv icv, const char * source);

#endif /* __GMYSQL_UTILS_H__ */
