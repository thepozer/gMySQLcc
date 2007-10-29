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

#include "gmlc_dump_source_query.h"
#include "gmlc_dump_source.h"

static void gmlc_dump_source_query_finalize (GmlcDumpSourceQuery * pGmlcDmpSrcQry);
static void gmlc_dump_source_query_get_property (GObject * object, guint prop_id, GValue * value, GParamSpec * pspec);
static void gmlc_dump_source_query_set_property (GObject * object, guint prop_id, const GValue * value, GParamSpec * pspec);

static void gmlc_dump_source_query_interface_init (gpointer g_iface, gpointer iface_data);
static gboolean gmlc_dump_source_query_can_get_struct (GmlcDumpSource * self);
static gchar * gmlc_dump_source_query_get_struct (GmlcDumpSource * self);
static gboolean gmlc_dump_source_query_can_get_data (GmlcDumpSource * self);
static GArray * gmlc_dump_source_query_get_data (GmlcDumpSource * self);


enum {
	PROP_0,
	PROP_DATABASE,
	PROP_QUERY,
};

G_DEFINE_TYPE_WITH_CODE (GmlcDumpSourceQuery, gmlc_dump_source_query, G_TYPE_OBJECT, 
	G_IMPLEMENT_INTERFACE (GMLC_DUMP_TYPE_SOURCE, gmlc_dump_source_query_interface_init));

static void gmlc_dump_source_query_interface_init (gpointer g_iface, gpointer iface_data) {
  GmlcDumpSourceInterface *pIface = (GmlcDumpSourceInterface *)g_iface;
  
  pIface->can_get_struct = (gboolean (*) (GmlcDumpSource * self))gmlc_dump_source_query_can_get_struct;
  pIface->get_struct = (gchar * (*) (GmlcDumpSource * self))gmlc_dump_source_query_get_struct;
  pIface->can_get_data = (gboolean (*) (GmlcDumpSource * self))gmlc_dump_source_query_can_get_data;
  pIface->get_data = (GArray * (*) (GmlcDumpSource * self))gmlc_dump_source_query_get_data;
}

static void gmlc_dump_source_query_class_init (GmlcDumpSourceQueryClass * pClass) {
	GObjectClass *pObjClass = G_OBJECT_CLASS(pClass);
	
	pObjClass->finalize = (GObjectFinalizeFunc) gmlc_dump_source_query_finalize;
	
	pObjClass->get_property = gmlc_dump_source_query_get_property;
	pObjClass->set_property = gmlc_dump_source_query_set_property;

	g_object_class_install_property(pObjClass, PROP_DATABASE, 
		g_param_spec_object("database", "Database object", "Database object", GMLC_MYSQL_TYPE_DATABASE, G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY));
	g_object_class_install_property(pObjClass, PROP_QUERY, 
		g_param_spec_string("query", "Query string", "Query string", "", G_PARAM_READWRITE | G_PARAM_CONSTRUCT));
}

static void gmlc_dump_source_query_get_property (GObject * object, guint prop_id, GValue * value, GParamSpec * pspec) {
	GmlcDumpSourceQuery * pGmlcDmpSrcTbl = GMLC_DUMP_SOURCE_QUERY(object);
	
	switch (prop_id) {
		case PROP_DATABASE :
			g_value_set_object(value, pGmlcDmpSrcTbl->pGmlcMysqlDb);
			break;
		case PROP_QUERY :
			g_value_set_string(value, pGmlcDmpSrcTbl->pcQuery);
			break;
		default: {
			G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
			break;
		}
	}
}

static void gmlc_dump_source_query_set_property (GObject * object, guint prop_id, const GValue * value, GParamSpec * pspec) {
	GmlcDumpSourceQuery * pGmlcDmpSrcTbl = GMLC_DUMP_SOURCE_QUERY(object);
	
	switch (prop_id) {
		case PROP_DATABASE :
			pGmlcDmpSrcTbl->pGmlcMysqlDb = g_value_get_object(value);
			break;
		case PROP_QUERY :
			g_free(pGmlcDmpSrcTbl->pcQuery);
			pGmlcDmpSrcTbl->pcQuery = g_value_dup_string(value);
			break;
		default: {
			G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
			break;
		}
	}
}

static void gmlc_dump_source_query_init (GmlcDumpSourceQuery * pGmlcDmpSrcQry) {
	UNUSED_VAR(pGmlcDmpSrcQry);
	
}

static void gmlc_dump_source_query_finalize (GmlcDumpSourceQuery * pGmlcDmpSrcQry) {
	UNUSED_VAR(pGmlcDmpSrcQry);
	
}

static gboolean gmlc_dump_source_query_can_get_struct (GmlcDumpSource * self) {
	UNUSED_VAR(self);
	
	return TRUE;
}

static gchar * gmlc_dump_source_query_get_struct (GmlcDumpSource * self) {
	return NULL;
}

static gboolean gmlc_dump_source_query_can_get_data (GmlcDumpSource * self) {
	UNUSED_VAR(self);
	
	return TRUE;
}

static GArray * gmlc_dump_source_query_get_data (GmlcDumpSource * self) {
	return NULL;
}

GmlcDumpSourceQuery * gmlc_dump_source_query_new (GmlcMysqlDatabase * pGmlcMysqlDb, const gchar *pcQuery) {
	GmlcDumpSourceQuery * pGmlcDmpSrcQry = NULL;
	
	pGmlcDmpSrcQry = GMLC_DUMP_SOURCE_QUERY(g_object_new(GMLC_DUMP_TYPE_SOURCE_QUERY, NULL));
	
	return pGmlcDmpSrcQry;
}
