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

#include "gmlc_dump_format_sql.h"
#include "gmlc_dump_format.h"

static void gmlc_dump_format_sql_finalize (GmlcDumpFormatSql * pGmlcDmpSrcSvr);
static void gmlc_dump_format_sql_get_property (GObject * object, guint prop_id, GValue * value, GParamSpec * pspec);
static void gmlc_dump_format_sql_set_property (GObject * object, guint prop_id, const GValue * value, GParamSpec * pspec);

static void gmlc_dump_format_sql_interface_init (gpointer g_iface, gpointer iface_data);
static gboolean gmlc_dump_format_sql_set_struct (GmlcDumpFormat * self, const gchar * pcStruct);
static gboolean gmlc_dump_format_sql_set_data (GmlcDumpFormat * self, const GArray * arDatas);
static gchar * gmlc_dump_format_sql_run (GmlcDumpFormat * self);


enum {
	PROP_0,
	PROP_SERVER,
};

G_DEFINE_TYPE_WITH_CODE (GmlcDumpFormatSql, gmlc_dump_format_sql, G_TYPE_OBJECT, 
	G_IMPLEMENT_INTERFACE (GMLC_DUMP_TYPE_FORMAT, gmlc_dump_format_sql_interface_init));

static void gmlc_dump_format_sql_interface_init (gpointer g_iface, gpointer iface_data) {
	GmlcDumpFormatInterface *pIface = (GmlcDumpFormatInterface *)g_iface;
	pIface->set_struct = (gboolean (*) (GmlcDumpFormat * self, const gchar * pcStruct))gmlc_dump_format_sql_set_struct;
	pIface->set_data = (gboolean (*) (GmlcDumpFormat * self, const GArray * arDatas))gmlc_dump_format_sql_set_data;
	pIface->run = (gchar * (*) (GmlcDumpFormat * self))gmlc_dump_format_sql_run;
}

static void gmlc_dump_format_sql_class_init (GmlcDumpFormatSqlClass * pClass) {
	GObjectClass *pObjClass = G_OBJECT_CLASS(pClass);
	
	pObjClass->finalize = (GObjectFinalizeFunc) gmlc_dump_format_sql_finalize;
	
	pObjClass->get_property = gmlc_dump_format_sql_get_property;
	pObjClass->set_property = gmlc_dump_format_sql_set_property;

/*
	g_object_class_install_property(pObjClass, PROP_SERVER, 
		g_param_spec_object("server", "Server object", "Server object", GMLC_MYSQL_TYPE_SERVER, G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY));
*/
}

static void gmlc_dump_format_sql_get_property (GObject * object, guint prop_id, GValue * value, GParamSpec * pspec) {
	GmlcDumpFormatSql * pGmlcDmpSrcSvr = GMLC_DUMP_FORMAT_SQL(object);
	
	switch (prop_id) {
/*
		case PROP_SERVER :
			g_value_set_object(value, pGmlcDmpSrcSvr->pGmlcMysqlSrv);
			break;
*/
		default: {
			G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
			break;
		}
	}
}

static void gmlc_dump_format_sql_set_property (GObject * object, guint prop_id, const GValue * value, GParamSpec * pspec) {
	GmlcDumpFormatSql * pGmlcDmpSrcSvr = GMLC_DUMP_FORMAT_SQL(object);
	
	switch (prop_id) {
/*
		case PROP_SERVER :
			pGmlcDmpSrcSvr->pGmlcMysqlSrv = g_value_get_object(value);
			break;
*/
		default: {
			G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
			break;
		}
	}
}

static void gmlc_dump_format_sql_init (GmlcDumpFormatSql * pGmlcDmpSrcSvr) {
	UNUSED_VAR(pGmlcDmpSrcSvr);
	
}

static void gmlc_dump_format_sql_finalize (GmlcDumpFormatSql * pGmlcDmpSrcSvr) {
	UNUSED_VAR(pGmlcDmpSrcSvr);
	
}


static gboolean gmlc_dump_format_sql_set_struct (GmlcDumpFormat * self, const gchar * pcStruct) {
	UNUSED_VAR(self);
	
	return TRUE;
}

static gboolean gmlc_dump_format_sql_set_data (GmlcDumpFormat * self, const GArray * arDatas) {
	UNUSED_VAR(self);
	
	return TRUE;
}

static gchar * gmlc_dump_format_sql_run (GmlcDumpFormat * self) {
	UNUSED_VAR(self);
	
	return NULL;
}

GmlcDumpFormatSql * gmlc_dump_format_sql_new () {
	GmlcDumpFormatSql * pGmlcDmpSrcSvr = NULL;
	
	pGmlcDmpSrcSvr = GMLC_DUMP_FORMAT_SQL(g_object_new(GMLC_DUMP_TYPE_FORMAT_SQL, NULL));
	
	return pGmlcDmpSrcSvr;
}
