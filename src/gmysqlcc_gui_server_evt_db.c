
#include "gmysqlcc_gui_all.h"
#include "gmysqlcc_gui_server_evt.h"

void gmysqlcc_gui_server_evt_mnuDBOpsRefresh_activate (GtkWidget *widget, gpointer user_data) {
	p_gmysqlcc_gui_server gui_server = (p_gmysqlcc_gui_server)user_data;
	
	gmysqlcc_gui_server_fill_database_list(gui_server);
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


