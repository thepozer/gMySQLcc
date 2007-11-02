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

#include "gmlc_dump_source_table.h"
#include "gmlc_dump_source.h"

static void gmlc_dump_source_table_finalize (GmlcDumpSourceTable * pGmlcDmpSrcTbl);
static void gmlc_dump_source_table_get_property (GObject * object, guint prop_id, GValue * value, GParamSpec * pspec);
static void gmlc_dump_source_table_set_property (GObject * object, guint prop_id, const GValue * value, GParamSpec * pspec);

static void gmlc_dump_source_table_interface_init (gpointer g_iface, gpointer iface_data);
static gboolean gmlc_dump_source_table_can_get_struct (GmlcDumpSource * self);
static gchar * gmlc_dump_source_table_get_struct (GmlcDumpSource * self);
static gboolean gmlc_dump_source_table_can_get_data (GmlcDumpSource * self);
static GArray * gmlc_dump_source_table_get_data (GmlcDumpSource * self);


enum {
	PROP_0,
	PROP_TABLE,
};

G_DEFINE_TYPE_WITH_CODE (GmlcDumpSourceTable, gmlc_dump_source_table, G_TYPE_OBJECT, 
	G_IMPLEMENT_INTERFACE (GMLC_DUMP_TYPE_SOURCE, gmlc_dump_source_table_interface_init));

static void gmlc_dump_source_table_interface_init (gpointer g_iface, gpointer iface_data) {
  GmlcDumpSourceInterface *pIface = (GmlcDumpSourceInterface *)g_iface;
  
  pIface->can_get_struct = (gboolean (*) (GmlcDumpSource * self))gmlc_dump_source_table_can_get_struct;
  pIface->get_struct = (gchar * (*) (GmlcDumpSource * self))gmlc_dump_source_table_get_struct;
  pIface->can_get_data = (gboolean (*) (GmlcDumpSource * self))gmlc_dump_source_table_can_get_data;
  pIface->get_data = (GArray * (*) (GmlcDumpSource * self))gmlc_dump_source_table_get_data;
}

static void gmlc_dump_source_table_class_init (GmlcDumpSourceTableClass * pClass) {
	GObjectClass *pObjClass = G_OBJECT_CLASS(pClass);
	
	pObjClass->finalize = (GObjectFinalizeFunc) gmlc_dump_source_table_finalize;
	
	pObjClass->get_property = gmlc_dump_source_table_get_property;
	pObjClass->set_property = gmlc_dump_source_table_set_property;

	g_object_class_install_property(pObjClass, PROP_TABLE, 
		g_param_spec_object("table", "Table object", "Table object", GMLC_TYPE_MYSQL_TABLE, G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY));
}

static void gmlc_dump_source_table_get_property (GObject * object, guint prop_id, GValue * value, GParamSpec * pspec) {
	GmlcDumpSourceTable * pGmlcDmpSrcTbl = GMLC_DUMP_SOURCE_TABLE(object);
	
	switch (prop_id) {
		case PROP_TABLE :
			g_value_set_object(value, pGmlcDmpSrcTbl->pGmlcMysqlTbl);
			break;
		default: {
			G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
			break;
		}
	}
}

static void gmlc_dump_source_table_set_property (GObject * object, guint prop_id, const GValue * value, GParamSpec * pspec) {
	GmlcDumpSourceTable * pGmlcDmpSrcTbl = GMLC_DUMP_SOURCE_TABLE(object);
	
	switch (prop_id) {
		case PROP_TABLE :
			pGmlcDmpSrcTbl->pGmlcMysqlTbl = g_value_get_object(value);
			break;
		default: {
			G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
			break;
		}
	}
}

static void gmlc_dump_source_table_init (GmlcDumpSourceTable * pGmlcDmpSrcTbl) {
	UNUSED_VAR(pGmlcDmpSrcTbl);
	
}

static void gmlc_dump_source_table_finalize (GmlcDumpSourceTable * pGmlcDmpSrcTbl) {
	UNUSED_VAR(pGmlcDmpSrcTbl);
	
}

static gboolean gmlc_dump_source_table_can_get_struct (GmlcDumpSource * self) {
	UNUSED_VAR(self);
	
	return TRUE;
}

static gchar * gmlc_dump_source_table_get_struct (GmlcDumpSource * self) {
	GmlcDumpSourceTable * pGmlcDmpSrcTbl = GMLC_DUMP_SOURCE_TABLE(self);
	gchar * pcRetValue = NULL;
	
	pcRetValue = gmlc_mysql_structure_get_create(GMLC_MYSQL_STRUCTURE(pGmlcDmpSrcTbl->pGmlcMysqlTbl), TRUE, NULL);
	
	return pcRetValue;
}

static gboolean gmlc_dump_source_table_can_get_data (GmlcDumpSource * self) {
	UNUSED_VAR(self);
	
	return TRUE;
}

static GArray * gmlc_dump_source_table_get_data (GmlcDumpSource * self) {
	GmlcDumpSourceTable * poGmlcDmpSrcTbl = GMLC_DUMP_SOURCE_TABLE(self);
	GmlcDumpSourceData * poGmlcDmpSrcData = NULL;
	GmlcMysqlDatabase * poGmlcMysqlDb = NULL;
	GmlcMysqlQuery * poGmlcMysqlQry = NULL;
	GArray * arRetValue = NULL, * arAllRecords = NULL, * arRecord = NULL;
	gchar * pcDbName = NULL, * pcTblName = NULL, * pcSql = NULL;
	
	arRetValue = g_array_new(TRUE, TRUE, 1);
	
	if (poGmlcDmpSrcTbl->pGmlcMysqlTbl == NULL) {
		return NULL;
	}
	g_object_get(poGmlcDmpSrcTbl->pGmlcMysqlTbl, "database", &poGmlcMysqlDb, "name", &pcTblName, NULL);
	
	if (poGmlcMysqlDb == NULL) {
		return NULL;
	}
	g_object_get(poGmlcMysqlDb, "db_name", &pcDbName, NULL);
	
	poGmlcMysqlQry = gmlc_mysql_database_get_query(poGmlcMysqlDb);
	if (poGmlcMysqlQry == NULL) {
		return NULL;
	}
	
	poGmlcDmpSrcData = g_try_new0(GmlcDumpSourceData, 1);
	g_array_append_val(arRetValue, poGmlcDmpSrcData);
	
	poGmlcDmpSrcData->pcDatabaseName = g_strdup(pcDbName);
	poGmlcDmpSrcData->pcTableName = g_strdup(pcTblName);
	
	
	pcSql = g_strdup_printf("SELECT * FROM `%s`.`%s`;", pcDbName, pcTblName);
	
	if (gmlc_mysql_query_execute(poGmlcMysqlQry, pcSql, strlen(pcSql), FALSE) && gmlc_mysql_query_have_record(poGmlcMysqlQry)) {
		arAllRecords = g_array_new(TRUE, TRUE, 10);
		
		while ((arRecord = gmlc_mysql_query_next_record(poGmlcMysqlQry)) != NULL) {
			g_array_append_val(arAllRecords, arRecord);
			arRecord = NULL;
		}
		
		poGmlcDmpSrcData->arDatas = arAllRecords;
		poGmlcDmpSrcData->arHeaders = gmlc_mysql_query_get_headers(poGmlcMysqlQry);
	} else {
		poGmlcDmpSrcData->arHeaders = g_array_new(TRUE, TRUE, 0);
		poGmlcDmpSrcData->arDatas = g_array_new(TRUE, TRUE, 0);
	}
	
	g_free(pcTblName);
	g_free(pcDbName);
	
	return arRetValue;
}

GmlcDumpSourceTable * gmlc_dump_source_table_new (GmlcMysqlTable * pGmlcMysqlTbl) {
	GmlcDumpSourceTable * pGmlcDmpSrcTbl = NULL;
	
	pGmlcDmpSrcTbl = GMLC_DUMP_SOURCE_TABLE(g_object_new(GMLC_DUMP_TYPE_SOURCE_TABLE, "table", pGmlcMysqlTbl, NULL));
	
	return pGmlcDmpSrcTbl;
}
