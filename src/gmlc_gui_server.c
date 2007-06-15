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
#include "gmlc_gui_server_tab.h"
#include "gmlc_gui_server_tab_data.h"
#include "gmlc_gui_server_tab_help.h"
#include "gmlc_gui_server_tab_query.h"

static void gmlc_gui_server_finalize (GmlcGuiServer * pGmlcGuiSrv);
static void gmlc_gui_server_get_property (GObject * object, guint prop_id, GValue * value, GParamSpec * pspec);
static void gmlc_gui_server_set_property (GObject * object, guint prop_id, const GValue * value, GParamSpec * pspec);

static void gmlc_gui_server_create_widgets (GmlcGuiServer * pGmlcGuiSrv);
static void gmlc_gui_server_create_tabs(GmlcGuiServer * pGmlcGuiSrv);
static void gmlc_gui_server_init_widgets (GmlcGuiServer * pGmlcGuiSrv);


static void gmlc_gui_server_evt_destroy(GtkWidget *widget, gpointer user_data);
static void gmlc_gui_server_evt_tab_selected (GtkNotebook *notebook, GtkNotebookPage *page, guint page_num, gpointer user_data);
static void gmlc_gui_server_evt_btnTlbrClose_clicked (GtkWidget *widget, gpointer user_data);
static void gmlc_gui_server_evt_btnTlbrSqlServerList_clicked (GtkWidget *widget, gpointer user_data);


enum {
	PROP_0,
	PROP_SERVER,
	PROP_TOOLBAR,
	PROP_VBOX_TOOLBAR,
};

G_DEFINE_TYPE (GmlcGuiServer, gmlc_gui_server, GTK_TYPE_WINDOW);

static void gmlc_gui_server_class_init (GmlcGuiServerClass * pClass) {
	GObjectClass *pObjClass = G_OBJECT_CLASS(pClass);
	
	pObjClass->finalize = (GObjectFinalizeFunc) gmlc_gui_server_finalize;
	
	pObjClass->get_property = gmlc_gui_server_get_property;
	pObjClass->set_property = gmlc_gui_server_set_property;
	
	g_object_class_install_property(pObjClass, PROP_SERVER, 
		g_param_spec_object("server", "Server object", "Server object", GMLC_TYPE_MYSQL_SERVER, G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY));
	g_object_class_install_property(pObjClass, PROP_VBOX_TOOLBAR, 
		g_param_spec_object("toolbar-hbox", "HBox where is the toolbar object", "HBox where is the toolbar object", GTK_TYPE_HBOX, G_PARAM_READABLE));
}

static void gmlc_gui_server_get_property (GObject * object, guint prop_id, GValue * value, GParamSpec * pspec) {
	GmlcGuiServer * pGmlcGuiSrv = GMLC_GUI_SERVER(object);
	
	switch (prop_id) {
		case PROP_SERVER :
			g_value_set_object(value, pGmlcGuiSrv->pGmlcMysqlSrv);
			break;
		case PROP_VBOX_TOOLBAR :
			g_value_set_object(value, pGmlcGuiSrv->poHBoxToolbar);
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
	GtkWidget * statusbar;
	GtkWidget * vbox;
	GtkWidget * imgBtn;
	GtkWidget * btnSqlServerList, * btnClose;
	
	GtkTooltips * tooltips;
	
	tooltips = gtk_tooltips_new();

	vbox = gtk_vbox_new (FALSE, 0);
	gtk_widget_show (vbox);
	gtk_container_add (GTK_CONTAINER (pGmlcGuiSrv), vbox);
	
	pGmlcGuiSrv->poHBoxToolbar = gtk_hbox_new (FALSE, 0);
	gtk_widget_show (pGmlcGuiSrv->poHBoxToolbar);
	gtk_box_pack_start (GTK_BOX (vbox), pGmlcGuiSrv->poHBoxToolbar, FALSE, FALSE, 0);
	
	btnClose = gtk_button_new_from_stock(GTK_STOCK_CLOSE);
	gtk_button_set_relief(GTK_BUTTON(btnClose), GTK_RELIEF_NONE);
	g_signal_connect (G_OBJECT (btnClose), "clicked", 
			G_CALLBACK (gmlc_gui_server_evt_btnTlbrClose_clicked), pGmlcGuiSrv);
	gtk_widget_show(GTK_WIDGET(btnClose));
	gtk_box_pack_end (GTK_BOX (pGmlcGuiSrv->poHBoxToolbar), btnClose, FALSE, FALSE, 0);
	
	imgBtn = gtk_image_new_from_stock(GTK_STOCK_OPEN, GTK_ICON_SIZE_LARGE_TOOLBAR);
	gtk_widget_show(imgBtn);
	btnSqlServerList = gtk_button_new_with_label (_("Servers list"));
	gtk_button_set_image(GTK_BUTTON(btnSqlServerList), imgBtn);
	gtk_button_set_relief(GTK_BUTTON(btnSqlServerList), GTK_RELIEF_NONE);
	g_signal_connect (G_OBJECT (btnSqlServerList), "clicked", 
			G_CALLBACK (gmlc_gui_server_evt_btnTlbrSqlServerList_clicked), pGmlcGuiSrv);
	gtk_widget_show(GTK_WIDGET(btnSqlServerList));
	gtk_box_pack_end (GTK_BOX (pGmlcGuiSrv->poHBoxToolbar), btnSqlServerList, FALSE, FALSE, 0);
	
	pGmlcGuiSrv->nbkGeneral = gtk_notebook_new ();
	gtk_widget_show (pGmlcGuiSrv->nbkGeneral);
	gtk_box_pack_start (GTK_BOX (vbox), pGmlcGuiSrv->nbkGeneral, TRUE, TRUE, 0);
	g_signal_connect (G_OBJECT (pGmlcGuiSrv->nbkGeneral), "switch-page", 
			G_CALLBACK (gmlc_gui_server_evt_tab_selected), pGmlcGuiSrv);
	
	statusbar = gtk_statusbar_new ();
	gtk_widget_show (statusbar);
	gtk_box_pack_start (GTK_BOX (vbox), statusbar, FALSE, FALSE, 0);

}

static void gmlc_gui_server_create_tabs(GmlcGuiServer * pGmlcGuiSrv) {
	GtkWidget * poLabel = NULL;
	GtkWidget * poTab = NULL;
	glong lServerVersion = 0;
	gint iPageShow = 0;
	
	g_object_get(G_OBJECT(pGmlcGuiSrv->pGmlcMysqlSrv), "version", &lServerVersion, NULL);
	
	gtk_notebook_set_scrollable(GTK_NOTEBOOK(pGmlcGuiSrv->nbkGeneral), TRUE);
	
	if (lServerVersion >= 40100) {
		poLabel = gtk_label_new(_("Help"));
		gtk_widget_show (poLabel);
		gtk_label_set_justify (GTK_LABEL (poLabel), GTK_JUSTIFY_LEFT);
		
		poTab = GTK_WIDGET(gmlc_gui_server_tab_help_new(pGmlcGuiSrv));
		gtk_widget_show (poTab);
		
		gtk_notebook_append_page(GTK_NOTEBOOK(pGmlcGuiSrv->nbkGeneral), poTab, poLabel);
		gtk_notebook_set_tab_reorderable(GTK_NOTEBOOK(pGmlcGuiSrv->nbkGeneral), poTab, TRUE);
		
		iPageShow = 1;
	}
	
	poLabel = gtk_label_new(_("Databases"));
	gtk_widget_show (poLabel);
	gtk_label_set_justify (GTK_LABEL (poLabel), GTK_JUSTIFY_LEFT);
	
	poTab = GTK_WIDGET(gmlc_gui_server_tab_data_new(pGmlcGuiSrv));
	gtk_widget_show (poTab);
	
	gtk_notebook_append_page(GTK_NOTEBOOK(pGmlcGuiSrv->nbkGeneral), poTab, poLabel);
	gtk_notebook_set_tab_reorderable(GTK_NOTEBOOK(pGmlcGuiSrv->nbkGeneral), poTab, TRUE);
	
	gtk_notebook_set_current_page(GTK_NOTEBOOK(pGmlcGuiSrv->nbkGeneral), iPageShow);
}

static void gmlc_gui_server_init_widgets (GmlcGuiServer * pGmlcGuiSrv) {
	
}

void gmlc_gui_server_add_query_tab(GmlcGuiServer * pGmlcGuiSrv, const gchar * pcDbName, const gchar * pcQuery, gboolean bExecNow) {
	GtkWidget * poLabel = NULL;
	GtkWidget * poTab = NULL;
	gchar * pcText = NULL;
	gint iNewPage = 0;
	
	pcText = g_strdup_printf(_("Query - db : '%s'"), pcDbName);
	
	poLabel = gtk_label_new(pcText);
	gtk_widget_show (poLabel);
	gtk_label_set_justify (GTK_LABEL (poLabel), GTK_JUSTIFY_LEFT);
	
	poTab = GTK_WIDGET(gmlc_gui_server_tab_query_new(pGmlcGuiSrv, pcDbName));
	gtk_widget_show (poTab);
	
	iNewPage = gtk_notebook_append_page(GTK_NOTEBOOK(pGmlcGuiSrv->nbkGeneral), poTab, poLabel);
	gtk_notebook_set_tab_reorderable(GTK_NOTEBOOK(pGmlcGuiSrv->nbkGeneral), poTab, TRUE);
	gtk_notebook_set_current_page(GTK_NOTEBOOK(pGmlcGuiSrv->nbkGeneral), iNewPage);
	
	if (pcQuery != NULL) {
		gmlc_gui_server_tab_query_set_query(GMLC_GUI_SERVER_TAB_QUERY(poTab), pcQuery, bExecNow);
	}
	
	g_free(pcText);
}

void gmlc_gui_server_close_query_tab(GmlcGuiServer * pGmlcGuiSrv, GtkWidget * pTabQuery) {
	gint iPageIdx = 0;
	
	iPageIdx = gtk_notebook_page_num(GTK_NOTEBOOK(pGmlcGuiSrv->nbkGeneral), pTabQuery);
	gtk_notebook_remove_page(GTK_NOTEBOOK(pGmlcGuiSrv->nbkGeneral), iPageIdx);
}

static void gmlc_gui_server_evt_destroy(GtkWidget *widget, gpointer user_data) {
	UNUSED_VAR(user_data);

	gtk_widget_hide(GTK_WIDGET(widget));
	GiNbrWnd --;
	
	if (GiNbrWnd <= 0) {
		gtk_main_quit();
	}
}

static void gmlc_gui_server_evt_tab_selected (GtkNotebook *poNoteBook, GtkNotebookPage *page, guint iPageNum, gpointer user_data) {
	GtkWidget * pChild = NULL;
	gint iPrevPageNum = 0;
	UNUSED_VAR(page);
	UNUSED_VAR(user_data);
	
	iPrevPageNum = gtk_notebook_get_current_page(poNoteBook);
	pChild = gtk_notebook_get_nth_page(poNoteBook, iPrevPageNum);
	gmlc_gui_server_tab_update_ui(GMLC_GUI_SERVER_TAB(pChild), FALSE);
	
	pChild = gtk_notebook_get_nth_page(poNoteBook, iPageNum);
	gmlc_gui_server_tab_update_ui(GMLC_GUI_SERVER_TAB(pChild), TRUE);
}

void gmlc_gui_server_evt_btnTlbrSqlServerList_clicked (GtkWidget *widget, gpointer user_data) {
	UNUSED_VAR(widget);
	UNUSED_VAR(user_data);
	
	gtk_widget_show(GpGmlcGuiCnxns);
}

void gmlc_gui_server_evt_btnTlbrClose_clicked (GtkWidget *widget, gpointer user_data) {
	GmlcGuiServer * pGmlcGuiSrv = GMLC_GUI_SERVER(user_data);
	UNUSED_VAR(widget);
	
	gtk_widget_destroy(GTK_WIDGET(pGmlcGuiSrv));
}

