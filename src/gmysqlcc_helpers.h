#ifndef __GMYSQLCC_HELPERS_H__
#define __GMYSQLCC_HELPERS_H__

#include <glib.h>
#include <errno.h>
#include <string.h>

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
GList * gmysqlcc_helpers_charset_list_new ();
gchar * gmysqlcc_helpers_charset_list_get_by_index (gint idx);
gint gmysqlcc_helpers_charset_list_get_by_name (const gchar * name);

/* Field capabilities */
p_fieldTypeCapability gmysqlcc_helpers_field_capability_create (char * name, int value, gboolean unsign, gboolean zerofill, gboolean binary, gboolean autoincr);
void gmysqlcc_helpers_field_capability_init_array ();

/* Character conversion helpers */
int gmysqlcc_helpers_count_noascii_character (const unsigned char * string);
gchar * gmysqlcc_helpers_alloc_iconv(GIConv icv, const char * source);

/* Character protection helpers */
GString * gmysqlcc_helpers_add_slashes (const gchar * str);
GString * gmysqlcc_helpers_protect_underscore (const gchar * str);

#endif /* __GMYSQLCC_HELPERS_H__ */
