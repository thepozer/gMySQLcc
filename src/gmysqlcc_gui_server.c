
#include "gmysqlcc_gui_all.h"

void gmysqlcc_gui_server_create_widget (p_gmysqlcc_gui_server gui_server);
void gmysqlcc_gui_server_init_widget (p_gmysqlcc_gui_server gui_server);

void gmysqlcc_gui_server_fill_database_list (p_gmysqlcc_gui_server gui_server);
void gmysqlcc_gui_server_fill_table_list (p_gmysqlcc_gui_server gui_server);
void gmysqlcc_gui_server_fill_user_list (p_gmysqlcc_gui_server gui_server);
void gmysqlcc_gui_server_open_query_window (p_gmysqlcc_gui_server gui_server, gboolean use_table);

void gmysqlcc_gui_server_evt_destroy (GtkWidget *widget, gpointer user_data);
void gmysqlcc_gui_server_evt_mnuDBOpsRefresh_activate (GtkWidget *widget, gpointer user_data);
void gmysqlcc_gui_server_evt_mnuDBOpsShowCreate_activate (GtkWidget *widget, gpointer user_data);
void gmysqlcc_gui_server_evt_mnuTBLOpsShowCreate_activate (GtkWidget *widget, gpointer user_data);
gboolean gmysqlcc_gui_server_evt_lstBase_btnpress (GtkWidget *widget, GdkEventButton *event, gpointer user_data);
gboolean gmysqlcc_gui_server_evt_lstTable_btnpress (GtkWidget *widget, GdkEventButton *event, gpointer user_data);
void gmysqlcc_gui_server_evt_lstBase_selected (GtkTreeSelection *selection, gpointer user_data);
void gmysqlcc_gui_server_evt_lstTable_selected (GtkTreeSelection *selection, gpointer user_data);
void gmysqlcc_gui_server_evt_btnTlbrSql_clicked (GtkWidget *widget, gpointer user_data);
void gmysqlcc_gui_server_evt_btnTlbrSqlFile_clicked (GtkWidget *widget, gpointer user_data);
void gmysqlcc_gui_server_evt_btnTlbrClose_clicked (GtkWidget *widget, gpointer user_data);
void gmysqlcc_gui_server_evt_btnDbAdd_clicked (GtkWidget *widget, gpointer user_data);
void gmysqlcc_gui_server_evt_btnDbDel_clicked (GtkWidget *widget, gpointer user_data);
void gmysqlcc_gui_server_evt_btnTblAdd_clicked (GtkWidget *widget, gpointer user_data);
void gmysqlcc_gui_server_evt_btnTblEdit_clicked (GtkWidget *widget, gpointer user_data);
void gmysqlcc_gui_server_evt_btnTblDump_clicked (GtkWidget *widget, gpointer user_data);
void gmysqlcc_gui_server_evt_btnTblDel_clicked (GtkWidget *widget, gpointer user_data);

	
p_gmysqlcc_gui_server gmysqlcc_gui_server_new (p_mysql_server mysql_srv) {
	p_gmysqlcc_gui_server gui_server;
	
	/* Init structure */
	gui_server = (p_gmysqlcc_gui_server) g_try_malloc(sizeof(s_gmysqlcc_gui_server));
	
	if (gui_server == NULL) {
		return NULL; /* return NULL pointer */
	}
	
	gui_server->mysql_srv = mysql_srv;
	gui_server->curr_mysql_db = NULL;
	gui_server->curr_mysql_tbl = NULL;

	gmysqlcc_gui_server_create_widget(gui_server);
	gmysqlcc_gui_server_init_widget(gui_server);
	
	NbrWnd ++;
	
	return gui_server;
}

gboolean gmysqlcc_gui_server_display (p_gmysqlcc_gui_server gui_server, gboolean display) {
	if (display) {
		gtk_widget_show (gui_server->window);
	} else {
		gtk_widget_hide (gui_server->window);
	}
	
	return TRUE;
}

gboolean gmysqlcc_gui_server_delete (p_gmysqlcc_gui_server gui_server) {
	/* Destroy Datas */
	g_free(gui_server);
	
	/* Destroy Application if needed */
	NbrWnd--;
	g_printerr("Destruction Server window - nbrWnd : %d\n", NbrWnd);
	if (NbrWnd <= 0) {
		g_printerr("Destroy App\n");
		gtk_main_quit();
	}
	
	return TRUE;
}

void gmysqlcc_gui_server_create_widget (p_gmysqlcc_gui_server gui_server) {
	GtkWidget *statusbar1;
	GtkWidget *vbox1, *vbox2, *vbox3;
	GtkWidget *hpaned1;
	GtkWidget *hbox2, *hbox3;
	GtkWidget *label1, *label2, *label3, *label6, *label7;
	GtkWidget *notebook1;
	GtkWidget *scrolledwindow1, *scrolledwindow2;
	GtkWidget *toolbar1;
	GtkWidget * imgToolbar;
	GtkToolItem * btnTlbrClose, * btnTlbrSql, * btnTlbrSqlFile;
	GtkWidget *btnDbAdd, *btnDbDel;
	GtkWidget *btnTblAdd, *btnTblEdit, *btnTblDump, *btnTblDel;
	GtkWidget *mnuDBOpsRefresh;
	GtkWidget *mnuDBOpsShowCreate;
	GtkWidget *mnuTBLOpsShowCreate;
	GtkTreeSelection *select;
	GtkTooltips * tooltips;
	GString * sTitle;

	tooltips = gtk_tooltips_new();

	gui_server->window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	sTitle = g_string_new("");
	g_string_printf(sTitle, _("Server : %s"), gui_server->mysql_srv->name);
	gtk_window_set_title (GTK_WINDOW (gui_server->window), sTitle->str);
	g_string_free(sTitle, TRUE);
	gtk_window_set_default_size (GTK_WINDOW (gui_server->window), 400, 300);
	g_signal_connect (G_OBJECT (gui_server->window), "destroy", 
										G_CALLBACK (gmysqlcc_gui_server_evt_destroy), (gpointer)gui_server);

	vbox1 = gtk_vbox_new (FALSE, 0);
	gtk_widget_show (vbox1);
	gtk_container_add (GTK_CONTAINER (gui_server->window), vbox1);



	toolbar1 = gtk_toolbar_new ();
	gtk_widget_show (toolbar1);
	gtk_box_pack_start (GTK_BOX (vbox1), toolbar1, FALSE, FALSE, 0);
	gtk_toolbar_set_style (GTK_TOOLBAR (toolbar1), GTK_TOOLBAR_BOTH_HORIZ);

	imgToolbar = gtk_image_new_from_stock(GTK_STOCK_EXECUTE, GTK_ICON_SIZE_LARGE_TOOLBAR);
	gtk_widget_show(imgToolbar);
	btnTlbrSql = gtk_tool_button_new (imgToolbar, _("SQL"));
	gtk_tool_item_set_is_important (GTK_TOOL_ITEM(btnTlbrSql), TRUE);
	g_signal_connect (G_OBJECT (btnTlbrSql), "clicked", 
										G_CALLBACK (gmysqlcc_gui_server_evt_btnTlbrSql_clicked), (gpointer)gui_server);
	gtk_widget_show(GTK_WIDGET(btnTlbrSql));
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar1), GTK_TOOL_ITEM(btnTlbrSql), -1);
	gtk_tool_item_set_tooltip (GTK_TOOL_ITEM(btnTlbrSql), tooltips, _("Exec SQL Query"), NULL);

	imgToolbar = gtk_image_new_from_stock(GTK_STOCK_EXECUTE, GTK_ICON_SIZE_LARGE_TOOLBAR);
	gtk_widget_show(imgToolbar);
	btnTlbrSqlFile = gtk_tool_button_new (imgToolbar, _("SQL File"));
	gtk_tool_item_set_is_important (GTK_TOOL_ITEM(btnTlbrSqlFile), TRUE);
	g_signal_connect (G_OBJECT (btnTlbrSqlFile), "clicked", 
										G_CALLBACK (gmysqlcc_gui_server_evt_btnTlbrSqlFile_clicked), (gpointer)gui_server);
	gtk_widget_show(GTK_WIDGET(btnTlbrSqlFile));
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar1), GTK_TOOL_ITEM(btnTlbrSqlFile), -1);
	gtk_tool_item_set_tooltip (GTK_TOOL_ITEM(btnTlbrSqlFile), tooltips, _("Exec SQL File"), NULL);

	imgToolbar = gtk_image_new_from_stock(GTK_STOCK_CLOSE, GTK_ICON_SIZE_LARGE_TOOLBAR);
	gtk_widget_show(imgToolbar);
	btnTlbrClose = gtk_tool_button_new (imgToolbar, _("Close"));
	gtk_tool_item_set_is_important (GTK_TOOL_ITEM(btnTlbrClose), TRUE);
	g_signal_connect (G_OBJECT (btnTlbrClose), "clicked", 
										G_CALLBACK (gmysqlcc_gui_server_evt_btnTlbrClose_clicked), (gpointer)gui_server);
	gtk_widget_show(GTK_WIDGET(btnTlbrClose));
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar1), GTK_TOOL_ITEM(btnTlbrClose), -1);
	gtk_tool_item_set_tooltip (GTK_TOOL_ITEM(btnTlbrClose), tooltips, _("Close window"), NULL);



	notebook1 = gtk_notebook_new ();
	gtk_widget_show (notebook1);
	gtk_box_pack_start (GTK_BOX (vbox1), notebook1, TRUE, TRUE, 0);

	hpaned1 = gtk_hpaned_new ();
	gtk_widget_show (hpaned1);
	gtk_container_add (GTK_CONTAINER (notebook1), hpaned1);
	gtk_paned_set_position (GTK_PANED (hpaned1), 200);



	vbox2 = gtk_vbox_new (FALSE, 0);
	gtk_widget_show (vbox2);
	gtk_paned_pack1 (GTK_PANED (hpaned1), vbox2, FALSE, TRUE);

	scrolledwindow1 = gtk_scrolled_window_new (NULL, NULL);
	gtk_widget_show (scrolledwindow1);
	gtk_box_pack_start (GTK_BOX (vbox2), scrolledwindow1, TRUE, TRUE, 0);
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow1), GTK_POLICY_NEVER, GTK_POLICY_ALWAYS);

	gui_server->lstBase = gtk_tree_view_new ();
	gtk_widget_show (gui_server->lstBase);
	gtk_tree_view_set_rules_hint(GTK_TREE_VIEW (gui_server->lstBase), TRUE);
	gtk_container_add (GTK_CONTAINER (scrolledwindow1), gui_server->lstBase);
	g_signal_connect (G_OBJECT (gui_server->lstBase), "button-press-event", 
										G_CALLBACK (gmysqlcc_gui_server_evt_lstBase_btnpress), (gpointer)gui_server);
	select = gtk_tree_view_get_selection (GTK_TREE_VIEW (gui_server->lstBase));
	gtk_tree_selection_set_mode (select, GTK_SELECTION_SINGLE);
	g_signal_connect (G_OBJECT (select), "changed", 
										G_CALLBACK (gmysqlcc_gui_server_evt_lstBase_selected), (gpointer)gui_server);

	hbox2 = gtk_hbox_new (FALSE, 0);
	gtk_widget_show (hbox2);
	gtk_box_pack_start (GTK_BOX (vbox2), hbox2, FALSE, TRUE, 0);

	btnDbAdd = createIconButton("gtk-add", _("Add"));
	gtk_widget_show (btnDbAdd);
	gtk_box_pack_start (GTK_BOX (hbox2), btnDbAdd, TRUE, TRUE, 0);
	g_signal_connect (G_OBJECT (btnDbAdd), "clicked", 
										G_CALLBACK (gmysqlcc_gui_server_evt_btnDbAdd_clicked), (gpointer)gui_server);

	btnDbDel = createIconButton("gtk-delete", _("Delete"));
	gtk_widget_show (btnDbDel);
	gtk_box_pack_start (GTK_BOX (hbox2), btnDbDel, TRUE, TRUE, 0);
	g_signal_connect (G_OBJECT (btnDbDel), "clicked", 
										G_CALLBACK (gmysqlcc_gui_server_evt_btnDbDel_clicked), (gpointer)gui_server);


	vbox3 = gtk_vbox_new (FALSE, 0);
	gtk_widget_show (vbox3);
	gtk_paned_pack2 (GTK_PANED (hpaned1), vbox3, TRUE, TRUE);

	scrolledwindow2 = gtk_scrolled_window_new (NULL, NULL);
	gtk_widget_show (scrolledwindow2);
	gtk_box_pack_start (GTK_BOX (vbox3), scrolledwindow2, TRUE, TRUE, 0);
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow2), GTK_POLICY_NEVER, GTK_POLICY_ALWAYS);

	gui_server->lstTable = gtk_tree_view_new ();
	gtk_widget_show (gui_server->lstTable);
	gtk_container_add (GTK_CONTAINER (scrolledwindow2), gui_server->lstTable);
	gtk_tree_view_set_rules_hint(GTK_TREE_VIEW (gui_server->lstTable), TRUE);
	g_signal_connect (G_OBJECT (gui_server->lstTable), "button-press-event", 
										G_CALLBACK (gmysqlcc_gui_server_evt_lstTable_btnpress), (gpointer)gui_server);
	select = gtk_tree_view_get_selection (GTK_TREE_VIEW (gui_server->lstTable));
	gtk_tree_selection_set_mode (select, GTK_SELECTION_SINGLE);
	g_signal_connect (G_OBJECT (select), "changed", 
										G_CALLBACK (gmysqlcc_gui_server_evt_lstTable_selected), (gpointer)gui_server);

	hbox3 = gtk_hbox_new (FALSE, 0);
	gtk_widget_show (hbox3);
	gtk_box_pack_start (GTK_BOX (vbox3), hbox3, FALSE, TRUE, 0);

	btnTblAdd = createIconButton("gtk-add", _("Add"));
	gtk_widget_show (btnTblAdd);
	gtk_box_pack_start (GTK_BOX (hbox3), btnTblAdd, TRUE, TRUE, 0);
	g_signal_connect (G_OBJECT (btnTblAdd), "clicked", 
										G_CALLBACK (gmysqlcc_gui_server_evt_btnTblAdd_clicked), (gpointer)gui_server);

	btnTblEdit = createIconButton("gtk-apply", _("Modify"));
	gtk_widget_show (btnTblEdit);
	gtk_box_pack_start (GTK_BOX (hbox3), btnTblEdit, TRUE, TRUE, 0);
	g_signal_connect (G_OBJECT (btnTblEdit), "clicked", 
										G_CALLBACK (gmysqlcc_gui_server_evt_btnTblEdit_clicked), (gpointer)gui_server);

	btnTblDump = createIconButton("gtk-save", _("Dump"));
	gtk_widget_show (btnTblDump);
	gtk_box_pack_start (GTK_BOX (hbox3), btnTblDump, TRUE, TRUE, 0);
	g_signal_connect (G_OBJECT (btnTblDump), "clicked", 
										G_CALLBACK (gmysqlcc_gui_server_evt_btnTblDump_clicked), (gpointer)gui_server);

	btnTblDel = createIconButton("gtk-delete", _("Delete"));
	gtk_widget_show (btnTblDel);
	gtk_box_pack_start (GTK_BOX (hbox3), btnTblDel, TRUE, TRUE, 0);
	g_signal_connect (G_OBJECT (btnTblDel), "clicked", 
										G_CALLBACK (gmysqlcc_gui_server_evt_btnTblDel_clicked), (gpointer)gui_server);

	label1 = gtk_label_new (_("Databases"));
	gtk_widget_show (label1);
	gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook1), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook1), 0), label1);
	gtk_label_set_justify (GTK_LABEL (label1), GTK_JUSTIFY_LEFT);



	label6 = gtk_label_new (_("In construction"));
	gtk_widget_show (label6);
	gtk_container_add (GTK_CONTAINER (notebook1), label6);
	gtk_label_set_justify (GTK_LABEL (label6), GTK_JUSTIFY_LEFT);

	label2 = gtk_label_new (_("Users"));
	gtk_widget_show (label2);
	gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook1), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook1), 1), label2);
	gtk_label_set_justify (GTK_LABEL (label2), GTK_JUSTIFY_LEFT);



	label7 = gtk_label_new (_("In construction"));
	gtk_widget_show (label7);
	gtk_container_add (GTK_CONTAINER (notebook1), label7);
	gtk_label_set_justify (GTK_LABEL (label7), GTK_JUSTIFY_LEFT);

	label3 = gtk_label_new (_("Server"));
	gtk_widget_show (label3);
	gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook1), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook1), 2), label3);
	gtk_label_set_justify (GTK_LABEL (label3), GTK_JUSTIFY_LEFT);



	statusbar1 = gtk_statusbar_new ();
	gtk_widget_show (statusbar1);
	gtk_box_pack_start (GTK_BOX (vbox1), statusbar1, FALSE, FALSE, 0);



	gui_server->mnuBdOps = gtk_menu_new();
	gtk_widget_show (gui_server->mnuBdOps);

	mnuDBOpsRefresh = gtk_menu_item_new_with_label(_("Refresh"));
	gtk_widget_show (mnuDBOpsRefresh);
	gtk_menu_append (GTK_MENU_SHELL(gui_server->mnuBdOps), mnuDBOpsRefresh);
	g_signal_connect (G_OBJECT (mnuDBOpsRefresh), "activate", 
										G_CALLBACK (gmysqlcc_gui_server_evt_mnuDBOpsRefresh_activate), 
										(gpointer)gui_server);

	mnuDBOpsShowCreate = gtk_menu_item_new_with_label(_("Display all create structures"));
	gtk_widget_show (mnuDBOpsShowCreate);
	gtk_menu_append (GTK_MENU_SHELL(gui_server->mnuBdOps), mnuDBOpsShowCreate);
	g_signal_connect (G_OBJECT (mnuDBOpsShowCreate), "activate", 
										G_CALLBACK (gmysqlcc_gui_server_evt_mnuDBOpsShowCreate_activate), 
										(gpointer)gui_server);

	gui_server->mnuTblOps = gtk_menu_new();
	gtk_widget_show (gui_server->mnuTblOps);

	mnuTBLOpsShowCreate = gtk_menu_item_new_with_label(_("Display create structure"));
	gtk_widget_show (mnuTBLOpsShowCreate);
	gtk_menu_append (GTK_MENU_SHELL(gui_server->mnuTblOps), mnuTBLOpsShowCreate);
	g_signal_connect (G_OBJECT (mnuTBLOpsShowCreate), "activate", 
										G_CALLBACK (gmysqlcc_gui_server_evt_mnuTBLOpsShowCreate_activate), 
										(gpointer)gui_server);
}

void gmysqlcc_gui_server_init_widget (p_gmysqlcc_gui_server gui_server) {
	GtkTreeViewColumn * currCol;

	/*gmysqlcc_gui_server_fill_user_list(gui_server);*/
	gmysqlcc_gui_server_fill_database_list(gui_server);

	currCol = gtk_tree_view_get_column(GTK_TREE_VIEW(gui_server->lstTable), 0);
	if (currCol != NULL) {
		gtk_tree_view_remove_column(GTK_TREE_VIEW(gui_server->lstTable), currCol);
	}

}

void gmysqlcc_gui_server_fill_database_list (p_gmysqlcc_gui_server gui_server) {
	GtkListStore * lstStrBase;
	GtkTreeViewColumn * currCol;
	GtkCellRenderer * renderer;

	void sub_ht_fill_database_list(gpointer key, gpointer value, gpointer user_data) {
		p_mysql_database mysql_db = (p_mysql_database)value;
		GtkListStore * lstStrBase = (GtkListStore *)user_data;
		GtkTreeIter iter;
		
		gtk_list_store_append (lstStrBase, &iter);
		gtk_list_store_set (lstStrBase, &iter, 0, mysql_db->name, 1, mysql_db, -1);
	}

	lstStrBase = gtk_list_store_new(2, G_TYPE_STRING, G_TYPE_POINTER);
	gtk_tree_sortable_set_sort_column_id (GTK_TREE_SORTABLE(lstStrBase), 0, GTK_SORT_ASCENDING);
	
	if (mysql_server_refresh_database_list(gui_server->mysql_srv)) {
		g_hash_table_foreach(gui_server->mysql_srv->hshDbs, &sub_ht_fill_database_list, (gpointer)lstStrBase);
	}
	
	gtk_tree_view_set_model(GTK_TREE_VIEW(gui_server->lstBase), GTK_TREE_MODEL(lstStrBase));
	g_object_unref (G_OBJECT (lstStrBase));
		
	currCol = gtk_tree_view_get_column(GTK_TREE_VIEW(gui_server->lstBase), 0);
	if (currCol != NULL) {
		gtk_tree_view_remove_column(GTK_TREE_VIEW(gui_server->lstBase), currCol);
	}

	renderer = gtk_cell_renderer_text_new ();
	currCol = gtk_tree_view_column_new_with_attributes (_("Databases"), renderer, "text", 0, NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW (gui_server->lstBase), currCol);
	
	gui_server->curr_mysql_db = NULL;
}

void gmysqlcc_gui_server_fill_table_list (p_gmysqlcc_gui_server gui_server) {
	GtkListStore * lstStrTable;
	GtkTreeViewColumn * currCol;
	GtkCellRenderer * renderer;

	void sub_ht_fill_table_list(gpointer key, gpointer value, gpointer user_data) {
		p_mysql_table mysql_tbl = (p_mysql_table)value;
		GtkListStore * lstStrTable = (GtkListStore *)user_data;
		GtkTreeIter iter;
		
		gtk_list_store_append (lstStrTable, &iter);
		gtk_list_store_set (lstStrTable, &iter, 0, mysql_tbl->name, 1, mysql_tbl->nbrRow, 2, mysql_tbl->size, 3, mysql_tbl->type, 4, mysql_tbl, -1);
	}

	lstStrTable = gtk_list_store_new(5, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_POINTER);
	gtk_tree_sortable_set_sort_column_id (GTK_TREE_SORTABLE(lstStrTable), 0, GTK_SORT_ASCENDING);
	
	/* Fill listStore */
	if (mysql_database_refresh_table_list(gui_server->curr_mysql_db)) {
		g_hash_table_foreach(gui_server->curr_mysql_db->hshTables, &sub_ht_fill_table_list, (gpointer)lstStrTable);
	}
	
	gtk_tree_view_set_model(GTK_TREE_VIEW(gui_server->lstTable), GTK_TREE_MODEL(lstStrTable));
	g_object_unref (G_OBJECT (lstStrTable));
		
	while ((currCol = gtk_tree_view_get_column(GTK_TREE_VIEW(gui_server->lstTable), 0)) != NULL) {
		gtk_tree_view_remove_column(GTK_TREE_VIEW(gui_server->lstTable), currCol);
	}

	renderer = gtk_cell_renderer_text_new ();
	currCol = gtk_tree_view_column_new_with_attributes (_("Table name"), renderer, "text", 0, NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW (gui_server->lstTable), currCol);
	currCol = gtk_tree_view_column_new_with_attributes (_("Rows"), renderer, "text", 1, NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW (gui_server->lstTable), currCol);
	currCol = gtk_tree_view_column_new_with_attributes (_("Size"), renderer, "text", 2, NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW (gui_server->lstTable), currCol);
	currCol = gtk_tree_view_column_new_with_attributes (_("Type"), renderer, "text", 3, NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW (gui_server->lstTable), currCol);

	gui_server->curr_mysql_tbl = NULL;
}

void gmysqlcc_gui_server_fill_user_list (p_gmysqlcc_gui_server gui_server) {
	
	void sub_ht_throught_user_list(gpointer key, gpointer value, gpointer user_data) {
		p_mysql_user mysql_usr = (p_mysql_user)value;
		
		g_print("User : '%s@%s'\n", mysql_usr->login, mysql_usr->host);
	}

	if (mysql_server_refresh_user_list(gui_server->mysql_srv)) {
		g_print("Fill user list ... Ok !\nDisplay now : \n");
		g_hash_table_foreach(gui_server->mysql_srv->hshUsers, &sub_ht_throught_user_list, NULL);
	} else {
		return;
	}
	
}

void gmysqlcc_gui_server_open_query_window (p_gmysqlcc_gui_server gui_server, gboolean use_table) {
	p_gmysqlcc_gui_query gui_query;
	p_mysql_query mysql_qry;

	if (gui_server->curr_mysql_db != NULL) {
		
		mysql_qry = mysql_database_query(gui_server->curr_mysql_db);
		
		gui_query = gmysqlcc_gui_query_new(mysql_qry);
		
		if (use_table && gui_server->curr_mysql_tbl != NULL) {
			GString * sqlQuery = g_string_new("");
			
			g_string_printf(sqlQuery, "SELECT * \nFROM `%s`.`%s`\nWHERE 1\nLIMIT 1000", 
											gui_server->curr_mysql_db->name, gui_server->curr_mysql_tbl->name);
			gmysqlcc_gui_query_set_query(gui_query, sqlQuery->str, TRUE);
			g_string_free(sqlQuery, TRUE);
		}
		
		gmysqlcc_gui_query_display(gui_query, TRUE);
	}
}

void gmysqlcc_gui_server_evt_destroy (GtkWidget *widget, gpointer user_data) {
	p_gmysqlcc_gui_server gui_server = (p_gmysqlcc_gui_server)user_data;
	
	gmysqlcc_gui_server_display(gui_server, FALSE);
	gmysqlcc_gui_server_delete(gui_server);
}

void gmysqlcc_gui_server_evt_mnuDBOpsRefresh_activate (GtkWidget *widget, gpointer user_data) {
	p_gmysqlcc_gui_server gui_server = (p_gmysqlcc_gui_server)user_data;
	
	gmysqlcc_gui_server_init_widget(gui_server);
}

void gmysqlcc_gui_server_evt_mnuDBOpsShowCreate_activate (GtkWidget *widget, gpointer user_data) {
	p_gmysqlcc_gui_server gui_server = (p_gmysqlcc_gui_server)user_data;
	p_gmysqlcc_gui_text gui_text;
	GString * sqlFilename, * dbStructDump; /*, * structDump;*/
	p_mysql_query mysql_qry;
	
	void ht_fill_create_script(gpointer key, gpointer value, gpointer user_data) {
		p_mysql_table mysql_tbl = (p_mysql_table)value;
		GString * dbStructDump = (GString *)user_data;
		GString * structDump;
		
		mysql_qry = mysql_table_query(mysql_tbl);
		
		structDump = mysql_table_get_sql_structure(mysql_tbl);
		g_string_append(dbStructDump, structDump->str);
		g_string_free(structDump, TRUE);
		
		mysql_query_delete(mysql_qry);
	}
	
	if (gui_server->curr_mysql_db != NULL) {
		dbStructDump = g_string_new("");
		
		/*
		structDump = mysql_dump_database_struct(gui_server->curr_mysql_db->name, FALSE, FALSE);
		g_string_append(dbStructDump, structDump->str);
		*/
		
		/* Fill SQL script */
		g_hash_table_foreach(gui_server->curr_mysql_db->hshTables, &ht_fill_create_script, (gpointer)dbStructDump);
		
		sqlFilename = g_string_new("");
		g_string_printf(sqlFilename, "%s-struct.sql", gui_server->curr_mysql_db->name);
		
		gui_text = gmysqlcc_gui_text_new();
		gmysqlcc_gui_text_set_content(gui_text, dbStructDump->str, sqlFilename->str);
		gmysqlcc_gui_text_display(gui_text, TRUE);
		
		/*g_string_free(structDump, TRUE);*/
		g_string_free(dbStructDump, TRUE);
		g_string_free(sqlFilename, TRUE);
	}
}

void gmysqlcc_gui_server_evt_mnuTBLOpsShowCreate_activate (GtkWidget *widget, gpointer user_data) {
	p_gmysqlcc_gui_server gui_server = (p_gmysqlcc_gui_server)user_data;
	p_gmysqlcc_gui_text gui_text;
	GString * sqlFilename, * structDump;
	p_mysql_query mysql_qry;
	
	if (gui_server->curr_mysql_tbl != NULL) {
		mysql_qry = mysql_table_query(gui_server->curr_mysql_tbl);
		
		sqlFilename = g_string_new("");
		g_string_printf(sqlFilename, "%s-%s-struct.sql", gui_server->curr_mysql_tbl->mysql_db->name, gui_server->curr_mysql_tbl->name);
		structDump = mysql_table_get_sql_structure(gui_server->curr_mysql_tbl);
		
		g_print(structDump->str);
		
		gui_text = gmysqlcc_gui_text_new();
		gmysqlcc_gui_text_set_content(gui_text, structDump->str, sqlFilename->str);
		gmysqlcc_gui_text_display(gui_text, TRUE);
		
		g_string_free(structDump, TRUE);
		g_string_free(sqlFilename, TRUE);
	}

}

gboolean gmysqlcc_gui_server_evt_lstBase_btnpress (GtkWidget *widget, GdkEventButton *event, gpointer user_data) {
	p_gmysqlcc_gui_server gui_server = (p_gmysqlcc_gui_server)user_data;
	
	switch (event->button) {
		case 1 : /* Left button */
			if (event->type == GDK_2BUTTON_PRESS) {
				gmysqlcc_gui_server_open_query_window (gui_server, FALSE);
			}
			break;
		case 3 : /* Right button */
			if (event->type == GDK_BUTTON_PRESS) {
				gtk_menu_popup(GTK_MENU(gui_server->mnuBdOps), NULL, NULL, NULL, user_data, event->button, event->time);
			}
			break;
	}
	return FALSE;
	
}

gboolean gmysqlcc_gui_server_evt_lstTable_btnpress (GtkWidget *widget, GdkEventButton *event, gpointer user_data) {
	p_gmysqlcc_gui_server gui_server = (p_gmysqlcc_gui_server)user_data;
	
	switch (event->button) {
		case 1 : /* Left button */
			if (event->type == GDK_2BUTTON_PRESS) {
				gmysqlcc_gui_server_open_query_window (gui_server, TRUE);
			}
			break;
		case 3 : /* Right button */
			if (event->type == GDK_BUTTON_PRESS) {
				gtk_menu_popup(GTK_MENU(gui_server->mnuTblOps), NULL, NULL, NULL, user_data, event->button, event->time);
			}
			break;
	}
	return FALSE;
}

void gmysqlcc_gui_server_evt_lstBase_selected (GtkTreeSelection *selection, gpointer user_data) {
	p_gmysqlcc_gui_server gui_server = (p_gmysqlcc_gui_server)user_data;
	GtkTreeIter iter;
	GtkTreeModel *model;
	
	g_print("Changed !\n");
	if (gtk_tree_selection_get_selected (selection, &model, &iter)) {
		gtk_tree_model_get (model, &iter, 1, &gui_server->curr_mysql_db, -1);
		g_print ("The database is %s\n", gui_server->curr_mysql_db->name);
		
		gmysqlcc_gui_server_fill_table_list(gui_server);
	}
}

void gmysqlcc_gui_server_evt_lstTable_selected (GtkTreeSelection *selection, gpointer user_data) {
	p_gmysqlcc_gui_server gui_server = (p_gmysqlcc_gui_server)user_data;
	GtkTreeIter iter;
	GtkTreeModel *model;
	
	g_print("Changed !\n");
	if (gtk_tree_selection_get_selected (selection, &model, &iter)) {
		gtk_tree_model_get (model, &iter, 4, &gui_server->curr_mysql_tbl, -1);
		g_print ("The table is %s\n", gui_server->curr_mysql_tbl->name);
	}
}

void gmysqlcc_gui_server_evt_btnTlbrSql_clicked (GtkWidget *widget, gpointer user_data) {
	p_gmysqlcc_gui_server gui_server = (p_gmysqlcc_gui_server)user_data;
	
	gmysqlcc_gui_server_open_query_window (gui_server, TRUE);
}

void gmysqlcc_gui_server_evt_btnTlbrSqlFile_clicked (GtkWidget *widget, gpointer user_data) {
	/*p_gmysqlcc_gui_server gui_server = (p_gmysqlcc_gui_server)user_data;*/
	p_gmysqlcc_gui_text gui_text;
	
	gui_text = gmysqlcc_gui_text_new();
	gmysqlcc_gui_text_display(gui_text, TRUE);
	/* gmysqlcc_gui_server_open_query_window (gui_server, TRUE); */
}

void gmysqlcc_gui_server_evt_btnTlbrClose_clicked (GtkWidget *widget, gpointer user_data) {
	p_gmysqlcc_gui_server gui_server = (p_gmysqlcc_gui_server)user_data;
	
	gtk_widget_destroy(GTK_WIDGET(gui_server->window));
}

void gmysqlcc_gui_server_evt_btnDbAdd_clicked (GtkWidget *widget, gpointer user_data) {
	p_gmysqlcc_gui_server gui_server = (p_gmysqlcc_gui_server)user_data;
	GString * dbname;
	GtkWidget * msgdlg;
	p_mysql_database mysql_db;
	
	dbname = askInfo("New database", "Give the name of the new database :", "new");
	
	if (dbname != NULL) {
		
		g_print("New database name : '%s'\n", dbname->str);
		
		mysql_db = mysql_database_new_create(gui_server->mysql_srv, dbname->str);
		
		if (mysql_db != NULL) {
			msgdlg = gtk_message_dialog_new(GTK_WINDOW(gui_server->window), GTK_DIALOG_MODAL, GTK_MESSAGE_INFO, GTK_BUTTONS_OK, _("Database '%s' created !"), dbname->str);
			mysql_database_delete(mysql_db);
		} else {
			msgdlg = gtk_message_dialog_new(GTK_WINDOW(gui_server->window), GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, _("Can't create database '%s' !"), dbname->str);
		}
		gtk_dialog_run (GTK_DIALOG (msgdlg));
		gtk_widget_destroy (msgdlg);
		
		
	}
	g_string_free (dbname, TRUE);
}

void gmysqlcc_gui_server_evt_btnDbDel_clicked (GtkWidget *widget, gpointer user_data) {
	p_gmysqlcc_gui_server gui_server = (p_gmysqlcc_gui_server)user_data;
	GString * query;
	GtkWidget * msgdlg;
	gboolean ok = FALSE;
	p_mysql_query mysql_qry;
	
	if (gui_server->curr_mysql_db != NULL) {
		ok = askConfirmation("Delete database", "Are you sure ?");
		if (ok) {
			query = g_string_new("");
			g_string_printf(query, "DROP DATABASE `%s`", gui_server->curr_mysql_db->name);
			
			mysql_qry = mysql_server_query(gui_server->mysql_srv, NULL);
			
			if (mysql_query_execute_query(mysql_qry, query->str, FALSE)) {
				msgdlg = gtk_message_dialog_new(GTK_WINDOW(gui_server->window), GTK_DIALOG_MODAL, GTK_MESSAGE_INFO, GTK_BUTTONS_OK, _("Database '%s' deleted !"), gui_server->curr_mysql_db->name);
				gtk_dialog_run (GTK_DIALOG (msgdlg));
				gtk_widget_destroy (msgdlg);
				
				gmysqlcc_gui_server_fill_database_list (gui_server);
			}
			
			mysql_query_delete(mysql_qry);
			g_string_free (query, TRUE);
		}
	}
}

void gmysqlcc_gui_server_evt_btnTblAdd_clicked (GtkWidget *widget, gpointer user_data) {
	/*p_gmysqlcc_gui_server gui_server = (p_gmysqlcc_gui_server)user_data;*/
	
}

void gmysqlcc_gui_server_evt_btnTblEdit_clicked (GtkWidget *widget, gpointer user_data) {
	/*
	p_gmysqlcc_gui_server gui_server = (p_gmysqlcc_gui_server)user_data;
	p_tableWnd pTblWnd;
	
	if (gui_server->curr_mysql_db != NULL && gui_server->curr_mysql_tbl != NULL) {
		g_print("DBname : '%s' - TblName '%s'\n", gui_server->curr_mysql_db->name, gui_server->curr_mysql_tbl->name);
		g_print("Edit table : '%s.%s'\n", pSrvWnd->curr_mysql_db->name, pSrvWnd->currTblName->str);
		pTblWnd = create_wndTable(TRUE, pSrvWnd->p_mci, pSrvWnd->currTblName->str);
	}
	*/
}

void gmysqlcc_gui_server_evt_btnTblDump_clicked (GtkWidget *widget, gpointer user_data) {
	p_gmysqlcc_gui_server gui_server = (p_gmysqlcc_gui_server)user_data;
	p_gmysqlcc_gui_dump gui_dump;
	
	gui_dump = NULL;
	
	if (gui_server->curr_mysql_tbl != NULL) {
		g_print("Dump Table :'%s'.'%s' ...\n", gui_server->curr_mysql_db->name, gui_server->curr_mysql_tbl->name);
		gui_dump = gmysqlcc_gui_dump_new(NULL, NULL, gui_server->curr_mysql_tbl, NULL);
	} else if (gui_server->curr_mysql_db != NULL) {
		g_print("Dump Database :'%s' ...\n", gui_server->curr_mysql_db->name);
		gui_dump = gmysqlcc_gui_dump_new(NULL, gui_server->curr_mysql_db, NULL, NULL);
	} else {
		g_print("Dump server ...\n");
		gui_dump = gmysqlcc_gui_dump_new(gui_server->mysql_srv, NULL, NULL, NULL);
	}
	
	if (gui_dump != NULL) {
		gmysqlcc_gui_dump_display(gui_dump, TRUE);
	}
}

void gmysqlcc_gui_server_evt_btnTblDel_clicked (GtkWidget *widget, gpointer user_data) {
	p_gmysqlcc_gui_server gui_server = (p_gmysqlcc_gui_server)user_data;
	GString * query;
	GtkWidget * msgdlg;
	gboolean ok = FALSE;
	p_mysql_query mysql_qry;
	
	if (gui_server->curr_mysql_db != NULL && gui_server->curr_mysql_tbl != NULL) {
		ok = askConfirmation(_("Delete Table"), _("Are you sure ?"));
		if (ok) {
			g_print("Delete table : '%s'\n", gui_server->curr_mysql_tbl->name);
			
			query = g_string_new("");
			g_string_printf(query, "DROP TABLE `%s`.`%s`", gui_server->curr_mysql_db->name, gui_server->curr_mysql_tbl->name);
			
			mysql_qry = mysql_server_query(gui_server->mysql_srv, NULL);
			
			if (mysql_query_execute_query(mysql_qry, query->str, FALSE)) {
				msgdlg = gtk_message_dialog_new(GTK_WINDOW(gui_server->window), GTK_DIALOG_MODAL, GTK_MESSAGE_INFO, GTK_BUTTONS_OK, _("Table '%s'.'%s' deleted !"), gui_server->curr_mysql_db->name, gui_server->curr_mysql_tbl->name);
				gtk_dialog_run (GTK_DIALOG (msgdlg));
				gtk_widget_destroy (msgdlg);
				
				gmysqlcc_gui_server_fill_database_list (gui_server);
			}
			
			mysql_query_delete(mysql_qry);
			g_string_free (query, TRUE);
		}
	}
}


