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
#include "gmlc_mysql_query.h"


void gmlc_gui_server_fill_database_list (GmlcGuiServer * pGmlcGuiSrv);
void gmlc_gui_server_fill_tables_list (GmlcGuiServer * pGmlcGuiSrv);
void gmlc_gui_server_fill_views_list (GmlcGuiServer * pGmlcGuiSrv);
void gmlc_gui_server_fill_procedures_list (GmlcGuiServer * pGmlcGuiSrv);
void gmlc_gui_server_fill_functions_list (GmlcGuiServer * pGmlcGuiSrv);

gboolean gmlc_gui_server_evt_lstBase_btnpress (GtkWidget *widget, GdkEventButton *event, gpointer user_data);
gboolean gmlc_gui_server_evt_lstTables_btnpress (GtkWidget *widget, GdkEventButton *event, gpointer user_data);
void gmlc_gui_server_evt_lstBase_selected (GtkTreeSelection *selection, gpointer user_data);
void gmlc_gui_server_evt_lstTables_selected (GtkTreeSelection *selection, gpointer user_data);

void gmlc_gui_server_evt_btnDbAdd_clicked (GtkWidget *widget, gpointer user_data);
void gmlc_gui_server_evt_btnDbDel_clicked (GtkWidget *widget, gpointer user_data);
void gmlc_gui_server_evt_btnTblAdd_clicked (GtkWidget *widget, gpointer user_data);
void gmlc_gui_server_evt_btnTblEdit_clicked (GtkWidget *widget, gpointer user_data);
void gmlc_gui_server_evt_btnTblDump_clicked (GtkWidget *widget, gpointer user_data);
void gmlc_gui_server_evt_btnTblDel_clicked (GtkWidget *widget, gpointer user_data);


void gmlc_gui_server_create_widget_tab_data (GmlcGuiServer * pGmlcGuiSrv) {
	GtkWidget *vbox, *hpaned, *hbuttonbox, *nbkDatas;
	GtkWidget *label;
	GtkWidget *scrolledwindow;
	GtkWidget *btnDbAdd, *btnDbDel;
	GtkWidget *btnTblAdd, *btnTblEdit, *btnTblDump, *btnTblDel;
	GtkWidget *btnAdd, *btnEdit, *btnDump, *btnDel;
	GtkWidget *mnuDBOpsRefresh, *mnuDBOpsShowCreate, *mnuTBLOpsShowCreate;
	GtkTreeSelection *select;
	glong lServerVersion = 0;
	
	g_object_get(G_OBJECT(pGmlcGuiSrv->pGmlcMysqlSrv), "version", &lServerVersion, NULL);
	
	label = gtk_label_new (_("Databases"));
	gtk_widget_show (label);
	gtk_label_set_justify (GTK_LABEL (label), GTK_JUSTIFY_LEFT);
	
	hpaned = gtk_hpaned_new ();
	gtk_widget_show (hpaned);
	gtk_paned_set_position (GTK_PANED (hpaned), 200);
	
	gtk_notebook_append_page(GTK_NOTEBOOK(pGmlcGuiSrv->nbkGeneral), hpaned, label);

	vbox = gtk_vbox_new (FALSE, 0);
	gtk_widget_show (vbox);
	gtk_paned_pack1 (GTK_PANED (hpaned), vbox, FALSE, TRUE);

	scrolledwindow = gtk_scrolled_window_new (NULL, NULL);
	gtk_widget_show (scrolledwindow);
	gtk_box_pack_start (GTK_BOX (vbox), scrolledwindow, TRUE, TRUE, 0);
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow), GTK_POLICY_NEVER, GTK_POLICY_ALWAYS);

	pGmlcGuiSrv->lstBase = gtk_tree_view_new ();
	gtk_widget_show (pGmlcGuiSrv->lstBase);
	gtk_tree_view_set_rules_hint(GTK_TREE_VIEW (pGmlcGuiSrv->lstBase), TRUE);
	gtk_container_add (GTK_CONTAINER (scrolledwindow), pGmlcGuiSrv->lstBase);
	g_signal_connect (G_OBJECT (pGmlcGuiSrv->lstBase), "button-press-event", 
										G_CALLBACK (gmlc_gui_server_evt_lstBase_btnpress), pGmlcGuiSrv);
	select = gtk_tree_view_get_selection (GTK_TREE_VIEW (pGmlcGuiSrv->lstBase));
	gtk_tree_selection_set_mode (select, GTK_SELECTION_SINGLE);
	g_signal_connect (G_OBJECT (select), "changed", 
										G_CALLBACK (gmlc_gui_server_evt_lstBase_selected), pGmlcGuiSrv);

	hbuttonbox = gtk_hbutton_box_new ();
	gtk_widget_show (hbuttonbox);
	gtk_box_pack_start (GTK_BOX (vbox), hbuttonbox, FALSE, TRUE, 0);
	gtk_button_box_set_layout (GTK_BUTTON_BOX (hbuttonbox), GTK_BUTTONBOX_SPREAD);
	gtk_box_set_spacing (GTK_BOX (hbuttonbox), 2);

	btnDbAdd = gtk_button_new_from_stock(GTK_STOCK_ADD);
	gtk_widget_show (btnDbAdd);
	gtk_box_pack_start (GTK_BOX (hbuttonbox), btnDbAdd, TRUE, TRUE, 0);
	g_signal_connect (G_OBJECT (btnDbAdd), "clicked", 
										G_CALLBACK (gmlc_gui_server_evt_btnDbAdd_clicked), pGmlcGuiSrv);

	btnDbDel = gtk_button_new_from_stock(GTK_STOCK_DELETE);
	gtk_widget_show (btnDbDel);
	gtk_box_pack_start (GTK_BOX (hbuttonbox), btnDbDel, TRUE, TRUE, 0);
	g_signal_connect (G_OBJECT (btnDbDel), "clicked", 
										G_CALLBACK (gmlc_gui_server_evt_btnDbDel_clicked), pGmlcGuiSrv);
	
	pGmlcGuiSrv->mnuBdOps = gtk_menu_new();
	gtk_widget_show (pGmlcGuiSrv->mnuBdOps);

	mnuDBOpsRefresh = gtk_menu_item_new_with_label(_("Refresh"));
	gtk_widget_show (mnuDBOpsRefresh);
	gtk_menu_append (GTK_MENU_SHELL(pGmlcGuiSrv->mnuBdOps), mnuDBOpsRefresh);
	/*g_signal_connect (G_OBJECT (mnuDBOpsRefresh), "activate", 
										G_CALLBACK (gmysqlcc_gui_server_evt_mnuDBOpsRefresh_activate), pGmlcGuiSrv);*/

	mnuDBOpsShowCreate = gtk_menu_item_new_with_label(_("Display all create structures"));
	gtk_widget_show (mnuDBOpsShowCreate);
	gtk_menu_append (GTK_MENU_SHELL(pGmlcGuiSrv->mnuBdOps), mnuDBOpsShowCreate);
	/*g_signal_connect (G_OBJECT (mnuDBOpsShowCreate), "activate", 
										G_CALLBACK (gmysqlcc_gui_server_evt_mnuDBOpsShowCreate_activate), pGmlcGuiSrv);*/

	
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

	pGmlcGuiSrv->lstTables = gtk_tree_view_new ();
	gtk_widget_show (pGmlcGuiSrv->lstTables);
	gtk_container_add (GTK_CONTAINER (scrolledwindow), pGmlcGuiSrv->lstTables);
	gtk_tree_view_set_rules_hint(GTK_TREE_VIEW (pGmlcGuiSrv->lstTables), TRUE);
	g_signal_connect (G_OBJECT (pGmlcGuiSrv->lstTables), "button-press-event", 
										G_CALLBACK (gmlc_gui_server_evt_lstTables_btnpress), pGmlcGuiSrv);
	select = gtk_tree_view_get_selection (GTK_TREE_VIEW (pGmlcGuiSrv->lstTables));
	gtk_tree_selection_set_mode (select, GTK_SELECTION_SINGLE);
	g_signal_connect (G_OBJECT (select), "changed", 
										G_CALLBACK (gmlc_gui_server_evt_lstTables_selected), pGmlcGuiSrv);

	hbuttonbox = gtk_hbutton_box_new ();
	gtk_widget_show (hbuttonbox);
	gtk_box_pack_start (GTK_BOX (vbox), hbuttonbox, FALSE, TRUE, 0);
	gtk_button_box_set_layout (GTK_BUTTON_BOX (hbuttonbox), GTK_BUTTONBOX_SPREAD);
	gtk_box_set_spacing (GTK_BOX (hbuttonbox), 2);
	
	btnTblAdd = gtk_button_new_from_stock(GTK_STOCK_ADD);
	gtk_widget_show (btnTblAdd);
	gtk_box_pack_start (GTK_BOX (hbuttonbox), btnTblAdd, TRUE, TRUE, 0);
	g_signal_connect (G_OBJECT (btnTblAdd), "clicked", 
										G_CALLBACK (gmlc_gui_server_evt_btnTblAdd_clicked), pGmlcGuiSrv);

	/*btnTblEdit = createIconButton("gtk-apply", _("Modify"));*/
	btnTblEdit = gtk_button_new_from_stock(GTK_STOCK_EDIT);
	gtk_widget_show (btnTblEdit);
	gtk_box_pack_start (GTK_BOX (hbuttonbox), btnTblEdit, TRUE, TRUE, 0);
	g_signal_connect (G_OBJECT (btnTblEdit), "clicked", 
										G_CALLBACK (gmlc_gui_server_evt_btnTblEdit_clicked), pGmlcGuiSrv);

	/*btnTblDump = createIconButton("gtk-save", _("Dump"));*/
	btnTblDump = gtk_button_new_from_stock(GTK_STOCK_SAVE_AS);
	gtk_widget_show (btnTblDump);
	gtk_box_pack_start (GTK_BOX (hbuttonbox), btnTblDump, TRUE, TRUE, 0);
	g_signal_connect (G_OBJECT (btnTblDump), "clicked", 
										G_CALLBACK (gmlc_gui_server_evt_btnTblDump_clicked), pGmlcGuiSrv);

	btnTblDel = gtk_button_new_from_stock(GTK_STOCK_DELETE);
	gtk_widget_show (btnTblDel);
	gtk_box_pack_start (GTK_BOX (hbuttonbox), btnTblDel, TRUE, TRUE, 0);
	g_signal_connect (G_OBJECT (btnTblDel), "clicked", 
										G_CALLBACK (gmlc_gui_server_evt_btnTblDel_clicked), pGmlcGuiSrv);

	pGmlcGuiSrv->mnuTblOps = gtk_menu_new();
	gtk_widget_show (pGmlcGuiSrv->mnuTblOps);

	mnuTBLOpsShowCreate = gtk_menu_item_new_with_label(_("Display create structure"));
	gtk_widget_show (mnuTBLOpsShowCreate);
	gtk_menu_append (GTK_MENU_SHELL(pGmlcGuiSrv->mnuTblOps), mnuTBLOpsShowCreate);
	/*g_signal_connect (G_OBJECT (mnuTBLOpsShowCreate), "activate", 
										G_CALLBACK (gmysqlcc_gui_server_evt_mnuTBLOpsShowCreate_activate), pGmlcGuiSrv);*/

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
		
		pGmlcGuiSrv->lstViews = gtk_tree_view_new ();
		gtk_widget_show (pGmlcGuiSrv->lstViews);
		gtk_container_add (GTK_CONTAINER (scrolledwindow), pGmlcGuiSrv->lstViews);
		gtk_tree_view_set_rules_hint(GTK_TREE_VIEW (pGmlcGuiSrv->lstViews), TRUE);
		/*g_signal_connect (G_OBJECT (pGmlcGuiSrv->lstViews), "button-press-event", 
											G_CALLBACK (gmlc_gui_server_evt_lstTable_btnpress), pGmlcGuiSrv);*/
		select = gtk_tree_view_get_selection (GTK_TREE_VIEW (pGmlcGuiSrv->lstViews));
		gtk_tree_selection_set_mode (select, GTK_SELECTION_SINGLE);
		/*g_signal_connect (G_OBJECT (select), "changed", 
											G_CALLBACK (gmlc_gui_server_evt_lstTable_selected), pGmlcGuiSrv);*/
		
		hbuttonbox = gtk_hbutton_box_new ();
		gtk_widget_show (hbuttonbox);
		gtk_box_pack_start (GTK_BOX (vbox), hbuttonbox, FALSE, TRUE, 0);
		gtk_button_box_set_layout (GTK_BUTTON_BOX (hbuttonbox), GTK_BUTTONBOX_SPREAD);
		gtk_box_set_spacing (GTK_BOX (hbuttonbox), 2);
		
		btnAdd = gtk_button_new_from_stock(GTK_STOCK_ADD);
		gtk_widget_show (btnAdd);
		gtk_box_pack_start (GTK_BOX (hbuttonbox), btnAdd, TRUE, TRUE, 0);
		/*g_signal_connect (G_OBJECT (btnAdd), "clicked", 
											G_CALLBACK (gmlc_gui_server_evt_btnVwAdd_clicked), pGmlcGuiSrv);*/
		
		btnEdit = gtk_button_new_from_stock(GTK_STOCK_EDIT);
		gtk_widget_show (btnEdit);
		gtk_box_pack_start (GTK_BOX (hbuttonbox), btnEdit, TRUE, TRUE, 0);
		/*g_signal_connect (G_OBJECT (btnEdit), "clicked", 
											G_CALLBACK (gmlc_gui_server_evt_btnVwEdit_clicked), pGmlcGuiSrv);*/
		
		btnDump = gtk_button_new_from_stock(GTK_STOCK_SAVE_AS);
		gtk_widget_show (btnDump);
		gtk_box_pack_start (GTK_BOX (hbuttonbox), btnDump, TRUE, TRUE, 0);
		/*g_signal_connect (G_OBJECT (btnDump), "clicked", 
											G_CALLBACK (gmlc_gui_server_evt_btnVwDump_clicked), pGmlcGuiSrv);*/
		
		btnDel = gtk_button_new_from_stock(GTK_STOCK_DELETE);
		gtk_widget_show (btnDel);
		gtk_box_pack_start (GTK_BOX (hbuttonbox), btnDel, TRUE, TRUE, 0);
		/*g_signal_connect (G_OBJECT (btnDel), "clicked", 
											G_CALLBACK (gmlc_gui_server_evt_btnVwDel_clicked), pGmlcGuiSrv);*/
		
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
		
		pGmlcGuiSrv->lstProcedures = gtk_tree_view_new ();
		gtk_widget_show (pGmlcGuiSrv->lstProcedures);
		gtk_container_add (GTK_CONTAINER (scrolledwindow), pGmlcGuiSrv->lstProcedures);
		gtk_tree_view_set_rules_hint(GTK_TREE_VIEW (pGmlcGuiSrv->lstProcedures), TRUE);
		/*g_signal_connect (G_OBJECT (pGmlcGuiSrv->lstProcedures), "button-press-event", 
											G_CALLBACK (gmlc_gui_server_evt_lstTable_btnpress), pGmlcGuiSrv);*/
		select = gtk_tree_view_get_selection (GTK_TREE_VIEW (pGmlcGuiSrv->lstProcedures));
		gtk_tree_selection_set_mode (select, GTK_SELECTION_SINGLE);
		/*g_signal_connect (G_OBJECT (select), "changed", 
											G_CALLBACK (gmlc_gui_server_evt_lstTable_selected), pGmlcGuiSrv);*/
		
		hbuttonbox = gtk_hbutton_box_new ();
		gtk_widget_show (hbuttonbox);
		gtk_box_pack_start (GTK_BOX (vbox), hbuttonbox, FALSE, TRUE, 0);
		gtk_button_box_set_layout (GTK_BUTTON_BOX (hbuttonbox), GTK_BUTTONBOX_SPREAD);
		gtk_box_set_spacing (GTK_BOX (hbuttonbox), 2);
		
		btnAdd = gtk_button_new_from_stock(GTK_STOCK_ADD);
		gtk_widget_show (btnAdd);
		gtk_box_pack_start (GTK_BOX (hbuttonbox), btnAdd, TRUE, TRUE, 0);
		/*g_signal_connect (G_OBJECT (btnAdd), "clicked", 
											G_CALLBACK (gmlc_gui_server_evt_btnVwAdd_clicked), pGmlcGuiSrv);*/
		
		btnEdit = gtk_button_new_from_stock(GTK_STOCK_EDIT);
		gtk_widget_show (btnEdit);
		gtk_box_pack_start (GTK_BOX (hbuttonbox), btnEdit, TRUE, TRUE, 0);
		/*g_signal_connect (G_OBJECT (btnEdit), "clicked", 
											G_CALLBACK (gmlc_gui_server_evt_btnVwEdit_clicked), pGmlcGuiSrv);*/
		
		btnDump = gtk_button_new_from_stock(GTK_STOCK_SAVE_AS);
		gtk_widget_show (btnDump);
		gtk_box_pack_start (GTK_BOX (hbuttonbox), btnDump, TRUE, TRUE, 0);
		/*g_signal_connect (G_OBJECT (btnDump), "clicked", 
											G_CALLBACK (gmlc_gui_server_evt_btnVwDump_clicked), pGmlcGuiSrv);*/
		
		btnDel = gtk_button_new_from_stock(GTK_STOCK_DELETE);
		gtk_widget_show (btnDel);
		gtk_box_pack_start (GTK_BOX (hbuttonbox), btnDel, TRUE, TRUE, 0);
		/*g_signal_connect (G_OBJECT (btnDel), "clicked", 
											G_CALLBACK (gmlc_gui_server_evt_btnVwDel_clicked), pGmlcGuiSrv);*/
		
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
		
		pGmlcGuiSrv->lstFunctions = gtk_tree_view_new ();
		gtk_widget_show (pGmlcGuiSrv->lstFunctions);
		gtk_container_add (GTK_CONTAINER (scrolledwindow), pGmlcGuiSrv->lstFunctions);
		gtk_tree_view_set_rules_hint(GTK_TREE_VIEW (pGmlcGuiSrv->lstFunctions), TRUE);
		/*g_signal_connect (G_OBJECT (pGmlcGuiSrv->lstFunctions), "button-press-event", 
											G_CALLBACK (gmlc_gui_server_evt_lstTable_btnpress), pGmlcGuiSrv);*/
		select = gtk_tree_view_get_selection (GTK_TREE_VIEW (pGmlcGuiSrv->lstFunctions));
		gtk_tree_selection_set_mode (select, GTK_SELECTION_SINGLE);
		/*g_signal_connect (G_OBJECT (select), "changed", 
											G_CALLBACK (gmlc_gui_server_evt_lstTable_selected), pGmlcGuiSrv);*/
		
		hbuttonbox = gtk_hbutton_box_new ();
		gtk_widget_show (hbuttonbox);
		gtk_box_pack_start (GTK_BOX (vbox), hbuttonbox, FALSE, TRUE, 0);
		gtk_button_box_set_layout (GTK_BUTTON_BOX (hbuttonbox), GTK_BUTTONBOX_SPREAD);
		gtk_box_set_spacing (GTK_BOX (hbuttonbox), 2);
		
		btnAdd = gtk_button_new_from_stock(GTK_STOCK_ADD);
		gtk_widget_show (btnAdd);
		gtk_box_pack_start (GTK_BOX (hbuttonbox), btnAdd, TRUE, TRUE, 0);
		/*g_signal_connect (G_OBJECT (btnAdd), "clicked", 
											G_CALLBACK (gmlc_gui_server_evt_btnVwAdd_clicked), pGmlcGuiSrv);*/
		
		btnEdit = gtk_button_new_from_stock(GTK_STOCK_EDIT);
		gtk_widget_show (btnEdit);
		gtk_box_pack_start (GTK_BOX (hbuttonbox), btnEdit, TRUE, TRUE, 0);
		/*g_signal_connect (G_OBJECT (btnEdit), "clicked", 
											G_CALLBACK (gmlc_gui_server_evt_btnVwEdit_clicked), pGmlcGuiSrv);*/
		
		btnDump = gtk_button_new_from_stock(GTK_STOCK_SAVE_AS);
		gtk_widget_show (btnDump);
		gtk_box_pack_start (GTK_BOX (hbuttonbox), btnDump, TRUE, TRUE, 0);
		/*g_signal_connect (G_OBJECT (btnDump), "clicked", 
											G_CALLBACK (gmlc_gui_server_evt_btnVwDump_clicked), pGmlcGuiSrv);*/
		
		btnDel = gtk_button_new_from_stock(GTK_STOCK_DELETE);
		gtk_widget_show (btnDel);
		gtk_box_pack_start (GTK_BOX (hbuttonbox), btnDel, TRUE, TRUE, 0);
		/*g_signal_connect (G_OBJECT (btnDel), "clicked", 
											G_CALLBACK (gmlc_gui_server_evt_btnVwDel_clicked), pGmlcGuiSrv);*/
		
	}
	
	
}

void gmlc_gui_server_init_widget_tab_data (GmlcGuiServer * pGmlcGuiSrv) {
	GtkTreeViewColumn * currCol;
	GtkCellRenderer * renderer;
	GtkListStore * lstEmpty;
	
	/* Generate columns for all tree views */
	renderer = gtk_cell_renderer_text_new ();
	
	/* Databases columns */
	currCol = gtk_tree_view_column_new_with_attributes (_("Databases"), renderer, "text", 0, NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW (pGmlcGuiSrv->lstBase), currCol);
	
	lstEmpty = gtk_list_store_new(1, G_TYPE_STRING);
	gtk_tree_view_set_model(GTK_TREE_VIEW(pGmlcGuiSrv->lstBase), GTK_TREE_MODEL(lstEmpty));
	g_object_unref (G_OBJECT (lstEmpty));
	
	/* Table columns */
	currCol = gtk_tree_view_column_new_with_attributes (_("Name"), renderer, "text", 0, NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW (pGmlcGuiSrv->lstTables), currCol);
	currCol = gtk_tree_view_column_new_with_attributes (_("Rows"), renderer, "text", 1, NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW (pGmlcGuiSrv->lstTables), currCol);
	currCol = gtk_tree_view_column_new_with_attributes (_("Size"), renderer, "text", 2, NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW (pGmlcGuiSrv->lstTables), currCol);
	currCol = gtk_tree_view_column_new_with_attributes (_("Type"), renderer, "text", 3, NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW (pGmlcGuiSrv->lstTables), currCol);
	
	lstEmpty = gtk_list_store_new(4, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);
	gtk_tree_view_set_model(GTK_TREE_VIEW(pGmlcGuiSrv->lstTables), GTK_TREE_MODEL(lstEmpty));
	g_object_unref (G_OBJECT (lstEmpty));
	
	/* View columns */
	currCol = gtk_tree_view_column_new_with_attributes (_("Name"), renderer, "text", 0, NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW (pGmlcGuiSrv->lstViews), currCol);
	
	lstEmpty = gtk_list_store_new(1, G_TYPE_STRING);
	gtk_tree_view_set_model(GTK_TREE_VIEW(pGmlcGuiSrv->lstViews), GTK_TREE_MODEL(lstEmpty));
	g_object_unref (G_OBJECT (lstEmpty));
	
	/* Procedure columns */
	currCol = gtk_tree_view_column_new_with_attributes (_("Name"), renderer, "text", 0, NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW (pGmlcGuiSrv->lstProcedures), currCol);
	
	lstEmpty = gtk_list_store_new(1, G_TYPE_STRING);
	gtk_tree_view_set_model(GTK_TREE_VIEW(pGmlcGuiSrv->lstProcedures), GTK_TREE_MODEL(lstEmpty));
	g_object_unref (G_OBJECT (lstEmpty));
	
	/* Function columns */
	currCol = gtk_tree_view_column_new_with_attributes (_("Name"), renderer, "text", 0, NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW (pGmlcGuiSrv->lstFunctions), currCol);
	
	lstEmpty = gtk_list_store_new(1, G_TYPE_STRING);
	gtk_tree_view_set_model(GTK_TREE_VIEW(pGmlcGuiSrv->lstFunctions), GTK_TREE_MODEL(lstEmpty));
	g_object_unref (G_OBJECT (lstEmpty));
	
	gmlc_gui_server_fill_database_list(pGmlcGuiSrv);
}

void gmlc_gui_server_fill_database_list (GmlcGuiServer * pGmlcGuiSrv) {
	GtkListStore * lstStrBase = NULL, * lstEmpty = NULL;

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
	
	if (gmlc_mysql_server_update_databases_list(pGmlcGuiSrv->pGmlcMysqlSrv)) {
		g_hash_table_foreach(pGmlcGuiSrv->pGmlcMysqlSrv->htbDatabases, &sub_ht_fill_database_list, lstStrBase);
	}
	
	gtk_tree_view_set_model(GTK_TREE_VIEW(pGmlcGuiSrv->lstBase), GTK_TREE_MODEL(lstStrBase));
	g_object_unref (G_OBJECT (lstStrBase));
	
	lstEmpty = gtk_list_store_new(4, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);
	gtk_tree_view_set_model(GTK_TREE_VIEW(pGmlcGuiSrv->lstTables), GTK_TREE_MODEL(lstEmpty));
	g_object_unref (G_OBJECT (lstEmpty));
	
	lstEmpty = gtk_list_store_new(1, G_TYPE_STRING);
	gtk_tree_view_set_model(GTK_TREE_VIEW(pGmlcGuiSrv->lstViews), GTK_TREE_MODEL(lstEmpty));
	g_object_unref (G_OBJECT (lstEmpty));
	
	lstEmpty = gtk_list_store_new(1, G_TYPE_STRING);
	gtk_tree_view_set_model(GTK_TREE_VIEW(pGmlcGuiSrv->lstProcedures), GTK_TREE_MODEL(lstEmpty));
	g_object_unref (G_OBJECT (lstEmpty));
	
	lstEmpty = gtk_list_store_new(1, G_TYPE_STRING);
	gtk_tree_view_set_model(GTK_TREE_VIEW(pGmlcGuiSrv->lstFunctions), GTK_TREE_MODEL(lstEmpty));
	g_object_unref (G_OBJECT (lstEmpty));
	
	pGmlcGuiSrv->pGmlcMysqlDb = NULL;
	pGmlcGuiSrv->pGmlcMysqlTbl = NULL;
	pGmlcGuiSrv->pGmlcMysqlVw = NULL;
	pGmlcGuiSrv->pGmlcMysqlProc = NULL;
	pGmlcGuiSrv->pGmlcMysqlFunc = NULL;
}

void gmlc_gui_server_fill_tables_list (GmlcGuiServer * pGmlcGuiSrv) {
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
	if (gmlc_mysql_database_update_tables_list(pGmlcGuiSrv->pGmlcMysqlDb)) {
		g_hash_table_foreach(pGmlcGuiSrv->pGmlcMysqlDb->htbTables, &sub_ht_fill_table_list, lstStrTable);
	}
	
	gtk_tree_view_set_model(GTK_TREE_VIEW(pGmlcGuiSrv->lstTables), GTK_TREE_MODEL(lstStrTable));
	g_object_unref (G_OBJECT (lstStrTable));
	
	pGmlcGuiSrv->pGmlcMysqlTbl = NULL;
}

void gmlc_gui_server_fill_views_list (GmlcGuiServer * pGmlcGuiSrv) {
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
	if (gmlc_mysql_database_update_views_list(pGmlcGuiSrv->pGmlcMysqlDb)) {
		g_hash_table_foreach(pGmlcGuiSrv->pGmlcMysqlDb->htbViews, sub_ht_fill_view_list, lstStrView);
	}
	
	gtk_tree_view_set_model(GTK_TREE_VIEW(pGmlcGuiSrv->lstViews), GTK_TREE_MODEL(lstStrView));
	g_object_unref (G_OBJECT (lstStrView));
	
	pGmlcGuiSrv->pGmlcMysqlVw = NULL;
}

void gmlc_gui_server_fill_procedures_list (GmlcGuiServer * pGmlcGuiSrv) {
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
	if (gmlc_mysql_database_update_views_list(pGmlcGuiSrv->pGmlcMysqlDb)) {
		g_hash_table_foreach(pGmlcGuiSrv->pGmlcMysqlDb->htbProcedures, sub_ht_fill_procedure_list, lstStrProcedure);
	}
	
	gtk_tree_view_set_model(GTK_TREE_VIEW(pGmlcGuiSrv->lstProcedures), GTK_TREE_MODEL(lstStrProcedure));
	g_object_unref (G_OBJECT (lstStrProcedure));
	
	pGmlcGuiSrv->pGmlcMysqlProc = NULL;
}

void gmlc_gui_server_fill_functions_list (GmlcGuiServer * pGmlcGuiSrv) {
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
	if (gmlc_mysql_database_update_views_list(pGmlcGuiSrv->pGmlcMysqlDb)) {
		g_hash_table_foreach(pGmlcGuiSrv->pGmlcMysqlDb->htbFunctions, sub_ht_fill_function_list, lstStrFunction);
	}
	
	gtk_tree_view_set_model(GTK_TREE_VIEW(pGmlcGuiSrv->lstFunctions), GTK_TREE_MODEL(lstStrFunction));
	g_object_unref (G_OBJECT (lstStrFunction));
	
	pGmlcGuiSrv->pGmlcMysqlFunc = NULL;
}

gboolean gmlc_gui_server_evt_lstBase_btnpress (GtkWidget *widget, GdkEventButton *event, gpointer user_data) {
	GmlcGuiServer * pGmlcGuiSrv = GMLC_GUI_SERVER(user_data);
	
	switch (event->button) {
		case 1 : /* Left button */
			if (event->type == GDK_2BUTTON_PRESS) {
				gmlc_gui_server_open_query_window (pGmlcGuiSrv, FALSE);
			}
			break;
		case 3 : /* Right button */
			if (event->type == GDK_BUTTON_PRESS) {
				gtk_menu_popup(GTK_MENU(pGmlcGuiSrv->mnuBdOps), NULL, NULL, NULL, user_data, event->button, event->time);
			}
			break;
	}
	return FALSE;
	
}

gboolean gmlc_gui_server_evt_lstTables_btnpress (GtkWidget *widget, GdkEventButton *event, gpointer user_data) {
	GmlcGuiServer * pGmlcGuiSrv = GMLC_GUI_SERVER(user_data);
	
	switch (event->button) {
		case 1 : /* Left button */
			if (event->type == GDK_2BUTTON_PRESS) {
				gmlc_gui_server_open_query_window (pGmlcGuiSrv, TRUE);
			}
			break;
		case 3 : /* Right button */
			if (event->type == GDK_BUTTON_PRESS) {
				gtk_menu_popup(GTK_MENU(pGmlcGuiSrv->mnuTblOps), NULL, NULL, NULL, user_data, event->button, event->time);
			}
			break;
	}
	return FALSE;
}

void gmlc_gui_server_evt_lstBase_selected (GtkTreeSelection *selection, gpointer user_data) {
	GmlcGuiServer * pGmlcGuiSrv = GMLC_GUI_SERVER(user_data);
	GtkTreeIter iter;
	GtkTreeModel *model;
	
	g_print("Changed !\n");
	if (gtk_tree_selection_get_selected (selection, &model, &iter)) {
		gtk_tree_model_get (model, &iter, 1, &pGmlcGuiSrv->pGmlcMysqlDb, -1);
		
		gmlc_gui_server_fill_tables_list(pGmlcGuiSrv);
		gmlc_gui_server_fill_views_list(pGmlcGuiSrv);
		gmlc_gui_server_fill_procedures_list(pGmlcGuiSrv);
		gmlc_gui_server_fill_functions_list(pGmlcGuiSrv);
	}
}

void gmlc_gui_server_evt_lstTables_selected (GtkTreeSelection *selection, gpointer user_data) {
	GmlcGuiServer * pGmlcGuiSrv = GMLC_GUI_SERVER(user_data);
	GtkTreeIter iter;
	GtkTreeModel *model;
	
	g_print("Changed !\n");
	if (gtk_tree_selection_get_selected (selection, &model, &iter)) {
		gtk_tree_model_get (model, &iter, 4, &pGmlcGuiSrv->pGmlcMysqlTbl, -1);
	}
}

void gmlc_gui_server_evt_btnDbAdd_clicked (GtkWidget *widget, gpointer user_data) {
	GmlcGuiServer * pGmlcGuiSrv = GMLC_GUI_SERVER(user_data);
	UNUSED_VAR(widget);
	
	gmlc_gui_server_open_query_window(pGmlcGuiSrv, FALSE);
}

void gmlc_gui_server_evt_btnDbDel_clicked (GtkWidget *widget, gpointer user_data) {
	UNUSED_VAR(widget);
	UNUSED_VAR(user_data);
	
}

void gmlc_gui_server_evt_btnTblAdd_clicked (GtkWidget *widget, gpointer user_data) {
	UNUSED_VAR(widget);
	UNUSED_VAR(user_data);
	
}

void gmlc_gui_server_evt_btnTblEdit_clicked (GtkWidget *widget, gpointer user_data) {
	UNUSED_VAR(widget);
	UNUSED_VAR(user_data);
	
}

void gmlc_gui_server_evt_btnTblDump_clicked (GtkWidget *widget, gpointer user_data) {
	UNUSED_VAR(widget);
	UNUSED_VAR(user_data);
	
}

void gmlc_gui_server_evt_btnTblDel_clicked (GtkWidget *widget, gpointer user_data) {
	UNUSED_VAR(widget);
	UNUSED_VAR(user_data);
	
}

