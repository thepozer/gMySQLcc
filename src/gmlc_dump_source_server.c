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

#include "gmlc_dump_source_server.h"
#include "gmlc_dump_source.h"

static void gmlc_dump_source_server_finalize (GmlcDumpSourceServer * pGmlcDmpSrcSvr);
static void gmlc_dump_source_server_get_property (GObject * object, guint prop_id, GValue * value, GParamSpec * pspec);
static void gmlc_dump_source_server_set_property (GObject * object, guint prop_id, const GValue * value, GParamSpec * pspec);

static void gmlc_dump_source_server_interface_init (gpointer g_iface, gpointer iface_data);
static gboolean gmlc_dump_source_server_can_get_struct (GmlcDumpSource * self);
static gchar * gmlc_dump_source_server_get_struct (GmlcDumpSource * self);
static gboolean gmlc_dump_source_server_can_get_data (GmlcDumpSource * self);
static GArray * gmlc_dump_source_server_get_data (GmlcDumpSource * self);


enum {
	PROP_0,
	PROP_SERVER,
};

G_DEFINE_TYPE_WITH_CODE (GmlcDumpSourceServer, gmlc_dump_source_server, G_TYPE_OBJECT, 
	G_IMPLEMENT_INTERFACE (GMLC_DUMP_TYPE_SOURCE, gmlc_dump_source_server_interface_init));

static void gmlc_dump_source_server_interface_init (gpointer g_iface, gpointer iface_data) {
  GmlcDumpSourceInterface *pIface = (GmlcDumpSourceInterface *)g_iface;
  
  pIface->can_get_struct = (gboolean (*) (GmlcDumpSource * self))gmlc_dump_source_server_can_get_struct;
  pIface->get_struct = (gchar * (*) (GmlcDumpSource * self))gmlc_dump_source_server_get_struct;
  pIface->can_get_data = (gboolean (*) (GmlcDumpSource * self))gmlc_dump_source_server_can_get_data;
  pIface->get_data = (GArray * (*) (GmlcDumpSource * self))gmlc_dump_source_server_get_data;
}

static void gmlc_dump_source_server_class_init (GmlcDumpSourceServerClass * pClass) {
	GObjectClass *pObjClass = G_OBJECT_CLASS(pClass);
	
	pObjClass->finalize = (GObjectFinalizeFunc) gmlc_dump_source_server_finalize;
	
	pObjClass->get_property = gmlc_dump_source_server_get_property;
	pObjClass->set_property = gmlc_dump_source_server_set_property;

	g_object_class_install_property(pObjClass, PROP_SERVER, 
		g_param_spec_object("server", "Server object", "Server object", GMLC_MYSQL_TYPE_SERVER, G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY));
}

static void gmlc_dump_source_server_get_property (GObject * object, guint prop_id, GValue * value, GParamSpec * pspec) {
	GmlcDumpSourceServer * pGmlcDmpSrcSvr = GMLC_DUMP_SOURCE_SERVER(object);
	
	switch (prop_id) {
		case PROP_SERVER :
			g_value_set_object(value, pGmlcDmpSrcSvr->pGmlcMysqlSrv);
			break;
		default: {
			G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
			break;
		}
	}
}

static void gmlc_dump_source_server_set_property (GObject * object, guint prop_id, const GValue * value, GParamSpec * pspec) {
	GmlcDumpSourceServer * pGmlcDmpSrcSvr = GMLC_DUMP_SOURCE_SERVER(object);
	
	switch (prop_id) {
		case PROP_SERVER :
			pGmlcDmpSrcSvr->pGmlcMysqlSrv = g_value_get_object(value);
			break;
		default: {
			G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
			break;
		}
	}
}

static void gmlc_dump_source_server_init (GmlcDumpSourceServer * pGmlcDmpSrcSvr) {
	UNUSED_VAR(pGmlcDmpSrcSvr);
	
}

static void gmlc_dump_source_server_finalize (GmlcDumpSourceServer * pGmlcDmpSrcSvr) {
	UNUSED_VAR(pGmlcDmpSrcSvr);
	
}

static gboolean gmlc_dump_source_server_can_get_struct (GmlcDumpSource * self) {
	UNUSED_VAR(self);
	
	return TRUE;
}

static gchar * gmlc_dump_source_server_get_struct (GmlcDumpSource * self) {
	return NULL;
}

static gboolean gmlc_dump_source_server_can_get_data (GmlcDumpSource * self) {
	UNUSED_VAR(self);
	
	return TRUE;
}

static GArray * gmlc_dump_source_server_get_data (GmlcDumpSource * self) {
	return NULL;
}

GmlcDumpSourceServer * gmlc_dump_source_server_new (GmlcMysqlServer * pGmlcMysqlSvr) {
	GmlcDumpSourceServer * pGmlcDmpSrcSvr = NULL;
	
	pGmlcDmpSrcSvr = GMLC_DUMP_SOURCE_SERVER(g_object_new(GMLC_DUMP_TYPE_SOURCE_SERVER, "server", pGmlcMysqlSvr, NULL));
	
	return pGmlcDmpSrcSvr;
}
