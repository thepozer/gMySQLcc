
#include "gmysql_gui.h"

void initDataServer (p_servWnd pSrvWnd);
void fillUserList (p_servWnd pSrvWnd);
void fillBaseList (p_servWnd pSrvWnd);
void fillTableList (p_servWnd pSrvWnd, p_mysql_database mysql_db);
void openQueryWindow (p_mysql_database mysql_db, p_mysql_table mysql_tbl);

static void destroy(GtkWidget *widget, gpointer user_data);
static void btntlbrsql_clicked (GtkWidget *widget, gpointer user_data);
static void btntlbrclose_clicked (GtkWidget *widget, gpointer user_data);
static void btndbadd_clicked (GtkWidget *widget, gpointer user_data);
static void baseSelected (GtkTreeSelection *selection, gpointer data);
static void tableSelected (GtkTreeSelection *selection, gpointer data);

void initDataServer (p_servWnd pSrvWnd) {
	GtkTreeViewColumn * currCol;

	/*fillUserList(pSrvWnd);*/
	fillBaseList(pSrvWnd);

	currCol = gtk_tree_view_get_column(GTK_TREE_VIEW(pSrvWnd->lstTable), 0);
	if (currCol != (GtkTreeViewColumn *)NULL) {
		gtk_tree_view_remove_column(GTK_TREE_VIEW(pSrvWnd->lstTable), currCol);
	}

	pSrvWnd->curr_mysql_tbl = (p_mysql_table)NULL;
	g_string_assign(pSrvWnd->currTblName, "");

}

void fillUserList (p_servWnd pSrvWnd) {
	
	void ht_throught_user_list(gpointer key, gpointer value, gpointer user_data) {
		p_mysql_user mysql_usr = (p_mysql_user)value;
		
		g_print("User : '%s@%s'\n", mysql_usr->login, mysql_usr->host);
	}

	if (mysql_server_refresh_user_list(pSrvWnd->mysql_srv)) {
		g_print("Fill user list ... Ok !\nDisplay now : \n");
		g_hash_table_foreach(pSrvWnd->mysql_srv->hshUsers, &ht_throught_user_list, NULL);
	} else {
		return;
	}
	
}

void fillBaseList (p_servWnd pSrvWnd) {
	GtkListStore * lstStrBase;
	GtkTreeViewColumn * currCol;
	GtkCellRenderer * renderer;

	void ht_fill_database_list(gpointer key, gpointer value, gpointer user_data) {
		p_mysql_database mysql_db = (p_mysql_database)value;
		GtkListStore * lstStrBase = (GtkListStore *)user_data;
		GtkTreeIter iter;
		
		gtk_list_store_append (lstStrBase, &iter);
		gtk_list_store_set (lstStrBase, &iter, 0, mysql_db->name, 1, mysql_db, -1);
	}

	lstStrBase = gtk_list_store_new(2, G_TYPE_STRING, G_TYPE_POINTER);
	gtk_tree_sortable_set_sort_column_id (GTK_TREE_SORTABLE(lstStrBase), 0, GTK_SORT_ASCENDING);
	
	if (mysql_server_refresh_database_list(pSrvWnd->mysql_srv)) {
		g_hash_table_foreach(pSrvWnd->mysql_srv->hshDbs, &ht_fill_database_list, (gpointer)lstStrBase);
	}
	
	gtk_tree_view_set_model(GTK_TREE_VIEW(pSrvWnd->lstBase), GTK_TREE_MODEL(lstStrBase));
	g_object_unref (G_OBJECT (lstStrBase));
		
	currCol = gtk_tree_view_get_column(GTK_TREE_VIEW(pSrvWnd->lstBase), 0);
	if (currCol != (GtkTreeViewColumn *)NULL) {
		gtk_tree_view_remove_column(GTK_TREE_VIEW(pSrvWnd->lstBase), currCol);
	}

	renderer = gtk_cell_renderer_text_new ();
	currCol = gtk_tree_view_column_new_with_attributes (_("Databases"), renderer, "text", 0, NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW (pSrvWnd->lstBase), currCol);
	
	pSrvWnd->curr_mysql_db = (p_mysql_database)NULL;
	g_string_assign(pSrvWnd->currDbName, "");
}

void fillTableList (p_servWnd pSrvWnd, p_mysql_database mysql_db) {

	void ht_fill_table_list(gpointer key, gpointer value, gpointer user_data) {
		p_mysql_table mysql_tbl = (p_mysql_table)value;
		GtkListStore * lstStrTable = (GtkListStore *)user_data;
		GtkTreeIter iter;
		
		gtk_list_store_append (lstStrTable, &iter);
		gtk_list_store_set (lstStrTable, &iter, 0, mysql_tbl->name, 1, mysql_tbl->nbrRow, 2, mysql_tbl->size, 3, mysql_tbl->type, 4, mysql_tbl, -1);
	}

	GtkListStore * lstStrTable;
	GtkTreeViewColumn * currCol;
	GtkCellRenderer * renderer;
	
	lstStrTable = gtk_list_store_new(5, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_POINTER);
	gtk_tree_sortable_set_sort_column_id (GTK_TREE_SORTABLE(lstStrTable), 0, GTK_SORT_ASCENDING);
	
	/* Fill listStore */
	if (mysql_database_refresh_table_list(mysql_db)) {
		g_hash_table_foreach(mysql_db->hshTables, &ht_fill_table_list, (gpointer)lstStrTable);
	}
	
	gtk_tree_view_set_model(GTK_TREE_VIEW(pSrvWnd->lstTable), GTK_TREE_MODEL(lstStrTable));
	g_object_unref (G_OBJECT (lstStrTable));
		
	while ((currCol = gtk_tree_view_get_column(GTK_TREE_VIEW(pSrvWnd->lstTable), 0)) != (GtkTreeViewColumn *)NULL) {
		gtk_tree_view_remove_column(GTK_TREE_VIEW(pSrvWnd->lstTable), currCol);
	}

	renderer = gtk_cell_renderer_text_new ();
	currCol = gtk_tree_view_column_new_with_attributes (_("Table name"), renderer, "text", 0, NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW (pSrvWnd->lstTable), currCol);
	currCol = gtk_tree_view_column_new_with_attributes (_("Rows"), renderer, "text", 1, NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW (pSrvWnd->lstTable), currCol);
	currCol = gtk_tree_view_column_new_with_attributes (_("Size"), renderer, "text", 2, NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW (pSrvWnd->lstTable), currCol);
	currCol = gtk_tree_view_column_new_with_attributes (_("Type"), renderer, "text", 3, NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW (pSrvWnd->lstTable), currCol);

	pSrvWnd->curr_mysql_tbl = (p_mysql_table)NULL;
	g_string_assign(pSrvWnd->currTblName, "");
}


void openQueryWindow (p_mysql_database mysql_db, p_mysql_table mysql_tbl) {
	execSqlWnd * psqlWnd;
	GString * sqlQuery;
	p_mysql_query mysql_qry;

	if (mysql_db != NULL) {
		
		mysql_qry = mysql_database_query(mysql_db);
		
		if (mysql_tbl != NULL) {
			sqlQuery = g_string_new("");
			g_string_printf(sqlQuery, "SELECT * \nFROM `%s`\nWHERE 1\nLIMIT 1000", mysql_tbl->name);
			psqlWnd = create_wndSQL(TRUE, mysql_qry, sqlQuery->str, TRUE);
			g_string_free(sqlQuery, TRUE);
		} else {
			psqlWnd = create_wndSQL(TRUE, mysql_qry, (gchar *)NULL, FALSE);
		}
	}
}

static gboolean lstbase_btnpress(GtkWidget *widget, GdkEventButton *event, gpointer user_data) {
	p_servWnd pSrvWnd = (p_servWnd)user_data;

	switch (event->button) {
		case 1 : /* Left button */
			if (event->type == GDK_2BUTTON_PRESS) {
				openQueryWindow (pSrvWnd->curr_mysql_db, NULL);
			}
			break;
		case 3 : /* Right button */
			if (event->type == GDK_BUTTON_PRESS) {
				gtk_menu_popup(GTK_MENU(pSrvWnd->mnuBdOps), GTK_WIDGET(NULL), GTK_WIDGET(NULL), (void *)NULL, user_data, event->button, event->time);
			}
			break;
	}
	return FALSE;
}

static gboolean lsttable_btnpress(GtkWidget *widget, GdkEventButton *event, gpointer user_data) {
	p_servWnd pSrvWnd = (p_servWnd)user_data;

	switch (event->button) {
		case 1 : /* Left button */
			if (event->type == GDK_2BUTTON_PRESS) {
				openQueryWindow (pSrvWnd->curr_mysql_db, pSrvWnd->curr_mysql_tbl);
			}
			break;
		case 3 : /* Right button */
			if (event->type == GDK_BUTTON_PRESS) {
				gtk_menu_popup(GTK_MENU(pSrvWnd->mnuTblOps), GTK_WIDGET(NULL), GTK_WIDGET(NULL), (void *)NULL, user_data, event->button, event->time);
			}
			break;
	}
	return FALSE;
}

static void btntlbrsql_clicked (GtkWidget *widget, gpointer user_data) {
	p_servWnd pSrvWnd = (p_servWnd)user_data;
	
	openQueryWindow (pSrvWnd->curr_mysql_db, pSrvWnd->curr_mysql_tbl);
}

static void btntlbrsqlfile_clicked (GtkWidget *widget, gpointer user_data) {
	p_servWnd pSrvWnd = (p_servWnd)user_data;
	
	openQueryWindow (pSrvWnd->curr_mysql_db, pSrvWnd->curr_mysql_tbl);
}

static void btntlbrclose_clicked (GtkWidget *widget, gpointer user_data) {
	p_servWnd pSrvWnd = (p_servWnd)user_data;
	
	gtk_widget_destroy(GTK_WIDGET(pSrvWnd->wndServer));
}

static void btndbadd_clicked (GtkWidget *widget, gpointer user_data) {
	p_servWnd pSrvWnd = (p_servWnd)user_data;
	GString * dbname, * query;
	GtkWidget * msgdlg;
	p_mysql_query mysql_qry;
	
	dbname = askInfo("New database", "Give the name of the new database :", "new");
	
	if (dbname != (GString *)NULL) {
		
		g_print("New database name : '%s'\n", dbname->str);
		query = g_string_new("");
		g_string_printf(query, "CREATE DATABASE `%s`", dbname->str);
		mysql_qry = mysql_server_query(pSrvWnd->mysql_srv, (gchar *)NULL);
		if (mysql_query_execute_query(mysql_qry, query->str, FALSE)) {
			msgdlg = gtk_message_dialog_new(GTK_WINDOW(NULL), GTK_DIALOG_MODAL, GTK_MESSAGE_INFO, GTK_BUTTONS_OK, _("Database '%s' created !"), dbname->str);
			gtk_dialog_run (GTK_DIALOG (msgdlg));
			gtk_widget_destroy (msgdlg);
			fillBaseList (pSrvWnd);
		}
		mysql_query_delete(mysql_qry);
		g_string_free (query, TRUE);
	}
	g_string_free (dbname, TRUE);
}

static void btndbdel_clicked (GtkWidget *widget, gpointer user_data) {
	p_servWnd pSrvWnd = (p_servWnd)user_data;
	GString * query;
	GtkWidget * msgdlg;
	gboolean ok = FALSE;
	p_mysql_query mysql_qry;
	
	if (pSrvWnd->curr_mysql_db != (p_mysql_database)NULL) {
		ok = askConfirmation("Delete database", "Are you sure ?");
		if (ok) {
			/*g_print("Delete database : '%s'\n", pSrvWnd->curr_mysql_db->name);*/
			query = g_string_new("");
			g_string_printf(query, "DROP DATABASE `%s`", pSrvWnd->curr_mysql_db->name);
			
			mysql_qry = mysql_server_query(pSrvWnd->mysql_srv, (gchar *)NULL);
			if (mysql_query_execute_query(mysql_qry, query->str, FALSE)) {
				msgdlg = gtk_message_dialog_new(GTK_WINDOW(NULL), GTK_DIALOG_MODAL, GTK_MESSAGE_INFO, GTK_BUTTONS_OK, _("Database '%s' deleted !"), pSrvWnd->curr_mysql_db->name);
				gtk_dialog_run (GTK_DIALOG (msgdlg));
				gtk_widget_destroy (msgdlg);
				fillBaseList (pSrvWnd);
			}
			mysql_query_delete(mysql_qry);
			g_string_free (query, TRUE);
		}
	}
}

static void btntbladd_clicked (GtkWidget *widget, gpointer user_data) {
	/*p_servWnd pSrvWnd = (p_servWnd)user_data;*/

}

static void btntbledit_clicked (GtkWidget *widget, gpointer user_data) {
	p_servWnd pSrvWnd = (p_servWnd)user_data;
	/* p_tableWnd pTblWnd; */
	
	g_print("DBname : '%s' - TblName '%s'\n", pSrvWnd->curr_mysql_db->name, pSrvWnd->currTblName->str);
	/*
	if (pSrvWnd->currDbName->len > 0 && pSrvWnd->currTblName->len > 0) {
		g_print("Edit table : '%s.%s'\n", pSrvWnd->curr_mysql_db->name, pSrvWnd->currTblName->str);
		pTblWnd = create_wndTable(TRUE, pSrvWnd->p_mci, pSrvWnd->currTblName->str);
	}
	*/
}

static void btntbldump_clicked (GtkWidget *widget, gpointer user_data) {
	p_servWnd pSrvWnd = (p_servWnd)user_data;
	p_dumpWnd pDmpWnd;
	
	if (pSrvWnd->curr_mysql_tbl != (p_mysql_table)NULL) {
		g_print("Dump Table :'%s'.'%s' ...\n", pSrvWnd->curr_mysql_db->name, pSrvWnd->currTblName->str);
		pDmpWnd = create_wndDump(TRUE, (p_mysql_server)NULL, (p_mysql_database)NULL, pSrvWnd->curr_mysql_tbl, (gchar *)NULL);
	} else if (pSrvWnd->curr_mysql_db != (p_mysql_database)NULL) {
		g_print("Dump Database :'%s' ...\n", pSrvWnd->curr_mysql_db->name);
		pDmpWnd = create_wndDump(TRUE, (p_mysql_server)NULL, pSrvWnd->curr_mysql_db, (p_mysql_table)NULL, (gchar *)NULL);
	} else {
		g_print("Dump server ...\n");
		pDmpWnd = create_wndDump(TRUE, pSrvWnd->mysql_srv, (p_mysql_database)NULL, (p_mysql_table)NULL, (gchar *)NULL);
	}
}

static void btntbldel_clicked (GtkWidget *widget, gpointer user_data) {
	p_servWnd pSrvWnd = (p_servWnd)user_data;
	GString * query;
	GtkWidget * msgdlg;
	gboolean ok = FALSE;
	p_mysql_query mysql_qry;
	
	if (pSrvWnd->currDbName->len > 0 && pSrvWnd->currTblName->len > 0) {
		ok = askConfirmation(_("Delete Table"), _("Are you sure ?"));
		if (ok) {
			g_print("Delete table : '%s'\n", pSrvWnd->currTblName->str);
			query = g_string_new("");
			g_string_printf(query, "DROP TABLE `%s`.`%s`", pSrvWnd->curr_mysql_db->name, pSrvWnd->currTblName->str);
			mysql_qry = mysql_server_query(pSrvWnd->mysql_srv, (gchar *)NULL);
			if (mysql_query_execute_query(mysql_qry, query->str, FALSE)) {
				msgdlg = gtk_message_dialog_new(GTK_WINDOW(NULL), GTK_DIALOG_MODAL, GTK_MESSAGE_INFO, GTK_BUTTONS_OK, _("Table '%s'.'%s' deleted !"), pSrvWnd->curr_mysql_db->name, pSrvWnd->currTblName->str);
				gtk_dialog_run (GTK_DIALOG (msgdlg));
				gtk_widget_destroy (msgdlg);
				fillBaseList (pSrvWnd);
			}
			mysql_query_delete(mysql_qry);
			g_string_free (query, TRUE);
		}
	}
}

static void mnuDBOpsRefresh_activate (GtkWidget *widget, gpointer user_data) {
	p_servWnd pSrvWnd = (p_servWnd)user_data;
	
	initDataServer (pSrvWnd);
}

static void mnuDBOpsShowCreate_activate (GtkWidget *widget, gpointer user_data) {
	p_servWnd pSrvWnd = (p_servWnd)user_data;
	p_textWnd p_txtWnd;
	GString * sqlFilename, * dbStructDump, * structDump;
	p_mysql_query mysql_qry;
	
	void ht_fill_create_script(gpointer key, gpointer value, gpointer user_data) {
		p_mysql_table mysql_tbl = (p_mysql_table)value;
		GString * dbStructDump = (GString *)user_data;
		GString * structDump;
		
		mysql_qry = mysql_table_query(mysql_tbl);
		
		structDump = mysql_dump_table_struct(mysql_qry, mysql_tbl->name, FALSE);
		g_string_append(dbStructDump, structDump->str);
		g_string_free(structDump, TRUE);
		
		mysql_query_delete(mysql_qry);
	}
	
	if (pSrvWnd->curr_mysql_db != (p_mysql_database)NULL) {
		dbStructDump = g_string_new("");
		
		structDump = mysql_dump_database_struct(pSrvWnd->curr_mysql_db->name, FALSE, FALSE);
		g_string_append(dbStructDump, structDump->str);
		
		/* Fill SQL script */
		g_hash_table_foreach(pSrvWnd->curr_mysql_db->hshTables, &ht_fill_create_script, (gpointer)dbStructDump);
		
		sqlFilename = g_string_new("");
		g_string_printf(sqlFilename, "%s-struct.sql", pSrvWnd->curr_mysql_db->name);
		
		p_txtWnd = create_wndText(TRUE, dbStructDump->str, sqlFilename->str);
		
		g_string_free(structDump, TRUE);
		g_string_free(dbStructDump, TRUE);
		g_string_free(sqlFilename, TRUE);
	}
}

static void mnuTBLOpsShowCreate_activate (GtkWidget *widget, gpointer user_data) {
	p_servWnd pSrvWnd = (p_servWnd)user_data;
	p_textWnd p_txtWnd;
	GString * sqlFilename, * structDump;
	p_mysql_query mysql_qry;
	
	if (pSrvWnd->curr_mysql_tbl != (p_mysql_table)NULL) {
		mysql_qry = mysql_table_query(pSrvWnd->curr_mysql_tbl);
		
		sqlFilename = g_string_new("");
		g_string_printf(sqlFilename, "%s-%s-struct.sql", pSrvWnd->curr_mysql_tbl->mysql_db->name, pSrvWnd->curr_mysql_tbl->name);
		structDump = mysql_dump_table_struct(mysql_qry, pSrvWnd->curr_mysql_tbl->name, FALSE);
		
		p_txtWnd = create_wndText(TRUE, structDump->str, sqlFilename->str);
		
		g_string_free(structDump, TRUE);
		g_string_free(sqlFilename, TRUE);
	}
}

static void baseSelected (GtkTreeSelection *selection, gpointer data) {
	GtkTreeIter iter;
	GtkTreeModel *model;
	p_servWnd pSrvWnd = (p_servWnd)data;
	
	g_print("Changed !\n");
	if (gtk_tree_selection_get_selected (selection, &model, &iter)) {
		gtk_tree_model_get (model, &iter, 1, &pSrvWnd->curr_mysql_db, -1);
		g_print ("The database is %s\n", pSrvWnd->curr_mysql_db->name);
		g_string_assign(pSrvWnd->currTblName, "");
		
		fillTableList(pSrvWnd, pSrvWnd->curr_mysql_db);
	}
}

static void tableSelected (GtkTreeSelection *selection, gpointer data) {
	p_servWnd pSrvWnd = (p_servWnd)data;
	GtkTreeIter iter;
	GtkTreeModel *model;
	
	g_print("Changed !\n");
	if (gtk_tree_selection_get_selected (selection, &model, &iter)) {
		gtk_tree_model_get (model, &iter, 4, &pSrvWnd->curr_mysql_tbl, -1);
		g_string_assign(pSrvWnd->currTblName, pSrvWnd->curr_mysql_tbl->name);
		g_print ("The table is %s\n", pSrvWnd->currTblName->str);
	}
}

static void destroy(GtkWidget *widget, gpointer user_data) {
	p_servWnd pSrvWnd = (p_servWnd)user_data;
	
	/* Destroy GUI */
	gtk_widget_hide (GTK_WIDGET(pSrvWnd->wndServer));
	gtk_widget_destroy (GTK_WIDGET(pSrvWnd->wndServer));
	
	/* Destroy Datas */
	g_string_free(pSrvWnd->currDbName, TRUE);
	g_string_free(pSrvWnd->currTblName, TRUE);
	g_free(pSrvWnd);
	
	/* Destroy Application if needed */
	NbrWnd--;
	g_printerr("Destruction Server window - nbrWnd : %d\n", NbrWnd);
	if (NbrWnd <= 0) {
		g_printerr("Destroy App\n");
		gtk_main_quit();
	}
}

p_servWnd create_wndServer (gboolean display, p_mysql_server mysql_srv) {
  GtkWidget *statusbar1;
  GtkWidget *vbox1, *vbox2, *vbox3;
  GtkWidget *hpaned1;
  GtkWidget *hbox2, *hbox3;
  GtkWidget *label1, *label2, *label3, *label6, *label7;
  GtkWidget *notebook1;
  GtkWidget *scrolledwindow1, *scrolledwindow2;
  GtkAccelGroup *accel_group;
  GtkWidget *toolbar1;
	GtkWidget * imgToolbar;
  GtkToolItem * btnTlbrClose, * btnTlbrSql, * btnTlbrSqlFile;
	GtkWidget *btnDBAdd, *btnDBDel;
  GtkWidget *btnTblAdd, *btnTblEdit, *btnTblDump, *btnTblDel;
	GtkWidget *mnuDBOpsRefresh;
	GtkWidget *mnuDBOpsShowCreate;
	GtkWidget *mnuTBLOpsShowCreate;
	GtkTreeSelection *select;
	GtkTooltips * tooltips;
	GString * sTitle;
	
	p_servWnd p_svr;
	
	/* Init structure */
	p_svr = (p_servWnd) g_try_malloc(sizeof(servWnd));
	
	if (p_svr == (p_servWnd)NULL) {
		return (p_servWnd)NULL; /* return NULL pointer */
	}
	
	p_svr->mysql_srv = mysql_srv;
	p_svr->currDbName = g_string_new("");
	p_svr->currTblName = g_string_new("");
	
 	g_print("Using server(%d) - serveur name : '%s'\n", (int)mysql_srv, mysql_srv->name);

	accel_group = gtk_accel_group_new ();

	tooltips = gtk_tooltips_new();
	
  p_svr->wndServer = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	sTitle = g_string_new("");
	g_string_printf(sTitle, _("Server : %s"), mysql_srv->name);
  gtk_window_set_title (GTK_WINDOW (p_svr->wndServer), sTitle->str);
	g_string_free(sTitle, TRUE);
	gtk_window_set_default_size (GTK_WINDOW (p_svr->wndServer), 400, 300);
	g_signal_connect (G_OBJECT (p_svr->wndServer), "destroy", G_CALLBACK (destroy), p_svr);

  vbox1 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox1);
  gtk_container_add (GTK_CONTAINER (p_svr->wndServer), vbox1);

  toolbar1 = gtk_toolbar_new ();
  gtk_widget_show (toolbar1);
  gtk_box_pack_start (GTK_BOX (vbox1), toolbar1, FALSE, FALSE, 0);
  gtk_toolbar_set_style (GTK_TOOLBAR (toolbar1), GTK_TOOLBAR_BOTH_HORIZ);

	imgToolbar = gtk_image_new_from_stock(GTK_STOCK_EXECUTE, GTK_ICON_SIZE_LARGE_TOOLBAR);
	gtk_widget_show(imgToolbar);
	btnTlbrSql = gtk_tool_button_new (imgToolbar, _("SQL"));
	gtk_tool_item_set_is_important (GTK_TOOL_ITEM(btnTlbrSql), TRUE);
	g_signal_connect (G_OBJECT (btnTlbrSql), "clicked", G_CALLBACK (btntlbrsql_clicked), p_svr);
	gtk_widget_show(GTK_WIDGET(btnTlbrSql));
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar1), GTK_TOOL_ITEM(btnTlbrSql), -1);
	gtk_tool_item_set_tooltip (GTK_TOOL_ITEM(btnTlbrSql), tooltips, _("Exec SQL Query"), NULL);
	
	imgToolbar = gtk_image_new_from_stock(GTK_STOCK_EXECUTE, GTK_ICON_SIZE_LARGE_TOOLBAR);
	gtk_widget_show(imgToolbar);
	btnTlbrSqlFile = gtk_tool_button_new (imgToolbar, _("SQL File"));
	gtk_tool_item_set_is_important (GTK_TOOL_ITEM(btnTlbrSqlFile), TRUE);
	g_signal_connect (G_OBJECT (btnTlbrSqlFile), "clicked", G_CALLBACK (btntlbrsqlfile_clicked), p_svr);
	gtk_widget_show(GTK_WIDGET(btnTlbrSqlFile));
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar1), GTK_TOOL_ITEM(btnTlbrSqlFile), -1);
	gtk_tool_item_set_tooltip (GTK_TOOL_ITEM(btnTlbrSqlFile), tooltips, _("Exec SQL File"), NULL);
	
	imgToolbar = gtk_image_new_from_stock(GTK_STOCK_CLOSE, GTK_ICON_SIZE_LARGE_TOOLBAR);
	gtk_widget_show(imgToolbar);
	btnTlbrClose = gtk_tool_button_new (imgToolbar, _("Close"));
	gtk_tool_item_set_is_important (GTK_TOOL_ITEM(btnTlbrClose), TRUE);
	g_signal_connect (G_OBJECT (btnTlbrClose), "clicked", G_CALLBACK (btntlbrclose_clicked), p_svr);
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

  p_svr->lstBase = gtk_tree_view_new ();
  gtk_widget_show (p_svr->lstBase);
  gtk_container_add (GTK_CONTAINER (scrolledwindow1), p_svr->lstBase);
	g_signal_connect (G_OBJECT (p_svr->lstBase), "button-press-event", G_CALLBACK (lstbase_btnpress), (gpointer)p_svr);
	select = gtk_tree_view_get_selection (GTK_TREE_VIEW (p_svr->lstBase));
	gtk_tree_selection_set_mode (select, GTK_SELECTION_SINGLE);
	g_signal_connect (G_OBJECT (select), "changed", G_CALLBACK (baseSelected), (gpointer)p_svr);

  hbox2 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox2);
  gtk_box_pack_start (GTK_BOX (vbox2), hbox2, FALSE, TRUE, 0);

 	btnDBAdd = createIconButton("gtk-add", _("Add"));
  gtk_widget_show (btnDBAdd);
  gtk_box_pack_start (GTK_BOX (hbox2), btnDBAdd, TRUE, TRUE, 0);
	g_signal_connect (G_OBJECT (btnDBAdd), "clicked", G_CALLBACK (btndbadd_clicked), (gpointer)p_svr);

 	btnDBDel = createIconButton("gtk-delete", _("Delete"));
	gtk_widget_show (btnDBDel);
  gtk_box_pack_start (GTK_BOX (hbox2), btnDBDel, TRUE, TRUE, 0);
	g_signal_connect (G_OBJECT (btnDBDel), "clicked", G_CALLBACK (btndbdel_clicked), (gpointer)p_svr);

  vbox3 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox3);
  gtk_paned_pack2 (GTK_PANED (hpaned1), vbox3, TRUE, TRUE);

  scrolledwindow2 = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (scrolledwindow2);
  gtk_box_pack_start (GTK_BOX (vbox3), scrolledwindow2, TRUE, TRUE, 0);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow2), GTK_POLICY_NEVER, GTK_POLICY_ALWAYS);

  p_svr->lstTable = gtk_tree_view_new ();
  gtk_widget_show (p_svr->lstTable);
  gtk_container_add (GTK_CONTAINER (scrolledwindow2), p_svr->lstTable);
	g_signal_connect (G_OBJECT (p_svr->lstTable), "button-press-event", G_CALLBACK (lsttable_btnpress), (gpointer)p_svr);
	select = gtk_tree_view_get_selection (GTK_TREE_VIEW (p_svr->lstTable));
	gtk_tree_selection_set_mode (select, GTK_SELECTION_SINGLE);
	g_signal_connect (G_OBJECT (select), "changed", G_CALLBACK (tableSelected), (gpointer)p_svr);

  hbox3 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox3);
  gtk_box_pack_start (GTK_BOX (vbox3), hbox3, FALSE, TRUE, 0);

	btnTblAdd = createIconButton("gtk-add", _("Add"));
	gtk_widget_show (btnTblAdd);
  gtk_box_pack_start (GTK_BOX (hbox3), btnTblAdd, TRUE, TRUE, 0);
	g_signal_connect (G_OBJECT (btnTblAdd), "clicked", G_CALLBACK (btntbladd_clicked), (gpointer)p_svr);

	btnTblEdit = createIconButton("gtk-apply", _("Modify"));
  gtk_widget_show (btnTblEdit);
  gtk_box_pack_start (GTK_BOX (hbox3), btnTblEdit, TRUE, TRUE, 0);
	g_signal_connect (G_OBJECT (btnTblEdit), "clicked", G_CALLBACK (btntbledit_clicked), (gpointer)p_svr);

	btnTblDump = createIconButton("gtk-save", _("Dump"));
  gtk_widget_show (btnTblDump);
  gtk_box_pack_start (GTK_BOX (hbox3), btnTblDump, TRUE, TRUE, 0);
	g_signal_connect (G_OBJECT (btnTblDump), "clicked", G_CALLBACK (btntbldump_clicked), (gpointer)p_svr);

	btnTblDel = createIconButton("gtk-delete", _("Delete"));
  gtk_widget_show (btnTblDel);
  gtk_box_pack_start (GTK_BOX (hbox3), btnTblDel, TRUE, TRUE, 0);
	g_signal_connect (G_OBJECT (btnTblDel), "clicked", G_CALLBACK (btntbldel_clicked), (gpointer)p_svr);

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

	gtk_window_add_accel_group (GTK_WINDOW (p_svr->wndServer), accel_group);

	p_svr->mnuBdOps = gtk_menu_new();
	gtk_widget_show (p_svr->mnuBdOps);

	mnuDBOpsRefresh = gtk_menu_item_new_with_label(_("Refresh"));
	gtk_widget_show (mnuDBOpsRefresh);
	gtk_menu_append (GTK_MENU_SHELL(p_svr->mnuBdOps), mnuDBOpsRefresh);
	g_signal_connect (G_OBJECT (mnuDBOpsRefresh), "activate", G_CALLBACK (mnuDBOpsRefresh_activate), (gpointer)p_svr);
	
	mnuDBOpsShowCreate = gtk_menu_item_new_with_label(_("Display all create structures"));
	gtk_widget_show (mnuDBOpsShowCreate);
	gtk_menu_append (GTK_MENU_SHELL(p_svr->mnuBdOps), mnuDBOpsShowCreate);
	g_signal_connect (G_OBJECT (mnuDBOpsShowCreate), "activate", G_CALLBACK (mnuDBOpsShowCreate_activate), (gpointer)p_svr);
	
	p_svr->mnuTblOps = gtk_menu_new();
	gtk_widget_show (p_svr->mnuTblOps);

	mnuTBLOpsShowCreate = gtk_menu_item_new_with_label(_("Display create structure"));
	gtk_widget_show (mnuTBLOpsShowCreate);
	gtk_menu_append (GTK_MENU_SHELL(p_svr->mnuTblOps), mnuTBLOpsShowCreate);
	g_signal_connect (G_OBJECT (mnuTBLOpsShowCreate), "activate", G_CALLBACK (mnuTBLOpsShowCreate_activate), (gpointer)p_svr);
	
	initDataServer(p_svr);
	
	if (display) {
		gtk_widget_show (p_svr->wndServer);
		NbrWnd ++;	
	}

  return p_svr;
}
