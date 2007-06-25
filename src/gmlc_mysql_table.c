/***************************************************************************
 *            gmlc_mysql_table.c
 *
 *  
 *  Copyright  2007  Didier "pozer" Prolhac
 *  pozer@thepozer.net
 ***************************************************************************/

/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include "gmlc_mysql_table.h"

static void gmlc_mysql_table_finalize (GmlcMysqlTable * pGmlcMysqlTbl);
static void gmlc_mysql_table_get_property (GObject * object, guint prop_id, GValue * value, GParamSpec * pspec);
static void gmlc_mysql_table_set_property (GObject * object, guint prop_id, const GValue * value, GParamSpec * pspec);

static void gmlc_mysql_table_interface_structure_init (gpointer g_iface, gpointer iface_data);
static gchar * gmlc_mysql_table_structure_get_create (GmlcMysqlTable * pGmlcMysqlTbl, gboolean bMyself, const gchar * pcOtherName);
static gchar * gmlc_mysql_table_structure_get_alter (GmlcMysqlTable * pGmlcMysqlTbl, gboolean bMyself, const gchar * pcOtherName);
static gchar * gmlc_mysql_table_structure_get_drop (GmlcMysqlTable * pGmlcMysqlTbl, gboolean bMyself, const gchar * pcOtherName);

enum {
	PROP_0,
	PROP_DATABASE,
	PROP_NAME,
	PROP_ROWS,
	PROP_SIZE,
	PROP_ENGINE,
	PROP_FLAGGED
};


G_DEFINE_TYPE_WITH_CODE (GmlcMysqlTable, gmlc_mysql_table, G_TYPE_OBJECT, 
	G_IMPLEMENT_INTERFACE (GMLC_MYSQL_TYPE_STRUCTURE, gmlc_mysql_table_interface_structure_init));

static void gmlc_mysql_table_interface_structure_init (gpointer g_iface, gpointer iface_data) {
	GmlcMysqlStructureInterface * pIface = (GmlcMysqlStructureInterface *) g_iface;
	pIface->get_create = (gchar * (*) (GmlcMysqlStructure * poSelf, gboolean bMyself, const gchar * pcOtherName))gmlc_mysql_table_structure_get_create;
	pIface->get_alter = (gchar * (*) (GmlcMysqlStructure * poSelf, gboolean bMyself, const gchar * pcOtherName))gmlc_mysql_table_structure_get_alter;
	pIface->get_drop = (gchar * (*) (GmlcMysqlStructure * poSelf, gboolean bMyself, const gchar * pcOtherName))gmlc_mysql_table_structure_get_drop;
}

static void gmlc_mysql_table_class_init(GmlcMysqlTableClass *pClass) {
	GObjectClass * pObjClass = G_OBJECT_CLASS(pClass);

	pObjClass->finalize = (GObjectFinalizeFunc) gmlc_mysql_table_finalize;
	pObjClass->get_property = gmlc_mysql_table_get_property;
	pObjClass->set_property = gmlc_mysql_table_set_property;
	
	g_object_class_install_property(pObjClass, PROP_DATABASE, 
		g_param_spec_object("database", "Database object", "Database object", GMLC_MYSQL_TYPE_DATABASE, G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY));
	g_object_class_install_property(pObjClass, PROP_NAME, 
		g_param_spec_string("name", "Table name", "Name of the table", "", G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY));
	g_object_class_install_property(pObjClass, PROP_ROWS, 
		g_param_spec_string("rows", "Table rows", "Rows' number of the table", "", G_PARAM_READWRITE | G_PARAM_CONSTRUCT));
	g_object_class_install_property(pObjClass, PROP_SIZE, 
		g_param_spec_string("size", "Table size", "Size of the table", "", G_PARAM_READWRITE | G_PARAM_CONSTRUCT));
	g_object_class_install_property(pObjClass, PROP_ENGINE, 
		g_param_spec_string("engine", "Table type", "Type of the table", "", G_PARAM_READWRITE | G_PARAM_CONSTRUCT));
	g_object_class_install_property(pObjClass, PROP_FLAGGED, 
		g_param_spec_boolean("flagged", "Object Flagged", "Object Flagged", FALSE, G_PARAM_READWRITE));
}

static void gmlc_mysql_table_init(GmlcMysqlTable * pGmlcMysqlTbl) {
	pGmlcMysqlTbl->pGmlcMysqlDb = NULL;
	pGmlcMysqlTbl->pcName = NULL;
	pGmlcMysqlTbl->pcRows = NULL;
	pGmlcMysqlTbl->pcSize = NULL;
	pGmlcMysqlTbl->pcEngine = NULL;
	pGmlcMysqlTbl->bFlagged = FALSE;
}

static void gmlc_mysql_table_finalize(GmlcMysqlTable * pGmlcMysqlTbl) {
	
	g_free(pGmlcMysqlTbl->pcName);
	g_free(pGmlcMysqlTbl->pcRows);
	g_free(pGmlcMysqlTbl->pcSize);
	g_free(pGmlcMysqlTbl->pcEngine);
	/*G_OBJECT_CLASS(parent_class)->finalize(object);*/
}

static void gmlc_mysql_table_get_property (GObject * object, guint prop_id, GValue * value, GParamSpec * pspec) {
	GmlcMysqlTable * pGmlcMysqlTbl = GMLC_MYSQL_TABLE(object);
	
	switch (prop_id) {
		case PROP_DATABASE :
			g_value_set_object(value, pGmlcMysqlTbl->pGmlcMysqlDb);
			break;
		case PROP_NAME :
			g_value_set_string(value, pGmlcMysqlTbl->pcName);
			break;
		case PROP_ROWS :
			g_value_set_string(value, pGmlcMysqlTbl->pcRows);
			break;
		case PROP_SIZE :
			g_value_set_string(value, pGmlcMysqlTbl->pcSize);
			break;
		case PROP_ENGINE :
			g_value_set_string(value, pGmlcMysqlTbl->pcEngine);
			break;
		case PROP_FLAGGED :
			g_value_set_boolean(value, pGmlcMysqlTbl->bFlagged);
			break;
		default: {
			G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
			break;
		}
	}
}

static void gmlc_mysql_table_set_property (GObject * object, guint prop_id, const GValue * value, GParamSpec * pspec) {
	GmlcMysqlTable * pGmlcMysqlTbl = GMLC_MYSQL_TABLE(object);
	
	switch (prop_id) {
		case PROP_DATABASE :
			pGmlcMysqlTbl->pGmlcMysqlDb = g_value_get_object(value);
			break;
		case PROP_NAME :
			g_free(pGmlcMysqlTbl->pcName);
			
			pGmlcMysqlTbl->pcName = g_value_dup_string(value);
			break;
		case PROP_ROWS :
			g_free(pGmlcMysqlTbl->pcRows);
			
			pGmlcMysqlTbl->pcRows = g_value_dup_string(value);
			break;
		case PROP_SIZE :
			g_free(pGmlcMysqlTbl->pcSize);
			
			pGmlcMysqlTbl->pcSize = g_value_dup_string(value);
			break;
		case PROP_ENGINE :
			g_free(pGmlcMysqlTbl->pcEngine);
			
			pGmlcMysqlTbl->pcEngine = g_value_dup_string(value);
			break;
		case PROP_FLAGGED :
			pGmlcMysqlTbl->bFlagged = g_value_get_boolean(value);
			break;
		default: {
			G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
			break;
		}
	}
}

static gchar * gmlc_mysql_table_structure_get_create (GmlcMysqlTable * pGmlcMysqlTbl, gboolean bMyself, const gchar * pcOtherName) {
	const gchar * pcName = NULL;
	gchar * pcSqlQuery = NULL, * pcQuery = NULL;
	
	if (bMyself) {
		g_object_get(pGmlcMysqlTbl, "name", &pcName, NULL);
		pcQuery = g_strdup_printf("SHOW CREATE TABLE `%s`;", pcName);
		
		pcSqlQuery = gmlc_mysql_query_static_get_one_result(pGmlcMysqlTbl->pGmlcMysqlDb->pGmlcMysqlSrv, pGmlcMysqlTbl->pGmlcMysqlDb->pcDbName, pcQuery, 1);
		
		g_free(pcQuery);
	} else {
		if (pcOtherName != NULL) {
			pcName = pcOtherName; 
		} else {
			pcName = "<Name of the database>";
		}
		pcSqlQuery = g_strdup_printf("CREATE TABLE `%s`;", pcName);
	}
	
	return pcSqlQuery;
}

static gchar * gmlc_mysql_table_structure_get_alter (GmlcMysqlTable * pGmlcMysqlTbl, gboolean bMyself, const gchar * pcOtherName) {
	const gchar * pcName = NULL;
	gchar * pcSqlQuery = NULL;
	
	if (bMyself) {
		pcName = pGmlcMysqlTbl->pcName;
	} else if (pcOtherName != NULL) {
		pcName = pcOtherName; 
	} else {
		pcName = "<Name of the database>";
	}
	
	pcSqlQuery = g_strdup_printf("ALTER TABLE `%s`\n <Add actions>;", pcName);
	
	return pcSqlQuery;
}

static gchar * gmlc_mysql_table_structure_get_drop (GmlcMysqlTable * pGmlcMysqlTbl, gboolean bMyself, const gchar * pcOtherName) {
	const gchar * pcName = NULL;
	gchar * pcSqlQuery = NULL;
	
	if (bMyself) {
		pcName = pGmlcMysqlTbl->pcName;
	} else if (pcOtherName != NULL) {
		pcName = pcOtherName; 
	} else {
		pcName = "<Name of the database>";
	}
	
	pcSqlQuery = g_strdup_printf("DROP TABLE `%s`;", pcName);
	
	return pcSqlQuery;
}

GmlcMysqlTable * gmlc_mysql_table_new(GmlcMysqlDatabase * pGmlcMysqlDb, gchar * pcTblName) {
	GmlcMysqlTable * pGmlcMysqlTbl;
	
	pGmlcMysqlTbl = GMLC_MYSQL_TABLE(g_object_new(GMLC_TYPE_MYSQL_TABLE, "database", pGmlcMysqlDb, "name", pcTblName, NULL));
	
	return pGmlcMysqlTbl;
}

GmlcMysqlTable * gmlc_mysql_table_new_with_stat(GmlcMysqlDatabase * pGmlcMysqlDb, gchar * pcTblName, gchar * pcRows, gchar * pcSize, gchar * pcEngine) {
	GmlcMysqlTable * pGmlcMysqlTbl;
	
	pGmlcMysqlTbl = GMLC_MYSQL_TABLE(g_object_new(GMLC_TYPE_MYSQL_TABLE, "database", pGmlcMysqlDb, "name", pcTblName, "rows", pcRows, "size", pcSize, "engine", pcEngine, NULL));
	
	return pGmlcMysqlTbl;
}

