
#include "gmysqlcc_gui_all.h"


void gmysqlcc_gui_exec_file_create_widget (p_gmysqlcc_gui_exec_file gui_xcfl);
void gmysqlcc_gui_exec_file_init_widget (p_gmysqlcc_gui_exec_file gui_xcfl);
void gmysqlcc_gui_exec_file_fill_database_list(p_gmysqlcc_gui_exec_file gui_xcfl);

void gmysqlcc_gui_exec_file_evt_update_status(p_mysql_multi_query mysql_mlt_qry, gboolean error, gpointer user_data);

void gmysqlcc_gui_exec_file_evt_destroy (GtkWidget *widget, gpointer user_data);
void gmysqlcc_gui_exec_file_evt_cmbServerList_changed (GtkComboBox *combobox, gpointer user_data);
void gmysqlcc_gui_exec_file_evt_cmbDatabaseList_changed (GtkComboBox *combobox, gpointer user_data);
void gmysqlcc_gui_exec_file_evt_btnExecute_clicked(GtkWidget *widget, gpointer user_data);
void gmysqlcc_gui_exec_file_evt_btnCancel_clicked(GtkWidget *widget, gpointer user_data);
void gmysqlcc_gui_exec_file_evt_btnShowReport_clicked(GtkWidget *widget, gpointer user_data);
void gmysqlcc_gui_exec_file_evt_rbtDatabase_clicked (GtkWidget *widget, gpointer user_data);

p_gmysqlcc_gui_exec_file gmysqlcc_gui_exec_file_new_open_file () {
	GtkWidget * chooser = NULL;
	p_gmysqlcc_gui_exec_file gui_xcfl = NULL;
	gchar * filename = NULL;
	gint response;
	
	gui_xcfl = gmysqlcc_gui_exec_file_new();
	
	if (gui_xcfl == NULL) {
		return NULL;
	}
	
	chooser = gtk_file_chooser_dialog_new (_("Select sql file"), GTK_WINDOW(gui_xcfl->window), GTK_FILE_CHOOSER_ACTION_OPEN,
		GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, GTK_STOCK_OPEN, GTK_RESPONSE_OK, NULL);
	
	response = gtk_dialog_run (GTK_DIALOG (chooser));
	if (response == GTK_RESPONSE_OK) {
		filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (chooser));
	}
	
	if (filename != NULL) {
		gmysqlcc_gui_exec_file_set_file (gui_xcfl, filename);
		g_free (filename);
	} else {
		gmysqlcc_gui_exec_file_delete(gui_xcfl);
		gui_xcfl = NULL;
	}
	
	gtk_widget_destroy (chooser);
	
	return gui_xcfl;
}

p_gmysqlcc_gui_exec_file gmysqlcc_gui_exec_file_new () {
	p_gmysqlcc_gui_exec_file gui_xcfl;
	
	/* Init structure */
	gui_xcfl = (p_gmysqlcc_gui_exec_file) g_try_malloc(sizeof(s_gmysqlcc_gui_exec_file));
	
	if (gui_xcfl == NULL) {
		return NULL; /* return NULL pointer */
	}
	
	gui_xcfl->filename = NULL;
	gui_xcfl->content = NULL;
	gui_xcfl->mysql_srv = NULL;
	gui_xcfl->mysql_db = NULL;
	gui_xcfl->mysql_mlt_qry = NULL;
	
	gmysqlcc_gui_exec_file_create_widget(gui_xcfl);
	gmysqlcc_gui_exec_file_init_widget(gui_xcfl);
	
	NbrWnd ++;
	
	return gui_xcfl;
}

void gmysqlcc_gui_exec_file_create_widget (p_gmysqlcc_gui_exec_file gui_xcfl) {
	GtkWidget *vbox;
	GtkWidget *hbuttonbox6, *hbuttonbox7;
	GtkWidget *table4, *table5;
	GtkWidget *label39, *label41, *label42, *label43;
	GtkWidget *btnShowReport, *btnExecute, *btnCancel;
  GSList *rbtDatabase_group = NULL;
	
	gui_xcfl->window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_container_set_border_width (GTK_CONTAINER (gui_xcfl->window), 3);
	gtk_window_set_title (GTK_WINDOW (gui_xcfl->window), _("Execute SQL File"));
	gtk_window_set_resizable (GTK_WINDOW (gui_xcfl->window), FALSE);
	g_signal_connect (G_OBJECT (gui_xcfl->window), "destroy", 
										G_CALLBACK (gmysqlcc_gui_exec_file_evt_destroy), (gpointer)gui_xcfl);
	
	vbox = gtk_vbox_new (FALSE, 0);
	gtk_widget_show (vbox);
	gtk_container_add (GTK_CONTAINER (gui_xcfl->window), vbox);
	gtk_container_set_border_width (GTK_CONTAINER (vbox), 2);
	
	table4 = gtk_table_new (5, 2, FALSE);
	gtk_widget_show (table4);
	gtk_box_pack_start (GTK_BOX (vbox), table4, TRUE, TRUE, 0);
	gtk_container_set_border_width (GTK_CONTAINER (table4), 2);
	gtk_table_set_row_spacings (GTK_TABLE (table4), 7);
	gtk_table_set_col_spacings (GTK_TABLE (table4), 2);
	
	label39 = gtk_label_new_with_mnemonic (_("Select _server :"));
	gtk_widget_show (label39);
	gtk_table_attach (GTK_TABLE (table4), label39, 0, 1, 0, 1,
										(GtkAttachOptions) (GTK_FILL),
										(GtkAttachOptions) (0), 0, 0);
	
	gui_xcfl->cmbServerList = gtk_combo_box_new_text ();
	gtk_widget_show (gui_xcfl->cmbServerList);
	gtk_table_attach (GTK_TABLE (table4), gui_xcfl->cmbServerList, 1, 2, 0, 1,
										(GtkAttachOptions) (GTK_FILL),
										(GtkAttachOptions) (GTK_EXPAND | GTK_FILL), 0, 0);
	g_signal_connect (G_OBJECT (gui_xcfl->cmbServerList), "changed", G_CALLBACK (gmysqlcc_gui_exec_file_evt_cmbServerList_changed), gui_xcfl);
	
  gui_xcfl->rbtExistDatabase = gtk_radio_button_new_with_mnemonic (NULL, _("Select _database :"));
  gtk_widget_show (gui_xcfl->rbtExistDatabase);
  gtk_table_attach (GTK_TABLE (table4), gui_xcfl->rbtExistDatabase, 0, 1, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_radio_button_set_group (GTK_RADIO_BUTTON (gui_xcfl->rbtExistDatabase), rbtDatabase_group);
  rbtDatabase_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (gui_xcfl->rbtExistDatabase));
	g_signal_connect (G_OBJECT (gui_xcfl->rbtExistDatabase), "clicked", G_CALLBACK (gmysqlcc_gui_exec_file_evt_rbtDatabase_clicked), gui_xcfl);

	gui_xcfl->cmbDatabaseList = gtk_combo_box_new_text ();
	gtk_widget_show (gui_xcfl->cmbDatabaseList);
	gtk_table_attach (GTK_TABLE (table4), gui_xcfl->cmbDatabaseList, 1, 2, 1, 2,
										(GtkAttachOptions) (GTK_FILL),
										(GtkAttachOptions) (GTK_EXPAND | GTK_FILL), 0, 0);
	g_signal_connect (G_OBJECT (gui_xcfl->cmbDatabaseList), "changed", G_CALLBACK (gmysqlcc_gui_exec_file_evt_cmbDatabaseList_changed), gui_xcfl);
	
  gui_xcfl->rbtNewDatabase = gtk_radio_button_new_with_mnemonic (NULL, _("_Create database :"));
  gtk_widget_show (gui_xcfl->rbtNewDatabase);
  gtk_table_attach (GTK_TABLE (table4), gui_xcfl->rbtNewDatabase, 0, 1, 2, 3,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_radio_button_set_group (GTK_RADIO_BUTTON (gui_xcfl->rbtNewDatabase), rbtDatabase_group);
  rbtDatabase_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (gui_xcfl->rbtNewDatabase));
	g_signal_connect (G_OBJECT (gui_xcfl->rbtNewDatabase), "clicked", G_CALLBACK (gmysqlcc_gui_exec_file_evt_rbtDatabase_clicked), gui_xcfl);

  gui_xcfl->txtNewDatabase = gtk_entry_new ();
  gtk_widget_show (gui_xcfl->txtNewDatabase);
  gtk_table_attach (GTK_TABLE (table4), gui_xcfl->txtNewDatabase, 1, 2, 2, 3,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

	gui_xcfl->chkStopError = gtk_check_button_new_with_mnemonic (_("Stop on _error"));
	gtk_widget_show (gui_xcfl->chkStopError);
	gtk_table_attach (GTK_TABLE (table4), gui_xcfl->chkStopError, 0, 2, 3, 4,
										(GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
										(GtkAttachOptions) (0), 0, 0);
	
	gui_xcfl->xpdResults = gtk_expander_new (NULL);
	gtk_widget_show (gui_xcfl->xpdResults);
	gtk_table_attach (GTK_TABLE (table4), gui_xcfl->xpdResults, 0, 2, 4, 5,
										(GtkAttachOptions) (GTK_SHRINK | GTK_FILL),
										(GtkAttachOptions) (GTK_EXPAND | GTK_SHRINK | GTK_FILL), 0, 0);
	gtk_container_set_border_width (GTK_CONTAINER (gui_xcfl->xpdResults), 1);
	
	table5 = gtk_table_new (4, 2, FALSE);
	gtk_widget_show (table5);
	gtk_container_add (GTK_CONTAINER (gui_xcfl->xpdResults), table5);
	gtk_container_set_border_width (GTK_CONTAINER (table5), 2);
	gtk_table_set_row_spacings (GTK_TABLE (table5), 2);
	gtk_table_set_col_spacings (GTK_TABLE (table5), 2);
	
	label42 = gtk_label_new (_("Number of executed queries :"));
	gtk_widget_show (label42);
	gtk_table_attach (GTK_TABLE (table5), label42, 0, 1, 0, 1,
										(GtkAttachOptions) (GTK_FILL),
										(GtkAttachOptions) (0), 0, 0);
	
	label43 = gtk_label_new (_("Number of queries in error :"));
	gtk_widget_show (label43);
	gtk_table_attach (GTK_TABLE (table5), label43, 0, 1, 1, 2,
										(GtkAttachOptions) (GTK_FILL),
										(GtkAttachOptions) (0), 0, 0);
	
	gui_xcfl->pgbWorking = gtk_progress_bar_new ();
	gtk_widget_show (gui_xcfl->pgbWorking);
	gtk_table_attach (GTK_TABLE (table5), gui_xcfl->pgbWorking, 0, 2, 2, 3,
										(GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
										(GtkAttachOptions) (0), 0, 0);
	
	gui_xcfl->lblExecutedQueries = gtk_label_new ("0");
	gtk_widget_show (gui_xcfl->lblExecutedQueries);
	gtk_table_attach (GTK_TABLE (table5), gui_xcfl->lblExecutedQueries, 1, 2, 0, 1,
										(GtkAttachOptions) (GTK_FILL),
										(GtkAttachOptions) (0), 0, 0);
	
	gui_xcfl->lblErrorQueries = gtk_label_new ("0");
	gtk_widget_show (gui_xcfl->lblErrorQueries);
	gtk_table_attach (GTK_TABLE (table5), gui_xcfl->lblErrorQueries, 1, 2, 1, 2,
										(GtkAttachOptions) (GTK_FILL),
										(GtkAttachOptions) (0), 0, 0);
	
	hbuttonbox7 = gtk_hbutton_box_new ();
	gtk_widget_show (hbuttonbox7);
	gtk_table_attach (GTK_TABLE (table5), hbuttonbox7, 0, 2, 3, 4,
										(GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
										(GtkAttachOptions) (GTK_FILL), 0, 0);
	
	btnShowReport = createIconButton("gtk-zoom-fit", _("Show report"));
	gtk_widget_show (btnShowReport);
	gtk_container_add (GTK_CONTAINER (hbuttonbox7), btnShowReport);
	g_signal_connect (G_OBJECT (btnShowReport), "clicked", G_CALLBACK (gmysqlcc_gui_exec_file_evt_btnShowReport_clicked), gui_xcfl);
	
	label41 = gtk_label_new_with_mnemonic (_("Execution _result"));
	gtk_widget_show (label41);
	gtk_expander_set_label_widget (GTK_EXPANDER (gui_xcfl->xpdResults), label41);
	
	hbuttonbox6 = gtk_hbutton_box_new ();
	gtk_widget_show (hbuttonbox6);
	gtk_box_pack_start (GTK_BOX (vbox), hbuttonbox6, FALSE, TRUE, 3);
	gtk_button_box_set_layout (GTK_BUTTON_BOX (hbuttonbox6), GTK_BUTTONBOX_SPREAD);
	
	btnExecute = gtk_button_new_from_stock ("gtk-execute");
	gtk_widget_show (btnExecute);
	gtk_container_add (GTK_CONTAINER (hbuttonbox6), btnExecute);
	g_signal_connect (G_OBJECT (btnExecute), "clicked", G_CALLBACK (gmysqlcc_gui_exec_file_evt_btnExecute_clicked), gui_xcfl);
	
	btnCancel = gtk_button_new_from_stock ("gtk-close");
	gtk_widget_show (btnCancel);
	gtk_container_add (GTK_CONTAINER (hbuttonbox6), btnCancel);
	g_signal_connect (G_OBJECT (btnCancel), "clicked", G_CALLBACK (gmysqlcc_gui_exec_file_evt_btnCancel_clicked), gui_xcfl);
}

void gmysqlcc_gui_exec_file_init_widget (p_gmysqlcc_gui_exec_file gui_xcfl) {
	GList * items = NULL;
	p_mysql_server mysql_srv;
	
	items = g_list_first(gmysqlcc_conf->lst_servers);
	while (items != NULL) {
		mysql_srv = (p_mysql_server)items->data;
		
		/* Display if needed the field */
		if (mysql_srv != NULL) {
			gtk_combo_box_append_text(GTK_COMBO_BOX(gui_xcfl->cmbServerList), mysql_srv->name);
		}
		
		/* Go to next item */
		items = g_list_next(items);
	}
	
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gui_xcfl->rbtExistDatabase), TRUE);
	gmysqlcc_gui_exec_file_evt_rbtDatabase_clicked(gui_xcfl->rbtExistDatabase, (gpointer)gui_xcfl);
}

void gmysqlcc_gui_exec_file_fill_database_list(p_gmysqlcc_gui_exec_file gui_xcfl) {
	
	void sub_ht_fill_database_list(gpointer key, gpointer value, gpointer user_data) {
		p_mysql_database mysql_db = (p_mysql_database)value;
		GtkComboBox * cmbDatabaseList = GTK_COMBO_BOX(gui_xcfl->cmbDatabaseList);
		
		gtk_combo_box_append_text(cmbDatabaseList, mysql_db->name);
	}
	
	if (gui_xcfl->mysql_srv != NULL) {
		g_print("gmysqlcc_gui_exec_file_fill_database_list - Server : '%s'\n", gui_xcfl->mysql_srv->name);
		
		/* Fill database list */
		if (mysql_server_refresh_database_list(gui_xcfl->mysql_srv)) {
			g_hash_table_foreach(gui_xcfl->mysql_srv->hshDbs, &sub_ht_fill_database_list, (gpointer)gui_xcfl->cmbDatabaseList);
		}
	}
}

gboolean gmysqlcc_gui_exec_file_delete (p_gmysqlcc_gui_exec_file gui_xcfl) {
	/* Destroy Datas */
	g_free(gui_xcfl->filename);
	g_free(gui_xcfl->content);
	mysql_multi_query_delete(gui_xcfl->mysql_mlt_qry);
	g_free(gui_xcfl);
	
	/* Destroy Application if needed */
	NbrWnd--;
	g_printerr("Destruction Server window - nbrWnd : %d\n", NbrWnd);
	if (NbrWnd <= 0) {
		g_printerr("Destroy App\n");
		gtk_main_quit();
	}
	
	return TRUE;
}

gboolean gmysqlcc_gui_exec_file_set_file (p_gmysqlcc_gui_exec_file gui_xcfl, const gchar * filename) {
	if (filename == NULL) {
		return FALSE;
	}
	
	if (gui_xcfl->content != NULL) {
		g_free(gui_xcfl->content);
		gui_xcfl->content = NULL;
	}
	
	gui_xcfl->filename = g_strdup(filename);
	
	return TRUE;
}

gboolean gmysqlcc_gui_exec_file_set_content (p_gmysqlcc_gui_exec_file gui_xcfl, const gchar * content) {
	if (content == NULL) {
		return FALSE;
	}
	
	if (gui_xcfl->filename != NULL) {
		g_free(gui_xcfl->filename);
		gui_xcfl->filename = NULL;
	}
	
	gui_xcfl->content = g_strdup(content);
	
	return TRUE;
}

gboolean gmysqlcc_gui_exec_file_display (p_gmysqlcc_gui_exec_file gui_xcfl, gboolean display) {
	if (display) {
		gtk_widget_show (gui_xcfl->window);
	} else {
		gtk_widget_hide (gui_xcfl->window);
	}
	
	return TRUE;
}

void gmysqlcc_gui_exec_file_evt_destroy (GtkWidget *widget, gpointer user_data) {
	p_gmysqlcc_gui_exec_file gui_xcfl = (p_gmysqlcc_gui_exec_file)user_data;
	
	gmysqlcc_gui_exec_file_display(gui_xcfl, FALSE);
	gmysqlcc_gui_exec_file_delete(gui_xcfl);
}

void gmysqlcc_gui_exec_file_evt_cmbServerList_changed (GtkComboBox *combobox, gpointer user_data) {
	p_gmysqlcc_gui_exec_file gui_xcfl = (p_gmysqlcc_gui_exec_file)user_data;
	const gchar * server_name;
	GtkTreeModel * tree_model;
	GtkTreeIter iter;
	GValue val;
	
	tree_model = gtk_combo_box_get_model(GTK_COMBO_BOX(combobox));
	if (gtk_combo_box_get_active_iter (GTK_COMBO_BOX(combobox), &iter)) {
		gtk_tree_model_get_value(tree_model, &iter, 0, &val);
		
		server_name = g_value_get_string(&val);
		
		gui_xcfl->mysql_srv = gmysqlcc_config_get_server(gmysqlcc_conf, server_name);
		gui_xcfl->mysql_db = NULL;
		
		gmysqlcc_gui_exec_file_fill_database_list(gui_xcfl);
	}
}

void gmysqlcc_gui_exec_file_evt_cmbDatabaseList_changed (GtkComboBox *combobox, gpointer user_data) {
	p_gmysqlcc_gui_exec_file gui_xcfl = (p_gmysqlcc_gui_exec_file)user_data;
	const gchar * database_name;
	GtkTreeModel * tree_model;
	GtkTreeIter iter;
	GValue val;
	
	tree_model = gtk_combo_box_get_model(GTK_COMBO_BOX(combobox));
	if (gtk_combo_box_get_active_iter (GTK_COMBO_BOX(combobox), &iter)) {
		gtk_tree_model_get_value(tree_model, &iter, 0, &val);
		
		database_name = g_value_get_string(&val);
		
		gui_xcfl->mysql_db = mysql_server_get_database(gui_xcfl->mysql_srv, database_name);
	}
}

void gmysqlcc_gui_exec_file_evt_btnExecute_clicked(GtkWidget *widget, gpointer user_data) {
	p_gmysqlcc_gui_exec_file gui_xcfl = (p_gmysqlcc_gui_exec_file)user_data;
	GtkWidget * msgdlg;
	gboolean b_stop_error;
	const gchar * db_name;
	
	msgdlg = NULL;
	
	if (gui_xcfl->mysql_srv != NULL) {
		if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(gui_xcfl->rbtExistDatabase)) && gui_xcfl->mysql_db == NULL) {
			msgdlg = gtk_message_dialog_new(GTK_WINDOW(gui_xcfl->window), GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, _("Select a database !"));
		} else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(gui_xcfl->rbtNewDatabase))) {
			db_name = gtk_entry_get_text(GTK_ENTRY(gui_xcfl->txtNewDatabase));
			
			gui_xcfl->mysql_db = mysql_database_new_create(gui_xcfl->mysql_srv, db_name);
			
			if (gui_xcfl->mysql_db == NULL) {
				msgdlg = gtk_message_dialog_new(GTK_WINDOW(gui_xcfl->window), GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, _("Can't create new database !"));
			}
		}
	} else {
		msgdlg = gtk_message_dialog_new(GTK_WINDOW(gui_xcfl->window), GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, _("Select a server !"));
	}
	
	if (msgdlg != NULL) {
		gtk_dialog_run (GTK_DIALOG (msgdlg));
		gtk_widget_destroy (msgdlg);
		return;
	}
	
	gtk_expander_set_expanded(GTK_EXPANDER(gui_xcfl->xpdResults), TRUE);
	
	b_stop_error = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(gui_xcfl->chkStopError));
	
	gui_xcfl->mysql_mlt_qry = mysql_multi_query_new(gui_xcfl->mysql_db);
	mysql_multi_query_set_status_callback(gui_xcfl->mysql_mlt_qry, gmysqlcc_gui_exec_file_evt_update_status, gui_xcfl);
	
	if (gui_xcfl->filename != NULL && gui_xcfl->content == NULL) {
		mysql_multi_query_from_file(gui_xcfl->mysql_mlt_qry, gui_xcfl->filename, b_stop_error);
	} else if (gui_xcfl->filename == NULL && gui_xcfl->content != NULL) {
		mysql_multi_query_from_string(gui_xcfl->mysql_mlt_qry, gui_xcfl->filename, b_stop_error);
	}
	
	g_print("Report : \n%s", mysql_multi_query_get_report(gui_xcfl->mysql_mlt_qry));
}

void gmysqlcc_gui_exec_file_evt_btnCancel_clicked(GtkWidget *widget, gpointer user_data) {
	p_gmysqlcc_gui_exec_file gui_xcfl = (p_gmysqlcc_gui_exec_file)user_data;
	
	gtk_widget_destroy(GTK_WIDGET(gui_xcfl->window));
}

void gmysqlcc_gui_exec_file_evt_btnShowReport_clicked(GtkWidget *widget, gpointer user_data) {
	p_gmysqlcc_gui_exec_file gui_xcfl = (p_gmysqlcc_gui_exec_file)user_data;
	p_gmysqlcc_gui_text gui_text;
	
	gui_text = gmysqlcc_gui_text_new();
	gmysqlcc_gui_text_set_content(gui_text, gui_xcfl->mysql_mlt_qry->report->str, _("report.txt"));
	gmysqlcc_gui_text_display(gui_text, TRUE);
}

void gmysqlcc_gui_exec_file_evt_rbtDatabase_clicked (GtkWidget *widget, gpointer user_data) {
	p_gmysqlcc_gui_exec_file gui_xcfl = (p_gmysqlcc_gui_exec_file)user_data;
	
	if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(gui_xcfl->rbtExistDatabase))) {
		gtk_widget_set_sensitive(gui_xcfl->cmbDatabaseList, TRUE);
		gtk_widget_set_sensitive(gui_xcfl->txtNewDatabase, FALSE);
	} else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(gui_xcfl->rbtNewDatabase))) {
		gtk_widget_set_sensitive(gui_xcfl->cmbDatabaseList, FALSE);
		gtk_widget_set_sensitive(gui_xcfl->txtNewDatabase, TRUE);
	}
}

void gmysqlcc_gui_exec_file_evt_update_status(p_mysql_multi_query mysql_mlt_qry, gboolean error, gpointer user_data) {
	p_gmysqlcc_gui_exec_file gui_xcfl = (p_gmysqlcc_gui_exec_file)user_data;
	GString * pgs_content;
	
	pgs_content = g_string_new("");
	
	g_string_printf(pgs_content, "%d", mysql_mlt_qry->nbr_query);
	gtk_label_set_text(GTK_LABEL(gui_xcfl->lblExecutedQueries), pgs_content->str);
	
	g_string_printf(pgs_content, "%d", mysql_mlt_qry->nbr_error);
	gtk_label_set_text(GTK_LABEL(gui_xcfl->lblErrorQueries), pgs_content->str);
	
	g_string_free(pgs_content, TRUE);
} 
