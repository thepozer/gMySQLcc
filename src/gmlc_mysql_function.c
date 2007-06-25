/***************************************************************************
 *            gmlc_mysql_function.c
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

#include "gmlc_mysql_function.h"

static void gmlc_mysql_function_finalize (GmlcMysqlFunction * pGmlcMysqlFnct);
static void gmlc_mysql_function_get_property (GObject * object, guint prop_id, GValue * value, GParamSpec * pspec);
static void gmlc_mysql_function_set_property (GObject * object, guint prop_id, const GValue * value, GParamSpec * pspec);

static void gmlc_mysql_function_interface_structure_init (gpointer g_iface, gpointer iface_data);
static gchar * gmlc_mysql_function_structure_get_create (GmlcMysqlTable * pGmlcMysqlTbl, gboolean bMyself, const gchar * pcOtherName);
static gchar * gmlc_mysql_function_structure_get_alter (GmlcMysqlTable * pGmlcMysqlTbl, gboolean bMyself, const gchar * pcOtherName);
static gchar * gmlc_mysql_function_structure_get_drop (GmlcMysqlTable * pGmlcMysqlTbl, gboolean bMyself, const gchar * pcOtherName);

enum {
	PROP_0,
	PROP_DATABASE,
	PROP_NAME,
	PROP_FLAGGED
};


G_DEFINE_TYPE_WITH_CODE (GmlcMysqlFunction, gmlc_mysql_function, G_TYPE_OBJECT,
	G_IMPLEMENT_INTERFACE (GMLC_MYSQL_TYPE_STRUCTURE, gmlc_mysql_function_interface_structure_init));

static void gmlc_mysql_function_interface_structure_init (gpointer g_iface, gpointer iface_data) {
	GmlcMysqlStructureInterface * pIface = (GmlcMysqlStructureInterface *) g_iface;
	pIface->get_create = (gchar * (*) (GmlcMysqlStructure * poSelf, gboolean bMyself, const gchar * pcOtherName))gmlc_mysql_function_structure_get_create;
	pIface->get_alter = (gchar * (*) (GmlcMysqlStructure * poSelf, gboolean bMyself, const gchar * pcOtherName))gmlc_mysql_function_structure_get_alter;
	pIface->get_drop = (gchar * (*) (GmlcMysqlStructure * poSelf, gboolean bMyself, const gchar * pcOtherName))gmlc_mysql_function_structure_get_drop;
}


static void gmlc_mysql_function_class_init(GmlcMysqlFunctionClass *pClass) {
	GObjectClass * pObjClass = G_OBJECT_CLASS(pClass);

	pObjClass->finalize = (GObjectFinalizeFunc) gmlc_mysql_function_finalize;
	pObjClass->get_property = gmlc_mysql_function_get_property;
	pObjClass->set_property = gmlc_mysql_function_set_property;
	
	g_object_class_install_property(pObjClass, PROP_DATABASE, 
		g_param_spec_object("database", "Database object", "Database object", GMLC_MYSQL_TYPE_DATABASE, G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY));
	g_object_class_install_property(pObjClass, PROP_NAME, 
		g_param_spec_string("name", "Function name", "Name of the function", "", G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY));
	g_object_class_install_property(pObjClass, PROP_FLAGGED, 
		g_param_spec_boolean("flagged", "Object Flagged", "Object Flagged", FALSE, G_PARAM_READWRITE));
}

static void gmlc_mysql_function_init(GmlcMysqlFunction * pGmlcMysqlFnct) {
	pGmlcMysqlFnct->pGmlcMysqlDb = NULL;
	pGmlcMysqlFnct->pcName = NULL;
	pGmlcMysqlFnct->bFlagged = FALSE;
}

static void gmlc_mysql_function_finalize(GmlcMysqlFunction * pGmlcMysqlFnct) {
	
	g_free(pGmlcMysqlFnct->pcName);
	/*G_OBJECT_CLASS(parent_class)->finalize(object);*/
}

static void gmlc_mysql_function_get_property (GObject * object, guint prop_id, GValue * value, GParamSpec * pspec) {
	GmlcMysqlFunction * pGmlcMysqlFnct = GMLC_MYSQL_FUNCTION(object);
	
	switch (prop_id) {
		case PROP_DATABASE :
			g_value_set_object(value, pGmlcMysqlFnct->pGmlcMysqlDb);
			break;
		case PROP_NAME :
			g_value_set_string(value, pGmlcMysqlFnct->pcName);
			break;
		case PROP_FLAGGED :
			g_value_set_boolean(value, pGmlcMysqlFnct->bFlagged);
			break;
		default: {
			G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
			break;
		}
	}
}

static void gmlc_mysql_function_set_property (GObject * object, guint prop_id, const GValue * value, GParamSpec * pspec) {
	GmlcMysqlFunction * pGmlcMysqlFnct = GMLC_MYSQL_FUNCTION(object);
	
	switch (prop_id) {
		case PROP_DATABASE :
			pGmlcMysqlFnct->pGmlcMysqlDb = g_value_get_object(value);
			break;
		case PROP_NAME :
			g_free(pGmlcMysqlFnct->pcName);
			
			pGmlcMysqlFnct->pcName = g_value_dup_string(value);
			break;
		case PROP_FLAGGED :
			pGmlcMysqlFnct->bFlagged = g_value_get_boolean(value);
			break;
		default: {
			G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
			break;
		}
	}
}

static gchar * gmlc_mysql_function_structure_get_create (GmlcMysqlTable * pGmlcMysqlTbl, gboolean bMyself, const gchar * pcOtherName) {
	return NULL;
}

static gchar * gmlc_mysql_function_structure_get_alter (GmlcMysqlTable * pGmlcMysqlTbl, gboolean bMyself, const gchar * pcOtherName) {
	return NULL;
}

static gchar * gmlc_mysql_function_structure_get_drop (GmlcMysqlTable * pGmlcMysqlTbl, gboolean bMyself, const gchar * pcOtherName) {
	return NULL;
}

GmlcMysqlFunction * gmlc_mysql_function_new(GmlcMysqlDatabase * pGmlcMysqlDb, gchar * pcName) {
	GmlcMysqlFunction * pGmlcMysqlFnct;
	
	pGmlcMysqlFnct = GMLC_MYSQL_FUNCTION(g_object_new(GMLC_TYPE_MYSQL_FUNCTION, "database", pGmlcMysqlDb, "name", pcName, NULL));
	
	return pGmlcMysqlFnct;
}
