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

enum {
	PROP_0,
	PROP_DATABASE,
	PROP_NAME,
	PROP_ROWS,
	PROP_SIZE,
	PROP_TYPE,
	PROP_FLAGGED
};


G_DEFINE_TYPE (GmlcMysqlTable, gmlc_mysql_table, G_TYPE_OBJECT)

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
		g_param_spec_string("rows", "Table rows", "Rows' number of the table", "", G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY));
	g_object_class_install_property(pObjClass, PROP_SIZE, 
		g_param_spec_string("size", "Table size", "Size of the table", "", G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY));
	g_object_class_install_property(pObjClass, PROP_TYPE, 
		g_param_spec_string("type", "Table type", "Type of the table", "", G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY));
	g_object_class_install_property(pObjClass, PROP_FLAGGED, 
		g_param_spec_boolean("flagged", "Object Flagged", "Object Flagged", FALSE, G_PARAM_READWRITE));
}

static void gmlc_mysql_table_init(GmlcMysqlTable * pGmlcMysqlTbl) {
	pGmlcMysqlTbl->pGmlcMysqlDb = NULL;
	pGmlcMysqlTbl->pcName = NULL;
	pGmlcMysqlTbl->pcRows = NULL;
	pGmlcMysqlTbl->pcSize = NULL;
	pGmlcMysqlTbl->pcType = NULL;
	pGmlcMysqlTbl->bFlagged = FALSE;
}

static void gmlc_mysql_table_finalize(GmlcMysqlTable * pGmlcMysqlTbl) {
	
	g_free(pGmlcMysqlTbl->pcName);
	g_free(pGmlcMysqlTbl->pcRows);
	g_free(pGmlcMysqlTbl->pcSize);
	g_free(pGmlcMysqlTbl->pcType);
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
		case PROP_TYPE :
			g_value_set_string(value, pGmlcMysqlTbl->pcType);
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
		case PROP_TYPE :
			g_free(pGmlcMysqlTbl->pcType);
			
			pGmlcMysqlTbl->pcType = g_value_dup_string(value);
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

GmlcMysqlTable * gmlc_mysql_table_new(GmlcMysqlDatabase * pGmlcMysqlDb, gchar * pcTblName) {
	GmlcMysqlTable * pGmlcMysqlTbl;
	
	pGmlcMysqlTbl = GMLC_MYSQL_TABLE(g_object_new(GMLC_TYPE_MYSQL_TABLE, "database", pGmlcMysqlDb, "name", pcTblName, NULL));
	
	return pGmlcMysqlTbl;
}

GmlcMysqlTable * gmlc_mysql_table_new_with_stat(GmlcMysqlDatabase * pGmlcMysqlDb, gchar * pcTblName, gchar * pcRows, gchar * pcSize, gchar * pcType) {
	GmlcMysqlTable * pGmlcMysqlTbl;
	
	pGmlcMysqlTbl = GMLC_MYSQL_TABLE(g_object_new(GMLC_TYPE_MYSQL_TABLE, "database", pGmlcMysqlDb, "name", pcTblName, "rows", pcRows, "size", pcSize, "type", pcType, NULL));
	
	return pGmlcMysqlTbl;
}

