
#ifndef __GTABLEUTILS_H__
#define __GTABLEUTILS_H__

#include <gtk/gtk.h>
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

#define STTS_ORIGIN -1
#define STTS_ADD     1
#define STTS_MODIFY  2
#define STTS_DEL     3

#define FI_ORIGIN -1
#define FI_ADD     1
#define FI_MODIFY  2
#define FI_DEL     3

typedef struct _fieldInfo {
	GString * name;
	int type_idx;
	int length;
	int decimal;
	GString * set;
	GString * def;
	gboolean unsign;
	gboolean zerofill;
	gboolean binary;
	gboolean autoIncr;
	gboolean nullOk;
	int status;
	struct _fieldInfo * origin;
	struct _fieldInfo * update;
} fieldInfo;
typedef fieldInfo * p_fieldInfo;

typedef struct _indexInfo {
	GString * name;
	gboolean primary;
	gboolean unique;
	GList * fieldsIdx;
	int status;
	struct _indexInfo * origin;
	struct _indexInfo * update;
} indexInfo;
typedef indexInfo * p_indexInfo;

typedef struct _indexFieldInfo {
	GString * name;
	p_fieldInfo field_ref;
	int subpart;
	int status;
} indexFieldInfo;
typedef indexFieldInfo * p_indexFieldInfo;


p_fieldInfo createFieldInfo (int status);
gboolean checkFieldNameOk (GList * lstFields, GString * fieldName);
p_fieldInfo getFieldByName (GList * lstFields, GString * nameField);

GList * generateFieldAlterTable (GList * lstFields, GString * table);
GString * fieldToSQL (p_fieldInfo p_fi, GString * table);

p_indexInfo createIndexInfo (int status);
p_indexFieldInfo createIndexFieldInfo (int status);


#endif /* __GTABLEUTILS_H__ */
