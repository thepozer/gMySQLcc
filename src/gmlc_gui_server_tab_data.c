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

#include "gmlc_gui_server_tab_data.h"
#include "gmlc_gui_server_tab.h"
#include "gmlc_gui_query.h"
#include "gmlc_mysql_query.h"

static void gmlc_gui_server_tab_data_finalize (GmlcGuiServerTabData * pGmlcGuiSrvTabData);
static void gmlc_gui_server_tab_data_get_property (GObject * object, guint prop_id, GValue * value, GParamSpec * pspec);
static void gmlc_gui_server_tab_data_set_property (GObject * object, guint prop_id, const GValue * value, GParamSpec * pspec);

static void gmlc_gui_server_tab_data_interface_init (gpointer g_iface, gpointer iface_data);
static void gmlc_gui_server_tab_data_update_ui (GmlcGuiServerTabData * pGmlcGuiSrvTabData);

void gmlc_gui_server_tab_data_create_toolbar_items (GmlcGuiServerTabData * pGmlcGuiSrvTabData);
void gmlc_gui_server_tab_data_create_widgets (GmlcGuiServerTabData * pGmlcGuiSrvTabData);
void gmlc_gui_server_tab_data_init_widgets (GmlcGuiServerTabData * pGmlcGuiSrvTabData);

void gmlc_gui_server_tab_data_open_query_window (GmlcGuiServerTabData * pGmlcGuiSrvTabData, gboolean bUseTable);

void gmlc_gui_server_tab_data_fill_database_list (GmlcGuiServerTabData * pGmlcGuiSrvTabData);
void gmlc_gui_server_tab_data_fill_tables_list (GmlcGuiServerTabData * pGmlcGuiSrvTabData);
void gmlc_gui_server_tab_data_fill_views_list (GmlcGuiServerTabData * pGmlcGuiSrvTabData);
void gmlc_gui_server_tab_data_fill_procedures_list (GmlcGuiServerTabData * pGmlcGuiSrvTabData);
void gmlc_gui_server_tab_data_fill_functions_list (GmlcGuiServerTabData * pGmlcGuiSrvTabData);

static gboolean gmlc_gui_server_tab_data_evt_lstBase_btnpress (GtkWidget *widget, GdkEventButton *event, gpointer user_data);
static gboolean gmlc_gui_server_tab_data_evt_lstTables_btnpress (GtkWidget *widget, GdkEventButton *event, gpointer user_data);
static void gmlc_gui_server_tab_data_evt_lstBase_selected (GtkTreeSelection *selection, gpointer user_data);
static void gmlc_gui_server_tab_data_evt_lstTables_selected (GtkTreeSelection *selection, gpointer user_data);
static void gmlc_gui_server_tab_data_evt_btnTlbrSqlFile_clicked (GtkWidget *widget, gpointer user_data);
static void gmlc_gui_server_tab_data_evt_btnTlbrSql_clicked (GtkWidget *widget, gpointer user_data);

static void gmlc_gui_server_tab_data_evt_btnDbAdd_clicked (GtkWidget *widget, gpointer user_data);
static void gmlc_gui_server_tab_data_evt_btnDbDel_clicked (GtkWidget *widget, gpointer user_data);
static void gmlc_gui_server_tab_data_evt_btnTblAdd_clicked (GtkWidget *widget, gpointer user_data);
static void gmlc_gui_server_tab_data_evt_btnTblEdit_clicked (GtkWidget *widget, gpointer user_data);
static void gmlc_gui_server_tab_data_evt_btnTblDump_clicked (GtkWidget *widget, gpointer user_data);
static void gmlc_gui_server_tab_data_evt_btnTblDel_clicked (GtkWidget *widget, gpointer user_data);

enum {
	PROP_0,
	PROP_SERVER,
	PROP_SERVER_WINDOW,
};

G_DEFINE_TYPE_WITH_CODE (GmlcGuiServerTabData, gmlc_gui_server_tab_data, GTK_TYPE_VBOX, 
	G_IMPLEMENT_INTERFACE (GMLC_GUI_TYPE_SERVER_TAB, gmlc_gui_server_tab_data_interface_init));

static void gmlc_gui_server_tab_data_interface_init (gpointer g_iface, gpointer iface_data) {
  GmlcGuiServerTabInterface *pIface = (GmlcGuiServerTabInterface *)g_iface;
  pIface->update_ui = (void (*) (GmlcGuiServerTab * self))gmlc_gui_server_tab_data_update_ui;
}

static void gmlc_gui_server_tab_data_class_init (GmlcGuiServerTabDataClass * pClass) {
	GObjectClass *pObjClass = G_OBJECT_CLASS(pClass);
	
	pObjClass->finalize = (GObjectFinalizeFunc) gmlc_gui_server_tab_data_finalize;
	
	pObjClass->get_property = gmlc_gui_server_tab_data_get_property;
	pObjClass->set_property = gmlc_gui_server_tab_data_set_property;
	
	g_object_class_install_property(pObjClass, PROP_SERVER, 
		g_param_spec_object("server", "Server object", "Server object", GMLC_TYPE_MYSQL_SERVER, G_PARAM_READABLE));
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

static void gmlc_gui_server_tab_data_update_ui (GmlcGuiServerTabData * pGmlcGuiSrvTabData) {
	gtk_widget_show(GTK_WIDGET(pGmlcGuiSrvTabData->btnTlbrSql));
	gtk_widget_show(GTK_WIDGET(pGmlcGuiSrvTabData->btnTlbrSqlFile));
}

GmlcGuiServerTabData * gmlc_gui_server_tab_data_new (GmlcGuiServer * pGmlcGuiSrv) {
	GmlcGuiServerTabData * pGmlcGuiSrvTabData = NULL;
	
	pGmlcGuiSrvTabData = GMLC_GUI_SERVER_TAB_DATA(g_object_new(GMLC_GUI_TYPE_SERVER_TAB_DATA, "server_window", pGmlcGuiSrv, NULL));
	
	return pGmlcGuiSrvTabData;
}

void gmlc_gui_server_tab_data_create_toolbar_items (GmlcGuiServerTabData * pGmlcGuiSrvTabData) {
	GtkWidget * imgToolbar = NULL;
	GtkToolbar * tlbMainToolbar = NULL;
	
	g_object_get(pGmlcGuiSrvTabData->pGmlcGuiSrv, "toolbar", &tlbMainToolbar, NULL);
	
	imgToolbar = gtk_image_new_from_stock(GTK_STOCK_EXECUTE, GTK_ICON_SIZE_LARGE_TOOLBAR);
	gtk_widget_show(imgToolbar);
	pGmlcGuiSrvTabData->btnTlbrSqlFile = gtk_tool_button_new (imgToolbar, _("SQL File"));
	gtk_tool_item_set_is_important (GTK_TOOL_ITEM(pGmlcGuiSrvTabData->btnTlbrSqlFile), TRUE);
	g_signal_connect (G_OBJECT (pGmlcGuiSrvTabData->btnTlbrSqlFile), "clicked", 
			G_CALLBACK (gmlc_gui_server_tab_data_evt_btnTlbrSqlFile_clicked), pGmlcGuiSrvTabData);
	gtk_widget_show(GTK_WIDGET(pGmlcGuiSrvTabData->btnTlbrSqlFile));
	gtk_toolbar_insert(GTK_TOOLBAR(tlbMainToolbar), GTK_TOOL_ITEM(pGmlcGuiSrvTabData->btnTlbrSqlFile), 0);
	/*gtk_tool_item_set_tooltip (GTK_TOOL_ITEM(pGmlcGuiSrvTabData->btnTlbrSqlFile), tooltips, _("Exec SQL File"), NULL);*/
	
	imgToolbar = gtk_image_new_from_stock(GTK_STOCK_EXECUTE, GTK_ICON_SIZE_LARGE_TOOLBAR);
	gtk_widget_show(imgToolbar);
	pGmlcGuiSrvTabData->btnTlbrSql = gtk_tool_button_new (imgToolbar, _("SQL"));
	gtk_tool_item_set_is_important (GTK_TOOL_ITEM(pGmlcGuiSrvTabData->btnTlbrSql), TRUE);
	g_signal_connect (G_OBJECT (pGmlcGuiSrvTabData->btnTlbrSql), "clicked", 
			G_CALLBACK (gmlc_gui_server_tab_data_evt_btnTlbrSql_clicked), pGmlcGuiSrvTabData);
	gtk_widget_show(GTK_WIDGET(pGmlcGuiSrvTabData->btnTlbrSql));
	gtk_toolbar_insert(GTK_TOOLBAR(tlbMainToolbar), GTK_TOOL_ITEM(pGmlcGuiSrvTabData->btnTlbrSql), 0);
	/*gtk_tool_item_set_tooltip (GTK_TOOL_ITEM(pGmlcGuiSrvTabData->btnTlbrSql), tooltips, _("Exec SQL Query"), NULL);*/
}

void gmlc_gui_server_tab_data_create_widgets (GmlcGuiServerTabData * pGmlcGuiSrvTabData) {
	GtkWidget *vbox, *hpaned, *hbuttonbox, *nbkDatas;
	GtkWidget *label;
	GtkWidget *scrolledwindow;
	GtkWidget *btnDbAdd, *btnDbDel;
	GtkWidget *btnTblAdd, *btnTblEdit, *btnTblDump, *btnTblDel;
	GtkWidget *btnAdd, *btnEdit, *btnDump, *btnDel;
	GtkWidget *mnuDBOpsRefresh, *mnuDBOpsShowCreate, *mnuTBLOpsShowCreate;
	GtkTreeSelection *select;
	glong lServerVersion = 0;
	
	g_object_get(G_OBJECT(pGmlcGuiSrvTabData->pGmlcMysqlSrv), "version", &lServerVersion, NULL);
	
	hpaned = gtk_hpaned_new ();
	gtk_widget_show (hpaned);
	gtk_paned_set_position (GTK_PANED (hpaned), 200);
	/*gtk_container_add(GTK_CONTAINER(pGmlcGuiSrvTabData), hpaned);*/
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
	g_signal_connect (G_OBJECT (pGmlcGuiSrvTabData->lstBase), "button-press-event", 
										G_CALLBACK (gmlc_gui_server_tab_data_evt_lstBase_btnpress), pGmlcGuiSrvTabData);
	select = gtk_tree_view_get_selection (GTK_TREE_VIEW (pGmlcGuiSrvTabData->lstBase));
	gtk_tree_selection_set_mode (select, GTK_SELECTION_SINGLE);
	g_signal_connect (G_OBJECT (select), "changed", 
										G_CALLBACK (gmlc_gui_server_tab_data_evt_lstBase_selected), pGmlcGuiSrvTabData);

	hbuttonbox = gtk_hbutton_box_new ();
	gtk_widget_show (hbuttonbox);
	gtk_box_pack_start (GTK_BOX (vbox), hbuttonbox, FALSE, TRUE, 0);
	gtk_button_box_set_layout (GTK_BUTTON_BOX (hbuttonbox), GTK_BUTTONBOX_SPREAD);
	gtk_box_set_spacing (GTK_BOX (hbuttonbox), 2);

	btnDbAdd = gtk_button_new_from_stock(GTK_STOCK_ADD);
	gtk_widget_show (btnDbAdd);
	gtk_box_pack_start (GTK_BOX (hbuttonbox), btnDbAdd, TRUE, TRUE, 0);
	g_signal_connect (G_OBJECT (btnDbAdd), "clicked", 
										G_CALLBACK (gmlc_gui_server_tab_data_evt_btnDbAdd_clicked), pGmlcGuiSrvTabData);

	btnDbDel = gtk_button_new_from_stock(GTK_STOCK_DELETE);
	gtk_widget_show (btnDbDel);
	gtk_box_pack_start (GTK_BOX (hbuttonbox), btnDbDel, TRUE, TRUE, 0);
	g_signal_connect (G_OBJECT (btnDbDel), "clicked", 
										G_CALLBACK (gmlc_gui_server_tab_data_evt_btnDbDel_clicked), pGmlcGuiSrvTabData);
	
	pGmlcGuiSrvTabData->mnuBdOps = gtk_menu_new();
	gtk_widget_show (pGmlcGuiSrvTabData->mnuBdOps);

	mnuDBOpsRefresh = gtk_menu_item_new_with_label(_("Refresh"));
	gtk_widget_show (mnuDBOpsRefresh);
	gtk_menu_append (GTK_MENU_SHELL(pGmlcGuiSrvTabData->mnuBdOps), mnuDBOpsRefresh);
	/*g_signal_connect (G_OBJECT (mnuDBOpsRefresh), "activate", 
										G_CALLBACK (gmysqlcc_gui_server_evt_mnuDBOpsRefresh_activate), pGmlcGuiSrvTabData);*/

	mnuDBOpsShowCreate = gtk_menu_item_new_with_label(_("Display all create structures"));
	gtk_widget_show (mnuDBOpsShowCreate);
	gtk_menu_append (GTK_MENU_SHELL(pGmlcGuiSrvTabData->mnuBdOps), mnuDBOpsShowCreate);
	/*g_signal_connect (G_OBJECT (mnuDBOpsShowCreate), "activate", 
										G_CALLBACK (gmysqlcc_gui_server_evt_mnuDBOpsShowCreate_activate), pGmlcGuiSrvTabData);*/

	
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
	g_signal_connect (G_OBJECT (pGmlcGuiSrvTabData->lstTables), "button-press-event", 
										G_CALLBACK (gmlc_gui_server_tab_data_evt_lstTables_btnpress), pGmlcGuiSrvTabData);
	select = gtk_tree_view_get_selection (GTK_TREE_VIEW (pGmlcGuiSrvTabData->lstTables));
	gtk_tree_selection_set_mode (select, GTK_SELECTION_SINGLE);
	g_signal_connect (G_OBJECT (select), "changed", 
										G_CALLBACK (gmlc_gui_server_tab_data_evt_lstTables_selected), pGmlcGuiSrvTabData);

	hbuttonbox = gtk_hbutton_box_new ();
	gtk_widget_show (hbuttonbox);
	gtk_box_pack_start (GTK_BOX (vbox), hbuttonbox, FALSE, TRUE, 0);
	gtk_button_box_set_layout (GTK_BUTTON_BOX (hbuttonbox), GTK_BUTTONBOX_SPREAD);
	gtk_box_set_spacing (GTK_BOX (hbuttonbox), 2);
	
	btnTblAdd = gtk_button_new_from_stock(GTK_STOCK_ADD);
	gtk_widget_show (btnTblAdd);
	gtk_box_pack_start (GTK_BOX (hbuttonbox), btnTblAdd, TRUE, TRUE, 0);
	g_signal_connect (G_OBJECT (btnTblAdd), "clicked", 
										G_CALLBACK (gmlc_gui_server_tab_data_evt_btnTblAdd_clicked), pGmlcGuiSrvTabData);

	/*btnTblEdit = createIconButton("gtk-apply", _("Modify"));*/
	btnTblEdit = gtk_button_new_from_stock(GTK_STOCK_EDIT);
	gtk_widget_show (btnTblEdit);
	gtk_box_pack_start (GTK_BOX (hbuttonbox), btnTblEdit, TRUE, TRUE, 0);
	g_signal_connect (G_OBJECT (btnTblEdit), "clicked", 
										G_CALLBACK (gmlc_gui_server_tab_data_evt_btnTblEdit_clicked), pGmlcGuiSrvTabData);

	/*btnTblDump = createIconButton("gtk-save", _("Dump"));*/
	btnTblDump = gtk_button_new_from_stock(GTK_STOCK_SAVE_AS);
	gtk_widget_show (btnTblDump);
	gtk_box_pack_start (GTK_BOX (hbuttonbox), btnTblDump, TRUE, TRUE, 0);
	g_signal_connect (G_OBJECT (btnTblDump), "clicked", 
										G_CALLBACK (gmlc_gui_server_tab_data_evt_btnTblDump_clicked), pGmlcGuiSrvTabData);

	btnTblDel = gtk_button_new_from_stock(GTK_STOCK_DELETE);
	gtk_widget_show (btnTblDel);
	gtk_box_pack_start (GTK_BOX (hbuttonbox), btnTblDel, TRUE, TRUE, 0);
	g_signal_connect (G_OBJECT (btnTblDel), "clicked", 
										G_CALLBACK (gmlc_gui_server_tab_data_evt_btnTblDel_clicked), pGmlcGuiSrvTabData);

	pGmlcGuiSrvTabData->mnuTblOps = gtk_menu_new();
	gtk_widget_show (pGmlcGuiSrvTabData->mnuTblOps);

	mnuTBLOpsShowCreate = gtk_menu_item_new_with_label(_("Display create structure"));
	gtk_widget_show (mnuTBLOpsShowCreate);
	gtk_menu_append (GTK_MENU_SHELL(pGmlcGuiSrvTabData->mnuTblOps), mnuTBLOpsShowCreate);
	/*g_signal_connect (G_OBJECT (mnuTBLOpsShowCreate), "activate", 
										G_CALLBACK (gmysqlcc_gui_server_evt_mnuTBLOpsShowCreate_activate), pGmlcGuiSrvTabData);*/

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
		/*g_signal_connect (G_OBJECT (pGmlcGuiSrvTabData->lstViews), "button-press-event", 
											G_CALLBACK (gmlc_gui_server_tab_data_evt_lstTable_btnpress), pGmlcGuiSrvTabData);*/
		select = gtk_tree_view_get_selection (GTK_TREE_VIEW (pGmlcGuiSrvTabData->lstViews));
		gtk_tree_selection_set_mode (select, GTK_SELECTION_SINGLE);
		/*g_signal_connect (G_OBJECT (select), "changed", 
											G_CALLBACK (gmlc_gui_server_tab_data_evt_lstTable_selected), pGmlcGuiSrvTabData);*/
		
		hbuttonbox = gtk_hbutton_box_new ();
		gtk_widget_show (hbuttonbox);
		gtk_box_pack_start (GTK_BOX (vbox), hbuttonbox, FALSE, TRUE, 0);
		gtk_button_box_set_layout (GTK_BUTTON_BOX (hbuttonbox), GTK_BUTTONBOX_SPREAD);
		gtk_box_set_spacing (GTK_BOX (hbuttonbox), 2);
		
		btnAdd = gtk_button_new_from_stock(GTK_STOCK_ADD);
		gtk_widget_show (btnAdd);
		gtk_box_pack_start (GTK_BOX (hbuttonbox), btnAdd, TRUE, TRUE, 0);
		/*g_signal_connect (G_OBJECT (btnAdd), "clicked", 
											G_CALLBACK (gmlc_gui_server_tab_data_evt_btnVwAdd_clicked), pGmlcGuiSrvTabData);*/
		
		btnEdit = gtk_button_new_from_stock(GTK_STOCK_EDIT);
		gtk_widget_show (btnEdit);
		gtk_box_pack_start (GTK_BOX (hbuttonbox), btnEdit, TRUE, TRUE, 0);
		/*g_signal_connect (G_OBJECT (btnEdit), "clicked", 
											G_CALLBACK (gmlc_gui_server_tab_data_evt_btnVwEdit_clicked), pGmlcGuiSrvTabData);*/
		
		btnDump = gtk_button_new_from_stock(GTK_STOCK_SAVE_AS);
		gtk_widget_show (btnDump);
		gtk_box_pack_start (GTK_BOX (hbuttonbox), btnDump, TRUE, TRUE, 0);
		/*g_signal_connect (G_OBJECT (btnDump), "clicked", 
											G_CALLBACK (gmlc_gui_server_tab_data_evt_btnVwDump_clicked), pGmlcGuiSrvTabData);*/
		
		btnDel = gtk_button_new_from_stock(GTK_STOCK_DELETE);
		gtk_widget_show (btnDel);
		gtk_box_pack_start (GTK_BOX (hbuttonbox), btnDel, TRUE, TRUE, 0);
		/*g_signal_connect (G_OBJECT (btnDel), "clicked", 
											G_CALLBACK (gmlc_gui_server_tab_data_evt_btnVwDel_clicked), pGmlcGuiSrvTabData);*/
		
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
		/*g_signal_connect (G_OBJECT (pGmlcGuiSrvTabData->lstProcedures), "button-press-event", 
											G_CALLBACK (gmlc_gui_server_tab_data_evt_lstTable_btnpress), pGmlcGuiSrvTabData);*/
		select = gtk_tree_view_get_selection (GTK_TREE_VIEW (pGmlcGuiSrvTabData->lstProcedures));
		gtk_tree_selection_set_mode (select, GTK_SELECTION_SINGLE);
		/*g_signal_connect (G_OBJECT (select), "changed", 
											G_CALLBACK (gmlc_gui_server_tab_data_evt_lstTable_selected), pGmlcGuiSrvTabData);*/
		
		hbuttonbox = gtk_hbutton_box_new ();
		gtk_widget_show (hbuttonbox);
		gtk_box_pack_start (GTK_BOX (vbox), hbuttonbox, FALSE, TRUE, 0);
		gtk_button_box_set_layout (GTK_BUTTON_BOX (hbuttonbox), GTK_BUTTONBOX_SPREAD);
		gtk_box_set_spacing (GTK_BOX (hbuttonbox), 2);
		
		btnAdd = gtk_button_new_from_stock(GTK_STOCK_ADD);
		gtk_widget_show (btnAdd);
		gtk_box_pack_start (GTK_BOX (hbuttonbox), btnAdd, TRUE, TRUE, 0);
		/*g_signal_connect (G_OBJECT (btnAdd), "clicked", 
											G_CALLBACK (gmlc_gui_server_tab_data_evt_btnVwAdd_clicked), pGmlcGuiSrvTabData);*/
		
		btnEdit = gtk_button_new_from_stock(GTK_STOCK_EDIT);
		gtk_widget_show (btnEdit);
		gtk_box_pack_start (GTK_BOX (hbuttonbox), btnEdit, TRUE, TRUE, 0);
		/*g_signal_connect (G_OBJECT (btnEdit), "clicked", 
											G_CALLBACK (gmlc_gui_server_tab_data_evt_btnVwEdit_clicked), pGmlcGuiSrvTabData);*/
		
		btnDump = gtk_button_new_from_stock(GTK_STOCK_SAVE_AS);
		gtk_widget_show (btnDump);
		gtk_box_pack_start (GTK_BOX (hbuttonbox), btnDump, TRUE, TRUE, 0);
		/*g_signal_connect (G_OBJECT (btnDump), "clicked", 
											G_CALLBACK (gmlc_gui_server_tab_data_evt_btnVwDump_clicked), pGmlcGuiSrvTabData);*/
		
		btnDel = gtk_button_new_from_stock(GTK_STOCK_DELETE);
		gtk_widget_show (btnDel);
		gtk_box_pack_start (GTK_BOX (hbuttonbox), btnDel, TRUE, TRUE, 0);
		/*g_signal_connect (G_OBJECT (btnDel), "clicked", 
											G_CALLBACK (gmlc_gui_server_tab_data_evt_btnVwDel_clicked), pGmlcGuiSrvTabData);*/
		
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
		/*g_signal_connect (G_OBJECT (pGmlcGuiSrvTabData->lstFunctions), "button-press-event", 
											G_CALLBACK (gmlc_gui_server_tab_data_evt_lstTable_btnpress), pGmlcGuiSrvTabData);*/
		select = gtk_tree_view_get_selection (GTK_TREE_VIEW (pGmlcGuiSrvTabData->lstFunctions));
		gtk_tree_selection_set_mode (select, GTK_SELECTION_SINGLE);
		/*g_signal_connect (G_OBJECT (select), "changed", 
											G_CALLBACK (gmlc_gui_server_tab_data_evt_lstTable_selected), pGmlcGuiSrvTabData);*/
		
		hbuttonbox = gtk_hbutton_box_new ();
		gtk_widget_show (hbuttonbox);
		gtk_box_pack_start (GTK_BOX (vbox), hbuttonbox, FALSE, TRUE, 0);
		gtk_button_box_set_layout (GTK_BUTTON_BOX (hbuttonbox), GTK_BUTTONBOX_SPREAD);
		gtk_box_set_spacing (GTK_BOX (hbuttonbox), 2);
		
		btnAdd = gtk_button_new_from_stock(GTK_STOCK_ADD);
		gtk_widget_show (btnAdd);
		gtk_box_pack_start (GTK_BOX (hbuttonbox), btnAdd, TRUE, TRUE, 0);
		/*g_signal_connect (G_OBJECT (btnAdd), "clicked", 
											G_CALLBACK (gmlc_gui_server_tab_data_evt_btnVwAdd_clicked), pGmlcGuiSrvTabData);*/
		
		btnEdit = gtk_button_new_from_stock(GTK_STOCK_EDIT);
		gtk_widget_show (btnEdit);
		gtk_box_pack_start (GTK_BOX (hbuttonbox), btnEdit, TRUE, TRUE, 0);
		/*g_signal_connect (G_OBJECT (btnEdit), "clicked", 
											G_CALLBACK (gmlc_gui_server_tab_data_evt_btnVwEdit_clicked), pGmlcGuiSrvTabData);*/
		
		btnDump = gtk_button_new_from_stock(GTK_STOCK_SAVE_AS);
		gtk_widget_show (btnDump);
		gtk_box_pack_start (GTK_BOX (hbuttonbox), btnDump, TRUE, TRUE, 0);
		/*g_signal_connect (G_OBJECT (btnDump), "clicked", 
											G_CALLBACK (gmlc_gui_server_tab_data_evt_btnVwDump_clicked), pGmlcGuiSrvTabData);*/
		
		btnDel = gtk_button_new_from_stock(GTK_STOCK_DELETE);
		gtk_widget_show (btnDel);
		gtk_box_pack_start (GTK_BOX (hbuttonbox), btnDel, TRUE, TRUE, 0);
		/*g_signal_connect (G_OBJECT (btnDel), "clicked", 
											G_CALLBACK (gmlc_gui_server_tab_data_evt_btnVwDel_clicked), pGmlcGuiSrvTabData);*/
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

void gmlc_gui_server_tab_data_open_query_window (GmlcGuiServerTabData * pGmlcGuiSrvTabData, gboolean bUseTable) {
	GmlcGuiQuery * pGmlcGuiQry = NULL;
	gchar * pcSql = NULL, * pcDbName = NULL, * pcTblName = NULL;
	
	if (pGmlcGuiSrvTabData->pGmlcMysqlDb != NULL) {
		
		g_object_get(G_OBJECT(pGmlcGuiSrvTabData->pGmlcMysqlDb), "db_name", &pcDbName, NULL);
		
		pGmlcGuiQry = gmlc_gui_query_new(pGmlcGuiSrvTabData->pGmlcMysqlSrv, pcDbName);
		
		if (bUseTable && pGmlcGuiSrvTabData->pGmlcMysqlTbl != NULL) {
			g_object_get(G_OBJECT(pGmlcGuiSrvTabData->pGmlcMysqlTbl), "name", &pcTblName, NULL);
			
			pcSql = g_strdup_printf("SELECT * \nFROM `%s`.`%s`\nWHERE 1\nLIMIT 1000", pcDbName, pcTblName);
						
			gmlc_gui_query_set_query(pGmlcGuiQry, pcSql, TRUE);
			g_free(pcSql);
		}
		
		gtk_widget_show(GTK_WIDGET(pGmlcGuiQry));
	}
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
		gchar * pcName = NULL, * pcRows = NULL, * pcSize = NULL, * pcType = NULL;
		
		g_object_get(G_OBJECT(pGmlcMysqlTbl), "name", &pcName, "rows", &pcRows, "size", &pcSize, "type", &pcType, NULL);
		
		gtk_list_store_append (lstStrTable, &iter);
		gtk_list_store_set (lstStrTable, &iter, 0, pcName, 1, pcRows, 2, pcSize, 3, pcType, 4, pGmlcMysqlTbl, -1);
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
	if (gmlc_mysql_database_update_views_list(pGmlcGuiSrvTabData->pGmlcMysqlDb)) {
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
	if (gmlc_mysql_database_update_views_list(pGmlcGuiSrvTabData->pGmlcMysqlDb)) {
		g_hash_table_foreach(pGmlcGuiSrvTabData->pGmlcMysqlDb->htbFunctions, sub_ht_fill_function_list, lstStrFunction);
	}
	
	gtk_tree_view_set_model(GTK_TREE_VIEW(pGmlcGuiSrvTabData->lstFunctions), GTK_TREE_MODEL(lstStrFunction));
	g_object_unref (G_OBJECT (lstStrFunction));
	
	pGmlcGuiSrvTabData->pGmlcMysqlFunc = NULL;
}

static gboolean gmlc_gui_server_tab_data_evt_lstBase_btnpress (GtkWidget *widget, GdkEventButton *event, gpointer user_data) {
	GmlcGuiServerTabData * pGmlcGuiSrvTabData = GMLC_GUI_SERVER_TAB_DATA(user_data);
	
	switch (event->button) {
		case 1 : /* Left button */
			if (event->type == GDK_2BUTTON_PRESS) {
				gmlc_gui_server_tab_data_open_query_window (pGmlcGuiSrvTabData, FALSE);
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

static gboolean gmlc_gui_server_tab_data_evt_lstTables_btnpress (GtkWidget *widget, GdkEventButton *event, gpointer user_data) {
	GmlcGuiServerTabData * pGmlcGuiSrvTabData = GMLC_GUI_SERVER_TAB_DATA(user_data);
	
	switch (event->button) {
		case 1 : /* Left button */
			if (event->type == GDK_2BUTTON_PRESS) {
				gmlc_gui_server_tab_data_open_query_window (pGmlcGuiSrvTabData, TRUE);
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

static void gmlc_gui_server_tab_data_evt_lstBase_selected (GtkTreeSelection *selection, gpointer user_data) {
	GmlcGuiServerTabData * pGmlcGuiSrvTabData = GMLC_GUI_SERVER_TAB_DATA(user_data);
	GtkTreeIter iter;
	GtkTreeModel *model;
	
	g_print("Changed !\n");
	if (gtk_tree_selection_get_selected (selection, &model, &iter)) {
		gtk_tree_model_get (model, &iter, 1, &pGmlcGuiSrvTabData->pGmlcMysqlDb, -1);
		
		gmlc_gui_server_tab_data_fill_tables_list(pGmlcGuiSrvTabData);
		gmlc_gui_server_tab_data_fill_views_list(pGmlcGuiSrvTabData);
		gmlc_gui_server_tab_data_fill_procedures_list(pGmlcGuiSrvTabData);
		gmlc_gui_server_tab_data_fill_functions_list(pGmlcGuiSrvTabData);
	}
}

static void gmlc_gui_server_tab_data_evt_lstTables_selected (GtkTreeSelection *selection, gpointer user_data) {
	GmlcGuiServerTabData * pGmlcGuiSrvTabData = GMLC_GUI_SERVER_TAB_DATA(user_data);
	GtkTreeIter iter;
	GtkTreeModel *model;
	
	g_print("Changed !\n");
	if (gtk_tree_selection_get_selected (selection, &model, &iter)) {
		gtk_tree_model_get (model, &iter, 4, &pGmlcGuiSrvTabData->pGmlcMysqlTbl, -1);
	}
}

static void gmlc_gui_server_tab_data_evt_btnTlbrSqlFile_clicked (GtkWidget *widget, gpointer user_data) {
	GmlcGuiServerTabData * pGmlcGuiSrvTabData = GMLC_GUI_SERVER_TAB_DATA(user_data);
	UNUSED_VAR(widget);
	
	gmlc_gui_server_tab_data_open_query_window (pGmlcGuiSrvTabData, FALSE);
}

static void gmlc_gui_server_tab_data_evt_btnTlbrSql_clicked (GtkWidget *widget, gpointer user_data) {
	GmlcGuiServerTabData * pGmlcGuiSrvTabData = GMLC_GUI_SERVER_TAB_DATA(user_data);
	UNUSED_VAR(widget);
	
	gmlc_gui_server_tab_data_open_query_window (pGmlcGuiSrvTabData, FALSE);
}

static void gmlc_gui_server_tab_data_evt_btnDbAdd_clicked (GtkWidget *widget, gpointer user_data) {
	GmlcGuiServerTabData * pGmlcGuiSrvTabData = GMLC_GUI_SERVER_TAB_DATA(user_data);
	UNUSED_VAR(widget);
	
	gmlc_gui_server_tab_data_open_query_window(pGmlcGuiSrvTabData, FALSE);
}

static void gmlc_gui_server_tab_data_evt_btnDbDel_clicked (GtkWidget *widget, gpointer user_data) {
	UNUSED_VAR(widget);
	UNUSED_VAR(user_data);
	
}

static void gmlc_gui_server_tab_data_evt_btnTblAdd_clicked (GtkWidget *widget, gpointer user_data) {
	UNUSED_VAR(widget);
	UNUSED_VAR(user_data);
	
}

static void gmlc_gui_server_tab_data_evt_btnTblEdit_clicked (GtkWidget *widget, gpointer user_data) {
	UNUSED_VAR(widget);
	UNUSED_VAR(user_data);
	
}

static void gmlc_gui_server_tab_data_evt_btnTblDump_clicked (GtkWidget *widget, gpointer user_data) {
	UNUSED_VAR(widget);
	UNUSED_VAR(user_data);
	
}

static void gmlc_gui_server_tab_data_evt_btnTblDel_clicked (GtkWidget *widget, gpointer user_data) {
	UNUSED_VAR(widget);
	UNUSED_VAR(user_data);
	
}

