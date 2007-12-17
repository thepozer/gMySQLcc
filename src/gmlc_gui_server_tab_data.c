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

#include "gmlc_gui_server_tab_data.h"
#include "gmlc_gui_server_tab_query.h"
#include "gmlc_gui_server_tab.h"
#include "gmlc_mysql_query.h"

static void gmlc_gui_server_tab_data_finalize (GmlcGuiServerTabData * pGmlcGuiSrvTabData);
static void gmlc_gui_server_tab_data_get_property (GObject * object, guint prop_id, GValue * value, GParamSpec * pspec);
static void gmlc_gui_server_tab_data_set_property (GObject * object, guint prop_id, const GValue * value, GParamSpec * pspec);

static void gmlc_gui_server_tab_data_interface_init (gpointer g_iface, gpointer iface_data);
static void gmlc_gui_server_tab_data_update_ui (GmlcGuiServerTabData * pGmlcGuiSrvTabData, gboolean bShow);

void gmlc_gui_server_tab_data_create_toolbar_items (GmlcGuiServerTabData * pGmlcGuiSrvTabData);
void gmlc_gui_server_tab_data_create_widgets (GmlcGuiServerTabData * pGmlcGuiSrvTabData);
void gmlc_gui_server_tab_data_init_widgets (GmlcGuiServerTabData * pGmlcGuiSrvTabData);

void gmlc_gui_server_tab_data_open_query (GmlcGuiServerTabData * pGmlcGuiSrvTabData, const gchar * pcQuery, gboolean bExecNow);
void gmlc_gui_server_tab_data_open_file (GmlcGuiServerTabData * pGmlcGuiSrvTabData);
void gmlc_gui_server_tab_data_open_query_select (GmlcGuiServerTabData * pGmlcGuiSrvTabData, gboolean bUseTable);

void gmlc_gui_server_tab_data_fill_database_list (GmlcGuiServerTabData * pGmlcGuiSrvTabData);
void gmlc_gui_server_tab_data_fill_tables_list (GmlcGuiServerTabData * pGmlcGuiSrvTabData);
void gmlc_gui_server_tab_data_fill_views_list (GmlcGuiServerTabData * pGmlcGuiSrvTabData);
void gmlc_gui_server_tab_data_fill_procedures_list (GmlcGuiServerTabData * pGmlcGuiSrvTabData);
void gmlc_gui_server_tab_data_fill_functions_list (GmlcGuiServerTabData * pGmlcGuiSrvTabData);

static void gmlc_gui_server_tab_data_evt_btn_sql_file_clicked (GtkWidget *widget, gpointer user_data);
static void gmlc_gui_server_tab_data_evt_btn_sql_clicked (GtkWidget *widget, gpointer user_data);

static gboolean gmlc_gui_server_tab_data_evt_lst_db_btnpress (GtkWidget *widget, GdkEventButton *event, gpointer user_data);
static void gmlc_gui_server_tab_data_evt_lst_db_selected (GtkTreeSelection *selection, gpointer user_data);
static void gmlc_gui_server_tab_data_evt_menu_db_add_activate (GtkWidget *widget, gpointer user_data);
static void gmlc_gui_server_tab_data_evt_menu_db_alter_activate (GtkWidget *widget, gpointer user_data);
static void gmlc_gui_server_tab_data_evt_menu_db_remove_activate (GtkWidget *widget, gpointer user_data);
static void gmlc_gui_server_tab_data_evt_menu_db_refresh_activate (GtkWidget *widget, gpointer user_data);
static void gmlc_gui_server_tab_data_evt_menu_db_show_create_activate (GtkWidget *widget, gpointer user_data);

static gboolean gmlc_gui_server_tab_data_evt_lst_tbl_btnpress (GtkWidget *widget, GdkEventButton *event, gpointer user_data);
static void gmlc_gui_server_tab_data_evt_lst_tbl_selected (GtkTreeSelection *selection, gpointer user_data);
static void gmlc_gui_server_tab_data_evt_menu_tbl_display_activate (GtkWidget *widget, gpointer user_data);
static void gmlc_gui_server_tab_data_evt_menu_tbl_add_activate (GtkWidget *widget, gpointer user_data);
static void gmlc_gui_server_tab_data_evt_menu_tbl_alter_activate (GtkWidget *widget, gpointer user_data);
static void gmlc_gui_server_tab_data_evt_menu_tbl_remove_activate (GtkWidget *widget, gpointer user_data);
static void gmlc_gui_server_tab_data_evt_menu_tbl_refresh_activate (GtkWidget *widget, gpointer user_data);
static void gmlc_gui_server_tab_data_evt_menu_tbl_show_create_activate (GtkWidget *widget, gpointer user_data);

static gboolean gmlc_gui_server_tab_data_evt_lst_vw_btnpress (GtkWidget *widget, GdkEventButton *event, gpointer user_data);
static void gmlc_gui_server_tab_data_evt_lst_vw_selected (GtkTreeSelection *selection, gpointer user_data);
static void gmlc_gui_server_tab_data_evt_menu_vw_display_activate (GtkWidget *widget, gpointer user_data);
static void gmlc_gui_server_tab_data_evt_menu_vw_add_activate (GtkWidget *widget, gpointer user_data);
static void gmlc_gui_server_tab_data_evt_menu_vw_alter_activate (GtkWidget *widget, gpointer user_data);
static void gmlc_gui_server_tab_data_evt_menu_vw_remove_activate (GtkWidget *widget, gpointer user_data);
static void gmlc_gui_server_tab_data_evt_menu_vw_refresh_activate (GtkWidget *widget, gpointer user_data);
static void gmlc_gui_server_tab_data_evt_menu_vw_show_create_activate (GtkWidget *widget, gpointer user_data);

static gboolean gmlc_gui_server_tab_data_evt_lst_proc_btnpress (GtkWidget *widget, GdkEventButton *event, gpointer user_data);
static void gmlc_gui_server_tab_data_evt_lst_proc_selected (GtkTreeSelection *selection, gpointer user_data);
static void gmlc_gui_server_tab_data_evt_menu_proc_display_activate (GtkWidget *widget, gpointer user_data);
static void gmlc_gui_server_tab_data_evt_menu_proc_add_activate (GtkWidget *widget, gpointer user_data);
static void gmlc_gui_server_tab_data_evt_menu_proc_alter_activate (GtkWidget *widget, gpointer user_data);
static void gmlc_gui_server_tab_data_evt_menu_proc_remove_activate (GtkWidget *widget, gpointer user_data);
static void gmlc_gui_server_tab_data_evt_menu_proc_refresh_activate (GtkWidget *widget, gpointer user_data);
static void gmlc_gui_server_tab_data_evt_menu_proc_show_create_activate (GtkWidget *widget, gpointer user_data);

static gboolean gmlc_gui_server_tab_data_evt_lst_func_btnpress (GtkWidget *widget, GdkEventButton *event, gpointer user_data);
static void gmlc_gui_server_tab_data_evt_lst_func_selected (GtkTreeSelection *selection, gpointer user_data);
static void gmlc_gui_server_tab_data_evt_menu_func_display_activate (GtkWidget *widget, gpointer user_data);
static void gmlc_gui_server_tab_data_evt_menu_func_add_activate (GtkWidget *widget, gpointer user_data);
static void gmlc_gui_server_tab_data_evt_menu_func_alter_activate (GtkWidget *widget, gpointer user_data);
static void gmlc_gui_server_tab_data_evt_menu_func_remove_activate (GtkWidget *widget, gpointer user_data);
static void gmlc_gui_server_tab_data_evt_menu_func_refresh_activate (GtkWidget *widget, gpointer user_data);
static void gmlc_gui_server_tab_data_evt_menu_func_show_create_activate (GtkWidget *widget, gpointer user_data);

enum {
	PROP_0,
	PROP_SERVER,
	PROP_SERVER_WINDOW,
};

G_DEFINE_TYPE_WITH_CODE (GmlcGuiServerTabData, gmlc_gui_server_tab_data, GTK_TYPE_VBOX, 
	G_IMPLEMENT_INTERFACE (GMLC_GUI_TYPE_SERVER_TAB, gmlc_gui_server_tab_data_interface_init));

static void gmlc_gui_server_tab_data_interface_init (gpointer g_iface, gpointer iface_data) {
  GmlcGuiServerTabInterface *pIface = (GmlcGuiServerTabInterface *)g_iface;
  pIface->update_ui = (void (*) (GmlcGuiServerTab * self, gboolean bShow))gmlc_gui_server_tab_data_update_ui;
}

static void gmlc_gui_server_tab_data_class_init (GmlcGuiServerTabDataClass * pClass) {
	GObjectClass *pObjClass = G_OBJECT_CLASS(pClass);
	
	pObjClass->finalize = (GObjectFinalizeFunc) gmlc_gui_server_tab_data_finalize;
	
	pObjClass->get_property = gmlc_gui_server_tab_data_get_property;
	pObjClass->set_property = gmlc_gui_server_tab_data_set_property;
	
	g_object_class_install_property(pObjClass, PROP_SERVER, 
		g_param_spec_object("server", "Server object", "Server object", GMLC_MYSQL_TYPE_SERVER, G_PARAM_READABLE));
	g_object_class_install_property(pObjClass, PROP_SERVER_WINDOW, 
		g_param_spec_object("server_window", "Server window object", "Server window object", GMLC_GUI_TYPE_SERVER, G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY));
}

static void gmlc_gui_server_tab_data_get_property (GObject * object, guint prop_id, GValue * value, GParamSpec * pspec) {
	GmlcGuiServerTabData * pGmlcGuiSrvTabData = GMLC_GUI_SERVER_TAB_DATA(object);
	
	switch (prop_id) {
		case PROP_SERVER :
			g_value_set_object(value, pGmlcGuiSrvTabData->pGmlcMysqlSrv);
			break;
		case PROP_SERVER_WINDOW :
			g_value_set_object(value, pGmlcGuiSrvTabData->pGmlcGuiSrv);
			break;
		default: {
			G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
			break;
		}
	}
}

static void gmlc_gui_server_tab_data_set_property (GObject * object, guint prop_id, const GValue * value, GParamSpec * pspec) {
	GmlcGuiServerTabData * pGmlcGuiSrvTabData = GMLC_GUI_SERVER_TAB_DATA(object);
	
	switch (prop_id) {
		case PROP_SERVER_WINDOW :
			pGmlcGuiSrvTabData->pGmlcGuiSrv = g_value_get_object(value);
			g_object_get(pGmlcGuiSrvTabData->pGmlcGuiSrv, "server", &pGmlcGuiSrvTabData->pGmlcMysqlSrv, NULL);
			gmlc_gui_server_tab_data_create_widgets(pGmlcGuiSrvTabData);
			gmlc_gui_server_tab_data_init_widgets(pGmlcGuiSrvTabData);
			gmlc_gui_server_tab_data_create_toolbar_items(pGmlcGuiSrvTabData);
			break;
		default: {
			G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
			break;
		}
	}
}

static void gmlc_gui_server_tab_data_init (GmlcGuiServerTabData * pGmlcGuiSrvTabData) {
	
	pGmlcGuiSrvTabData->pGmlcGuiSrv 	= NULL;
	pGmlcGuiSrvTabData->pGmlcMysqlSrv	= NULL;
	pGmlcGuiSrvTabData->pGmlcMysqlDb	= NULL;
	pGmlcGuiSrvTabData->pGmlcMysqlTbl	= NULL;
	pGmlcGuiSrvTabData->pGmlcMysqlVw	= NULL;
	pGmlcGuiSrvTabData->pGmlcMysqlProc	= NULL;
	pGmlcGuiSrvTabData->pGmlcMysqlFunc	= NULL;
}

static void gmlc_gui_server_tab_data_finalize (GmlcGuiServerTabData * pGmlcGuiSrvTabData) {
	UNUSED_VAR(pGmlcGuiSrvTabData);
	
}

static void gmlc_gui_server_tab_data_update_ui (GmlcGuiServerTabData * pGmlcGuiSrvTabData, gboolean bShow) {
	
	if (bShow) {
		gtk_widget_show(pGmlcGuiSrvTabData->poDataToolbar);
	} else {
		gtk_widget_hide(pGmlcGuiSrvTabData->poDataToolbar);
	}
}

GmlcGuiServerTabData * gmlc_gui_server_tab_data_new (GmlcGuiServer * pGmlcGuiSrv) {
	GmlcGuiServerTabData * pGmlcGuiSrvTabData = NULL;
	
	pGmlcGuiSrvTabData = GMLC_GUI_SERVER_TAB_DATA(g_object_new(GMLC_GUI_TYPE_SERVER_TAB_DATA, "server_window", pGmlcGuiSrv, NULL));
	
	return pGmlcGuiSrvTabData;
}

void gmlc_gui_server_tab_data_create_toolbar_items (GmlcGuiServerTabData * pGmlcGuiSrvTabData) {
	GtkWidget * poHBoxToolbar = NULL;
	GtkWidget * poImgBtn = NULL;
	GtkWidget * poBtnSql = NULL;
	GtkWidget * poBtnSqlFile = NULL;
	
	g_object_get(pGmlcGuiSrvTabData->pGmlcGuiSrv, "toolbar-hbox", &poHBoxToolbar, NULL);
	
	pGmlcGuiSrvTabData->poDataToolbar = gtk_hbox_new (FALSE, 2);
	gtk_widget_show (pGmlcGuiSrvTabData->poDataToolbar);
	gtk_box_pack_start (GTK_BOX (poHBoxToolbar), pGmlcGuiSrvTabData->poDataToolbar, TRUE, TRUE, 0);
	
	poImgBtn = gtk_image_new_from_stock(GTK_STOCK_EXECUTE, GTK_ICON_SIZE_LARGE_TOOLBAR);
	gtk_widget_show(poImgBtn);
	poBtnSql = gtk_button_new_with_label (_("SQL"));
	gtk_button_set_image(GTK_BUTTON(poBtnSql), poImgBtn);
	gtk_button_set_relief(GTK_BUTTON(poBtnSql), GTK_RELIEF_NONE);
	g_signal_connect (poBtnSql, "clicked", G_CALLBACK (gmlc_gui_server_tab_data_evt_btn_sql_clicked), pGmlcGuiSrvTabData);
	gtk_widget_show(poBtnSql);
	gtk_box_pack_start (GTK_BOX (pGmlcGuiSrvTabData->poDataToolbar), poBtnSql, FALSE, FALSE, 0);
	
	poImgBtn = gtk_image_new_from_stock(GTK_STOCK_EXECUTE, GTK_ICON_SIZE_LARGE_TOOLBAR);
	gtk_widget_show(poImgBtn);
	poBtnSqlFile = gtk_button_new_with_label (_("SQL File"));
	gtk_button_set_image(GTK_BUTTON(poBtnSqlFile), poImgBtn);
	gtk_button_set_relief(GTK_BUTTON(poBtnSqlFile), GTK_RELIEF_NONE);
	g_signal_connect (poBtnSqlFile, "clicked", G_CALLBACK (gmlc_gui_server_tab_data_evt_btn_sql_file_clicked), pGmlcGuiSrvTabData);
	gtk_widget_show(poBtnSqlFile);
	gtk_box_pack_start (GTK_BOX (pGmlcGuiSrvTabData->poDataToolbar), poBtnSqlFile, FALSE, FALSE, 0);
}

void gmlc_gui_server_tab_data_create_widgets (GmlcGuiServerTabData * pGmlcGuiSrvTabData) {
	GtkTreeSelection * select;
	GtkWidget * vbox, * hpaned, * nbkDatas;
	GtkWidget * label;
	GtkWidget * scrolledwindow;
	GtkWidget * poMenuOpsItem;
	glong lServerVersion = 0;
	
	g_object_get(G_OBJECT(pGmlcGuiSrvTabData->pGmlcMysqlSrv), "version", &lServerVersion, NULL);
	
	hpaned = gtk_hpaned_new ();
	gtk_widget_show (hpaned);
	gtk_paned_set_position (GTK_PANED (hpaned), 200);
	gtk_box_pack_start (GTK_BOX (pGmlcGuiSrvTabData), hpaned, TRUE, TRUE, 0);

	vbox = gtk_vbox_new (FALSE, 0);
	gtk_widget_show (vbox);
	gtk_paned_pack1 (GTK_PANED (hpaned), vbox, FALSE, TRUE);

	scrolledwindow = gtk_scrolled_window_new (NULL, NULL);
	gtk_widget_show (scrolledwindow);
	gtk_box_pack_start (GTK_BOX (vbox), scrolledwindow, TRUE, TRUE, 0);
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow), GTK_POLICY_NEVER, GTK_POLICY_ALWAYS);

	pGmlcGuiSrvTabData->lstBase = gtk_tree_view_new ();
	gtk_widget_show (pGmlcGuiSrvTabData->lstBase);
	gtk_tree_view_set_rules_hint(GTK_TREE_VIEW (pGmlcGuiSrvTabData->lstBase), TRUE);
	gtk_container_add (GTK_CONTAINER (scrolledwindow), pGmlcGuiSrvTabData->lstBase);
	select = gtk_tree_view_get_selection (GTK_TREE_VIEW (pGmlcGuiSrvTabData->lstBase));
	gtk_tree_selection_set_mode (select, GTK_SELECTION_SINGLE);
	g_signal_connect(pGmlcGuiSrvTabData->lstBase, "button-press-event", G_CALLBACK (gmlc_gui_server_tab_data_evt_lst_db_btnpress), pGmlcGuiSrvTabData);
	g_signal_connect(select, "changed", G_CALLBACK (gmlc_gui_server_tab_data_evt_lst_db_selected), pGmlcGuiSrvTabData);

	
	pGmlcGuiSrvTabData->mnuBdOps = gtk_menu_new();
	gtk_widget_show (pGmlcGuiSrvTabData->mnuBdOps);

	poMenuOpsItem = gtk_menu_item_new_with_label(_("Refresh"));
	gtk_widget_show (poMenuOpsItem);
	gtk_menu_append (GTK_MENU_SHELL(pGmlcGuiSrvTabData->mnuBdOps), poMenuOpsItem);
	g_signal_connect (poMenuOpsItem, "activate", G_CALLBACK (gmlc_gui_server_tab_data_evt_menu_db_refresh_activate), pGmlcGuiSrvTabData);

	poMenuOpsItem = gtk_separator_menu_item_new();
	gtk_widget_show (poMenuOpsItem);
	gtk_menu_append (GTK_MENU_SHELL(pGmlcGuiSrvTabData->mnuBdOps), poMenuOpsItem);
	
	poMenuOpsItem = gtk_menu_item_new_with_label(_("Add a database"));
	gtk_widget_show (poMenuOpsItem);
	gtk_menu_append (GTK_MENU_SHELL(pGmlcGuiSrvTabData->mnuBdOps), poMenuOpsItem);
	g_signal_connect (poMenuOpsItem, "activate", G_CALLBACK (gmlc_gui_server_tab_data_evt_menu_db_add_activate), pGmlcGuiSrvTabData);

	poMenuOpsItem = gtk_menu_item_new_with_label(_("Alter a database"));
	gtk_widget_show (poMenuOpsItem);
	gtk_menu_append (GTK_MENU_SHELL(pGmlcGuiSrvTabData->mnuBdOps), poMenuOpsItem);
	g_signal_connect (poMenuOpsItem, "activate", G_CALLBACK (gmlc_gui_server_tab_data_evt_menu_db_alter_activate), pGmlcGuiSrvTabData);

	poMenuOpsItem = gtk_menu_item_new_with_label(_("Remove a database"));
	gtk_widget_show (poMenuOpsItem);
	gtk_menu_append (GTK_MENU_SHELL(pGmlcGuiSrvTabData->mnuBdOps), poMenuOpsItem);
	g_signal_connect (poMenuOpsItem, "activate", G_CALLBACK (gmlc_gui_server_tab_data_evt_menu_db_remove_activate), pGmlcGuiSrvTabData);

	poMenuOpsItem = gtk_separator_menu_item_new();
	gtk_widget_show (poMenuOpsItem);
	gtk_menu_append (GTK_MENU_SHELL(pGmlcGuiSrvTabData->mnuBdOps), poMenuOpsItem);
	
	poMenuOpsItem = gtk_menu_item_new_with_label(_("Display all create structures"));
	gtk_widget_show (poMenuOpsItem);
	gtk_menu_append (GTK_MENU_SHELL(pGmlcGuiSrvTabData->mnuBdOps), poMenuOpsItem);
	g_signal_connect (poMenuOpsItem, "activate", G_CALLBACK (gmlc_gui_server_tab_data_evt_menu_db_show_create_activate), pGmlcGuiSrvTabData);

	
	nbkDatas = gtk_notebook_new ();
	gtk_widget_show (nbkDatas);
	gtk_paned_pack2 (GTK_PANED (hpaned), nbkDatas, TRUE, TRUE);
	

	label = gtk_label_new (_("Tables"));
	gtk_widget_show (label);
	gtk_label_set_justify (GTK_LABEL (label), GTK_JUSTIFY_LEFT);
	
	vbox = gtk_vbox_new (FALSE, 0);
	gtk_widget_show (vbox);
	
	gtk_notebook_append_page(GTK_NOTEBOOK(nbkDatas), vbox, label);

	scrolledwindow = gtk_scrolled_window_new (NULL, NULL);
	gtk_widget_show (scrolledwindow);
	gtk_box_pack_start (GTK_BOX (vbox), scrolledwindow, TRUE, TRUE, 0);
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow), GTK_POLICY_NEVER, GTK_POLICY_ALWAYS);

	pGmlcGuiSrvTabData->lstTables = gtk_tree_view_new ();
	gtk_widget_show (pGmlcGuiSrvTabData->lstTables);
	gtk_container_add (GTK_CONTAINER (scrolledwindow), pGmlcGuiSrvTabData->lstTables);
	gtk_tree_view_set_rules_hint(GTK_TREE_VIEW (pGmlcGuiSrvTabData->lstTables), TRUE);
	select = gtk_tree_view_get_selection (GTK_TREE_VIEW (pGmlcGuiSrvTabData->lstTables));
	gtk_tree_selection_set_mode (select, GTK_SELECTION_SINGLE);
	g_signal_connect(pGmlcGuiSrvTabData->lstTables, "button-press-event", G_CALLBACK (gmlc_gui_server_tab_data_evt_lst_tbl_btnpress), pGmlcGuiSrvTabData);
	g_signal_connect(select, "changed", G_CALLBACK (gmlc_gui_server_tab_data_evt_lst_tbl_selected), pGmlcGuiSrvTabData);
	
	pGmlcGuiSrvTabData->mnuTblOps = gtk_menu_new();
	gtk_widget_show (pGmlcGuiSrvTabData->mnuTblOps);

	poMenuOpsItem = gtk_menu_item_new_with_label(_("Refresh"));
	gtk_widget_show (poMenuOpsItem);
	gtk_menu_append (GTK_MENU_SHELL(pGmlcGuiSrvTabData->mnuTblOps), poMenuOpsItem);
	g_signal_connect (poMenuOpsItem, "activate", G_CALLBACK (gmlc_gui_server_tab_data_evt_menu_tbl_refresh_activate), pGmlcGuiSrvTabData);

	poMenuOpsItem = gtk_separator_menu_item_new();
	gtk_widget_show (poMenuOpsItem);
	gtk_menu_append (GTK_MENU_SHELL(pGmlcGuiSrvTabData->mnuTblOps), poMenuOpsItem);
	
	poMenuOpsItem = gtk_menu_item_new_with_label(_("Display table data"));
	gtk_widget_show (poMenuOpsItem);
	gtk_menu_append (GTK_MENU_SHELL(pGmlcGuiSrvTabData->mnuTblOps), poMenuOpsItem);
	g_signal_connect (poMenuOpsItem, "activate", G_CALLBACK (gmlc_gui_server_tab_data_evt_menu_tbl_display_activate), pGmlcGuiSrvTabData);
	
	poMenuOpsItem = gtk_separator_menu_item_new();
	gtk_widget_show (poMenuOpsItem);
	gtk_menu_append (GTK_MENU_SHELL(pGmlcGuiSrvTabData->mnuTblOps), poMenuOpsItem);
	
	poMenuOpsItem = gtk_menu_item_new_with_label(_("Add a table"));
	gtk_widget_show (poMenuOpsItem);
	gtk_menu_append (GTK_MENU_SHELL(pGmlcGuiSrvTabData->mnuTblOps), poMenuOpsItem);
	g_signal_connect (poMenuOpsItem, "activate", G_CALLBACK (gmlc_gui_server_tab_data_evt_menu_tbl_add_activate), pGmlcGuiSrvTabData);

	poMenuOpsItem = gtk_menu_item_new_with_label(_("Alter a table"));
	gtk_widget_show (poMenuOpsItem);
	gtk_menu_append (GTK_MENU_SHELL(pGmlcGuiSrvTabData->mnuTblOps), poMenuOpsItem);
	g_signal_connect (poMenuOpsItem, "activate", G_CALLBACK (gmlc_gui_server_tab_data_evt_menu_tbl_alter_activate), pGmlcGuiSrvTabData);

	poMenuOpsItem = gtk_menu_item_new_with_label(_("Remove a table"));
	gtk_widget_show (poMenuOpsItem);
	gtk_menu_append (GTK_MENU_SHELL(pGmlcGuiSrvTabData->mnuTblOps), poMenuOpsItem);
	g_signal_connect (poMenuOpsItem, "activate", G_CALLBACK (gmlc_gui_server_tab_data_evt_menu_tbl_remove_activate), pGmlcGuiSrvTabData);

	poMenuOpsItem = gtk_separator_menu_item_new();
	gtk_widget_show (poMenuOpsItem);
	gtk_menu_append (GTK_MENU_SHELL(pGmlcGuiSrvTabData->mnuTblOps), poMenuOpsItem);
	
	poMenuOpsItem = gtk_menu_item_new_with_label(_("Display create structure"));
	gtk_widget_show (poMenuOpsItem);
	gtk_menu_append (GTK_MENU_SHELL(pGmlcGuiSrvTabData->mnuTblOps), poMenuOpsItem);
	g_signal_connect (poMenuOpsItem, "activate", G_CALLBACK (gmlc_gui_server_tab_data_evt_menu_tbl_show_create_activate), pGmlcGuiSrvTabData);

	if (lServerVersion >= 50001) {
		
		label = gtk_label_new (_("Views"));
		gtk_widget_show (label);
		gtk_label_set_justify (GTK_LABEL (label), GTK_JUSTIFY_LEFT);
		
		vbox = gtk_vbox_new (FALSE, 0);
		gtk_widget_show (vbox);
		
		gtk_notebook_append_page(GTK_NOTEBOOK(nbkDatas), vbox, label);
		
		scrolledwindow = gtk_scrolled_window_new (NULL, NULL);
		gtk_widget_show (scrolledwindow);
		gtk_box_pack_start (GTK_BOX (vbox), scrolledwindow, TRUE, TRUE, 0);
		gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow), GTK_POLICY_NEVER, GTK_POLICY_ALWAYS);
		
		pGmlcGuiSrvTabData->lstViews = gtk_tree_view_new ();
		gtk_widget_show (pGmlcGuiSrvTabData->lstViews);
		gtk_container_add (GTK_CONTAINER (scrolledwindow), pGmlcGuiSrvTabData->lstViews);
		gtk_tree_view_set_rules_hint(GTK_TREE_VIEW (pGmlcGuiSrvTabData->lstViews), TRUE);
		select = gtk_tree_view_get_selection (GTK_TREE_VIEW (pGmlcGuiSrvTabData->lstViews));
		gtk_tree_selection_set_mode (select, GTK_SELECTION_SINGLE);
		g_signal_connect(pGmlcGuiSrvTabData->lstViews, "button-press-event", G_CALLBACK (gmlc_gui_server_tab_data_evt_lst_vw_btnpress), pGmlcGuiSrvTabData);
		g_signal_connect(select, "changed", G_CALLBACK (gmlc_gui_server_tab_data_evt_lst_vw_selected), pGmlcGuiSrvTabData);
		
		pGmlcGuiSrvTabData->mnuVwOps = gtk_menu_new();
		gtk_widget_show (pGmlcGuiSrvTabData->mnuVwOps);

		poMenuOpsItem = gtk_menu_item_new_with_label(_("Refresh"));
		gtk_widget_show (poMenuOpsItem);
		gtk_menu_append (GTK_MENU_SHELL(pGmlcGuiSrvTabData->mnuVwOps), poMenuOpsItem);
		g_signal_connect (poMenuOpsItem, "activate", G_CALLBACK (gmlc_gui_server_tab_data_evt_menu_vw_refresh_activate), pGmlcGuiSrvTabData);

		poMenuOpsItem = gtk_separator_menu_item_new();
		gtk_widget_show (poMenuOpsItem);
		gtk_menu_append (GTK_MENU_SHELL(pGmlcGuiSrvTabData->mnuVwOps), poMenuOpsItem);
		
		poMenuOpsItem = gtk_menu_item_new_with_label(_("Display view data"));
		gtk_widget_show (poMenuOpsItem);
		gtk_menu_append (GTK_MENU_SHELL(pGmlcGuiSrvTabData->mnuVwOps), poMenuOpsItem);
		g_signal_connect (poMenuOpsItem, "activate", G_CALLBACK (gmlc_gui_server_tab_data_evt_menu_vw_display_activate), pGmlcGuiSrvTabData);
		
		poMenuOpsItem = gtk_separator_menu_item_new();
		gtk_widget_show (poMenuOpsItem);
		gtk_menu_append (GTK_MENU_SHELL(pGmlcGuiSrvTabData->mnuVwOps), poMenuOpsItem);
		
		poMenuOpsItem = gtk_menu_item_new_with_label(_("Add a view"));
		gtk_widget_show (poMenuOpsItem);
		gtk_menu_append (GTK_MENU_SHELL(pGmlcGuiSrvTabData->mnuVwOps), poMenuOpsItem);
		g_signal_connect (poMenuOpsItem, "activate", G_CALLBACK (gmlc_gui_server_tab_data_evt_menu_vw_add_activate), pGmlcGuiSrvTabData);

		poMenuOpsItem = gtk_menu_item_new_with_label(_("Alter a view"));
		gtk_widget_show (poMenuOpsItem);
		gtk_menu_append (GTK_MENU_SHELL(pGmlcGuiSrvTabData->mnuVwOps), poMenuOpsItem);
		g_signal_connect (poMenuOpsItem, "activate", G_CALLBACK (gmlc_gui_server_tab_data_evt_menu_vw_alter_activate), pGmlcGuiSrvTabData);

		poMenuOpsItem = gtk_menu_item_new_with_label(_("Remove a view"));
		gtk_widget_show (poMenuOpsItem);
		gtk_menu_append (GTK_MENU_SHELL(pGmlcGuiSrvTabData->mnuVwOps), poMenuOpsItem);
		g_signal_connect (poMenuOpsItem, "activate", G_CALLBACK (gmlc_gui_server_tab_data_evt_menu_vw_remove_activate), pGmlcGuiSrvTabData);

		poMenuOpsItem = gtk_separator_menu_item_new();
		gtk_widget_show (poMenuOpsItem);
		gtk_menu_append (GTK_MENU_SHELL(pGmlcGuiSrvTabData->mnuVwOps), poMenuOpsItem);
		
		poMenuOpsItem = gtk_menu_item_new_with_label(_("Display create structure"));
		gtk_widget_show (poMenuOpsItem);
		gtk_menu_append (GTK_MENU_SHELL(pGmlcGuiSrvTabData->mnuVwOps), poMenuOpsItem);
		g_signal_connect (poMenuOpsItem, "activate", G_CALLBACK (gmlc_gui_server_tab_data_evt_menu_vw_show_create_activate), pGmlcGuiSrvTabData);


		label = gtk_label_new (_("Procedures"));
		gtk_widget_show (label);
		gtk_label_set_justify (GTK_LABEL (label), GTK_JUSTIFY_LEFT);
		
		vbox = gtk_vbox_new (FALSE, 0);
		gtk_widget_show (vbox);
		
		gtk_notebook_append_page(GTK_NOTEBOOK(nbkDatas), vbox, label);
		
		scrolledwindow = gtk_scrolled_window_new (NULL, NULL);
		gtk_widget_show (scrolledwindow);
		gtk_box_pack_start (GTK_BOX (vbox), scrolledwindow, TRUE, TRUE, 0);
		gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow), GTK_POLICY_NEVER, GTK_POLICY_ALWAYS);
		
		pGmlcGuiSrvTabData->lstProcedures = gtk_tree_view_new ();
		gtk_widget_show (pGmlcGuiSrvTabData->lstProcedures);
		gtk_container_add (GTK_CONTAINER (scrolledwindow), pGmlcGuiSrvTabData->lstProcedures);
		gtk_tree_view_set_rules_hint(GTK_TREE_VIEW (pGmlcGuiSrvTabData->lstProcedures), TRUE);
		select = gtk_tree_view_get_selection (GTK_TREE_VIEW (pGmlcGuiSrvTabData->lstProcedures));
		gtk_tree_selection_set_mode (select, GTK_SELECTION_SINGLE);
		g_signal_connect(pGmlcGuiSrvTabData->lstProcedures, "button-press-event", G_CALLBACK (gmlc_gui_server_tab_data_evt_lst_proc_btnpress), pGmlcGuiSrvTabData);
		g_signal_connect(select, "changed", G_CALLBACK (gmlc_gui_server_tab_data_evt_lst_proc_selected), pGmlcGuiSrvTabData);
		
		pGmlcGuiSrvTabData->mnuProcOps = gtk_menu_new();
		gtk_widget_show (pGmlcGuiSrvTabData->mnuProcOps);

		poMenuOpsItem = gtk_menu_item_new_with_label(_("Refresh"));
		gtk_widget_show (poMenuOpsItem);
		gtk_menu_append (GTK_MENU_SHELL(pGmlcGuiSrvTabData->mnuProcOps), poMenuOpsItem);
		g_signal_connect (poMenuOpsItem, "activate", G_CALLBACK (gmlc_gui_server_tab_data_evt_menu_proc_refresh_activate), pGmlcGuiSrvTabData);

		poMenuOpsItem = gtk_separator_menu_item_new();
		gtk_widget_show (poMenuOpsItem);
		gtk_menu_append (GTK_MENU_SHELL(pGmlcGuiSrvTabData->mnuProcOps), poMenuOpsItem);
		
		poMenuOpsItem = gtk_menu_item_new_with_label(_("Execute a procedure"));
		gtk_widget_show (poMenuOpsItem);
		gtk_menu_append (GTK_MENU_SHELL(pGmlcGuiSrvTabData->mnuProcOps), poMenuOpsItem);
		g_signal_connect (poMenuOpsItem, "activate", G_CALLBACK (gmlc_gui_server_tab_data_evt_menu_proc_display_activate), pGmlcGuiSrvTabData);

		poMenuOpsItem = gtk_separator_menu_item_new();
		gtk_widget_show (poMenuOpsItem);
		gtk_menu_append (GTK_MENU_SHELL(pGmlcGuiSrvTabData->mnuProcOps), poMenuOpsItem);
		
		poMenuOpsItem = gtk_menu_item_new_with_label(_("Add a procedure"));
		gtk_widget_show (poMenuOpsItem);
		gtk_menu_append (GTK_MENU_SHELL(pGmlcGuiSrvTabData->mnuProcOps), poMenuOpsItem);
		g_signal_connect (poMenuOpsItem, "activate", G_CALLBACK (gmlc_gui_server_tab_data_evt_menu_proc_add_activate), pGmlcGuiSrvTabData);

		poMenuOpsItem = gtk_menu_item_new_with_label(_("Alter a procedure"));
		gtk_widget_show (poMenuOpsItem);
		gtk_menu_append (GTK_MENU_SHELL(pGmlcGuiSrvTabData->mnuProcOps), poMenuOpsItem);
		g_signal_connect (poMenuOpsItem, "activate", G_CALLBACK (gmlc_gui_server_tab_data_evt_menu_proc_alter_activate), pGmlcGuiSrvTabData);

		poMenuOpsItem = gtk_menu_item_new_with_label(_("Remove a procedure"));
		gtk_widget_show (poMenuOpsItem);
		gtk_menu_append (GTK_MENU_SHELL(pGmlcGuiSrvTabData->mnuProcOps), poMenuOpsItem);
		g_signal_connect (poMenuOpsItem, "activate", G_CALLBACK (gmlc_gui_server_tab_data_evt_menu_proc_remove_activate), pGmlcGuiSrvTabData);

		poMenuOpsItem = gtk_separator_menu_item_new();
		gtk_widget_show (poMenuOpsItem);
		gtk_menu_append (GTK_MENU_SHELL(pGmlcGuiSrvTabData->mnuProcOps), poMenuOpsItem);
		
		poMenuOpsItem = gtk_menu_item_new_with_label(_("Display create structure"));
		gtk_widget_show (poMenuOpsItem);
		gtk_menu_append (GTK_MENU_SHELL(pGmlcGuiSrvTabData->mnuProcOps), poMenuOpsItem);
		g_signal_connect (poMenuOpsItem, "activate", G_CALLBACK (gmlc_gui_server_tab_data_evt_menu_proc_show_create_activate), pGmlcGuiSrvTabData);
		
				
		label = gtk_label_new (_("Functions"));
		gtk_widget_show (label);
		gtk_label_set_justify (GTK_LABEL (label), GTK_JUSTIFY_LEFT);
		
		vbox = gtk_vbox_new (FALSE, 0);
		gtk_widget_show (vbox);
		
		gtk_notebook_append_page(GTK_NOTEBOOK(nbkDatas), vbox, label);
		
		scrolledwindow = gtk_scrolled_window_new (NULL, NULL);
		gtk_widget_show (scrolledwindow);
		gtk_box_pack_start (GTK_BOX (vbox), scrolledwindow, TRUE, TRUE, 0);
		gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow), GTK_POLICY_NEVER, GTK_POLICY_ALWAYS);
		
		pGmlcGuiSrvTabData->lstFunctions = gtk_tree_view_new ();
		gtk_widget_show (pGmlcGuiSrvTabData->lstFunctions);
		gtk_container_add (GTK_CONTAINER (scrolledwindow), pGmlcGuiSrvTabData->lstFunctions);
		gtk_tree_view_set_rules_hint(GTK_TREE_VIEW (pGmlcGuiSrvTabData->lstFunctions), TRUE);
		select = gtk_tree_view_get_selection (GTK_TREE_VIEW (pGmlcGuiSrvTabData->lstFunctions));
		gtk_tree_selection_set_mode (select, GTK_SELECTION_SINGLE);
		g_signal_connect(pGmlcGuiSrvTabData->lstFunctions, "button-press-event", G_CALLBACK (gmlc_gui_server_tab_data_evt_lst_func_btnpress), pGmlcGuiSrvTabData);
		g_signal_connect(select, "changed", G_CALLBACK (gmlc_gui_server_tab_data_evt_lst_func_selected), pGmlcGuiSrvTabData);
		
		pGmlcGuiSrvTabData->mnuFuncOps = gtk_menu_new();
		gtk_widget_show (pGmlcGuiSrvTabData->mnuFuncOps);

		poMenuOpsItem = gtk_menu_item_new_with_label(_("Refresh"));
		gtk_widget_show (poMenuOpsItem);
		gtk_menu_append (GTK_MENU_SHELL(pGmlcGuiSrvTabData->mnuFuncOps), poMenuOpsItem);
		g_signal_connect (poMenuOpsItem, "activate", G_CALLBACK (gmlc_gui_server_tab_data_evt_menu_func_refresh_activate), pGmlcGuiSrvTabData);

		poMenuOpsItem = gtk_separator_menu_item_new();
		gtk_widget_show (poMenuOpsItem);
		gtk_menu_append (GTK_MENU_SHELL(pGmlcGuiSrvTabData->mnuFuncOps), poMenuOpsItem);
		
		poMenuOpsItem = gtk_menu_item_new_with_label(_("Execute a function"));
		gtk_widget_show (poMenuOpsItem);
		gtk_menu_append (GTK_MENU_SHELL(pGmlcGuiSrvTabData->mnuFuncOps), poMenuOpsItem);
		g_signal_connect (poMenuOpsItem, "activate", G_CALLBACK (gmlc_gui_server_tab_data_evt_menu_func_display_activate), pGmlcGuiSrvTabData);

		poMenuOpsItem = gtk_separator_menu_item_new();
		gtk_widget_show (poMenuOpsItem);
		gtk_menu_append (GTK_MENU_SHELL(pGmlcGuiSrvTabData->mnuFuncOps), poMenuOpsItem);
		
		poMenuOpsItem = gtk_menu_item_new_with_label(_("Add a function"));
		gtk_widget_show (poMenuOpsItem);
		gtk_menu_append (GTK_MENU_SHELL(pGmlcGuiSrvTabData->mnuFuncOps), poMenuOpsItem);
		g_signal_connect (poMenuOpsItem, "activate", G_CALLBACK (gmlc_gui_server_tab_data_evt_menu_func_add_activate), pGmlcGuiSrvTabData);

		poMenuOpsItem = gtk_menu_item_new_with_label(_("Alter a function"));
		gtk_widget_show (poMenuOpsItem);
		gtk_menu_append (GTK_MENU_SHELL(pGmlcGuiSrvTabData->mnuFuncOps), poMenuOpsItem);
		g_signal_connect (poMenuOpsItem, "activate", G_CALLBACK (gmlc_gui_server_tab_data_evt_menu_func_alter_activate), pGmlcGuiSrvTabData);

		poMenuOpsItem = gtk_menu_item_new_with_label(_("Remove a function"));
		gtk_widget_show (poMenuOpsItem);
		gtk_menu_append (GTK_MENU_SHELL(pGmlcGuiSrvTabData->mnuFuncOps), poMenuOpsItem);
		g_signal_connect (poMenuOpsItem, "activate", G_CALLBACK (gmlc_gui_server_tab_data_evt_menu_func_remove_activate), pGmlcGuiSrvTabData);

		poMenuOpsItem = gtk_separator_menu_item_new();
		gtk_widget_show (poMenuOpsItem);
		gtk_menu_append (GTK_MENU_SHELL(pGmlcGuiSrvTabData->mnuFuncOps), poMenuOpsItem);
		
		poMenuOpsItem = gtk_menu_item_new_with_label(_("Display create structure"));
		gtk_widget_show (poMenuOpsItem);
		gtk_menu_append (GTK_MENU_SHELL(pGmlcGuiSrvTabData->mnuFuncOps), poMenuOpsItem);
		g_signal_connect (poMenuOpsItem, "activate", G_CALLBACK (gmlc_gui_server_tab_data_evt_menu_func_show_create_activate), pGmlcGuiSrvTabData);
	}
}

void gmlc_gui_server_tab_data_init_widgets (GmlcGuiServerTabData * pGmlcGuiSrvTabData) {
	GtkTreeViewColumn * currCol;
	GtkCellRenderer * renderer;
	GtkListStore * lstEmpty;
	glong lServerVersion = 0;
	
	g_object_get(G_OBJECT(pGmlcGuiSrvTabData->pGmlcMysqlSrv), "version", &lServerVersion, NULL);
	
	/* Generate columns for all tree views */
	renderer = gtk_cell_renderer_text_new ();
	
	/* Databases columns */
	currCol = gtk_tree_view_column_new_with_attributes (_("Databases"), renderer, "text", 0, NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW (pGmlcGuiSrvTabData->lstBase), currCol);
	
	lstEmpty = gtk_list_store_new(1, G_TYPE_STRING);
	gtk_tree_view_set_model(GTK_TREE_VIEW(pGmlcGuiSrvTabData->lstBase), GTK_TREE_MODEL(lstEmpty));
	g_object_unref (G_OBJECT (lstEmpty));
	
	/* Table columns */
	currCol = gtk_tree_view_column_new_with_attributes (_("Name"), renderer, "text", 0, NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW (pGmlcGuiSrvTabData->lstTables), currCol);
	currCol = gtk_tree_view_column_new_with_attributes (_("Rows"), renderer, "text", 1, NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW (pGmlcGuiSrvTabData->lstTables), currCol);
	currCol = gtk_tree_view_column_new_with_attributes (_("Size"), renderer, "text", 2, NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW (pGmlcGuiSrvTabData->lstTables), currCol);
	currCol = gtk_tree_view_column_new_with_attributes (_("Type"), renderer, "text", 3, NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW (pGmlcGuiSrvTabData->lstTables), currCol);
	
	lstEmpty = gtk_list_store_new(4, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);
	gtk_tree_view_set_model(GTK_TREE_VIEW(pGmlcGuiSrvTabData->lstTables), GTK_TREE_MODEL(lstEmpty));
	g_object_unref (G_OBJECT (lstEmpty));
	
	if (lServerVersion >= 50001) {
		/* View columns */
		currCol = gtk_tree_view_column_new_with_attributes (_("Name"), renderer, "text", 0, NULL);
		gtk_tree_view_append_column (GTK_TREE_VIEW (pGmlcGuiSrvTabData->lstViews), currCol);
		
		lstEmpty = gtk_list_store_new(1, G_TYPE_STRING);
		gtk_tree_view_set_model(GTK_TREE_VIEW(pGmlcGuiSrvTabData->lstViews), GTK_TREE_MODEL(lstEmpty));
		g_object_unref (G_OBJECT (lstEmpty));
		
		/* Procedure columns */
		currCol = gtk_tree_view_column_new_with_attributes (_("Name"), renderer, "text", 0, NULL);
		gtk_tree_view_append_column (GTK_TREE_VIEW (pGmlcGuiSrvTabData->lstProcedures), currCol);
		
		lstEmpty = gtk_list_store_new(1, G_TYPE_STRING);
		gtk_tree_view_set_model(GTK_TREE_VIEW(pGmlcGuiSrvTabData->lstProcedures), GTK_TREE_MODEL(lstEmpty));
		g_object_unref (G_OBJECT (lstEmpty));
		
		/* Function columns */
		currCol = gtk_tree_view_column_new_with_attributes (_("Name"), renderer, "text", 0, NULL);
		gtk_tree_view_append_column (GTK_TREE_VIEW (pGmlcGuiSrvTabData->lstFunctions), currCol);
		
		lstEmpty = gtk_list_store_new(1, G_TYPE_STRING);
		gtk_tree_view_set_model(GTK_TREE_VIEW(pGmlcGuiSrvTabData->lstFunctions), GTK_TREE_MODEL(lstEmpty));
		g_object_unref (G_OBJECT (lstEmpty));
	}
	
	gmlc_gui_server_tab_data_fill_database_list(pGmlcGuiSrvTabData);
}

void gmlc_gui_server_tab_data_open_file (GmlcGuiServerTabData * pGmlcGuiSrvTabData) {
	gchar * pcDbName = NULL;
	GtkWidget * poQueryTab = NULL;
	
	if (pGmlcGuiSrvTabData->pGmlcMysqlDb != NULL) {
		g_object_get(G_OBJECT(pGmlcGuiSrvTabData->pGmlcMysqlDb), "db_name", &pcDbName, NULL);
		poQueryTab = gmlc_gui_server_add_query_tab(pGmlcGuiSrvTabData->pGmlcGuiSrv, pcDbName, NULL, FALSE);
	} else {
		poQueryTab = gmlc_gui_server_add_query_tab(pGmlcGuiSrvTabData->pGmlcGuiSrv, NULL, NULL, FALSE);
	}
	
	gmlc_gui_server_tab_query_evt_btn_load_clicked(NULL, poQueryTab);
}

void gmlc_gui_server_tab_data_open_query (GmlcGuiServerTabData * pGmlcGuiSrvTabData, const gchar * pcQuery, gboolean bExecNow) {
	gchar * pcDbName = NULL;
	
	if (pGmlcGuiSrvTabData->pGmlcMysqlDb != NULL) {
		g_object_get(G_OBJECT(pGmlcGuiSrvTabData->pGmlcMysqlDb), "db_name", &pcDbName, NULL);
		gmlc_gui_server_add_query_tab(pGmlcGuiSrvTabData->pGmlcGuiSrv, pcDbName, pcQuery, bExecNow);
	} else {
		gmlc_gui_server_add_query_tab(pGmlcGuiSrvTabData->pGmlcGuiSrv, NULL, pcQuery, bExecNow);
	}
}

void gmlc_gui_server_tab_data_open_query_select (GmlcGuiServerTabData * pGmlcGuiSrvTabData, gboolean bUseTable) {
	gchar * pcSql = NULL, * pcDbName = NULL, * pcTblName = NULL;
	
	if (bUseTable && pGmlcGuiSrvTabData->pGmlcMysqlTbl != NULL) {
		g_object_get(G_OBJECT(pGmlcGuiSrvTabData->pGmlcMysqlDb), "db_name", &pcDbName, NULL);
		g_object_get(G_OBJECT(pGmlcGuiSrvTabData->pGmlcMysqlTbl), "name", &pcTblName, NULL);
		
		pcSql = g_strdup_printf("SELECT * \nFROM `%s`.`%s`\nWHERE 1\nLIMIT 1000", pcDbName, pcTblName);
	}
	
	gmlc_gui_server_tab_data_open_query(pGmlcGuiSrvTabData, pcSql, (pcSql != NULL));
	g_free(pcSql);
}

void gmlc_gui_server_tab_data_fill_database_list (GmlcGuiServerTabData * pGmlcGuiSrvTabData) {
	GtkListStore * lstStrBase = NULL, * lstEmpty = NULL;
	glong lServerVersion = 0;
	
	g_object_get(G_OBJECT(pGmlcGuiSrvTabData->pGmlcMysqlSrv), "version", &lServerVersion, NULL);

	void sub_ht_fill_database_list(gpointer key, gpointer value, gpointer user_data) {
		GmlcMysqlDatabase * pGmlcMysqlDb = GMLC_MYSQL_DATABASE(value);
		GtkListStore * lstStrBase = GTK_LIST_STORE(user_data);
		GtkTreeIter iter;
		gchar * pcName = NULL;
		
		g_object_get(G_OBJECT(pGmlcMysqlDb), "db_name", &pcName, NULL);
		
		gtk_list_store_append (lstStrBase, &iter);
		gtk_list_store_set (lstStrBase, &iter, 0, pcName, 1, pGmlcMysqlDb, -1);
	}

	lstStrBase = gtk_list_store_new(2, G_TYPE_STRING, G_TYPE_POINTER);
	gtk_tree_sortable_set_sort_column_id (GTK_TREE_SORTABLE(lstStrBase), 0, GTK_SORT_ASCENDING);
	
	if (gmlc_mysql_server_update_databases_list(pGmlcGuiSrvTabData->pGmlcMysqlSrv)) {
		g_hash_table_foreach(pGmlcGuiSrvTabData->pGmlcMysqlSrv->htbDatabases, &sub_ht_fill_database_list, lstStrBase);
	}
	
	gtk_tree_view_set_model(GTK_TREE_VIEW(pGmlcGuiSrvTabData->lstBase), GTK_TREE_MODEL(lstStrBase));
	g_object_unref (G_OBJECT (lstStrBase));
	
	lstEmpty = gtk_list_store_new(4, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);
	gtk_tree_view_set_model(GTK_TREE_VIEW(pGmlcGuiSrvTabData->lstTables), GTK_TREE_MODEL(lstEmpty));
	g_object_unref (G_OBJECT (lstEmpty));
	
	if (lServerVersion >= 50001) {
		lstEmpty = gtk_list_store_new(1, G_TYPE_STRING);
		gtk_tree_view_set_model(GTK_TREE_VIEW(pGmlcGuiSrvTabData->lstViews), GTK_TREE_MODEL(lstEmpty));
		g_object_unref (G_OBJECT (lstEmpty));
		
		lstEmpty = gtk_list_store_new(1, G_TYPE_STRING);
		gtk_tree_view_set_model(GTK_TREE_VIEW(pGmlcGuiSrvTabData->lstProcedures), GTK_TREE_MODEL(lstEmpty));
		g_object_unref (G_OBJECT (lstEmpty));
		
		lstEmpty = gtk_list_store_new(1, G_TYPE_STRING);
		gtk_tree_view_set_model(GTK_TREE_VIEW(pGmlcGuiSrvTabData->lstFunctions), GTK_TREE_MODEL(lstEmpty));
		g_object_unref (G_OBJECT (lstEmpty));
	}
	
	pGmlcGuiSrvTabData->pGmlcMysqlDb = NULL;
	pGmlcGuiSrvTabData->pGmlcMysqlTbl = NULL;
	pGmlcGuiSrvTabData->pGmlcMysqlVw = NULL;
	pGmlcGuiSrvTabData->pGmlcMysqlProc = NULL;
	pGmlcGuiSrvTabData->pGmlcMysqlFunc = NULL;
}

void gmlc_gui_server_tab_data_fill_tables_list (GmlcGuiServerTabData * pGmlcGuiSrvTabData) {
	GtkListStore * lstStrTable;

	void sub_ht_fill_table_list(gpointer key, gpointer value, gpointer user_data) {
		GmlcMysqlTable * pGmlcMysqlTbl = GMLC_MYSQL_TABLE(value);
		GtkListStore * lstStrTable = GTK_LIST_STORE(user_data);
		GtkTreeIter iter;
		gchar * pcName = NULL, * pcRows = NULL, * pcSize = NULL, * pcEngine = NULL;
		
		g_object_get(G_OBJECT(pGmlcMysqlTbl), "name", &pcName, "rows", &pcRows, "size", &pcSize, "engine", &pcEngine, NULL);
		
		gtk_list_store_append (lstStrTable, &iter);
		gtk_list_store_set (lstStrTable, &iter, 0, pcName, 1, pcRows, 2, pcSize, 3, pcEngine, 4, pGmlcMysqlTbl, -1);
	}
	
	lstStrTable = gtk_list_store_new(5, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_POINTER);
	gtk_tree_sortable_set_sort_column_id (GTK_TREE_SORTABLE(lstStrTable), 0, GTK_SORT_ASCENDING);
	
	/* Fill listStore with tables */
	if (gmlc_mysql_database_update_tables_list(pGmlcGuiSrvTabData->pGmlcMysqlDb)) {
		g_hash_table_foreach(pGmlcGuiSrvTabData->pGmlcMysqlDb->htbTables, &sub_ht_fill_table_list, lstStrTable);
	}
	
	gtk_tree_view_set_model(GTK_TREE_VIEW(pGmlcGuiSrvTabData->lstTables), GTK_TREE_MODEL(lstStrTable));
	g_object_unref (G_OBJECT (lstStrTable));
	
	pGmlcGuiSrvTabData->pGmlcMysqlTbl = NULL;
}

void gmlc_gui_server_tab_data_fill_views_list (GmlcGuiServerTabData * pGmlcGuiSrvTabData) {
	GtkListStore * lstStrView;

	void sub_ht_fill_view_list(gpointer key, gpointer value, gpointer user_data) {
		GmlcMysqlView * pGmlcMysqlVw = GMLC_MYSQL_VIEW(value);
		GtkListStore * lstStrView = GTK_LIST_STORE(user_data);
		GtkTreeIter iter;
		gchar * pcName = NULL;
		
		g_object_get(G_OBJECT(pGmlcMysqlVw), "name", &pcName, NULL);
		
		gtk_list_store_append (lstStrView, &iter);
		gtk_list_store_set (lstStrView, &iter, 0, pcName, 1, pGmlcMysqlVw, -1);
	}
	
	lstStrView = gtk_list_store_new(2, G_TYPE_STRING, G_TYPE_POINTER);
	gtk_tree_sortable_set_sort_column_id (GTK_TREE_SORTABLE(lstStrView), 0, GTK_SORT_ASCENDING);
	
	/* Fill listStore with views */
	if (gmlc_mysql_database_update_views_list(pGmlcGuiSrvTabData->pGmlcMysqlDb)) {
		g_hash_table_foreach(pGmlcGuiSrvTabData->pGmlcMysqlDb->htbViews, sub_ht_fill_view_list, lstStrView);
	}
	
	gtk_tree_view_set_model(GTK_TREE_VIEW(pGmlcGuiSrvTabData->lstViews), GTK_TREE_MODEL(lstStrView));
	g_object_unref (G_OBJECT (lstStrView));
	
	pGmlcGuiSrvTabData->pGmlcMysqlVw = NULL;
}

void gmlc_gui_server_tab_data_fill_procedures_list (GmlcGuiServerTabData * pGmlcGuiSrvTabData) {
	GtkListStore * lstStrProcedure;

	void sub_ht_fill_procedure_list(gpointer key, gpointer value, gpointer user_data) {
		GmlcMysqlProcedure * pGmlcMysqlProc = GMLC_MYSQL_PROCEDURE(value);
		GtkListStore * lstStrProcedure = GTK_LIST_STORE(user_data);
		GtkTreeIter iter;
		gchar * pcName = NULL;
		
		g_object_get(G_OBJECT(pGmlcMysqlProc), "name", &pcName, NULL);
		
		gtk_list_store_append (lstStrProcedure, &iter);
		gtk_list_store_set (lstStrProcedure, &iter, 0, pcName, 1, pGmlcMysqlProc, -1);
	}
	
	lstStrProcedure = gtk_list_store_new(2, G_TYPE_STRING, G_TYPE_POINTER);
	gtk_tree_sortable_set_sort_column_id (GTK_TREE_SORTABLE(lstStrProcedure), 0, GTK_SORT_ASCENDING);
	
	/* Fill listStore with procedures */
	if (gmlc_mysql_database_update_procedures_list(pGmlcGuiSrvTabData->pGmlcMysqlDb)) {
		g_hash_table_foreach(pGmlcGuiSrvTabData->pGmlcMysqlDb->htbProcedures, sub_ht_fill_procedure_list, lstStrProcedure);
	}
	
	gtk_tree_view_set_model(GTK_TREE_VIEW(pGmlcGuiSrvTabData->lstProcedures), GTK_TREE_MODEL(lstStrProcedure));
	g_object_unref (G_OBJECT (lstStrProcedure));
	
	pGmlcGuiSrvTabData->pGmlcMysqlProc = NULL;
}

void gmlc_gui_server_tab_data_fill_functions_list (GmlcGuiServerTabData * pGmlcGuiSrvTabData) {
	GtkListStore * lstStrFunction;

	void sub_ht_fill_function_list(gpointer key, gpointer value, gpointer user_data) {
		GmlcMysqlFunction * pGmlcMysqlFunc = GMLC_MYSQL_FUNCTION(value);
		GtkListStore * lstStrFunction = GTK_LIST_STORE(user_data);
		GtkTreeIter iter;
		gchar * pcName = NULL;
		
		g_object_get(G_OBJECT(pGmlcMysqlFunc), "name", &pcName, NULL);
		
		gtk_list_store_append (lstStrFunction, &iter);
		gtk_list_store_set (lstStrFunction, &iter, 0, pcName, 1, pGmlcMysqlFunc, -1);
	}
	
	lstStrFunction = gtk_list_store_new(2, G_TYPE_STRING, G_TYPE_POINTER);
	gtk_tree_sortable_set_sort_column_id (GTK_TREE_SORTABLE(lstStrFunction), 0, GTK_SORT_ASCENDING);
	
	/* Fill listStore with views*/
	if (gmlc_mysql_database_update_functions_list(pGmlcGuiSrvTabData->pGmlcMysqlDb)) {
		g_hash_table_foreach(pGmlcGuiSrvTabData->pGmlcMysqlDb->htbFunctions, sub_ht_fill_function_list, lstStrFunction);
	}
	
	gtk_tree_view_set_model(GTK_TREE_VIEW(pGmlcGuiSrvTabData->lstFunctions), GTK_TREE_MODEL(lstStrFunction));
	g_object_unref (G_OBJECT (lstStrFunction));
	
	pGmlcGuiSrvTabData->pGmlcMysqlFunc = NULL;
}



static void gmlc_gui_server_tab_data_evt_btn_sql_file_clicked (GtkWidget *widget, gpointer user_data) {
	GmlcGuiServerTabData * pGmlcGuiSrvTabData = GMLC_GUI_SERVER_TAB_DATA(user_data);
	UNUSED_VAR(widget);
	UNUSED_VAR(pGmlcGuiSrvTabData);
	
	gmlc_gui_server_tab_data_open_file(pGmlcGuiSrvTabData);
}

static void gmlc_gui_server_tab_data_evt_btn_sql_clicked (GtkWidget *widget, gpointer user_data) {
	GmlcGuiServerTabData * pGmlcGuiSrvTabData = GMLC_GUI_SERVER_TAB_DATA(user_data);
	UNUSED_VAR(widget);
	
	gmlc_gui_server_tab_data_open_query_select (pGmlcGuiSrvTabData, FALSE);
}



static gboolean gmlc_gui_server_tab_data_evt_lst_db_btnpress (GtkWidget *widget, GdkEventButton *event, gpointer user_data) {
	GmlcGuiServerTabData * pGmlcGuiSrvTabData = GMLC_GUI_SERVER_TAB_DATA(user_data);
	
	switch (event->button) {
		case 1 : /* Left button */
			if (event->type == GDK_2BUTTON_PRESS) {
				gmlc_gui_server_tab_data_open_query_select (pGmlcGuiSrvTabData, FALSE);
			}
			break;
		case 3 : /* Right button */
			if (event->type == GDK_BUTTON_PRESS) {
				gtk_menu_popup(GTK_MENU(pGmlcGuiSrvTabData->mnuBdOps), NULL, NULL, NULL, user_data, event->button, event->time);
			}
			break;
	}
	return FALSE;
	
}

static void gmlc_gui_server_tab_data_evt_lst_db_selected (GtkTreeSelection *selection, gpointer user_data) {
	GmlcGuiServerTabData * pGmlcGuiSrvTabData = GMLC_GUI_SERVER_TAB_DATA(user_data);
	GtkTreeIter iter;
	GtkTreeModel *model;
	glong lServerVersion = 0;
	
	g_object_get(G_OBJECT(pGmlcGuiSrvTabData->pGmlcMysqlSrv), "version", &lServerVersion, NULL);
	
	g_print("Changed !\n");
	if (gtk_tree_selection_get_selected (selection, &model, &iter)) {
		gtk_tree_model_get (model, &iter, 1, &pGmlcGuiSrvTabData->pGmlcMysqlDb, -1);
		
		gmlc_gui_server_tab_data_fill_tables_list(pGmlcGuiSrvTabData);
		
		if (lServerVersion >= 50001) {
			gmlc_gui_server_tab_data_fill_views_list(pGmlcGuiSrvTabData);
			gmlc_gui_server_tab_data_fill_procedures_list(pGmlcGuiSrvTabData);
			gmlc_gui_server_tab_data_fill_functions_list(pGmlcGuiSrvTabData);
		}
	}
}

static void gmlc_gui_server_tab_data_evt_menu_db_refresh_activate (GtkWidget *widget, gpointer user_data) {
	GmlcGuiServerTabData * pGmlcGuiSrvTabData = GMLC_GUI_SERVER_TAB_DATA(user_data);
	UNUSED_VAR(widget);
	
	gmlc_gui_server_tab_data_fill_database_list(pGmlcGuiSrvTabData);
}

static void gmlc_gui_server_tab_data_evt_menu_db_add_activate (GtkWidget *widget, gpointer user_data) {
	GmlcGuiServerTabData * pGmlcGuiSrvTabData = GMLC_GUI_SERVER_TAB_DATA(user_data);
	gchar * pcQuery = NULL;
	UNUSED_VAR(widget);
	
	if (pGmlcGuiSrvTabData->pGmlcMysqlDb != NULL) {
		pcQuery = gmlc_mysql_structure_get_create(GMLC_MYSQL_STRUCTURE(pGmlcGuiSrvTabData->pGmlcMysqlDb), FALSE, NULL);
		
		gmlc_gui_server_tab_data_open_query(pGmlcGuiSrvTabData, pcQuery, FALSE);
		
		g_free(pcQuery);
	}
}

static void gmlc_gui_server_tab_data_evt_menu_db_alter_activate (GtkWidget *widget, gpointer user_data) {
	GmlcGuiServerTabData * pGmlcGuiSrvTabData = GMLC_GUI_SERVER_TAB_DATA(user_data);
	gchar * pcQuery = NULL;
	UNUSED_VAR(widget);
	
	if (pGmlcGuiSrvTabData->pGmlcMysqlDb != NULL) {
		pcQuery = gmlc_mysql_structure_get_alter(GMLC_MYSQL_STRUCTURE(pGmlcGuiSrvTabData->pGmlcMysqlDb), TRUE, NULL);
		
		gmlc_gui_server_tab_data_open_query(pGmlcGuiSrvTabData, pcQuery, FALSE);
		
		g_free(pcQuery);
	}
}

static void gmlc_gui_server_tab_data_evt_menu_db_remove_activate (GtkWidget *widget, gpointer user_data) {
	GmlcGuiServerTabData * pGmlcGuiSrvTabData = GMLC_GUI_SERVER_TAB_DATA(user_data);
	gchar * pcQuery = NULL;
	UNUSED_VAR(widget);
	
	if (pGmlcGuiSrvTabData->pGmlcMysqlDb != NULL) {
		pcQuery = gmlc_mysql_structure_get_drop(GMLC_MYSQL_STRUCTURE(pGmlcGuiSrvTabData->pGmlcMysqlDb), TRUE, NULL);
		
		gmlc_gui_server_tab_data_open_query(pGmlcGuiSrvTabData, pcQuery, FALSE);
		
		g_free(pcQuery);
	}
}

static void gmlc_gui_server_tab_data_evt_menu_db_show_create_activate (GtkWidget *widget, gpointer user_data) {
	GmlcGuiServerTabData * pGmlcGuiSrvTabData = GMLC_GUI_SERVER_TAB_DATA(user_data);
	gchar * pcQuery = NULL;
	UNUSED_VAR(widget);
	
	if (pGmlcGuiSrvTabData->pGmlcMysqlDb != NULL) {
		pcQuery = gmlc_mysql_structure_get_create(GMLC_MYSQL_STRUCTURE(pGmlcGuiSrvTabData->pGmlcMysqlDb), TRUE, NULL);
		
		gmlc_gui_server_tab_data_open_query(pGmlcGuiSrvTabData, pcQuery, FALSE);
		
		g_free(pcQuery);
	}
}



static gboolean gmlc_gui_server_tab_data_evt_lst_tbl_btnpress (GtkWidget *widget, GdkEventButton *event, gpointer user_data) {
	GmlcGuiServerTabData * pGmlcGuiSrvTabData = GMLC_GUI_SERVER_TAB_DATA(user_data);
	
	switch (event->button) {
		case 1 : /* Left button */
			if (event->type == GDK_2BUTTON_PRESS) {
				gmlc_gui_server_tab_data_evt_menu_tbl_display_activate (widget, user_data);
			}
			break;
		case 3 : /* Right button */
			if (event->type == GDK_BUTTON_PRESS) {
				gtk_menu_popup(GTK_MENU(pGmlcGuiSrvTabData->mnuTblOps), NULL, NULL, NULL, user_data, event->button, event->time);
			}
			break;
	}
	return FALSE;
}

static void gmlc_gui_server_tab_data_evt_lst_tbl_selected (GtkTreeSelection *selection, gpointer user_data) {
	GmlcGuiServerTabData * pGmlcGuiSrvTabData = GMLC_GUI_SERVER_TAB_DATA(user_data);
	GtkTreeIter iter;
	GtkTreeModel *model;
	
	if (gtk_tree_selection_get_selected (selection, &model, &iter)) {
		gtk_tree_model_get (model, &iter, 4, &pGmlcGuiSrvTabData->pGmlcMysqlTbl, -1);
	}
}

static void gmlc_gui_server_tab_data_evt_menu_tbl_display_activate (GtkWidget *widget, gpointer user_data) {
	GmlcGuiServerTabData * pGmlcGuiSrvTabData = GMLC_GUI_SERVER_TAB_DATA(user_data);
	gchar * pcQuery = NULL, * pcDbName = NULL, * pcTblName = NULL;
	UNUSED_VAR(widget);
	
	if (pGmlcGuiSrvTabData->pGmlcMysqlTbl != NULL) {
		g_object_get(G_OBJECT(pGmlcGuiSrvTabData->pGmlcMysqlDb), "db_name", &pcDbName, NULL);
		g_object_get(G_OBJECT(pGmlcGuiSrvTabData->pGmlcMysqlTbl), "name", &pcTblName, NULL);
		
		pcQuery = g_strdup_printf("SELECT * \nFROM `%s`.`%s`\nWHERE 1\nLIMIT 1000", pcDbName, pcTblName);
		gmlc_gui_server_tab_data_open_query(pGmlcGuiSrvTabData, pcQuery, FALSE);
	
		g_free(pcQuery);
		g_free(pcTblName);
		g_free(pcDbName);
	}
}

static void gmlc_gui_server_tab_data_evt_menu_tbl_add_activate (GtkWidget *widget, gpointer user_data) {
	GmlcGuiServerTabData * pGmlcGuiSrvTabData = GMLC_GUI_SERVER_TAB_DATA(user_data);
	gchar * pcQuery = NULL;
	UNUSED_VAR(widget);
	
	if (pGmlcGuiSrvTabData->pGmlcMysqlTbl != NULL) {
		pcQuery = gmlc_mysql_structure_get_create(GMLC_MYSQL_STRUCTURE(pGmlcGuiSrvTabData->pGmlcMysqlTbl), FALSE, NULL);
	} else {
		pcQuery = gmlc_mysql_database_create_new_table_sql(pGmlcGuiSrvTabData->pGmlcMysqlDb, NULL);
	}
	gmlc_gui_server_tab_data_open_query(pGmlcGuiSrvTabData, pcQuery, FALSE);
	
	g_free(pcQuery);
}

static void gmlc_gui_server_tab_data_evt_menu_tbl_alter_activate (GtkWidget *widget, gpointer user_data) {
	GmlcGuiServerTabData * pGmlcGuiSrvTabData = GMLC_GUI_SERVER_TAB_DATA(user_data);
	gchar * pcQuery = NULL;
	UNUSED_VAR(widget);
	
	if (pGmlcGuiSrvTabData->pGmlcMysqlTbl != NULL) {
		pcQuery = gmlc_mysql_structure_get_alter(GMLC_MYSQL_STRUCTURE(pGmlcGuiSrvTabData->pGmlcMysqlTbl), TRUE, NULL);
		
		gmlc_gui_server_tab_data_open_query(pGmlcGuiSrvTabData, pcQuery, FALSE);
		
		g_free(pcQuery);
	}
}

static void gmlc_gui_server_tab_data_evt_menu_tbl_remove_activate (GtkWidget *widget, gpointer user_data) {
	GmlcGuiServerTabData * pGmlcGuiSrvTabData = GMLC_GUI_SERVER_TAB_DATA(user_data);
	gchar * pcQuery = NULL;
	UNUSED_VAR(widget);
	
	if (pGmlcGuiSrvTabData->pGmlcMysqlTbl != NULL) {
		pcQuery = gmlc_mysql_structure_get_drop(GMLC_MYSQL_STRUCTURE(pGmlcGuiSrvTabData->pGmlcMysqlTbl), TRUE, NULL);
		
		gmlc_gui_server_tab_data_open_query(pGmlcGuiSrvTabData, pcQuery, FALSE);
		
		g_free(pcQuery);
	}
}

static void gmlc_gui_server_tab_data_evt_menu_tbl_refresh_activate (GtkWidget *widget, gpointer user_data) {
	GmlcGuiServerTabData * pGmlcGuiSrvTabData = GMLC_GUI_SERVER_TAB_DATA(user_data);
	UNUSED_VAR(widget);
	
	if (pGmlcGuiSrvTabData->pGmlcMysqlDb != NULL) {
		gmlc_gui_server_tab_data_fill_tables_list(pGmlcGuiSrvTabData);
	}
}

static void gmlc_gui_server_tab_data_evt_menu_tbl_show_create_activate (GtkWidget *widget, gpointer user_data) {
	GmlcGuiServerTabData * pGmlcGuiSrvTabData = GMLC_GUI_SERVER_TAB_DATA(user_data);
	gchar * pcQuery = NULL;
	UNUSED_VAR(widget);
	
	if (pGmlcGuiSrvTabData->pGmlcMysqlTbl != NULL) {
		pcQuery = gmlc_mysql_structure_get_create(GMLC_MYSQL_STRUCTURE(pGmlcGuiSrvTabData->pGmlcMysqlTbl), TRUE, NULL);
		
		gmlc_gui_server_tab_data_open_query(pGmlcGuiSrvTabData, pcQuery, FALSE);
		
		g_free(pcQuery);
	}
}



static gboolean gmlc_gui_server_tab_data_evt_lst_vw_btnpress (GtkWidget *widget, GdkEventButton *event, gpointer user_data) {
	GmlcGuiServerTabData * pGmlcGuiSrvTabData = GMLC_GUI_SERVER_TAB_DATA(user_data);
	
	switch (event->button) {
		case 1 : /* Left button */
			if (event->type == GDK_2BUTTON_PRESS) {
				gmlc_gui_server_tab_data_evt_menu_vw_display_activate(widget, user_data);
			}
			break;
		case 3 : /* Right button */
			if (event->type == GDK_BUTTON_PRESS) {
				gtk_menu_popup(GTK_MENU(pGmlcGuiSrvTabData->mnuVwOps), NULL, NULL, NULL, user_data, event->button, event->time);
			}
			break;
	}
	return FALSE;
}

static void gmlc_gui_server_tab_data_evt_lst_vw_selected (GtkTreeSelection *selection, gpointer user_data) {
	GmlcGuiServerTabData * pGmlcGuiSrvTabData = GMLC_GUI_SERVER_TAB_DATA(user_data);
	GtkTreeIter iter;
	GtkTreeModel *model;
	
	if (gtk_tree_selection_get_selected (selection, &model, &iter)) {
		gtk_tree_model_get (model, &iter, 1, &pGmlcGuiSrvTabData->pGmlcMysqlVw, -1);
	}
}

static void gmlc_gui_server_tab_data_evt_menu_vw_display_activate (GtkWidget *widget, gpointer user_data) {
	GmlcGuiServerTabData * pGmlcGuiSrvTabData = GMLC_GUI_SERVER_TAB_DATA(user_data);
	gchar * pcQuery = NULL, * pcDbName = NULL, * pcVwName = NULL;
	UNUSED_VAR(widget);
	
	if (pGmlcGuiSrvTabData->pGmlcMysqlVw != NULL) {
		g_object_get(G_OBJECT(pGmlcGuiSrvTabData->pGmlcMysqlDb), "db_name", &pcDbName, NULL);
		g_object_get(G_OBJECT(pGmlcGuiSrvTabData->pGmlcMysqlVw), "name", &pcVwName, NULL);
		
		pcQuery = g_strdup_printf("SELECT * \nFROM `%s`.`%s`\nWHERE 1\nLIMIT 1000", pcDbName, pcVwName);
		gmlc_gui_server_tab_data_open_query(pGmlcGuiSrvTabData, pcQuery, FALSE);
	
		g_free(pcQuery);
		g_free(pcVwName);
		g_free(pcDbName);
	}
}

static void gmlc_gui_server_tab_data_evt_menu_vw_add_activate (GtkWidget *widget, gpointer user_data) {
	GmlcGuiServerTabData * pGmlcGuiSrvTabData = GMLC_GUI_SERVER_TAB_DATA(user_data);
	gchar * pcQuery = NULL;
	UNUSED_VAR(widget);
	
	if (pGmlcGuiSrvTabData->pGmlcMysqlVw != NULL) {
		pcQuery = gmlc_mysql_structure_get_create(GMLC_MYSQL_STRUCTURE(pGmlcGuiSrvTabData->pGmlcMysqlVw), FALSE, NULL);
	} else {
		pcQuery = gmlc_mysql_database_create_new_view_sql(pGmlcGuiSrvTabData->pGmlcMysqlDb, NULL);
	}
	gmlc_gui_server_tab_data_open_query(pGmlcGuiSrvTabData, pcQuery, FALSE);
	
	g_free(pcQuery);
}

static void gmlc_gui_server_tab_data_evt_menu_vw_alter_activate (GtkWidget *widget, gpointer user_data) {
	GmlcGuiServerTabData * pGmlcGuiSrvTabData = GMLC_GUI_SERVER_TAB_DATA(user_data);
	gchar * pcQuery = NULL;
	UNUSED_VAR(widget);
	
	if (pGmlcGuiSrvTabData->pGmlcMysqlVw != NULL) {
		pcQuery = gmlc_mysql_structure_get_alter(GMLC_MYSQL_STRUCTURE(pGmlcGuiSrvTabData->pGmlcMysqlVw), TRUE, NULL);
		
		gmlc_gui_server_tab_data_open_query(pGmlcGuiSrvTabData, pcQuery, FALSE);
		
		g_free(pcQuery);
	}
	
}

static void gmlc_gui_server_tab_data_evt_menu_vw_remove_activate (GtkWidget *widget, gpointer user_data) {
	GmlcGuiServerTabData * pGmlcGuiSrvTabData = GMLC_GUI_SERVER_TAB_DATA(user_data);
	gchar * pcQuery = NULL;
	UNUSED_VAR(widget);
	
	if (pGmlcGuiSrvTabData->pGmlcMysqlVw != NULL) {
		pcQuery = gmlc_mysql_structure_get_drop(GMLC_MYSQL_STRUCTURE(pGmlcGuiSrvTabData->pGmlcMysqlVw), TRUE, NULL);
		
		gmlc_gui_server_tab_data_open_query(pGmlcGuiSrvTabData, pcQuery, FALSE);
		
		g_free(pcQuery);
	}
	
}

static void gmlc_gui_server_tab_data_evt_menu_vw_refresh_activate (GtkWidget *widget, gpointer user_data) {
	GmlcGuiServerTabData * pGmlcGuiSrvTabData = GMLC_GUI_SERVER_TAB_DATA(user_data);
	UNUSED_VAR(widget);
	
	if (pGmlcGuiSrvTabData->pGmlcMysqlDb != NULL) {
		gmlc_gui_server_tab_data_fill_views_list(pGmlcGuiSrvTabData);
	}
}

static void gmlc_gui_server_tab_data_evt_menu_vw_show_create_activate (GtkWidget *widget, gpointer user_data) {
	GmlcGuiServerTabData * pGmlcGuiSrvTabData = GMLC_GUI_SERVER_TAB_DATA(user_data);
	gchar * pcQuery = NULL;
	UNUSED_VAR(widget);
	
	if (pGmlcGuiSrvTabData->pGmlcMysqlVw != NULL) {
		pcQuery = gmlc_mysql_structure_get_create(GMLC_MYSQL_STRUCTURE(pGmlcGuiSrvTabData->pGmlcMysqlVw), TRUE, NULL);
		
		gmlc_gui_server_tab_data_open_query(pGmlcGuiSrvTabData, pcQuery, FALSE);
		
		g_free(pcQuery);
	}
}



static gboolean gmlc_gui_server_tab_data_evt_lst_proc_btnpress (GtkWidget *widget, GdkEventButton *event, gpointer user_data) {
	GmlcGuiServerTabData * pGmlcGuiSrvTabData = GMLC_GUI_SERVER_TAB_DATA(user_data);
	
	switch (event->button) {
		case 1 : /* Left button */
			if (event->type == GDK_2BUTTON_PRESS) {
				/*gmlc_gui_server_tab_data_open_query_select (pGmlcGuiSrvTabData, TRUE);*/
				gmlc_gui_server_tab_data_evt_menu_proc_display_activate (widget, user_data);
			}
			break;
		case 3 : /* Right button */
			if (event->type == GDK_BUTTON_PRESS) {
				gtk_menu_popup(GTK_MENU(pGmlcGuiSrvTabData->mnuProcOps), NULL, NULL, NULL, user_data, event->button, event->time);
			}
			break;
	}
	return FALSE;
}

static void gmlc_gui_server_tab_data_evt_lst_proc_selected (GtkTreeSelection *selection, gpointer user_data) {
	GmlcGuiServerTabData * pGmlcGuiSrvTabData = GMLC_GUI_SERVER_TAB_DATA(user_data);
	GtkTreeIter iter;
	GtkTreeModel *model;
	
	if (gtk_tree_selection_get_selected (selection, &model, &iter)) {
		gtk_tree_model_get (model, &iter, 1, &pGmlcGuiSrvTabData->pGmlcMysqlProc, -1);
	}
}

static void gmlc_gui_server_tab_data_evt_menu_proc_display_activate (GtkWidget *widget, gpointer user_data) {
	GmlcGuiServerTabData * pGmlcGuiSrvTabData = GMLC_GUI_SERVER_TAB_DATA(user_data);
	gchar * pcQuery = NULL, * pcDbName = NULL, * pcProcName = NULL;
	UNUSED_VAR(widget);
	
	if (pGmlcGuiSrvTabData->pGmlcMysqlProc != NULL) {
		g_object_get(G_OBJECT(pGmlcGuiSrvTabData->pGmlcMysqlDb), "db_name", &pcDbName, NULL);
		g_object_get(G_OBJECT(pGmlcGuiSrvTabData->pGmlcMysqlProc), "name", &pcProcName, NULL);
		
		pcQuery = g_strdup_printf("CALL `%s`.`%s`();\n", pcDbName, pcProcName);
		gmlc_gui_server_tab_data_open_query(pGmlcGuiSrvTabData, pcQuery, FALSE);
	
		g_free(pcQuery);
		g_free(pcProcName);
		g_free(pcDbName);
	}
	
}

static void gmlc_gui_server_tab_data_evt_menu_proc_add_activate (GtkWidget *widget, gpointer user_data) {
	GmlcGuiServerTabData * pGmlcGuiSrvTabData = GMLC_GUI_SERVER_TAB_DATA(user_data);
	gchar * pcQuery = NULL;
	UNUSED_VAR(widget);
	
	if (pGmlcGuiSrvTabData->pGmlcMysqlProc != NULL) {
		pcQuery = gmlc_mysql_structure_get_create(GMLC_MYSQL_STRUCTURE(pGmlcGuiSrvTabData->pGmlcMysqlProc), FALSE, NULL);
	} else {
		pcQuery = gmlc_mysql_database_create_new_procedure_sql(pGmlcGuiSrvTabData->pGmlcMysqlDb, NULL);
	}
	gmlc_gui_server_tab_data_open_query(pGmlcGuiSrvTabData, pcQuery, FALSE);
	
	g_free(pcQuery);
	
}

static void gmlc_gui_server_tab_data_evt_menu_proc_alter_activate (GtkWidget *widget, gpointer user_data) {
	GmlcGuiServerTabData * pGmlcGuiSrvTabData = GMLC_GUI_SERVER_TAB_DATA(user_data);
	gchar * pcQuery = NULL;
	UNUSED_VAR(widget);
	
	if (pGmlcGuiSrvTabData->pGmlcMysqlProc != NULL) {
		pcQuery = gmlc_mysql_structure_get_alter(GMLC_MYSQL_STRUCTURE(pGmlcGuiSrvTabData->pGmlcMysqlProc), TRUE, NULL);
		
		gmlc_gui_server_tab_data_open_query(pGmlcGuiSrvTabData, pcQuery, FALSE);
		
		g_free(pcQuery);
	}
	
}

static void gmlc_gui_server_tab_data_evt_menu_proc_remove_activate (GtkWidget *widget, gpointer user_data) {
	GmlcGuiServerTabData * pGmlcGuiSrvTabData = GMLC_GUI_SERVER_TAB_DATA(user_data);
	gchar * pcQuery = NULL;
	UNUSED_VAR(widget);
	
	if (pGmlcGuiSrvTabData->pGmlcMysqlProc != NULL) {
		pcQuery = gmlc_mysql_structure_get_drop(GMLC_MYSQL_STRUCTURE(pGmlcGuiSrvTabData->pGmlcMysqlProc), TRUE, NULL);
		
		gmlc_gui_server_tab_data_open_query(pGmlcGuiSrvTabData, pcQuery, FALSE);
		
		g_free(pcQuery);
	}
	
}

static void gmlc_gui_server_tab_data_evt_menu_proc_refresh_activate (GtkWidget *widget, gpointer user_data) {
	GmlcGuiServerTabData * pGmlcGuiSrvTabData = GMLC_GUI_SERVER_TAB_DATA(user_data);
	UNUSED_VAR(widget);
	
	if (pGmlcGuiSrvTabData->pGmlcMysqlDb != NULL) {
		gmlc_gui_server_tab_data_fill_procedures_list(pGmlcGuiSrvTabData);
	}
}

static void gmlc_gui_server_tab_data_evt_menu_proc_show_create_activate (GtkWidget *widget, gpointer user_data) {
	GmlcGuiServerTabData * pGmlcGuiSrvTabData = GMLC_GUI_SERVER_TAB_DATA(user_data);
	gchar * pcQuery = NULL;
	UNUSED_VAR(widget);
	
	if (pGmlcGuiSrvTabData->pGmlcMysqlProc != NULL) {
		pcQuery = gmlc_mysql_structure_get_create(GMLC_MYSQL_STRUCTURE(pGmlcGuiSrvTabData->pGmlcMysqlProc), TRUE, NULL);
		
		gmlc_gui_server_tab_data_open_query(pGmlcGuiSrvTabData, pcQuery, FALSE);
		
		g_free(pcQuery);
	}
}



static gboolean gmlc_gui_server_tab_data_evt_lst_func_btnpress (GtkWidget *widget, GdkEventButton *event, gpointer user_data) {
	GmlcGuiServerTabData * pGmlcGuiSrvTabData = GMLC_GUI_SERVER_TAB_DATA(user_data);
	
	switch (event->button) {
		case 1 : /* Left button */
			if (event->type == GDK_2BUTTON_PRESS) {
				/*gmlc_gui_server_tab_data_open_query_select (pGmlcGuiSrvTabData, TRUE);*/
			}
			break;
		case 3 : /* Right button */
			if (event->type == GDK_BUTTON_PRESS) {
				gtk_menu_popup(GTK_MENU(pGmlcGuiSrvTabData->mnuFuncOps), NULL, NULL, NULL, user_data, event->button, event->time);
			}
			break;
	}
	return FALSE;
}

static void gmlc_gui_server_tab_data_evt_lst_func_selected (GtkTreeSelection *selection, gpointer user_data) {
	GmlcGuiServerTabData * pGmlcGuiSrvTabData = GMLC_GUI_SERVER_TAB_DATA(user_data);
	GtkTreeIter iter;
	GtkTreeModel *model;
	
	if (gtk_tree_selection_get_selected (selection, &model, &iter)) {
		gtk_tree_model_get (model, &iter, 1, &pGmlcGuiSrvTabData->pGmlcMysqlFunc, -1);
	}
}

static void gmlc_gui_server_tab_data_evt_menu_func_display_activate (GtkWidget *widget, gpointer user_data) {
	GmlcGuiServerTabData * pGmlcGuiSrvTabData = GMLC_GUI_SERVER_TAB_DATA(user_data);
	gchar * pcQuery = NULL, * pcDbName = NULL, * pcFuncName = NULL;
	UNUSED_VAR(widget);
	
	if (pGmlcGuiSrvTabData->pGmlcMysqlProc != NULL) {
		g_object_get(G_OBJECT(pGmlcGuiSrvTabData->pGmlcMysqlDb), "db_name", &pcDbName, NULL);
		g_object_get(G_OBJECT(pGmlcGuiSrvTabData->pGmlcMysqlFunc), "name", &pcFuncName, NULL);
		
		pcQuery = g_strdup_printf("SELECT `%s`();\n", pcFuncName);
		gmlc_gui_server_tab_data_open_query(pGmlcGuiSrvTabData, pcQuery, FALSE);
	
		g_free(pcQuery);
		g_free(pcFuncName);
		g_free(pcDbName);
	}
	
}

static void gmlc_gui_server_tab_data_evt_menu_func_add_activate (GtkWidget *widget, gpointer user_data) {
	GmlcGuiServerTabData * pGmlcGuiSrvTabData = GMLC_GUI_SERVER_TAB_DATA(user_data);
	gchar * pcQuery = NULL;
	UNUSED_VAR(widget);
	
	if (pGmlcGuiSrvTabData->pGmlcMysqlFunc != NULL) {
		pcQuery = gmlc_mysql_structure_get_create(GMLC_MYSQL_STRUCTURE(pGmlcGuiSrvTabData->pGmlcMysqlFunc), FALSE, NULL);
	} else {
		pcQuery = gmlc_mysql_database_create_new_function_sql(pGmlcGuiSrvTabData->pGmlcMysqlDb, NULL);
	}
	gmlc_gui_server_tab_data_open_query(pGmlcGuiSrvTabData, pcQuery, FALSE);
	
	g_free(pcQuery);
}

static void gmlc_gui_server_tab_data_evt_menu_func_alter_activate (GtkWidget *widget, gpointer user_data) {
	GmlcGuiServerTabData * pGmlcGuiSrvTabData = GMLC_GUI_SERVER_TAB_DATA(user_data);
	gchar * pcQuery = NULL;
	UNUSED_VAR(widget);
	
	if (pGmlcGuiSrvTabData->pGmlcMysqlFunc != NULL) {
		pcQuery = gmlc_mysql_structure_get_alter(GMLC_MYSQL_STRUCTURE(pGmlcGuiSrvTabData->pGmlcMysqlFunc), TRUE, NULL);
		
		gmlc_gui_server_tab_data_open_query(pGmlcGuiSrvTabData, pcQuery, FALSE);
		
		g_free(pcQuery);
	}
}

static void gmlc_gui_server_tab_data_evt_menu_func_remove_activate (GtkWidget *widget, gpointer user_data) {
	GmlcGuiServerTabData * pGmlcGuiSrvTabData = GMLC_GUI_SERVER_TAB_DATA(user_data);
	gchar * pcQuery = NULL;
	UNUSED_VAR(widget);
	
	if (pGmlcGuiSrvTabData->pGmlcMysqlFunc != NULL) {
		pcQuery = gmlc_mysql_structure_get_drop(GMLC_MYSQL_STRUCTURE(pGmlcGuiSrvTabData->pGmlcMysqlFunc), TRUE, NULL);
		
		gmlc_gui_server_tab_data_open_query(pGmlcGuiSrvTabData, pcQuery, FALSE);
		
		g_free(pcQuery);
	}
}

static void gmlc_gui_server_tab_data_evt_menu_func_refresh_activate (GtkWidget *widget, gpointer user_data) {
	GmlcGuiServerTabData * pGmlcGuiSrvTabData = GMLC_GUI_SERVER_TAB_DATA(user_data);
	UNUSED_VAR(widget);
	
	if (pGmlcGuiSrvTabData->pGmlcMysqlDb != NULL) {
		gmlc_gui_server_tab_data_fill_functions_list(pGmlcGuiSrvTabData);
	}
}

static void gmlc_gui_server_tab_data_evt_menu_func_show_create_activate (GtkWidget *widget, gpointer user_data) {
	GmlcGuiServerTabData * pGmlcGuiSrvTabData = GMLC_GUI_SERVER_TAB_DATA(user_data);
	gchar * pcQuery = NULL;
	UNUSED_VAR(widget);
	
	if (pGmlcGuiSrvTabData->pGmlcMysqlFunc != NULL) {
		pcQuery = gmlc_mysql_structure_get_create(GMLC_MYSQL_STRUCTURE(pGmlcGuiSrvTabData->pGmlcMysqlFunc), TRUE, NULL);
		
		gmlc_gui_server_tab_data_open_query(pGmlcGuiSrvTabData, pcQuery, FALSE);
		
		g_free(pcQuery);
	}
}



