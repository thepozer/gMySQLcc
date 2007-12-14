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

#include "gmlc_mysql_view.h"

#include "gmlc_mysql_database.h"

static void gmlc_mysql_view_finalize (GmlcMysqlView * pGmlcMysqlVw);
static void gmlc_mysql_view_get_property (GObject * object, guint prop_id, GValue * value, GParamSpec * pspec);
static void gmlc_mysql_view_set_property (GObject * object, guint prop_id, const GValue * value, GParamSpec * pspec);

static void gmlc_mysql_view_interface_structure_init (gpointer g_iface, gpointer iface_data);
static gchar * gmlc_mysql_view_structure_get_create (GmlcMysqlView * pGmlcMysqlVw, gboolean bMyself, const gchar * pcOtherName);
static gchar * gmlc_mysql_view_structure_get_alter (GmlcMysqlView * pGmlcMysqlVw, gboolean bMyself, const gchar * pcOtherName);
static gchar * gmlc_mysql_view_structure_get_drop (GmlcMysqlView * pGmlcMysqlVw, gboolean bMyself, const gchar * pcOtherName);

enum {
	PROP_0,
	PROP_DATABASE,
	PROP_NAME,
	PROP_FLAGGED
};


G_DEFINE_TYPE_WITH_CODE (GmlcMysqlView, gmlc_mysql_view, G_TYPE_OBJECT,
	G_IMPLEMENT_INTERFACE (GMLC_MYSQL_TYPE_STRUCTURE, gmlc_mysql_view_interface_structure_init));

static void gmlc_mysql_view_interface_structure_init (gpointer g_iface, gpointer iface_data) {
	GmlcMysqlStructureInterface * pIface = (GmlcMysqlStructureInterface *) g_iface;
	pIface->get_create = (gchar * (*) (GmlcMysqlStructure * poSelf, gboolean bMyself, const gchar * pcOtherName))gmlc_mysql_view_structure_get_create;
	pIface->get_alter = (gchar * (*) (GmlcMysqlStructure * poSelf, gboolean bMyself, const gchar * pcOtherName))gmlc_mysql_view_structure_get_alter;
	pIface->get_drop = (gchar * (*) (GmlcMysqlStructure * poSelf, gboolean bMyself, const gchar * pcOtherName))gmlc_mysql_view_structure_get_drop;
}


static void gmlc_mysql_view_class_init(GmlcMysqlViewClass *pClass) {
	GObjectClass * pObjClass = G_OBJECT_CLASS(pClass);

	pObjClass->finalize = (GObjectFinalizeFunc) gmlc_mysql_view_finalize;
	pObjClass->get_property = gmlc_mysql_view_get_property;
	pObjClass->set_property = gmlc_mysql_view_set_property;
	
	g_object_class_install_property(pObjClass, PROP_DATABASE, 
		g_param_spec_object("database", "Database object", "Database object", GMLC_MYSQL_TYPE_DATABASE, G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY));
	g_object_class_install_property(pObjClass, PROP_NAME, 
		g_param_spec_string("name", "View name", "Name of the View", "", G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY));
	g_object_class_install_property(pObjClass, PROP_FLAGGED, 
		g_param_spec_boolean("flagged", "Object Flagged", "Object Flagged", FALSE, G_PARAM_READWRITE));
}

static void gmlc_mysql_view_init(GmlcMysqlView * pGmlcMysqlVw) {
	pGmlcMysqlVw->pGmlcMysqlDb = NULL;
	pGmlcMysqlVw->pcName = NULL;
	pGmlcMysqlVw->bFlagged = FALSE;
}

static void gmlc_mysql_view_finalize(GmlcMysqlView * pGmlcMysqlVw) {
	
	g_free(pGmlcMysqlVw->pcName);
	/*G_OBJECT_CLASS(parent_class)->finalize(object);*/
}

static void gmlc_mysql_view_get_property (GObject * object, guint prop_id, GValue * value, GParamSpec * pspec) {
	GmlcMysqlView * pGmlcMysqlViw = GMLC_MYSQL_VIEW(object);
	
	switch (prop_id) {
		case PROP_DATABASE :
			g_value_set_object(value, pGmlcMysqlViw->pGmlcMysqlDb);
			break;
		case PROP_NAME :
			g_value_set_string(value, pGmlcMysqlViw->pcName);
			break;
		case PROP_FLAGGED :
			g_value_set_boolean(value, pGmlcMysqlViw->bFlagged);
			break;
		default: {
			G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
			break;
		}
	}
}

static void gmlc_mysql_view_set_property (GObject * object, guint prop_id, const GValue * value, GParamSpec * pspec) {
	GmlcMysqlView * pGmlcMysqlViw = GMLC_MYSQL_VIEW(object);
	
	switch (prop_id) {
		case PROP_DATABASE :
			pGmlcMysqlViw->pGmlcMysqlDb = g_value_get_object(value);
			break;
		case PROP_NAME :
			g_free(pGmlcMysqlViw->pcName);
			
			pGmlcMysqlViw->pcName = g_value_dup_string(value);
			break;
		case PROP_FLAGGED :
			pGmlcMysqlViw->bFlagged = g_value_get_boolean(value);
			break;
		default: {
			G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
			break;
		}
	}
}

static gchar * gmlc_mysql_view_structure_get_create (GmlcMysqlView * pGmlcMysqlVw, gboolean bMyself, const gchar * pcOtherName) {
	const gchar * pcName = NULL;
	gchar * pcSqlQuery = NULL, * pcQuery = NULL;
	
	if (bMyself) {
		g_object_get(pGmlcMysqlVw, "name", &pcName, NULL);
		pcQuery = g_strdup_printf("SHOW CREATE VIEW `%s`;", pcName);
		
		pcSqlQuery = gmlc_mysql_query_static_get_one_result(pGmlcMysqlVw->pGmlcMysqlDb->pGmlcMysqlSrv, pGmlcMysqlVw->pGmlcMysqlDb->pcDbName, pcQuery, 1);
		
		g_free(pcQuery);
	} else {
		pcSqlQuery =  gmlc_mysql_database_create_new_view_sql(pGmlcMysqlVw->pGmlcMysqlDb, pcOtherName);
	}
	
	return pcSqlQuery;
}

static gchar * gmlc_mysql_view_structure_get_alter (GmlcMysqlView * pGmlcMysqlVw, gboolean bMyself, const gchar * pcOtherName) {
	gchar * pcSqlCreate = NULL, * pcSqlQuery = NULL, * pcTmp = NULL;	
	gint i = 0;
	
	pcSqlCreate = gmlc_mysql_view_structure_get_create(pGmlcMysqlVw, bMyself, pcOtherName);
	
	pcTmp = pcSqlCreate;
	while (pcTmp[i] != ' ') { pcTmp ++; }
	
	pcSqlQuery = g_strdup_printf("ALTER%s", pcTmp);
	
	g_free(pcSqlCreate);
	pcTmp = NULL;
	
	return pcSqlQuery;
}

static gchar * gmlc_mysql_view_structure_get_drop (GmlcMysqlView * pGmlcMysqlVw, gboolean bMyself, const gchar * pcOtherName) {
	const gchar * pcName = NULL;
	gchar * pcSqlQuery = NULL;
	
	if (bMyself) {
		pcName = pGmlcMysqlVw->pcName;
	} else if (pcOtherName != NULL) {
		pcName = pcOtherName; 
	} else {
		pcName = "<Name of the view>";
	}
	
	pcSqlQuery = g_strdup_printf("DROP VIEW `%s`;", pcName);
	
	return pcSqlQuery;
}

GmlcMysqlView * gmlc_mysql_view_new(GmlcMysqlDatabase * pGmlcMysqlDb, gchar * pcName) {
	GmlcMysqlView * pGmlcMysqlVw;
	
	pGmlcMysqlVw = GMLC_MYSQL_VIEW(g_object_new(GMLC_TYPE_MYSQL_VIEW, "database", pGmlcMysqlDb, "name", pcName, NULL));
	
	return pGmlcMysqlVw;
}
