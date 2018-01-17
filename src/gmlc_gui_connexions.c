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

#include "gmlc_gui_connexions.h"
#include "gmlc_gui_server.h"

static void gmlc_gui_connexions_finalize (GmlcGuiConnexions * pGmlcGuiCnxns);
static void gmlc_gui_connexions_create_widgets (GmlcGuiConnexions * pGmlcGuiCnxns);
static void gmlc_gui_connexions_init_widgets (GmlcGuiConnexions * pGmlcGuiCnxns);

void gmlc_gui_connexions_fill_server_list (GmlcGuiConnexions * pGmlcGuiCnxns);
void gmlc_gui_connexions_dislpay_current_server (GmlcGuiConnexions * pGmlcGuiCnxns);

static gboolean gmlc_gui_connexions_evt_delete (GtkWidget *widget, GdkEvent *event, gpointer user_data);
static void gmlc_gui_connexions_evt_destroy(GtkWidget *widget, gpointer user_data);
void gmlc_gui_connexions_evt_btnNew_clicked (GtkWidget *widget, gpointer user_data);
void gmlc_gui_connexions_evt_btnAdd_clicked (GtkWidget *widget, gpointer user_data);
void gmlc_gui_connexions_evt_btnEdit_clicked (GtkWidget *widget, gpointer user_data);
void gmlc_gui_connexions_evt_btnDel_clicked (GtkWidget *widget, gpointer user_data);
void gmlc_gui_connexions_evt_btnServerUp_clicked (GtkWidget *widget, gpointer user_data);
void gmlc_gui_connexions_evt_btnServerDown_clicked (GtkWidget *widget, gpointer user_data);
void gmlc_gui_connexions_evt_btnTlbrEdit_clicked (GtkWidget *widget, gpointer user_data);
void gmlc_gui_connexions_evt_btnTlbrLoad_clicked (GtkWidget *widget, gpointer user_data);
void gmlc_gui_connexions_evt_btnTlbrConnect_clicked (GtkWidget *widget, gpointer user_data);
void gmlc_gui_connexions_evt_btnTlbrClose_clicked (GtkWidget *widget, gpointer user_data);
void gmlc_gui_connexions_evt_lstListHosts_selected (GtkTreeSelection *selection, gpointer user_data);
gboolean gmlc_gui_connexions_evt_lstListHosts_btnpress(GtkWidget *widget, GdkEventButton *event, gpointer user_data);

char * arFieldNames[] = {"Name", "Host", "Port", "Login", "Password", "Local socket"};

enum {
	PROP_0,
};

G_DEFINE_TYPE (GmlcGuiConnexions, gmlc_gui_connexions, GTK_TYPE_WINDOW);

static void gmlc_gui_connexions_class_init (GmlcGuiConnexionsClass * pClass) {
	GObjectClass *pObjClass = G_OBJECT_CLASS(pClass);
	
	pObjClass->finalize = (GObjectFinalizeFunc) gmlc_gui_connexions_finalize;
}

static void gmlc_gui_connexions_init (GmlcGuiConnexions * pGmlcGuiCnxns) {
	
	gtk_window_set_title (GTK_WINDOW(pGmlcGuiCnxns), _("GMySQLcc - Connections list"));
	gtk_window_set_default_size (GTK_WINDOW(pGmlcGuiCnxns), 640, 480);
	
	/*g_signal_connect(G_OBJECT (pGmlcGuiCnxns), "destroy", G_CALLBACK (gmlc_gui_connexions_evt_destroy), NULL);*/
	g_signal_connect(G_OBJECT (pGmlcGuiCnxns), "delete-event", G_CALLBACK (gmlc_gui_connexions_evt_delete), NULL);
	
	pGmlcGuiCnxns->pCurrMysqlSrv = NULL;
	pGmlcGuiCnxns->bShowEditPart = TRUE;
	
	gmlc_gui_connexions_create_widgets(pGmlcGuiCnxns);
	gmlc_gui_connexions_init_widgets(pGmlcGuiCnxns);
	
	/* To hide edition server part */
	gmlc_gui_connexions_evt_btnTlbrEdit_clicked(NULL, pGmlcGuiCnxns);
}

static void gmlc_gui_connexions_finalize (GmlcGuiConnexions * pGmlcGuiCnxns) {
	UNUSED_VAR(pGmlcGuiCnxns);
	
}

GmlcGuiConnexions * gmlc_gui_connexions_new () {
	GmlcGuiConnexions * pGmlcGuiCnxns = NULL;
	
	pGmlcGuiCnxns = GMLC_GUI_CONNEXIONS(g_object_new(GMLC_GUI_TYPE_CONNEXIONS, NULL));
	
	return pGmlcGuiCnxns;
}

static void gmlc_gui_connexions_create_widgets (GmlcGuiConnexions * pGmlcGuiCnxns) {
	/*GtkWidget *vbox5, *vbox7;
	GtkWidget *hbox5, *hbox6, *hbox7, *hbox8, *hbox12, *hbox13, *hbox14, *hbox15, *hbox16, *hbox17;
	GtkWidget *label9, *label10, *label11, *label12, *label13, *label14, *label17;
	GtkWidget *scrolledwindow3;*/
	
	GtkWidget *frame, *vbxMain, *vbox, *hbox, *hpaned;
	GtkWidget *label;
	GtkWidget *scrolledwindow;
	GtkWidget *toolbar;
	GtkWidget *btnNew, *btnAdd, *btnEdit, *btnDel, *btnServerUp, *btnServerDown;
	GtkWidget * imgToolbar;
	GtkToolItem * btnTlbrConnect, * btnTlbrEdit, * btnTlbrClose, * btnTlbrLoad;

	GtkTreeSelection *select;
	
	vbxMain = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	gtk_widget_show (vbxMain);
	gtk_container_add (GTK_CONTAINER (pGmlcGuiCnxns), vbxMain);

	toolbar = gtk_toolbar_new ();
	gtk_widget_show (toolbar);
	gtk_box_pack_start (GTK_BOX (vbxMain), toolbar, FALSE, FALSE, 0);
	gtk_toolbar_set_style (GTK_TOOLBAR (toolbar), GTK_TOOLBAR_BOTH_HORIZ);

	imgToolbar = gtk_image_new_from_icon_name("window-new", GTK_ICON_SIZE_LARGE_TOOLBAR);
	gtk_widget_show(imgToolbar);
	btnTlbrConnect = gtk_tool_button_new (imgToolbar, _("Connection"));
	gtk_tool_item_set_is_important (GTK_TOOL_ITEM(btnTlbrConnect), TRUE);
	g_signal_connect (G_OBJECT (btnTlbrConnect), "clicked", 
			G_CALLBACK (gmlc_gui_connexions_evt_btnTlbrConnect_clicked), pGmlcGuiCnxns);
	gtk_widget_show(GTK_WIDGET(btnTlbrConnect));
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar), GTK_TOOL_ITEM(btnTlbrConnect), -1);
	gtk_tool_item_set_tooltip_text(GTK_TOOL_ITEM(btnTlbrConnect), _("Connect to a server"));
	
	imgToolbar = gtk_image_new_from_icon_name("document-properties", GTK_ICON_SIZE_LARGE_TOOLBAR);
	gtk_widget_show(imgToolbar);
	btnTlbrEdit = gtk_tool_button_new (imgToolbar, _("Edit Server"));
	gtk_tool_item_set_is_important (GTK_TOOL_ITEM(btnTlbrEdit), TRUE);
	g_signal_connect (G_OBJECT (btnTlbrEdit), "clicked", 
			G_CALLBACK (gmlc_gui_connexions_evt_btnTlbrEdit_clicked), pGmlcGuiCnxns);
	gtk_widget_show(GTK_WIDGET(btnTlbrEdit));
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar), GTK_TOOL_ITEM(btnTlbrEdit), -1);
	gtk_tool_item_set_tooltip_text(GTK_TOOL_ITEM(btnTlbrEdit), _("Display edit server part"));
	
	imgToolbar = gtk_image_new_from_icon_name("document-open", GTK_ICON_SIZE_LARGE_TOOLBAR);
	gtk_widget_show(imgToolbar);
	btnTlbrLoad = gtk_tool_button_new (imgToolbar, _("Execute SQL file"));
	gtk_tool_item_set_is_important (GTK_TOOL_ITEM(btnTlbrLoad), TRUE);
	g_signal_connect (G_OBJECT (btnTlbrLoad), "clicked", 
			G_CALLBACK (gmlc_gui_connexions_evt_btnTlbrLoad_clicked), pGmlcGuiCnxns);
	gtk_widget_show(GTK_WIDGET(btnTlbrLoad));
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar), GTK_TOOL_ITEM(btnTlbrLoad), -1);
	gtk_tool_item_set_tooltip_text(GTK_TOOL_ITEM(btnTlbrLoad), _("Execute an SQL file on a MySQL server"));
	
	imgToolbar = gtk_image_new_from_icon_name("window-close", GTK_ICON_SIZE_LARGE_TOOLBAR);
	gtk_widget_show(imgToolbar);
	btnTlbrClose = gtk_tool_button_new (imgToolbar, _("Close"));
	gtk_tool_item_set_is_important (GTK_TOOL_ITEM(btnTlbrClose), TRUE);
	g_signal_connect (G_OBJECT (btnTlbrClose), "clicked", 
			G_CALLBACK (gmlc_gui_connexions_evt_btnTlbrClose_clicked), pGmlcGuiCnxns);
	gtk_widget_show(GTK_WIDGET(btnTlbrClose));
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar), GTK_TOOL_ITEM(btnTlbrClose), -1);
	gtk_tool_item_set_tooltip_text(GTK_TOOL_ITEM(btnTlbrClose), _("Close window"));

	hpaned = gtk_paned_new (GTK_ORIENTATION_HORIZONTAL);
	gtk_widget_show (hpaned);
	gtk_box_pack_start (GTK_BOX (vbxMain), hpaned, TRUE, TRUE, 0);
	gtk_paned_set_position (GTK_PANED (hpaned), 250);

	vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	gtk_widget_show (vbox);
	gtk_paned_pack1 (GTK_PANED (hpaned), vbox, TRUE, TRUE);

	scrolledwindow = gtk_scrolled_window_new (NULL, NULL);
	gtk_widget_show (scrolledwindow);
	gtk_box_pack_start (GTK_BOX (vbox), scrolledwindow, TRUE, TRUE, 0);

	pGmlcGuiCnxns->lstListHosts = gtk_tree_view_new ();
	gtk_widget_show (pGmlcGuiCnxns->lstListHosts);
	gtk_container_add (GTK_CONTAINER (scrolledwindow), pGmlcGuiCnxns->lstListHosts);
	g_signal_connect (G_OBJECT (pGmlcGuiCnxns->lstListHosts), "button-press-event", 
			G_CALLBACK (gmlc_gui_connexions_evt_lstListHosts_btnpress), pGmlcGuiCnxns);
	select = gtk_tree_view_get_selection (GTK_TREE_VIEW (pGmlcGuiCnxns->lstListHosts));
	gtk_tree_selection_set_mode (select, GTK_SELECTION_SINGLE);
	g_signal_connect (G_OBJECT (select), "changed", 
			G_CALLBACK (gmlc_gui_connexions_evt_lstListHosts_selected), pGmlcGuiCnxns);

	hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_widget_show (hbox);
	gtk_box_pack_start (GTK_BOX (vbox), hbox, FALSE, TRUE, 0);

	btnServerUp = gtk_button_new_from_icon_name("go-up", GTK_ICON_SIZE_BUTTON);
	gtk_widget_show (btnServerUp);
	gtk_box_pack_start (GTK_BOX (hbox), btnServerUp, TRUE, TRUE, 0);
	g_signal_connect (G_OBJECT (btnServerUp), "clicked", 
			G_CALLBACK (gmlc_gui_connexions_evt_btnServerUp_clicked), pGmlcGuiCnxns);

	btnServerDown = gtk_button_new_from_icon_name("go-down", GTK_ICON_SIZE_BUTTON);
	gtk_widget_show (btnServerDown);
	gtk_box_pack_start (GTK_BOX (hbox), btnServerDown, TRUE, TRUE, 0);
	g_signal_connect (G_OBJECT (btnServerDown), "clicked", 
			G_CALLBACK (gmlc_gui_connexions_evt_btnServerDown_clicked), pGmlcGuiCnxns);

	pGmlcGuiCnxns->vbxEditPart = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	gtk_widget_show (pGmlcGuiCnxns->vbxEditPart);
	gtk_paned_pack2 (GTK_PANED (hpaned), pGmlcGuiCnxns->vbxEditPart, TRUE, TRUE);

	frame = gtk_frame_new (NULL);
	gtk_widget_show (frame);
	gtk_box_pack_start (GTK_BOX (pGmlcGuiCnxns->vbxEditPart), frame, FALSE, FALSE, 0);

	label = gtk_label_new (_("Server"));
	gtk_widget_show (label);
	gtk_frame_set_label_widget (GTK_FRAME (frame), label);
	gtk_label_set_justify (GTK_LABEL (label), GTK_JUSTIFY_LEFT);

	vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	gtk_widget_show (vbox);
	gtk_container_add (GTK_CONTAINER (frame), vbox);
	gtk_container_set_border_width (GTK_CONTAINER (vbox), 4);

	hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_widget_show (hbox);
	gtk_box_pack_start (GTK_BOX (vbox), hbox, TRUE, TRUE, 0);
	gtk_container_set_border_width (GTK_CONTAINER (hbox), 2);

	label = gtk_label_new (_("Name :"));
	gtk_widget_show (label);
	gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, FALSE, 0);
	gtk_label_set_justify (GTK_LABEL (label), GTK_JUSTIFY_LEFT);

	pGmlcGuiCnxns->txtName = gtk_entry_new ();
	gtk_widget_show (pGmlcGuiCnxns->txtName);
	gtk_box_pack_start (GTK_BOX (hbox), pGmlcGuiCnxns->txtName, TRUE, TRUE, 2);

	hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_widget_show (hbox);
	gtk_box_pack_start (GTK_BOX (vbox), hbox, TRUE, TRUE, 0);
	gtk_container_set_border_width (GTK_CONTAINER (hbox), 2);

	label = gtk_label_new (_("Host :"));
	gtk_widget_show (label);
	gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, FALSE, 0);
	gtk_label_set_justify (GTK_LABEL (label), GTK_JUSTIFY_LEFT);

	pGmlcGuiCnxns->txtHost = gtk_entry_new ();
	gtk_widget_show (pGmlcGuiCnxns->txtHost);
	gtk_box_pack_start (GTK_BOX (hbox), pGmlcGuiCnxns->txtHost, TRUE, TRUE, 2);

	hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_widget_show (hbox);
	gtk_box_pack_start (GTK_BOX (vbox), hbox, TRUE, TRUE, 0);
	gtk_container_set_border_width (GTK_CONTAINER (hbox), 2);

	label = gtk_label_new (_("Port :"));
	gtk_widget_show (label);
	gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, FALSE, 0);
	gtk_label_set_justify (GTK_LABEL (label), GTK_JUSTIFY_LEFT);

	pGmlcGuiCnxns->txtPort = gtk_entry_new ();
	gtk_widget_show (pGmlcGuiCnxns->txtPort);
	gtk_box_pack_start (GTK_BOX (hbox), pGmlcGuiCnxns->txtPort, TRUE, TRUE, 2);
	gtk_entry_set_max_length (GTK_ENTRY (pGmlcGuiCnxns->txtPort), 5);

	hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_widget_show (hbox);
	gtk_box_pack_start (GTK_BOX (vbox), hbox, TRUE, TRUE, 0);
	gtk_container_set_border_width (GTK_CONTAINER (hbox), 2);

	label = gtk_label_new (_("User name :"));
	gtk_widget_show (label);
	gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, FALSE, 0);
	gtk_label_set_justify (GTK_LABEL (label), GTK_JUSTIFY_LEFT);

	pGmlcGuiCnxns->txtLogin = gtk_entry_new ();
	gtk_widget_show (pGmlcGuiCnxns->txtLogin);
	gtk_box_pack_start (GTK_BOX (hbox), pGmlcGuiCnxns->txtLogin, TRUE, TRUE, 2);

	hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_widget_show (hbox);
	gtk_box_pack_start (GTK_BOX (vbox), hbox, TRUE, TRUE, 0);
	gtk_container_set_border_width (GTK_CONTAINER (hbox), 2);

	label = gtk_label_new (_("Password :"));
	gtk_widget_show (label);
	gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, FALSE, 0);
	gtk_label_set_justify (GTK_LABEL (label), GTK_JUSTIFY_LEFT);

	pGmlcGuiCnxns->txtPasswd = gtk_entry_new ();
	gtk_widget_show (pGmlcGuiCnxns->txtPasswd);
	gtk_box_pack_start (GTK_BOX (hbox), pGmlcGuiCnxns->txtPasswd, TRUE, TRUE, 2);

	hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_widget_show (hbox);
	gtk_box_pack_start (GTK_BOX (vbox), hbox, TRUE, TRUE, 0);
	gtk_container_set_border_width (GTK_CONTAINER (hbox), 2);

	label = gtk_label_new (_("Local socket file :"));
	gtk_widget_show (label);
	gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, FALSE, 0);
	gtk_label_set_justify (GTK_LABEL (label), GTK_JUSTIFY_LEFT);

	pGmlcGuiCnxns->txtLocalSock = gtk_entry_new ();
	gtk_widget_show (pGmlcGuiCnxns->txtLocalSock);
	gtk_box_pack_start (GTK_BOX (hbox), pGmlcGuiCnxns->txtLocalSock, TRUE, TRUE, 2);

	hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_widget_show (hbox);
	gtk_box_pack_start (GTK_BOX (vbox), hbox, TRUE, TRUE, 0);
	gtk_container_set_border_width (GTK_CONTAINER (hbox), 2);

	pGmlcGuiCnxns->chkReadOnly = gtk_check_button_new_with_label (_("Read only server"));
	gtk_widget_show (pGmlcGuiCnxns->chkReadOnly);
	gtk_box_pack_start (GTK_BOX (hbox), pGmlcGuiCnxns->chkReadOnly, TRUE, TRUE, 2);

	hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_widget_show (hbox);
	gtk_box_pack_start (GTK_BOX (vbox), hbox, TRUE, TRUE, 0);
	gtk_container_set_border_width (GTK_CONTAINER (hbox), 2);

	pGmlcGuiCnxns->chkWriteWarning = gtk_check_button_new_with_label (_("Warning about write query on this server"));
	gtk_widget_show (pGmlcGuiCnxns->chkWriteWarning);
	gtk_box_pack_start (GTK_BOX (hbox), pGmlcGuiCnxns->chkWriteWarning, TRUE, TRUE, 2);

	hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_widget_show (hbox);
	gtk_box_pack_start (GTK_BOX (pGmlcGuiCnxns->vbxEditPart), hbox, FALSE, TRUE, 0);
	gtk_container_set_border_width (GTK_CONTAINER (hbox), 1);

	btnNew = gtk_button_new_from_icon_name("document-new", GTK_ICON_SIZE_BUTTON);
	gtk_widget_show (btnNew);
	gtk_box_pack_start (GTK_BOX (hbox), btnNew, TRUE, TRUE, 0);
	gtk_container_set_border_width (GTK_CONTAINER (btnNew), 2);
	g_signal_connect (G_OBJECT (btnNew), "clicked", 
			G_CALLBACK (gmlc_gui_connexions_evt_btnNew_clicked), pGmlcGuiCnxns);

	btnAdd = gtk_button_new_from_icon_name("list-add", GTK_ICON_SIZE_BUTTON);
	gtk_widget_show (btnAdd);
	gtk_box_pack_start (GTK_BOX (hbox), btnAdd, TRUE, TRUE, 0);
	gtk_container_set_border_width (GTK_CONTAINER (btnAdd), 2);
	g_signal_connect (G_OBJECT (btnAdd), "clicked", 
			G_CALLBACK (gmlc_gui_connexions_evt_btnAdd_clicked), pGmlcGuiCnxns);

	btnEdit = gtk_button_new_from_icon_name("document-save", GTK_ICON_SIZE_BUTTON);
	gtk_widget_show (btnEdit);
	gtk_box_pack_start (GTK_BOX (hbox), btnEdit, TRUE, TRUE, 0);
	gtk_container_set_border_width (GTK_CONTAINER (btnEdit), 2);
	g_signal_connect (G_OBJECT (btnEdit), "clicked", 
			G_CALLBACK (gmlc_gui_connexions_evt_btnEdit_clicked), pGmlcGuiCnxns);

	btnDel = gtk_button_new_from_icon_name("list-remove", GTK_ICON_SIZE_BUTTON);
	gtk_widget_show (btnDel);
	gtk_box_pack_start (GTK_BOX (hbox), btnDel, TRUE, TRUE, 0);
	gtk_container_set_border_width (GTK_CONTAINER (btnDel), 2);
	g_signal_connect (G_OBJECT (btnDel), "clicked", 
			G_CALLBACK (gmlc_gui_connexions_evt_btnDel_clicked), pGmlcGuiCnxns);

}

static void gmlc_gui_connexions_init_widgets (GmlcGuiConnexions * pGmlcGuiCnxns) {
	GtkTreeViewColumn * currCol;
	GtkCellRenderer * renderer;
	
	renderer = gtk_cell_renderer_text_new ();
	currCol = gtk_tree_view_column_new_with_attributes (_("Server name"), renderer, "text", 0, NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW (pGmlcGuiCnxns->lstListHosts), currCol);
	
	gmlc_gui_connexions_fill_server_list(pGmlcGuiCnxns);
}

void gmlc_gui_connexions_fill_server_list (GmlcGuiConnexions * pGmlcGuiCnxns) {
	GmlcMysqlServer * pGmlcMysqlSrv = NULL;
	GList * items = NULL;
	GtkListStore * lstStrBase = NULL;
	GtkTreeIter iter, * itSel = NULL;
	GtkTreeSelection* selection = NULL;
	GtkTreePath * pathToSelRow = NULL;
	
	lstStrBase = gtk_list_store_new(2, G_TYPE_STRING, G_TYPE_POINTER);
	items = g_list_first(GpGmlcMscCfg->lstServers);
	while (items != NULL) {
		pGmlcMysqlSrv = GMLC_MYSQL_SERVER(items->data);
		
		/* Display if needed the field */
		if (pGmlcMysqlSrv != NULL) {
			gtk_list_store_append (lstStrBase, &iter);
			gtk_list_store_set (lstStrBase, &iter, 0, pGmlcMysqlSrv->pcName, -1);
			gtk_list_store_set (lstStrBase, &iter, 1, pGmlcMysqlSrv, -1);
			
			if (pGmlcMysqlSrv == pGmlcGuiCnxns->pCurrMysqlSrv) {
				itSel = gtk_tree_iter_copy(&iter);
			}
		}
		
		/* Go to next item */
		items = g_list_next(items);
	}
	
	gtk_tree_view_set_model(GTK_TREE_VIEW(pGmlcGuiCnxns->lstListHosts), GTK_TREE_MODEL(lstStrBase));
	
	if (itSel != NULL) {
		selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(pGmlcGuiCnxns->lstListHosts));
		gtk_tree_selection_select_iter(GTK_TREE_SELECTION(selection), itSel);
		
		pathToSelRow = gtk_tree_model_get_path (GTK_TREE_MODEL(lstStrBase), itSel);
		gtk_tree_view_scroll_to_cell (GTK_TREE_VIEW(pGmlcGuiCnxns->lstListHosts), pathToSelRow, NULL, TRUE, 0.5, 0.0);
		
		gtk_tree_path_free (pathToSelRow);
		gtk_tree_iter_free (itSel);
		
		gmlc_gui_connexions_dislpay_current_server (pGmlcGuiCnxns);
	}

	g_object_unref (G_OBJECT (lstStrBase));
	
}

void gmlc_gui_connexions_dislpay_current_server (GmlcGuiConnexions * pGmlcGuiCnxns) {
	GmlcMysqlServer * pGmlcMysqlSrv = pGmlcGuiCnxns->pCurrMysqlSrv;
	char tmpPort [G_ASCII_DTOSTR_BUF_SIZE + 1];
	
	if (pGmlcMysqlSrv != NULL) {
		g_ascii_dtostr(tmpPort, G_ASCII_DTOSTR_BUF_SIZE, pGmlcMysqlSrv->iPort);
		
		gtk_entry_set_text(GTK_ENTRY(pGmlcGuiCnxns->txtName), pGmlcMysqlSrv->pcName);
		gtk_entry_set_text(GTK_ENTRY(pGmlcGuiCnxns->txtHost), pGmlcMysqlSrv->pcHost);
		gtk_entry_set_text(GTK_ENTRY(pGmlcGuiCnxns->txtPort), tmpPort);
		gtk_entry_set_text(GTK_ENTRY(pGmlcGuiCnxns->txtLogin), pGmlcMysqlSrv->pcLogin);
		gtk_entry_set_text(GTK_ENTRY(pGmlcGuiCnxns->txtPasswd), pGmlcMysqlSrv->pcPassword);
		gtk_entry_set_text(GTK_ENTRY(pGmlcGuiCnxns->txtLocalSock), ((pGmlcMysqlSrv->pcLocalSock != NULL) ? pGmlcMysqlSrv->pcLocalSock : ""));
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(pGmlcGuiCnxns->chkReadOnly), pGmlcMysqlSrv->bReadOnly);
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(pGmlcGuiCnxns->chkWriteWarning), pGmlcMysqlSrv->bWriteWarning);
	} else {
		gtk_entry_set_text(GTK_ENTRY(pGmlcGuiCnxns->txtName), "");
		gtk_entry_set_text(GTK_ENTRY(pGmlcGuiCnxns->txtHost), "");
		gtk_entry_set_text(GTK_ENTRY(pGmlcGuiCnxns->txtPort), "3306");
		gtk_entry_set_text(GTK_ENTRY(pGmlcGuiCnxns->txtLogin), "");
		gtk_entry_set_text(GTK_ENTRY(pGmlcGuiCnxns->txtPasswd), "");
		gtk_entry_set_text(GTK_ENTRY(pGmlcGuiCnxns->txtLocalSock), "/var/run/mysqld/mysqld.sock");
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(pGmlcGuiCnxns->chkReadOnly), FALSE);
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(pGmlcGuiCnxns->chkWriteWarning), FALSE);
	}
}

static gboolean gmlc_gui_connexions_evt_delete (GtkWidget *widget, GdkEvent *event, gpointer user_data) {
	UNUSED_VAR(event);
	UNUSED_VAR(user_data);

	gtk_widget_hide(GTK_WIDGET(widget));
	
	if (GiNbrWnd <= 0) {
		gtk_main_quit();
		return TRUE;
	}
	
	return FALSE;
}

static void gmlc_gui_connexions_evt_destroy(GtkWidget *widget, gpointer user_data) {
	UNUSED_VAR(user_data);

	gtk_widget_hide(GTK_WIDGET(widget));
	
	if (GiNbrWnd <= 0) {
		gtk_main_quit();
	}
}

void gmlc_gui_connexions_evt_btnNew_clicked (GtkWidget *widget, gpointer user_data) {
	GmlcGuiConnexions * pGmlcGuiCnxns = GMLC_GUI_CONNEXIONS(user_data);
	
	pGmlcGuiCnxns->pCurrMysqlSrv = NULL;
	gmlc_gui_connexions_fill_server_list(pGmlcGuiCnxns);
}

void gmlc_gui_connexions_evt_btnAdd_clicked (GtkWidget *widget, gpointer user_data) {
	GmlcGuiConnexions * pGmlcGuiCnxns = GMLC_GUI_CONNEXIONS(user_data);
	GString * infos[6];
	GtkWidget * msgdlg;
	gboolean read_only, write_warning;
	int i, j, port;
	
	/* Get data */
	infos[0] = g_string_new(gtk_entry_get_text(GTK_ENTRY(pGmlcGuiCnxns->txtName)));
	infos[1] = g_string_new(gtk_entry_get_text(GTK_ENTRY(pGmlcGuiCnxns->txtHost)));
	infos[2] = g_string_new(gtk_entry_get_text(GTK_ENTRY(pGmlcGuiCnxns->txtPort)));
	infos[3] = g_string_new(gtk_entry_get_text(GTK_ENTRY(pGmlcGuiCnxns->txtLogin)));
	infos[4] = g_string_new(gtk_entry_get_text(GTK_ENTRY(pGmlcGuiCnxns->txtPasswd)));
	infos[5] = g_string_new(gtk_entry_get_text(GTK_ENTRY(pGmlcGuiCnxns->txtLocalSock)));
	read_only = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(pGmlcGuiCnxns->chkReadOnly));
	write_warning = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(pGmlcGuiCnxns->chkWriteWarning));
	
	/* Check data */
	for (i = 0; i < 4; i++) {
		if (infos[i]->len == 0) { /* check if not empty */
			msgdlg = gtk_message_dialog_new(GTK_WINDOW(pGmlcGuiCnxns), GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, _("Field %s cannot be empty !!!"), arFieldNames[i]);
			gtk_dialog_run (GTK_DIALOG (msgdlg));
			gtk_widget_destroy (msgdlg);
			return;
		} else if (i == 2) { /* check if port is a number */
			for (j = 0; j < infos[i]->len; j++) {
				if (!g_ascii_isdigit(infos[i]->str[j])) {
					msgdlg = gtk_message_dialog_new(GTK_WINDOW(pGmlcGuiCnxns), GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, _("Field %s can only contain digits !!!"), arFieldNames[i]);
					gtk_dialog_run (GTK_DIALOG (msgdlg));
					gtk_widget_destroy (msgdlg);
					return;
				}
			}
		}
	}
	
	/* Check if name is unique */
	if (gmlc_misc_config_get_server(GpGmlcMscCfg, infos[0]->str) != NULL) {
		msgdlg = gtk_message_dialog_new(GTK_WINDOW(pGmlcGuiCnxns), GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, _("Name already taken !\nPlease set another one."));
		gtk_dialog_run (GTK_DIALOG (msgdlg));
		gtk_widget_destroy (msgdlg);
		return;
	}
	
	/* Insert data in configuration */
	port = (int)g_ascii_strtoull(infos[2]->str, NULL, 10);
	if (gmlc_misc_config_add_server(GpGmlcMscCfg, infos[0]->str, infos[1]->str, port, infos[3]->str, infos[4]->str, NULL, infos[5]->str, read_only, write_warning)) {
		pGmlcGuiCnxns->pCurrMysqlSrv = gmlc_misc_config_get_server(GpGmlcMscCfg, infos[0]->str);
		gmlc_gui_connexions_fill_server_list(pGmlcGuiCnxns);
	}
	
	for (i = 0; i < 6; i++) {
		if (infos[i] != NULL) {
			g_string_free(infos[i], TRUE);
		}
	}
}

void gmlc_gui_connexions_evt_btnEdit_clicked (GtkWidget *widget, gpointer user_data) {
	GmlcGuiConnexions * pGmlcGuiCnxns = GMLC_GUI_CONNEXIONS(user_data);
	GmlcMysqlServer * pGmlcMysqlSrv;
	GString * infos[6];
	GtkWidget * msgdlg;
	gboolean read_only, write_warning;
	int i, j, port;
	
	if (pGmlcGuiCnxns->pCurrMysqlSrv == NULL) {
		msgdlg = gtk_message_dialog_new(GTK_WINDOW(pGmlcGuiCnxns), GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, _("Please select a server"));
		gtk_dialog_run (GTK_DIALOG (msgdlg));
		gtk_widget_destroy (msgdlg);
		return;
	}
	
	/* Get data */
	infos[0] = g_string_new(gtk_entry_get_text(GTK_ENTRY(pGmlcGuiCnxns->txtName)));
	infos[1] = g_string_new(gtk_entry_get_text(GTK_ENTRY(pGmlcGuiCnxns->txtHost)));
	infos[2] = g_string_new(gtk_entry_get_text(GTK_ENTRY(pGmlcGuiCnxns->txtPort)));
	infos[3] = g_string_new(gtk_entry_get_text(GTK_ENTRY(pGmlcGuiCnxns->txtLogin)));
	infos[4] = g_string_new(gtk_entry_get_text(GTK_ENTRY(pGmlcGuiCnxns->txtPasswd)));
	infos[5] = g_string_new(gtk_entry_get_text(GTK_ENTRY(pGmlcGuiCnxns->txtLocalSock)));
	read_only = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(pGmlcGuiCnxns->chkReadOnly));
	write_warning = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(pGmlcGuiCnxns->chkWriteWarning));
	
	/* Check data */
	for (i = 0; i < 4; i++) {
		if (infos[i]->len == 0) { /* check if not empty */
			msgdlg = gtk_message_dialog_new(GTK_WINDOW(pGmlcGuiCnxns), GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, _("Field %s cannot be empty !!!"), arFieldNames[i]);
			gtk_dialog_run (GTK_DIALOG (msgdlg));
			gtk_widget_destroy (msgdlg);
			return;		
		} else if (i == 2) { /* check if port is a number */
			for (j = 0; j < infos[i]->len; j++) {
				if (!g_ascii_isdigit(infos[i]->str[j])) {
					msgdlg = gtk_message_dialog_new(GTK_WINDOW(pGmlcGuiCnxns), GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, _("Field %s can only contain digits !!!"), arFieldNames[i]);
					gtk_dialog_run (GTK_DIALOG (msgdlg));
					gtk_widget_destroy (msgdlg);
					return;		
				}
			}
		}
	}
	
	/* Check if name is unique */
	pGmlcMysqlSrv = gmlc_misc_config_get_server(GpGmlcMscCfg, infos[0]->str);
	if (pGmlcMysqlSrv != NULL && pGmlcMysqlSrv != pGmlcGuiCnxns->pCurrMysqlSrv) {
		msgdlg = gtk_message_dialog_new(GTK_WINDOW(pGmlcGuiCnxns), GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, _("Name already taken !\nPlease set another one."));
		gtk_dialog_run (GTK_DIALOG (msgdlg));
		gtk_widget_destroy (msgdlg);
		return;		
	}
	
	/* Update data in configuration */
	port = (int)g_ascii_strtoull(infos[2]->str, NULL, 10);
	if (gmlc_misc_config_update_server(GpGmlcMscCfg, pGmlcGuiCnxns->pCurrMysqlSrv->pcName, infos[0]->str, infos[1]->str, port, infos[3]->str, infos[4]->str, NULL, infos[5]->str, read_only, write_warning)) {
		gmlc_gui_connexions_fill_server_list(pGmlcGuiCnxns);
	}
	
	for (i = 0; i < 6; i++) {
		if (infos[i] != NULL) {
			g_string_free(infos[i], 1);
		}
	}
}

void gmlc_gui_connexions_evt_btnDel_clicked (GtkWidget *widget, gpointer user_data) {
	GmlcGuiConnexions * pGmlcGuiCnxns = GMLC_GUI_CONNEXIONS(user_data);
	GtkWidget * msgdlg;
	
	if (pGmlcGuiCnxns->pCurrMysqlSrv != NULL) {
		if (!gmlc_misc_config_del_server (GpGmlcMscCfg, pGmlcGuiCnxns->pCurrMysqlSrv->pcName)) {
			msgdlg = gtk_message_dialog_new(GTK_WINDOW(pGmlcGuiCnxns), GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, _("Can't delete this server !!!"));
			gtk_dialog_run (GTK_DIALOG (msgdlg));
			gtk_widget_destroy (msgdlg);
		} else {
			gmlc_gui_connexions_fill_server_list(pGmlcGuiCnxns);
		}
	} else {
		msgdlg = gtk_message_dialog_new(GTK_WINDOW(pGmlcGuiCnxns), GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, _("Please select a server"));
		gtk_dialog_run (GTK_DIALOG (msgdlg));
		gtk_widget_destroy (msgdlg);
	}
}

void gmlc_gui_connexions_evt_btnServerUp_clicked (GtkWidget *widget, gpointer user_data) {
	GmlcGuiConnexions * pGmlcGuiCnxns = GMLC_GUI_CONNEXIONS(user_data);
	GtkTreeIter itCurr;
	GtkTreeModel *model = NULL;
	GtkTreeSelection* selection = NULL;
	GmlcMysqlServer * pGmlcMysqlSrv = NULL;
	GList * lstCurr = NULL, * lstTrgt = NULL;
	GtkWidget * msgdlg = NULL;
	gboolean error = FALSE;
	
	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(pGmlcGuiCnxns->lstListHosts));
	if (gtk_tree_selection_get_selected (selection, &model, &itCurr)) {
		gtk_tree_model_get (model, &itCurr, 1, &pGmlcMysqlSrv, -1);
		lstCurr = g_list_find (GpGmlcMscCfg->lstServers, pGmlcMysqlSrv);
		
		if (lstCurr != NULL && lstCurr->prev != NULL) {
			lstTrgt = lstCurr->prev;
			
			GpGmlcMscCfg->lstServers = g_list_insert_before(GpGmlcMscCfg->lstServers, lstTrgt, pGmlcMysqlSrv);
			GpGmlcMscCfg->lstServers = g_list_delete_link(GpGmlcMscCfg->lstServers, lstCurr);
			
			gmlc_gui_connexions_fill_server_list(pGmlcGuiCnxns);
		} else {
			error = TRUE;
		}
	} else {
		error = TRUE;
	}
	
	if (error) {
		msgdlg = gtk_message_dialog_new(GTK_WINDOW(pGmlcGuiCnxns), GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, _("Can't move up more !!!"));
		gtk_dialog_run (GTK_DIALOG (msgdlg));
		gtk_widget_destroy (msgdlg);
	}
}

void gmlc_gui_connexions_evt_btnServerDown_clicked (GtkWidget *widget, gpointer user_data) {
	GmlcGuiConnexions * pGmlcGuiCnxns = GMLC_GUI_CONNEXIONS(user_data);
	GtkTreeIter itCurr;
	GtkTreeModel * model = NULL;
	GtkTreeSelection * selection = NULL;
	GmlcMysqlServer * pGmlcMysqlSrv = NULL;
	GList * lstCurr = NULL, * lstTrgt = NULL;
	GtkWidget * msgdlg = NULL;
	gboolean error = FALSE;
	
	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(pGmlcGuiCnxns->lstListHosts));
	if (gtk_tree_selection_get_selected (selection, &model, &itCurr)) {
		gtk_tree_model_get (model, &itCurr, 1, &pGmlcMysqlSrv, -1);
		lstCurr = g_list_find (GpGmlcMscCfg->lstServers, pGmlcMysqlSrv);
		
		if (lstCurr != NULL && lstCurr->next != NULL) {
			lstTrgt = lstCurr->next->next;
			
			GpGmlcMscCfg->lstServers = g_list_insert_before(GpGmlcMscCfg->lstServers, lstTrgt, pGmlcMysqlSrv);
			GpGmlcMscCfg->lstServers = g_list_delete_link(GpGmlcMscCfg->lstServers, lstCurr);
			
			gmlc_gui_connexions_fill_server_list(pGmlcGuiCnxns);
		} else {
			error = TRUE;
		}
	} else {
		error = TRUE;
	}
	
	if (error) {
		msgdlg = gtk_message_dialog_new(GTK_WINDOW(pGmlcGuiCnxns), GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, _("Can't move down more !!!"));
		gtk_dialog_run (GTK_DIALOG (msgdlg));
		gtk_widget_destroy (msgdlg);
	}
}

void gmlc_gui_connexions_evt_btnTlbrEdit_clicked (GtkWidget *widget, gpointer user_data) {
	GmlcGuiConnexions * pGmlcGuiCnxns = GMLC_GUI_CONNEXIONS(user_data);
	
	pGmlcGuiCnxns->bShowEditPart = (pGmlcGuiCnxns->bShowEditPart) ? FALSE : TRUE ;
	
	if (pGmlcGuiCnxns->bShowEditPart) {
		gtk_widget_show(GTK_WIDGET(pGmlcGuiCnxns->vbxEditPart));
	} else {
		gtk_widget_hide(GTK_WIDGET(pGmlcGuiCnxns->vbxEditPart));
	}
}

void gmlc_gui_connexions_evt_btnTlbrLoad_clicked (GtkWidget *widget, gpointer user_data) {
/*	p_gmysqlcc_gui_exec_file gui_xcfl = NULL;
	
	gui_xcfl = gmysqlcc_gui_exec_file_new_open_file();
	
	if (gui_xcfl != NULL) {
		gmysqlcc_gui_exec_file_display(gui_xcfl, TRUE);
	}
*/
}

void gmlc_gui_connexions_evt_btnTlbrConnect_clicked (GtkWidget *widget, gpointer user_data) {
	GmlcGuiConnexions * pGmlcGuiCnxns = GMLC_GUI_CONNEXIONS(user_data);
	GmlcGuiServer * pGmlcGuiSrv = NULL;
	GtkWidget * msgdlg;
	
	if (pGmlcGuiCnxns->pCurrMysqlSrv != NULL) {
		pGmlcGuiSrv = gmlc_gui_server_new(pGmlcGuiCnxns->pCurrMysqlSrv);
		gtk_widget_show(GTK_WIDGET(pGmlcGuiSrv));
		gtk_widget_hide(GTK_WIDGET(pGmlcGuiCnxns));
	} else {
		msgdlg = gtk_message_dialog_new(GTK_WINDOW(pGmlcGuiCnxns), GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, _("Please select a server"));
		gtk_dialog_run (GTK_DIALOG (msgdlg));
		gtk_widget_destroy (msgdlg);
	}	
}

void gmlc_gui_connexions_evt_btnTlbrClose_clicked (GtkWidget *widget, gpointer user_data) {
	GmlcGuiConnexions * pGmlcGuiCnxns = GMLC_GUI_CONNEXIONS(user_data);
	
	gtk_widget_hide(GTK_WIDGET(pGmlcGuiCnxns));

	if (GiNbrWnd <= 0) {
		gtk_main_quit();
	}
}

void gmlc_gui_connexions_evt_lstListHosts_selected (GtkTreeSelection *selection, gpointer user_data) {
	GmlcGuiConnexions * pGmlcGuiCnxns = GMLC_GUI_CONNEXIONS(user_data);
	GtkTreeIter iter;
	GtkTreeModel *model;
	
	if (gtk_tree_selection_get_selected (selection, &model, &iter)) {
		gtk_tree_model_get (model, &iter, 1, &pGmlcGuiCnxns->pCurrMysqlSrv, -1);
		
		gmlc_gui_connexions_dislpay_current_server(pGmlcGuiCnxns);
	}
}

gboolean gmlc_gui_connexions_evt_lstListHosts_btnpress(GtkWidget *widget, GdkEventButton *event, gpointer user_data) {
	
	if (event->type == GDK_2BUTTON_PRESS) {
		gmlc_gui_connexions_evt_btnTlbrConnect_clicked(widget, user_data);
	}
	
	return FALSE;
}


