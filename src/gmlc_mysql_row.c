/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include "gmlc_mysql_row.h"

#include <stdlib.h>
#include <string.h>

static void gmlc_mysql_row_finalize (GmlcMysqlRow * pGmlcMysqlRow);
static void gmlc_mysql_row_set_property (GObject * object, guint prop_id, const GValue * value, GParamSpec * pspec);
static void gmlc_mysql_row_get_property (GObject * object, guint prop_id, GValue * value, GParamSpec * pspec);


enum {
	PROP_0,
	PROP_QUERY,
	PROP_CAN_EDIT,
};

G_DEFINE_TYPE (GmlcMysqlRow, gmlc_mysql_row, G_TYPE_OBJECT);

static void gmlc_mysql_row_class_init (GmlcMysqlRowClass * pClass) {
	GObjectClass *pObjClass = G_OBJECT_CLASS(pClass);
	
	pObjClass->finalize = (GObjectFinalizeFunc) gmlc_mysql_row_finalize;
	
	pObjClass->get_property = gmlc_mysql_row_get_property;
	pObjClass->set_property = gmlc_mysql_row_set_property;
	
	g_object_class_install_property(pObjClass, PROP_QUERY, 
		g_param_spec_object("query", "Query object", "Query object", GMLC_MYSQL_TYPE_QUERY, G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY));
	g_object_class_install_property(pObjClass, PROP_CAN_EDIT, 
		g_param_spec_boolean("can_edit", "Can edit row", "Can edit row", FALSE, G_PARAM_READABLE));
	
}

static void gmlc_mysql_row_init (GmlcMysqlRow * pGmlcMysqlRow) {
	pGmlcMysqlRow->pGmlcMysqlQry = NULL;/* r-c */
	pGmlcMysqlRow->arResults = NULL;
	pGmlcMysqlRow->arMysqlHeadersName = NULL;
	pGmlcMysqlRow->pcAbsTableName = NULL;
	pGmlcMysqlRow->pcPrimaryWherePart = NULL;
}

static void gmlc_mysql_row_finalize (GmlcMysqlRow * pGmlcMysqlRow) {
	g_free(pGmlcMysqlRow->pcAbsTableName);
	g_free(pGmlcMysqlRow->pcPrimaryWherePart);
}

static void gmlc_mysql_row_set_property (GObject * object, guint prop_id, const GValue * value, GParamSpec * pspec) {
	GmlcMysqlRow * pGmlcMysqlRow = GMLC_MYSQL_ROW(object);
	
	switch (prop_id) {
		case PROP_QUERY :
			pGmlcMysqlRow->pGmlcMysqlQry = g_value_get_object(value);
			break;
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
			break;
	}
}

static void gmlc_mysql_row_get_property (GObject * object, guint prop_id, GValue * value, GParamSpec * pspec) {
	GmlcMysqlRow * pGmlcMysqlRow = GMLC_MYSQL_ROW(object);
	UNUSED_VAR(pGmlcMysqlRow);
	
	switch (prop_id) {
		case PROP_CAN_EDIT:
			g_value_set_boolean(value, (pGmlcMysqlRow->pcAbsTableName != (gchar *) NULL));
			break;
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
			break;
	}
}

GmlcMysqlRow * gmlc_mysql_row_new_next_record (GmlcMysqlQuery * pGmlcMysqlQry) {
	GmlcMysqlRow * pGmlcMysqlRow = NULL;
	GArray * arRetData;
	
	arRetData = gmlc_mysql_query_next_record(pGmlcMysqlQry);
	
	if (arRetData == (GArray *)NULL) {
		return (GmlcMysqlRow *)NULL;
	}
	
	pGmlcMysqlRow = GMLC_MYSQL_ROW(g_object_new (GMLC_MYSQL_TYPE_ROW, "query", pGmlcMysqlQry, NULL));
	
	if (pGmlcMysqlRow == (GmlcMysqlRow *)NULL) {
		return (GmlcMysqlRow *)NULL;
	}
	
	pGmlcMysqlRow->arResults = arRetData;
	pGmlcMysqlRow->arMysqlHeadersName = gmlc_mysql_query_get_headers(pGmlcMysqlQry, TRUE);
	
	pGmlcMysqlRow->pcAbsTableName = gmlc_mysql_query_get_absolute_table_name(pGmlcMysqlQry, FALSE);
	if (pGmlcMysqlRow->pcAbsTableName != (gchar *)NULL) {
		pGmlcMysqlRow->pcPrimaryWherePart = gmlc_mysql_query_get_primary_where(pGmlcMysqlQry, pGmlcMysqlRow->arResults);
	} else {
		pGmlcMysqlRow->pcPrimaryWherePart = (gchar *)NULL;
	}
	
	return pGmlcMysqlRow;
}

gchar * gmlc_mysql_row_get_field_value(GmlcMysqlRow * pGmlcMysqlRow, gint idx) {
	return g_array_index(pGmlcMysqlRow->arResults, gchar *, idx);
}

gchar * gmlc_mysql_row_set_field_value(GmlcMysqlRow * pGmlcMysqlRow, gint idx, const gchar * new_value) {
	GmlcMysqlQuery * poGmlcMysqlQry = NULL;
	gboolean ret = FALSE;
	gchar * str = NULL, * pcSqlUpd = NULL, * pcFieldName = NULL;
	
	pcFieldName = g_array_index(pGmlcMysqlRow->arMysqlHeadersName, gchar *, idx);
	poGmlcMysqlQry = gmlc_mysql_query_new_duplicate(pGmlcMysqlRow->pGmlcMysqlQry);
	
	pcSqlUpd = g_strdup_printf("UPDATE %s SET `%s` = '%s' WHERE %s", pGmlcMysqlRow->pcAbsTableName, pcFieldName, new_value, pGmlcMysqlRow->pcPrimaryWherePart);
	g_print("Update SQL : '%s'\n", pcSqlUpd);
	
	ret = gmlc_mysql_query_execute(poGmlcMysqlQry, pcSqlUpd, -1, FALSE);
	g_object_unref(poGmlcMysqlQry);
	
	g_free(pcSqlUpd);
	
	if (ret) {
		str = g_strdup(new_value);
		g_array_insert_val(pGmlcMysqlRow->arResults, idx, str);
		g_array_remove_index(pGmlcMysqlRow->arResults, idx + 1);
		return str;
	} else {
		return (gchar *)NULL;
	}
}

gchar * gmlc_mysql_row_get_row_text(GmlcMysqlRow * pGmlcMysqlRow) {
	GString * strTextRow = NULL;
	gchar * pcRet = NULL, * pcValue = NULL;
	gint iIdx = 0;
	
	strTextRow = g_string_new("");
	
	for (iIdx = 0; iIdx < pGmlcMysqlRow->arResults->len; iIdx++) {
		pcValue = g_array_index(pGmlcMysqlRow->arResults, gchar *, iIdx);
		if (iIdx != 0) {
			g_string_append_c(strTextRow, '\t');
		}
		g_string_append(strTextRow, pcValue);
	}
	
	pcRet = g_strdup(strTextRow->str);
	g_string_free(strTextRow, TRUE);
	
	return pcRet;
}

gboolean gmlc_mysql_row_delete(GmlcMysqlRow * pGmlcMysqlRow) {
	GmlcMysqlQuery * poGmlcMysqlQry = NULL;
	gchar * pcSql = NULL;
	gboolean bRet = FALSE;
	
	poGmlcMysqlQry = gmlc_mysql_query_new_duplicate(pGmlcMysqlRow->pGmlcMysqlQry);
	
	pcSql = g_strdup_printf("DELETE FROM %s WHERE %s LIMIT 1", pGmlcMysqlRow->pcAbsTableName, pGmlcMysqlRow->pcPrimaryWherePart);
	g_print("Delete SQL : '%s'\n", pcSql);
	
	bRet = gmlc_mysql_query_execute(poGmlcMysqlQry, pcSql, -1, FALSE);
	
	g_object_unref(poGmlcMysqlQry);
	g_object_unref(pGmlcMysqlRow);
	
	return bRet;
}

