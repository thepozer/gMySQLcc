/***************************************************************************
 *            gmlc_mysql_view.c
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

#include "gmlc_mysql_view.h"

#include "gmlc_mysql_database.h"

static void gmlc_mysql_view_finalize (GmlcMysqlView * pGmlcMysqlViw);
static void gmlc_mysql_view_get_property (GObject * object, guint prop_id, GValue * value, GParamSpec * pspec);
static void gmlc_mysql_view_set_property (GObject * object, guint prop_id, const GValue * value, GParamSpec * pspec);

enum {
	PROP_0,
	PROP_DATABASE,
	PROP_NAME,
	PROP_FLAGGED
};


G_DEFINE_TYPE (GmlcMysqlView, gmlc_mysql_view, G_TYPE_OBJECT)

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

static void gmlc_mysql_view_init(GmlcMysqlView * pGmlcMysqlViw) {
	pGmlcMysqlViw->pGmlcMysqlDb = NULL;
	pGmlcMysqlViw->pcName = NULL;
	pGmlcMysqlViw->bFlagged = FALSE;
}

static void gmlc_mysql_view_finalize(GmlcMysqlView * pGmlcMysqlViw) {
	
	g_free(pGmlcMysqlViw->pcName);
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

GmlcMysqlView * gmlc_mysql_view_new(GmlcMysqlDatabase * pGmlcMysqlDb, gchar * pcName) {
	GmlcMysqlView * pGmlcMysqlViw;
	
	pGmlcMysqlViw = GMLC_MYSQL_VIEW(g_object_new(GMLC_TYPE_MYSQL_VIEW, "database", pGmlcMysqlDb, "name", pcName, NULL));
	
	return pGmlcMysqlViw;
}
