
#include "gmysqlcc_helpers.h"

#define __VAR_ARFIELDTYPES__

p_fieldTypeCapability * arFieldTypes;
int szarFieldTypes = -1;

#define __VAR_ARCHARSETLIST__

gchar * arOutputCharsets [] = {
	"ISO-8859-1", "ISO-8859-2", "ISO-8859-3", "ISO-8859-4", "ISO-8859-5", "ISO-8859-6", "ISO-8859-7",
	"ISO-8859-8", "ISO-8859-9", "ISO-8859-15", "LATIN 1", "LATIN 2", "UTF-8", 
	(gchar *)NULL };

/* 
		Charsets list functions
*/
GList * gmysqlcc_helpers_charset_list_new () {
	GList * lstCharset_items = (GList *)NULL;
	gchar * * pCurrCharset = (gchar * *)NULL;
	
	/* Fill output charset */
	pCurrCharset = arOutputCharsets;
	while (*pCurrCharset != (gchar *)NULL) {
		lstCharset_items = g_list_append (lstCharset_items, (gpointer) *pCurrCharset);
		pCurrCharset ++;
	}
	
	return lstCharset_items;
}

gchar * gmysqlcc_helpers_charset_list_get_by_index (gint idx) {
	return arOutputCharsets[idx];
}

gint gmysqlcc_helpers_charset_list_get_by_name (const gchar * name) {
	gchar * * pCurrCharset = (gchar * *)NULL;
	gchar * charset;
	
	charset = g_ascii_strup(name, -1);
	
	/* search charset */
	pCurrCharset = arOutputCharsets;
	while (*pCurrCharset != (gchar *)NULL) {
		if (g_ascii_strcasecmp(charset, *pCurrCharset)) {
			g_free(charset);
			return (pCurrCharset - arOutputCharsets) / sizeof(gchar *);
		}
		pCurrCharset ++;
	}
	
	g_free(charset);
	return -1;
}

/* 
		Field capabilities
*/
p_fieldTypeCapability gmysqlcc_helpers_field_capability_create (char * name, int value, gboolean unsign, gboolean zerofill, gboolean binary, gboolean autoincr) {
	p_fieldTypeCapability p_ftc;
	
	p_ftc = (p_fieldTypeCapability) g_try_malloc(sizeof(fieldTypeCapability));
	if (p_ftc != (p_fieldTypeCapability)NULL) {
		p_ftc->name = name;
		p_ftc->typeValue = value;
		p_ftc->unsign = unsign;
		p_ftc->zerofill = zerofill;
		p_ftc->binary = binary;
		p_ftc->autoincr = autoincr;
	}
	
	return p_ftc;
}

void gmysqlcc_helpers_field_capability_init_array () {
	szarFieldTypes = 0;
	arFieldTypes = (p_fieldTypeCapability *) g_try_malloc(sizeof(p_fieldTypeCapability) * 28);
	if (arFieldTypes != (p_fieldTypeCapability *)NULL) {
		arFieldTypes[szarFieldTypes] = gmysqlcc_helpers_field_capability_create("TINYINT"				, FTC_TV_LENGTH, TRUE, TRUE, FALSE, TRUE); szarFieldTypes ++;
		arFieldTypes[szarFieldTypes] = gmysqlcc_helpers_field_capability_create("SMALLINT"			, FTC_TV_LENGTH, TRUE, TRUE, FALSE, TRUE); szarFieldTypes ++;
		arFieldTypes[szarFieldTypes] = gmysqlcc_helpers_field_capability_create("MEDIUMINT"			, FTC_TV_LENGTH, TRUE, TRUE, FALSE, TRUE); szarFieldTypes ++;
		arFieldTypes[szarFieldTypes] = gmysqlcc_helpers_field_capability_create("INT"						, FTC_TV_LENGTH, TRUE, TRUE, FALSE, TRUE); szarFieldTypes ++;
		arFieldTypes[szarFieldTypes] = gmysqlcc_helpers_field_capability_create("INTEGER"				, FTC_TV_LENGTH, TRUE, TRUE, FALSE, TRUE); szarFieldTypes ++;
		arFieldTypes[szarFieldTypes] = gmysqlcc_helpers_field_capability_create("BIGINT"				, FTC_TV_LENGTH, TRUE, TRUE, FALSE, TRUE); szarFieldTypes ++;
		arFieldTypes[szarFieldTypes] = gmysqlcc_helpers_field_capability_create("REAL"					, FTC_TV_LENGTH_DECI, TRUE, TRUE, FALSE, FALSE); szarFieldTypes ++;
		arFieldTypes[szarFieldTypes] = gmysqlcc_helpers_field_capability_create("DOUBLE"				, FTC_TV_LENGTH_DECI, TRUE, TRUE, FALSE, FALSE); szarFieldTypes ++;
		arFieldTypes[szarFieldTypes] = gmysqlcc_helpers_field_capability_create("FLOAT"					, FTC_TV_LENGTH_DECI, TRUE, TRUE, FALSE, FALSE); szarFieldTypes ++;
		arFieldTypes[szarFieldTypes] = gmysqlcc_helpers_field_capability_create("DECIMAL"				, FTC_TV_LENGTH_DECI, TRUE, TRUE, FALSE, FALSE); szarFieldTypes ++;
		arFieldTypes[szarFieldTypes] = gmysqlcc_helpers_field_capability_create("NUMERIC"				, FTC_TV_LENGTH_DECI, TRUE, TRUE, FALSE, FALSE); szarFieldTypes ++;
		arFieldTypes[szarFieldTypes] = gmysqlcc_helpers_field_capability_create("CHAR"					, FTC_TV_LENGTH, FALSE, FALSE, TRUE, FALSE); szarFieldTypes ++;
		arFieldTypes[szarFieldTypes] = gmysqlcc_helpers_field_capability_create("VARCHAR"				, FTC_TV_LENGTH, FALSE, FALSE, TRUE, FALSE); szarFieldTypes ++;
		arFieldTypes[szarFieldTypes] = gmysqlcc_helpers_field_capability_create("DATE"					, FTC_TV_NONE, FALSE, FALSE, FALSE, FALSE); szarFieldTypes ++;
		arFieldTypes[szarFieldTypes] = gmysqlcc_helpers_field_capability_create("TIME"					, FTC_TV_NONE, FALSE, FALSE, FALSE, FALSE); szarFieldTypes ++;
		arFieldTypes[szarFieldTypes] = gmysqlcc_helpers_field_capability_create("TIMESTAMP"			, FTC_TV_NONE, FALSE, FALSE, FALSE, FALSE); szarFieldTypes ++;
		arFieldTypes[szarFieldTypes] = gmysqlcc_helpers_field_capability_create("DATETIME"			, FTC_TV_NONE, FALSE, FALSE, FALSE, FALSE); szarFieldTypes ++;
		arFieldTypes[szarFieldTypes] = gmysqlcc_helpers_field_capability_create("TINYBLOB"			, FTC_TV_NONE, FALSE, FALSE, FALSE, FALSE); szarFieldTypes ++;
		arFieldTypes[szarFieldTypes] = gmysqlcc_helpers_field_capability_create("BLOB"					, FTC_TV_NONE, FALSE, FALSE, FALSE, FALSE); szarFieldTypes ++;
		arFieldTypes[szarFieldTypes] = gmysqlcc_helpers_field_capability_create("MEDIUMBLOB"		, FTC_TV_NONE, FALSE, FALSE, FALSE, FALSE); szarFieldTypes ++;
		arFieldTypes[szarFieldTypes] = gmysqlcc_helpers_field_capability_create("LONGBLOB"			, FTC_TV_NONE, FALSE, FALSE, FALSE, FALSE); szarFieldTypes ++;
		arFieldTypes[szarFieldTypes] = gmysqlcc_helpers_field_capability_create("TINYTEXT"			, FTC_TV_NONE, FALSE, FALSE, FALSE, FALSE); szarFieldTypes ++;
		arFieldTypes[szarFieldTypes] = gmysqlcc_helpers_field_capability_create("TEXT"					, FTC_TV_NONE, FALSE, FALSE, FALSE, FALSE); szarFieldTypes ++;
		arFieldTypes[szarFieldTypes] = gmysqlcc_helpers_field_capability_create("MEDIUMTEXT"		, FTC_TV_NONE, FALSE, FALSE, FALSE, FALSE); szarFieldTypes ++;
		arFieldTypes[szarFieldTypes] = gmysqlcc_helpers_field_capability_create("LONGTEXT"			, FTC_TV_NONE, FALSE, FALSE, FALSE, FALSE); szarFieldTypes ++;
		arFieldTypes[szarFieldTypes] = gmysqlcc_helpers_field_capability_create("ENUM"					, FTC_TV_LIST_VALUE, FALSE, FALSE, FALSE, FALSE); szarFieldTypes ++;
		arFieldTypes[szarFieldTypes] = gmysqlcc_helpers_field_capability_create("SET"						, FTC_TV_LIST_VALUE, FALSE, FALSE, FALSE, FALSE); szarFieldTypes ++;
		arFieldTypes[szarFieldTypes] = NULL;
	}
}



/*
		Character conversion helpers
*/

int gmysqlcc_helpers_count_noascii_character (const gchar * string) {
	int i, size, retNbr = 0;
	
	size = strlen(string);
	
	for (i = 0; i < size; i++) {
		if ((unsigned char)string[i] > 127) {
			retNbr ++;
		}
	}
	
	/*g_printerr("*** Count noascii char : '%s'(%d) = (%d)\n", string, size, retNbr);*/
	
	return retNbr;
}

gchar * gmysqlcc_helpers_alloc_iconv(GIConv icv, const char * source) {
	gchar * tmpstrRead, * tmpstrWrite, * tmpstr = (gchar *)NULL;
	size_t retIcv;
	gsize nbRead, nbWrite;
	
	if (icv > 0) {
		tmpstrRead = (gchar *)source;
		nbRead = strlen(tmpstrRead);
		if (nbRead < 1) {
			tmpstr = (gchar *)NULL;
		} else {
			nbWrite = nbRead + (gmysqlcc_helpers_count_noascii_character(source) * 3) + 1;
			tmpstr = g_try_malloc((nbWrite) * sizeof(gchar));
			if (tmpstr != (gchar *)NULL) {
				*tmpstr = '\0';
				tmpstrWrite = tmpstr;
				
				retIcv = g_iconv(icv, &tmpstrRead, &nbRead, &tmpstrWrite, &nbWrite);
				
				if (retIcv < 0) {
					/*g_printerr("--- Iconv error (%d) : '%s'\n", errno, strerrror(errno));*/
					g_free(tmpstr);
					tmpstr = (gchar *)NULL;
				} else {
					/*g_printerr("+++ Number of translation pb : %d\n", retIcv);
					g_printerr("+++ Translation : '%s':'%s' -> '%s':'%s' \n", source, tmpstrRead, tmpstr, tmpstrWrite);*/
					*tmpstrWrite = '\0';
				}
			}
		}
	}
	
	if (tmpstr == (gchar *)NULL) {
		tmpstr = g_strdup(source);
	}
	
	return tmpstr;
}

GString * gmysqlcc_helpers_add_slashes (const gchar * str) {
	GString * pgstr;
	int i;
	
	pgstr = g_string_new(str);
	
	for (i = 0; i < pgstr->len; i++) {
		if (pgstr->str[i] == '\'' || pgstr->str[i] == '\"') {
			g_string_insert_c(pgstr, i, '\\');
			i++;
		}
	}
	
	return pgstr;
}

GString * gmysqlcc_helpers_protect_underscore (const gchar * str) {
	GString * pgstr;
	int i;
	
	pgstr = g_string_new(str);
	
	for (i = 0; i < pgstr->len; i++) {
		if (pgstr->str[i] == '_') {
			g_string_insert_c(pgstr, i, '_');
			i++;
		}
	}
	
	return pgstr;
}


