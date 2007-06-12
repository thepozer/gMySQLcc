/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License.
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

#include "gmlc_gui_server_tab_query.h"
#include "gmlc_gui_server_tab.h"
#include "gmlc_mysql_query.h"

static void gmlc_gui_server_tab_query_finalize (GmlcGuiServerTabQuery * pGmlcGuiSrvTabQuery);
static void gmlc_gui_server_tab_query_get_property (GObject * object, guint prop_id, GValue * value, GParamSpec * pspec);
static void gmlc_gui_server_tab_query_set_property (GObject * object, guint prop_id, const GValue * value, GParamSpec * pspec);

static void gmlc_gui_server_tab_query_interface_init (gpointer g_iface, gpointer iface_data);
static void gmlc_gui_server_tab_query_update_ui (GmlcGuiServerTabQuery * pGmlcGuiSrvTabQuery);

void gmlc_gui_server_tab_query_create_widgets (GmlcGuiServerTabQuery * pGmlcGuiSrvTabQuery);
void gmlc_gui_server_tab_query_init_widgets (GmlcGuiServerTabQuery * pGmlcGuiSrvTabQuery);

enum {
	PROP_0,
	PROP_SERVER,
	PROP_SERVER_WINDOW,
};

G_DEFINE_TYPE_WITH_CODE (GmlcGuiServerTabQuery, gmlc_gui_server_tab_query, GTK_TYPE_VBOX, 
	G_IMPLEMENT_INTERFACE (GMLC_GUI_TYPE_SERVER_TAB, gmlc_gui_server_tab_query_interface_init));

static void gmlc_gui_server_tab_query_interface_init (gpointer g_iface, gpointer iface_data) {
  GmlcGuiServerTabInterface *pIface = (GmlcGuiServerTabInterface *)g_iface;
  pIface->update_ui = (void (*) (GmlcGuiServerTab * self))gmlc_gui_server_tab_query_update_ui;
}

static void gmlc_gui_server_tab_query_class_init (GmlcGuiServerTabQueryClass * pClass) {
	GObjectClass *pObjClass = G_OBJECT_CLASS(pClass);
	
	pObjClass->finalize = (GObjectFinalizeFunc) gmlc_gui_server_tab_query_finalize;
	
	pObjClass->get_property = gmlc_gui_server_tab_query_get_property;
	pObjClass->set_property = gmlc_gui_server_tab_query_set_property;
	
	g_object_class_install_property(pObjClass, PROP_SERVER, 
		g_param_spec_object("server", "Server object", "Server object", GMLC_TYPE_MYSQL_SERVER, G_PARAM_READABLE));
	g_object_class_install_property(pObjClass, PROP_SERVER_WINDOW, 
		g_param_spec_object("server_window", "Server window object", "Server window object", GMLC_GUI_TYPE_SERVER, G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY));
}

static void gmlc_gui_server_tab_query_get_property (GObject * object, guint prop_id, GValue * value, GParamSpec * pspec) {
	GmlcGuiServerTabQuery * pGmlcGuiSrvTabQuery = GMLC_GUI_SERVER_TAB_QUERY(object);
	
	switch (prop_id) {
		case PROP_SERVER :
			g_value_set_object(value, pGmlcGuiSrvTabQuery->pGmlcMysqlSrv);
			break;
		case PROP_SERVER_WINDOW :
			g_value_set_object(value, pGmlcGuiSrvTabQuery->pGmlcGuiSrv);
			break;
		default: {
			G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
			break;
		}
	}
}

static void gmlc_gui_server_tab_query_set_property (GObject * object, guint prop_id, const GValue * value, GParamSpec * pspec) {
	GmlcGuiServerTabQuery * pGmlcGuiSrvTabQuery = GMLC_GUI_SERVER_TAB_QUERY(object);
	
	switch (prop_id) {
		case PROP_SERVER_WINDOW :
			pGmlcGuiSrvTabQuery->pGmlcGuiSrv = g_value_get_object(value);
			g_object_get(pGmlcGuiSrvTabQuery->pGmlcGuiSrv, "server", &pGmlcGuiSrvTabQuery->pGmlcMysqlSrv, NULL);
			gmlc_gui_server_tab_query_create_widgets(pGmlcGuiSrvTabQuery);
			gmlc_gui_server_tab_query_init_widgets(pGmlcGuiSrvTabQuery);
			break;
		default: {
			G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
			break;
		}
	}
}

static void gmlc_gui_server_tab_query_init (GmlcGuiServerTabQuery * pGmlcGuiSrvTabQuery) {
	UNUSED_VAR(pGmlcGuiSrvTabQuery);
}

static void gmlc_gui_server_tab_query_finalize (GmlcGuiServerTabQuery * pGmlcGuiSrvTabQuery) {
	UNUSED_VAR(pGmlcGuiSrvTabQuery);
}

static void gmlc_gui_server_tab_query_update_ui (GmlcGuiServerTabQuery * pGmlcGuiSrvTabQuery) {
	UNUSED_VAR(pGmlcGuiSrvTabQuery);
}

GmlcGuiServerTabQuery * gmlc_gui_server_tab_query_new (GmlcGuiServer * pGmlcGuiSrv) {
	GmlcGuiServerTabQuery * pGmlcGuiSrvTabQuery = NULL;
	
	pGmlcGuiSrvTabQuery = GMLC_GUI_SERVER_TAB_QUERY(g_object_new(GMLC_GUI_TYPE_SERVER_TAB_QUERY, "server_window", pGmlcGuiSrv, NULL));
	
	return pGmlcGuiSrvTabQuery;
}

void gmlc_gui_server_tab_query_create_widgets (GmlcGuiServerTabQuery * pGmlcGuiSrvTabQuery) {
	
}

void gmlc_gui_server_tab_query_init_widgets (GmlcGuiServerTabQuery * pGmlcGuiSrvTabQuery) {
	UNUSED_VAR(pGmlcGuiSrvTabQuery);
}
