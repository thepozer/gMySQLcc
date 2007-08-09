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

#include "gmlc_dump_control.h"

static void gmlc_dump_control_finalize (GmlcDumpControl * pGmlcDmpCtrl);
static void gmlc_dump_control_get_property (GObject * object, guint prop_id, GValue * value, GParamSpec * pspec);
static void gmlc_dump_control_set_property (GObject * object, guint prop_id, const GValue * value, GParamSpec * pspec);

enum {
	PROP_0,
	PROP_SERVER,
	PROP_SOURCE,
	PROP_FORMAT,
};


G_DEFINE_TYPE (GmlcDumpControl, gmlc_dump_control, G_TYPE_OBJECT);

static void gmlc_dump_control_class_init(GmlcDumpControlClass *pClass) {
	GObjectClass * pObjClass = G_OBJECT_CLASS(pClass);

	pObjClass->finalize = (GObjectFinalizeFunc) gmlc_dump_control_finalize;
	pObjClass->get_property = gmlc_dump_control_get_property;
	pObjClass->set_property = gmlc_dump_control_set_property;
	
	g_object_class_install_property(pObjClass, PROP_SERVER, 
		g_param_spec_object("server", "Server object", "Server object", GMLC_MYSQL_TYPE_SERVER, G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY));
	g_object_class_install_property(pObjClass, PROP_SOURCE, 
		g_param_spec_object("source", "Source object", "Source object", GMLC_DUMP_TYPE_SOURCE, G_PARAM_READWRITE | G_PARAM_CONSTRUCT));
	g_object_class_install_property(pObjClass, PROP_FORMAT, 
		g_param_spec_object("format", "Format object", "Format object", GMLC_DUMP_TYPE_SOURCE, G_PARAM_READWRITE | G_PARAM_CONSTRUCT));
/*
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
*/
}

static void gmlc_dump_control_init(GmlcDumpControl * pGmlcDmpCtrl) {
	pGmlcDmpCtrl->pGmlcMysqlSrv = NULL;
}

static void gmlc_dump_control_finalize(GmlcDumpControl * pGmlcDmpCtrl) {
	UNUSED_VAR(pGmlcDmpCtrl);
	
	/*G_OBJECT_CLASS(parent_class)->finalize(object);*/
}

static void gmlc_dump_control_get_property (GObject * object, guint prop_id, GValue * value, GParamSpec * pspec) {
	GmlcDumpControl * pGmlcDmpCtrl = GMLC_DUMP_CONTROL(object);
	
	switch (prop_id) {
		case PROP_SERVER :
			g_value_set_object(value, pGmlcDmpCtrl->pGmlcMysqlSrv);
			break;
		default: {
			G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
			break;
		}
	}
}

static void gmlc_dump_control_set_property (GObject * object, guint prop_id, const GValue * value, GParamSpec * pspec) {
	GmlcDumpControl * pGmlcDmpCtrl = GMLC_DUMP_CONTROL(object);
	
	switch (prop_id) {
		case PROP_SERVER :
			pGmlcDmpCtrl->pGmlcMysqlSrv = g_value_get_object(value);
			break;
		default: {
			G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
			break;
		}
	}
}

GmlcDumpControl * gmlc_dump_control_new(GmlcMysqlServer * pGmlcMysqlSrv) {
	GmlcDumpControl * pGmlcDmpCtrl = NULL;
	
	pGmlcDmpCtrl = GMLC_DUMP_CONTROL(g_object_new(GMLC_TYPE_DUMP_CONTROL, "server", pGmlcMysqlSrv, NULL));
	
	return pGmlcDmpCtrl;
}

gboolean gmlc_dump_control_dump (GmlcDumpControl * pGmlcDmpCtrl) {
	UNUSED_VAR(pGmlcDmpCtrl);
	
	return TRUE;
}


gboolean gmlc_dump_control_direct_dump (GmlcMysqlServer * pGmlcMysqlSrv, GmlcDumpSource * pGmlcDmpSrc, void * pGmlcDmpFrmt) {
	GmlcDumpControl * pGmlcDmpCtrl = NULL;
	gboolean bRetValue = FALSE;
	
	pGmlcDmpCtrl = GMLC_DUMP_CONTROL(g_object_new(GMLC_TYPE_DUMP_CONTROL, "server", pGmlcMysqlSrv, "source", pGmlcDmpSrc, "format", pGmlcDmpFrmt, NULL));
	
	bRetValue = gmlc_dump_control_dump(pGmlcDmpCtrl);
	
	g_object_unref(pGmlcDmpCtrl);
	
	return bRetValue;
}
