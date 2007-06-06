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


#include "gmlc_gui_server.h"
#include "gmlc_gui_server_tab_data.h"
#include "gmlc_gui_server_tab_help.h"
#include "gmlc_gui_query.h"

static void gmlc_gui_server_finalize (GmlcGuiServer * pGmlcGuiSrv);
static void gmlc_gui_server_get_property (GObject * object, guint prop_id, GValue * value, GParamSpec * pspec);
static void gmlc_gui_server_set_property (GObject * object, guint prop_id, const GValue * value, GParamSpec * pspec);

static void gmlc_gui_server_create_widgets (GmlcGuiServer * pGmlcGuiSrv);
static void gmlc_gui_server_create_tabs(GmlcGuiServer * pGmlcGuiSrv);
static void gmlc_gui_server_init_widgets (GmlcGuiServer * pGmlcGuiSrv);


static void gmlc_gui_server_evt_destroy(GtkWidget *widget, gpointer user_data);
void gmlc_gui_server_evt_btnTlbrClose_clicked (GtkWidget *widget, gpointer user_data);
void gmlc_gui_server_evt_btnTlbrSqlServerList_clicked (GtkWidget *widget, gpointer user_data);
void gmlc_gui_server_evt_btnTlbrSql_clicked (GtkWidget *widget, gpointer user_data);


enum {
	PROP_0,
	PROP_SERVER
};

G_DEFINE_TYPE (GmlcGuiServer, gmlc_gui_server, GTK_TYPE_WINDOW);

static void gmlc_gui_server_class_init (GmlcGuiServerClass * pClass) {
	GObjectClass *pObjClass = G_OBJECT_CLASS(pClass);
	
	pObjClass->finalize = (GObjectFinalizeFunc) gmlc_gui_server_finalize;
	
	pObjClass->get_property = gmlc_gui_server_get_property;
	pObjClass->set_property = gmlc_gui_server_set_property;
	
	g_object_class_install_property(pObjClass, PROP_SERVER, 
		g_param_spec_object("server", "Server object", "Server object", GMLC_TYPE_MYSQL_SERVER, G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY));
}

static void gmlc_gui_server_get_property (GObject * object, guint prop_id, GValue * value, GParamSpec * pspec) {
	GmlcGuiServer * pGmlcGuiSrv = GMLC_GUI_SERVER(object);
	
	switch (prop_id) {
		case PROP_SERVER :
			g_value_set_object(value, pGmlcGuiSrv->pGmlcMysqlSrv);
			break;
		default: {
			G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
			break;
		}
	}
}

static void gmlc_gui_server_set_property (GObject * object, guint prop_id, const GValue * value, GParamSpec * pspec) {
	GmlcGuiServer * pGmlcGuiSrv = GMLC_GUI_SERVER(object);
	
	switch (prop_id) {
		case PROP_SERVER : {
			gchar * pcTitle = NULL;
			gchar * pcName = NULL;
			
			pGmlcGuiSrv->pGmlcMysqlSrv = g_value_get_object(value);
			
			g_object_get(pGmlcGuiSrv->pGmlcMysqlSrv, "name", &pcName, NULL);
			pcTitle = g_strdup_printf(_("GMySQLcc - Server window : %s"), pcName); 
			gtk_window_set_title (GTK_WINDOW(pGmlcGuiSrv), pcTitle);
			
			gmlc_gui_server_create_tabs(pGmlcGuiSrv);
			gmlc_gui_server_init_widgets(pGmlcGuiSrv);
			
			g_free(pcTitle);
			g_free(pcName);
			
			break;
		}
		default: {
			G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
			break;
		}
	}
}

static void gmlc_gui_server_init (GmlcGuiServer * pGmlcGuiSrv) {
	
	GiNbrWnd ++;
	
	gtk_window_set_title (GTK_WINDOW(pGmlcGuiSrv), _("GMySQLcc - Server window : %s"));
	gtk_window_set_default_size (GTK_WINDOW(pGmlcGuiSrv), 640, 480);
	
	g_signal_connect(G_OBJECT (pGmlcGuiSrv), "destroy", G_CALLBACK (gmlc_gui_server_evt_destroy), NULL);
	
	pGmlcGuiSrv->pGmlcMysqlSrv	= NULL;
	pGmlcGuiSrv->pGmlcMysqlDb	= NULL;
	pGmlcGuiSrv->pGmlcMysqlTbl	= NULL;
	pGmlcGuiSrv->pGmlcMysqlVw	= NULL;
	pGmlcGuiSrv->pGmlcMysqlProc	= NULL;
	pGmlcGuiSrv->pGmlcMysqlFunc	= NULL;
	
	gmlc_gui_server_create_widgets(pGmlcGuiSrv);
}

static void gmlc_gui_server_finalize (GmlcGuiServer * pGmlcGuiSrv) {
	UNUSED_VAR(pGmlcGuiSrv);
	
}

GmlcGuiServer * gmlc_gui_server_new (GmlcMysqlServer * pGmlcMysqlSrv) {
	GmlcGuiServer * pGmlcGuiSrv = NULL;
	
	pGmlcGuiSrv = GMLC_GUI_SERVER(g_object_new(GMLC_GUI_TYPE_SERVER, "server", pGmlcMysqlSrv, NULL));
	
	return pGmlcGuiSrv;
}

static void gmlc_gui_server_create_widgets (GmlcGuiServer * pGmlcGuiSrv) {
	GtkWidget *statusbar;
	GtkWidget *vbox;
	GtkWidget *toolbar;
	GtkWidget * imgToolbar;
	GtkToolItem * btnTlbrClose, * btnTlbrSql, * btnTlbrSqlFile, * btnTlbrSqlServerList;
	
	GtkTooltips * tooltips;
	
	tooltips = gtk_tooltips_new();

	vbox = gtk_vbox_new (FALSE, 0);
	gtk_widget_show (vbox);
	gtk_container_add (GTK_CONTAINER (pGmlcGuiSrv), vbox);

	toolbar = gtk_toolbar_new ();
	gtk_widget_show (toolbar);
	gtk_box_pack_start (GTK_BOX (vbox), toolbar, FALSE, FALSE, 0);
	gtk_toolbar_set_style (GTK_TOOLBAR (toolbar), GTK_TOOLBAR_BOTH_HORIZ);

	imgToolbar = gtk_image_new_from_stock(GTK_STOCK_EXECUTE, GTK_ICON_SIZE_LARGE_TOOLBAR);
	gtk_widget_show(imgToolbar);
	btnTlbrSql = gtk_tool_button_new (imgToolbar, _("SQL"));
	gtk_tool_item_set_is_important (GTK_TOOL_ITEM(btnTlbrSql), TRUE);
	g_signal_connect (G_OBJECT (btnTlbrSql), "clicked", 
										G_CALLBACK (gmlc_gui_server_evt_btnTlbrSql_clicked), pGmlcGuiSrv);
	gtk_widget_show(GTK_WIDGET(btnTlbrSql));
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar), GTK_TOOL_ITEM(btnTlbrSql), -1);
	gtk_tool_item_set_tooltip (GTK_TOOL_ITEM(btnTlbrSql), tooltips, _("Exec SQL Query"), NULL);

	imgToolbar = gtk_image_new_from_stock(GTK_STOCK_EXECUTE, GTK_ICON_SIZE_LARGE_TOOLBAR);
	gtk_widget_show(imgToolbar);
	btnTlbrSqlFile = gtk_tool_button_new (imgToolbar, _("SQL File"));
	gtk_tool_item_set_is_important (GTK_TOOL_ITEM(btnTlbrSqlFile), TRUE);
	/*g_signal_connect (G_OBJECT (btnTlbrSqlFile), "clicked", 
										G_CALLBACK (gmlc_gui_server_evt_btnTlbrSqlFile_clicked), pGmlcGuiSrv);*/
	gtk_widget_show(GTK_WIDGET(btnTlbrSqlFile));
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar), GTK_TOOL_ITEM(btnTlbrSqlFile), -1);
	gtk_tool_item_set_tooltip (GTK_TOOL_ITEM(btnTlbrSqlFile), tooltips, _("Exec SQL File"), NULL);

	imgToolbar = gtk_image_new_from_stock(GTK_STOCK_OPEN, GTK_ICON_SIZE_LARGE_TOOLBAR);
	gtk_widget_show(imgToolbar);
	btnTlbrSqlServerList = gtk_tool_button_new (imgToolbar, _("Servers list"));
	gtk_tool_item_set_is_important (GTK_TOOL_ITEM(btnTlbrSqlServerList), TRUE);
	g_signal_connect (G_OBJECT (btnTlbrSqlServerList), "clicked", 
										G_CALLBACK (gmlc_gui_server_evt_btnTlbrSqlServerList_clicked), pGmlcGuiSrv);
	gtk_widget_show(GTK_WIDGET(btnTlbrSqlServerList));
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar), GTK_TOOL_ITEM(btnTlbrSqlServerList), -1);
	gtk_tool_item_set_tooltip (GTK_TOOL_ITEM(btnTlbrSqlServerList), tooltips, _("Open server list window"), NULL);

	imgToolbar = gtk_image_new_from_stock(GTK_STOCK_CLOSE, GTK_ICON_SIZE_LARGE_TOOLBAR);
	gtk_widget_show(imgToolbar);
	btnTlbrClose = gtk_tool_button_new (imgToolbar, _("Close"));
	gtk_tool_item_set_is_important (GTK_TOOL_ITEM(btnTlbrClose), TRUE);
	g_signal_connect (G_OBJECT (btnTlbrClose), "clicked", 
										G_CALLBACK (gmlc_gui_server_evt_btnTlbrClose_clicked), pGmlcGuiSrv);
	gtk_widget_show(GTK_WIDGET(btnTlbrClose));
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar), GTK_TOOL_ITEM(btnTlbrClose), -1);
	gtk_tool_item_set_tooltip (GTK_TOOL_ITEM(btnTlbrClose), tooltips, _("Close window"), NULL);
	
	pGmlcGuiSrv->nbkGeneral = gtk_notebook_new ();
	gtk_widget_show (pGmlcGuiSrv->nbkGeneral);
	gtk_box_pack_start (GTK_BOX (vbox), pGmlcGuiSrv->nbkGeneral, TRUE, TRUE, 0);
	
	statusbar = gtk_statusbar_new ();
	gtk_widget_show (statusbar);
	gtk_box_pack_start (GTK_BOX (vbox), statusbar, FALSE, FALSE, 0);

}

static void gmlc_gui_server_create_tabs(GmlcGuiServer * pGmlcGuiSrv) {
	glong lServerVersion = 0;
	
	g_object_get(G_OBJECT(pGmlcGuiSrv->pGmlcMysqlSrv), "version", &lServerVersion, NULL);
	
	gmlc_gui_server_create_widget_tab_data (pGmlcGuiSrv);
	/*gmysqlcc_gui_server_create_widget_tab_user (pGmlcGuiSrv);*/
	/*gmysqlcc_gui_server_create_widget_tab_server (pGmlcGuiSrv);*/
	if (lServerVersion >= 40100) {
		gmlc_gui_server_create_widget_tab_help (pGmlcGuiSrv);
	}
}

static void gmlc_gui_server_init_widgets (GmlcGuiServer * pGmlcGuiSrv) {
	glong lServerVersion = 0;
	
	g_object_get(G_OBJECT(pGmlcGuiSrv->pGmlcMysqlSrv), "version", &lServerVersion, NULL);
	
	gmlc_gui_server_init_widget_tab_data(pGmlcGuiSrv);
	/*gmysqlcc_gui_server_init_widget_tab_user(pGmlcGuiSrv);*/
	/*gmysqlcc_gui_server_init_widget_tab_server(pGmlcGuiSrv);*/
	if (lServerVersion >= 40100) {
		gmlc_gui_server_init_widget_tab_help (pGmlcGuiSrv);
	}
}

void gmlc_gui_server_open_query_window (GmlcGuiServer * pGmlcGuiSrv, gboolean bUseTable) {
	GmlcGuiQuery * pGmlcGuiQry = NULL;
	gchar * pcSql = NULL, * pcDbName = NULL, * pcTblName = NULL;
	
	if (pGmlcGuiSrv->pGmlcMysqlDb != NULL) {
		
		g_object_get(G_OBJECT(pGmlcGuiSrv->pGmlcMysqlDb), "db_name", &pcDbName, NULL);
		
		pGmlcGuiQry = gmlc_gui_query_new(pGmlcGuiSrv->pGmlcMysqlSrv, pcDbName);
		
		if (bUseTable && pGmlcGuiSrv->pGmlcMysqlTbl != NULL) {
			g_object_get(G_OBJECT(pGmlcGuiSrv->pGmlcMysqlTbl), "name", &pcTblName, NULL);
			
			pcSql = g_strdup_printf("SELECT * \nFROM `%s`.`%s`\nWHERE 1\nLIMIT 1000", pcDbName, pcTblName);
						
			gmlc_gui_query_set_query(pGmlcGuiQry, pcSql, TRUE);
			g_free(pcSql);
		}
		
		gtk_widget_show(GTK_WIDGET(pGmlcGuiQry));
	}
}



static void gmlc_gui_server_evt_destroy(GtkWidget *widget, gpointer user_data) {
	UNUSED_VAR(user_data);

	gtk_widget_hide(GTK_WIDGET(widget));
	GiNbrWnd --;
	
	if (GiNbrWnd <= 0) {
		gtk_main_quit();
	}
}

void gmlc_gui_server_evt_btnTlbrSql_clicked (GtkWidget *widget, gpointer user_data) {
	GmlcGuiServer * pGmlcGuiSrv = GMLC_GUI_SERVER(user_data);
	UNUSED_VAR(widget);
	
	gmlc_gui_server_open_query_window(pGmlcGuiSrv, TRUE);
}

void gmlc_gui_server_evt_btnTlbrSqlServerList_clicked (GtkWidget *widget, gpointer user_data) {
	GmlcGuiServer * pGmlcGuiSrv = GMLC_GUI_SERVER(user_data);
	UNUSED_VAR(widget);
	
	gtk_widget_show(GpGmlcGuiCnxns);
}

void gmlc_gui_server_evt_btnTlbrClose_clicked (GtkWidget *widget, gpointer user_data) {
	GmlcGuiServer * pGmlcGuiSrv = GMLC_GUI_SERVER(user_data);
	UNUSED_VAR(widget);
	
	gtk_widget_destroy(GTK_WIDGET(pGmlcGuiSrv));
}

