
#ifndef __GTABLE_H__
#define __GTABLE_H__

#include <gtk/gtk.h>
#include <string.h>

#include "mysql_funcs.h"
#include "conf_funcs.h"
#include "gUtils.h"

#include "gTableUtils.h"
#include "gTableEvents.h"

typedef struct _tableWnd {
	GtkWidget * LstField;
	GtkWidget * txtFieldName;
	GtkWidget * cmbFieldType;
	GtkWidget * txtFieldLen;
	GtkWidget * txtFieldDecimal;
	GtkWidget * txtFieldValues;
	GtkWidget * txtFieldDefault;
	GtkWidget * cmbFieldSpecific;
	GtkWidget * chkFieldUnsign;
	GtkWidget * chkFieldZeroFill;
	GtkWidget * chkFieldBinary;
	GtkWidget * chkFieldAutoIncr;
	GtkWidget * chkFieldNull;
	
	GtkWidget * LstIndex;
	GtkWidget * txtIndexName;
	GtkWidget * chkIndexPrimary;
	GtkWidget * chkIndexUnique;
	GtkWidget * cmbIndexLstFields;
	GtkWidget * lstIndexUsed;
	GtkWidget * txtIndexSubPart;
	
	GtkWidget * cmbTableFormat;
	GtkWidget * txtTableName;
	GtkWidget * txtXComments;
	GtkWidget * wndTable;
	
	mysql_connect_infos * p_mci;
	GString * tableName;
	GList * p_lstField;
	GList * p_lstIndex;
	GString * comments;
	GString * tableFormat;
	gboolean tableModified;
	GData * p_dctStatus;

	p_fieldInfo currField;
	int currType;
	p_indexInfo currIndex;
	p_indexFieldInfo currIndexFI;
} tableWnd;
typedef tableWnd * p_tableWnd;

p_tableWnd create_wndTable (gboolean display, mysql_connect_infos * p_mci, const char * table);

#endif /* __GTABLE_H__ */
