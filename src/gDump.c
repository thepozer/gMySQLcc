
#include "gmysql_gui.h"

void initDump (p_dumpWnd p_wnd);
/*void dumpSql (GtkWidget *widget, gpointer user_data);*/
void dumpSql (p_dumpWnd p_wnd, gchar * sqlFilename);

void rdoDumpLevel_select (GtkRadioButton *radiobutton, gpointer user_data);
void rdoDumpType_select (GtkRadioButton *radiobutton, gpointer user_data);
void cmbDataFormat_change (GtkList *list, GtkWidget *widget, gpointer user_data);
void chkSeparateFile_toggle (GtkToggleButton *togglebutton, gpointer user_data);
void btnDump_clicked (GtkWidget *widget, gpointer user_data);
void btnClose_clicked (GtkWidget *widget, gpointer user_data);
void destroy(GtkWidget *widget, gpointer user_data);

void initDump (p_dumpWnd p_wnd) {
	GList * cmbDataFormat_items = (GList *)NULL;
	GList * cmbOutputCharset_items = (GList *)NULL;
	
	p_wnd->dumpLevel = DUMP_LEVEL_NULL;
	p_wnd->dumpType = DUMP_TYPE_NULL;
	p_wnd->dumpFormat = DUMP_FORMAT_NULL;

	/* Fill format combo box */
	cmbDataFormat_items = g_list_append (cmbDataFormat_items, (gpointer) _("CSV"));
	cmbDataFormat_items = g_list_append (cmbDataFormat_items, (gpointer) _("XML"));
	cmbDataFormat_items = g_list_append (cmbDataFormat_items, (gpointer) _("SQL"));
	gtk_combo_set_popdown_strings (GTK_COMBO (p_wnd->cmbDataFormat), cmbDataFormat_items);
  g_list_free (cmbDataFormat_items);
	
	/* Fill output charset combo box */
	cmbOutputCharset_items = gmysql_charset_list_new();
	gtk_combo_set_popdown_strings (GTK_COMBO (p_wnd->cmbOutputCharset), cmbOutputCharset_items);
  g_list_free (cmbOutputCharset_items);
	
	/* Check datas */
	if (p_wnd->mysql_tbl != (p_mysql_table)NULL) {
		p_wnd->mysql_db = p_wnd->mysql_tbl->mysql_db;
		p_wnd->mysql_srv = p_wnd->mysql_db->mysql_srv;
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(p_wnd->rdoDumpTable), TRUE);
	} else if (p_wnd->mysql_db != (p_mysql_database)NULL) {
		p_wnd->mysql_tbl = (p_mysql_table)NULL;
		p_wnd->mysql_srv = p_wnd->mysql_db->mysql_srv;
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(p_wnd->rdoDumpDatabase), TRUE);
	} else if (p_wnd->mysql_srv != (p_mysql_server)NULL) {
		p_wnd->mysql_tbl = (p_mysql_table)NULL;
		p_wnd->mysql_db = (p_mysql_database)NULL;
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(p_wnd->rdoDumpRequest), TRUE);
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(p_wnd->rdoDumpServer), TRUE);
	} else {
		p_wnd->mysql_tbl = (p_mysql_table)NULL;
		p_wnd->mysql_db = (p_mysql_database)NULL;
		p_wnd->mysql_srv = (p_mysql_server)NULL;
	}
	
	/* Fill labels and (des)activate radio box */
	if (p_wnd->mysql_srv != (p_mysql_server)NULL) {
		gtk_label_set_text(GTK_LABEL(p_wnd->lblDumpServerName), p_wnd->mysql_srv->name);
	} else {
		gtk_label_set_text(GTK_LABEL(p_wnd->lblDumpServerName), _("-"));
		gtk_widget_set_sensitive(GTK_WIDGET(p_wnd->rdoDumpServer), FALSE);
	}
	if (p_wnd->mysql_db != (p_mysql_database)NULL) {
		gtk_label_set_text(GTK_LABEL(p_wnd->lblDumpDatabaseName), p_wnd->mysql_db->name);
	} else {
		gtk_label_set_text(GTK_LABEL(p_wnd->lblDumpDatabaseName), _("-"));
		gtk_widget_set_sensitive(GTK_WIDGET(p_wnd->rdoDumpDatabase), FALSE);
	}
	if (p_wnd->mysql_tbl != (p_mysql_table)NULL) {
		gtk_label_set_text(GTK_LABEL(p_wnd->lblDumpTableName), p_wnd->mysql_tbl->name);
	} else {
		gtk_label_set_text(GTK_LABEL(p_wnd->lblDumpTableName), _("-"));
		gtk_widget_set_sensitive(GTK_WIDGET(p_wnd->rdoDumpTable), FALSE);
	}

	if (p_wnd->onlyQuery) {
		gtk_widget_set_sensitive(GTK_WIDGET(p_wnd->rdoDumpServer), FALSE);
		gtk_widget_set_sensitive(GTK_WIDGET(p_wnd->rdoDumpDatabase), FALSE);
		gtk_widget_set_sensitive(GTK_WIDGET(p_wnd->rdoDumpTable), FALSE);
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(p_wnd->rdoDumpRequest), TRUE);
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(p_wnd->rdoDumpData), TRUE);
	} else {
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(p_wnd->rdoDumpStructData), TRUE);
	}
}

void rdoDumpLevel_select (GtkRadioButton *radiobutton, gpointer user_data) {
	p_dumpWnd p_wnd = (p_dumpWnd)user_data;

	if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON (p_wnd->rdoDumpServer))) {
		gtk_widget_set_sensitive(GTK_WIDGET(p_wnd->chkSeparateFile), TRUE);
		gtk_widget_set_sensitive(GTK_WIDGET(p_wnd->chkGroupInDirectory), FALSE);
		gtk_widget_set_sensitive(GTK_WIDGET(p_wnd->txtNewDirectory), FALSE);
		chkSeparateFile_toggle (GTK_TOGGLE_BUTTON(p_wnd->chkSeparateFile), (gpointer)p_wnd);
		gtk_widget_set_sensitive(GTK_WIDGET(p_wnd->chkDropDb), TRUE);
		gtk_widget_set_sensitive(GTK_WIDGET(p_wnd->chkAddUse), TRUE);
		gtk_widget_set_sensitive(GTK_WIDGET(p_wnd->chkDropTable), TRUE);
		gtk_widget_set_sensitive(GTK_WIDGET(p_wnd->rdoDumpStruct), TRUE);
		gtk_widget_set_sensitive(GTK_WIDGET(p_wnd->rdoDumpStructData), TRUE);
		p_wnd->dumpLevel = DUMP_LEVEL_SERVER;
	} else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON (p_wnd->rdoDumpDatabase))) {
		gtk_widget_set_sensitive(GTK_WIDGET(p_wnd->chkSeparateFile), FALSE);
		gtk_widget_set_sensitive(GTK_WIDGET(p_wnd->chkGroupInDirectory), FALSE);
		gtk_widget_set_sensitive(GTK_WIDGET(p_wnd->txtNewDirectory), FALSE);
		gtk_widget_set_sensitive(GTK_WIDGET(p_wnd->chkDropDb), TRUE);
		gtk_widget_set_sensitive(GTK_WIDGET(p_wnd->chkAddUse), TRUE);
		gtk_widget_set_sensitive(GTK_WIDGET(p_wnd->chkDropTable), TRUE);
		gtk_widget_set_sensitive(GTK_WIDGET(p_wnd->rdoDumpStruct), TRUE);
		gtk_widget_set_sensitive(GTK_WIDGET(p_wnd->rdoDumpStructData), TRUE);
		p_wnd->dumpLevel = DUMP_LEVEL_DATABASE;
	} else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON (p_wnd->rdoDumpTable))) {
		gtk_widget_set_sensitive(GTK_WIDGET(p_wnd->chkSeparateFile), FALSE);
		gtk_widget_set_sensitive(GTK_WIDGET(p_wnd->chkGroupInDirectory), FALSE);
		gtk_widget_set_sensitive(GTK_WIDGET(p_wnd->txtNewDirectory), FALSE);
		gtk_widget_set_sensitive(GTK_WIDGET(p_wnd->chkDropDb), FALSE);
		gtk_widget_set_sensitive(GTK_WIDGET(p_wnd->chkAddUse), FALSE);
		gtk_widget_set_sensitive(GTK_WIDGET(p_wnd->chkDropTable), TRUE);
		gtk_widget_set_sensitive(GTK_WIDGET(p_wnd->rdoDumpStruct), TRUE);
		gtk_widget_set_sensitive(GTK_WIDGET(p_wnd->rdoDumpStructData), TRUE);
		p_wnd->dumpLevel = DUMP_LEVEL_TABLE;
	} else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON (p_wnd->rdoDumpRequest))) {
		gtk_widget_set_sensitive(GTK_WIDGET(p_wnd->chkSeparateFile), FALSE);
		gtk_widget_set_sensitive(GTK_WIDGET(p_wnd->chkGroupInDirectory), FALSE);
		gtk_widget_set_sensitive(GTK_WIDGET(p_wnd->txtNewDirectory), FALSE);
		gtk_widget_set_sensitive(GTK_WIDGET(p_wnd->chkDropDb), FALSE);
		gtk_widget_set_sensitive(GTK_WIDGET(p_wnd->chkAddUse), FALSE);
		gtk_widget_set_sensitive(GTK_WIDGET(p_wnd->chkDropTable), FALSE);
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(p_wnd->rdoDumpData), TRUE);
		gtk_widget_set_sensitive(GTK_WIDGET(p_wnd->rdoDumpStruct), FALSE);
		gtk_widget_set_sensitive(GTK_WIDGET(p_wnd->rdoDumpStructData), FALSE);
		p_wnd->dumpLevel = DUMP_LEVEL_REQUEST;
	} 
}

void rdoDumpType_select (GtkRadioButton *radiobutton, gpointer user_data) {
	p_dumpWnd p_wnd = (p_dumpWnd)user_data;

	if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON (p_wnd->rdoDumpStruct))) {
		gtk_widget_set_sensitive(GTK_WIDGET(p_wnd->cmbDataFormat), FALSE);
		gtk_widget_set_sensitive(GTK_WIDGET(p_wnd->chkCompleteInsert), FALSE);
		p_wnd->dumpType = DUMP_TYPE_STRUCT;
	} else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON (p_wnd->rdoDumpStructData))) {
		gtk_widget_set_sensitive(GTK_WIDGET(p_wnd->cmbDataFormat), FALSE);
		gtk_widget_set_sensitive(GTK_WIDGET(p_wnd->chkCompleteInsert), TRUE);
		p_wnd->dumpType = DUMP_TYPE_STRUCT_DATA;
	} else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON (p_wnd->rdoDumpData))) {
		gtk_widget_set_sensitive(GTK_WIDGET(p_wnd->cmbDataFormat), TRUE);
		gtk_widget_set_sensitive(GTK_WIDGET(p_wnd->chkCompleteInsert), TRUE);
		p_wnd->dumpType = DUMP_TYPE_DATA;
	}
}

void cmbDataFormat_change (GtkList *list, GtkWidget *widget, gpointer user_data) {
	p_dumpWnd p_wnd = (p_dumpWnd)user_data;
	
	p_wnd->dumpFormat = gtk_list_child_position(list, widget) + 1;
	g_print("format selected : %d\n", p_wnd->dumpFormat);
}

void chkSeparateFile_toggle (GtkToggleButton *togglebutton, gpointer user_data) {
	p_dumpWnd p_wnd = (p_dumpWnd)user_data;
	
	if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON (p_wnd->chkSeparateFile))) {
		gtk_widget_set_sensitive(GTK_WIDGET(p_wnd->chkGroupInDirectory), TRUE);
		gtk_widget_set_sensitive(GTK_WIDGET(p_wnd->txtNewDirectory), TRUE);
	} else {
		gtk_widget_set_sensitive(GTK_WIDGET(p_wnd->chkGroupInDirectory), FALSE);
		gtk_widget_set_sensitive(GTK_WIDGET(p_wnd->txtNewDirectory), FALSE);
	}
}

void btnDump_clicked (GtkWidget *widget, gpointer user_data) {
	p_dumpWnd p_wnd = (p_dumpWnd)user_data;
	GtkWidget *chooser;
	gint response;
	gchar *filename;
	
	chooser = gtk_file_chooser_dialog_new (_("Dump file name"), GTK_WINDOW(p_wnd->wndDump), GTK_FILE_CHOOSER_ACTION_SAVE,
		GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, GTK_STOCK_SAVE, GTK_RESPONSE_OK, NULL);
	
	response = gtk_dialog_run (GTK_DIALOG (chooser));
	if (response == GTK_RESPONSE_OK) {
		filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (chooser));
	} else {
		filename = NULL;
	}
	
	gtk_widget_destroy (chooser);
		
	if (filename != NULL) {
		dumpSql (p_wnd, filename);
		g_free (filename);
	}
}

void dumpSql (p_dumpWnd p_wnd, gchar * sqlFilename) {
	s_dump_server_params dump_param_srv;
	p_mysql_query mysql_qry;
	GtkTextBuffer * txtBuffer;
	GtkTextIter begin, end;
	GtkWidget * msgdlg;
	GIOChannel * dumpFile;
	GError * err = (GError *)NULL;
	gchar * sqlRequest = (gchar *)NULL;
	gint selOutputCharset;
	
	txtBuffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(p_wnd->txtSQLRequest));
	gtk_text_buffer_get_start_iter (GTK_TEXT_BUFFER(txtBuffer), &begin);
	gtk_text_buffer_get_end_iter (GTK_TEXT_BUFFER(txtBuffer), &end);
	sqlRequest = (gchar *)gtk_text_buffer_get_text (GTK_TEXT_BUFFER(txtBuffer), &begin, &end, FALSE);

	dump_param_srv.separate_file = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(p_wnd->chkSeparateFile));
	dump_param_srv.group_in_directory = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(p_wnd->chkGroupInDirectory));
	dump_param_srv.sql_filename = sqlFilename;
	if (g_file_test(sqlFilename, G_FILE_TEST_IS_DIR)) {
		dump_param_srv.base_directory = sqlFilename;
	} else {
		dump_param_srv.base_directory = g_path_get_dirname(sqlFilename);
	}
	dump_param_srv.group_directory = gtk_editable_get_chars(GTK_EDITABLE(p_wnd->txtNewDirectory), 0, -1);

	dump_param_srv.database.drop_database = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(p_wnd->chkDropDb));
	dump_param_srv.database.use_database = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(p_wnd->chkAddUse));
	dump_param_srv.database.sql_filename = sqlFilename;
	
	dump_param_srv.database.table.drop_table = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(p_wnd->chkDropTable));
	dump_param_srv.database.table.data_complete_insert = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(p_wnd->chkCompleteInsert));
	dump_param_srv.database.table.structure = (p_wnd->dumpType == DUMP_TYPE_STRUCT || p_wnd->dumpType == DUMP_TYPE_STRUCT_DATA);
	dump_param_srv.database.table.data = (p_wnd->dumpType == DUMP_TYPE_DATA || p_wnd->dumpType == DUMP_TYPE_STRUCT_DATA);
	dump_param_srv.database.table.sql_filename = sqlFilename;
	
	selOutputCharset = gtk_combo_box_get_active(GTK_COMBO_BOX(p_wnd->cmbOutputCharset));
	g_print("Selected charset : '%s'\n", gmysql_charset_list_get_by_index(selOutputCharset));
	
	switch (p_wnd->dumpLevel) {
		case DUMP_LEVEL_SERVER : /* Dump the server */
			dump_param_srv.database.sql_filename = (gchar *)NULL;
			dump_param_srv.database.table.sql_filename = (gchar *)NULL;
			mysql_server_dump_direct(p_wnd->mysql_srv, &dump_param_srv, (GIOChannel *)NULL, gmysql_charset_list_get_by_index(selOutputCharset));
			break;
		case DUMP_LEVEL_DATABASE : /* Dump the database */
			dump_param_srv.database.table.sql_filename = (gchar *)NULL;
			mysql_database_dump_direct(p_wnd->mysql_db, &dump_param_srv.database, (GIOChannel *)NULL, gmysql_charset_list_get_by_index(selOutputCharset));
			break;
		case DUMP_LEVEL_TABLE : /* Dump the table */
			mysql_table_dump_direct(p_wnd->mysql_tbl, &dump_param_srv.database.table, (GIOChannel *)NULL, gmysql_charset_list_get_by_index(selOutputCharset));
			break;
		case DUMP_LEVEL_REQUEST : /* Dump with the request */
			
			if (p_wnd->mysql_db == (p_mysql_database)NULL) {
				mysql_qry = mysql_server_query(p_wnd->mysql_srv, (gchar *)NULL);
			} else {
				mysql_qry = mysql_database_query(p_wnd->mysql_db);
			}
			
			if (!mysql_query_execute_query(mysql_qry, sqlRequest, FALSE)) {
				/* Query Not Ok */
				msgdlg = gtk_message_dialog_new(GTK_WINDOW(p_wnd->wndDump), GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, _("Error during the query : (%d) %s"), mysql_qry->errCode, mysql_qry->errMsg);
				gtk_dialog_run (GTK_DIALOG (msgdlg));
				gtk_widget_destroy (msgdlg);
				mysql_query_free_query(mysql_qry);
				break;
			}
			
			dumpFile = g_io_channel_new_file(sqlFilename, "w", &err);
			g_io_channel_set_encoding(dumpFile, gmysql_charset_list_get_by_index(selOutputCharset), &err);

			switch(p_wnd->dumpFormat) {
				case DUMP_FORMAT_CSV : /* CSV Format */
					mysql_dump_query_csv_direct(mysql_qry, dumpFile);
					break;
				case DUMP_FORMAT_XML : /* XML Format */
					mysql_dump_query_xml_direct(mysql_qry, dumpFile, TRUE, TRUE, TRUE, TRUE);
					break;
				case DUMP_FORMAT_SQL : /* SQL Format */
					mysql_dump_query_sql_direct(mysql_qry, dump_param_srv.database.table.data_complete_insert, dumpFile);
					break;
			}

			g_io_channel_unref(dumpFile);
			
			mysql_query_delete(mysql_qry);
			break;
	}

	g_free(sqlFilename);
	g_free(dump_param_srv.base_directory);
	g_free(dump_param_srv.group_directory);
}

void btnClose_clicked (GtkWidget *widget, gpointer user_data) {
	p_dumpWnd p_wnd = (p_dumpWnd)user_data;

	gtk_widget_destroy(GTK_WIDGET(p_wnd->wndDump));
}

void destroy(GtkWidget *widget, gpointer user_data) {
	p_dumpWnd p_wnd = (p_dumpWnd)user_data;
	
	/* Destroy GUI */
	gtk_widget_hide (GTK_WIDGET(p_wnd->wndDump));
	gtk_widget_destroy (GTK_WIDGET(p_wnd->wndDump));
	
	/* Destroy Datas */
	g_free(p_wnd);
	
	/* Destroy Application if needed */
	NbrWnd--;
	g_printerr("Destruction Dump window - nbrWnd : %d\n", NbrWnd);
	if (NbrWnd <= 0) {
		g_printerr("Destroy App\n");
		gtk_main_quit();
	}
}

p_dumpWnd create_wndDump (gboolean display, p_mysql_server mysql_srv, p_mysql_database mysql_db, p_mysql_table mysql_tbl, gchar * sqlQuery) {
  GtkWidget *vbox1, *vbox2,  *vbox3;
  GtkWidget *frame1, *frame2, *frame3;
  GtkWidget *table1;
  GtkWidget *label5, *label6, *label7, *label8, *label9;
  GtkWidget *hbuttonbox1;
  GtkWidget *btnCancel, *btnOk;
	GtkWidget *scrlwndSQLRequest;
  GtkWidget *hbox1, *hbox2, *hbox3;
	GSList *rdoDumpLevel_group = NULL;
  GSList *rdoDumpType_group = NULL;
	GtkTextBuffer * sql_buff;

	p_dumpWnd p_wnd;
	
	/* Init structure */
	p_wnd = (p_dumpWnd) g_try_malloc(sizeof(dumpWnd));
	
	if (p_wnd == (p_dumpWnd)NULL) {
		return (p_dumpWnd)NULL; /* return NULL pointer */
	}

	p_wnd->mysql_srv = mysql_srv;
	p_wnd->mysql_db = mysql_db;
	p_wnd->mysql_tbl = mysql_tbl;
	p_wnd->onlyQuery = (sqlQuery != (gchar *)NULL);
	
  p_wnd->wndDump = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_container_set_border_width (GTK_CONTAINER (p_wnd->wndDump), 2);
  gtk_window_set_title (GTK_WINDOW (p_wnd->wndDump), _("Dump SQL"));
	gtk_window_set_default_size (GTK_WINDOW (p_wnd->wndDump), 400, 300);
	g_signal_connect (G_OBJECT (p_wnd->wndDump), "destroy", G_CALLBACK (destroy), p_wnd);

  vbox1 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox1);
  gtk_container_add (GTK_CONTAINER (p_wnd->wndDump), vbox1);

  frame1 = gtk_frame_new (NULL);
  gtk_widget_show (frame1);
  gtk_box_pack_start (GTK_BOX (vbox1), frame1, TRUE, TRUE, 3);

  table1 = gtk_table_new (5, 2, FALSE);
  gtk_widget_show (table1);
  gtk_container_add (GTK_CONTAINER (frame1), table1);

  p_wnd->rdoDumpServer = gtk_radio_button_new_with_mnemonic (NULL, _("Server name :"));
  gtk_widget_show (p_wnd->rdoDumpServer);
  gtk_table_attach (GTK_TABLE (table1), p_wnd->rdoDumpServer, 0, 1, 0, 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_container_set_border_width (GTK_CONTAINER (p_wnd->rdoDumpServer), 2);
  rdoDumpLevel_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (p_wnd->rdoDumpServer));
	g_signal_connect (G_OBJECT (p_wnd->rdoDumpServer), "toggled", G_CALLBACK (rdoDumpLevel_select), p_wnd);

  p_wnd->rdoDumpDatabase = gtk_radio_button_new_with_mnemonic (rdoDumpLevel_group, _("Database name :"));
  gtk_widget_show (p_wnd->rdoDumpDatabase);
  gtk_table_attach (GTK_TABLE (table1), p_wnd->rdoDumpDatabase, 0, 1, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_container_set_border_width (GTK_CONTAINER (p_wnd->rdoDumpDatabase), 2);
  rdoDumpLevel_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (p_wnd->rdoDumpDatabase));
	g_signal_connect (G_OBJECT (p_wnd->rdoDumpDatabase), "toggled", G_CALLBACK (rdoDumpLevel_select), p_wnd);

  p_wnd->rdoDumpTable = gtk_radio_button_new_with_mnemonic (rdoDumpLevel_group, _("Table name :"));
  gtk_widget_show (p_wnd->rdoDumpTable);
  gtk_table_attach (GTK_TABLE (table1), p_wnd->rdoDumpTable, 0, 1, 2, 3,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_container_set_border_width (GTK_CONTAINER (p_wnd->rdoDumpTable), 2);
  rdoDumpLevel_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (p_wnd->rdoDumpTable));
	g_signal_connect (G_OBJECT (p_wnd->rdoDumpTable), "toggled", G_CALLBACK (rdoDumpLevel_select), p_wnd);

  p_wnd->rdoDumpRequest = gtk_radio_button_new_with_mnemonic (rdoDumpLevel_group, _("SQL query :"));
  gtk_widget_show (p_wnd->rdoDumpRequest);
  gtk_table_attach (GTK_TABLE (table1), p_wnd->rdoDumpRequest, 0, 2, 3, 4,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_container_set_border_width (GTK_CONTAINER (p_wnd->rdoDumpRequest), 2);
  rdoDumpLevel_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (p_wnd->rdoDumpRequest));
	g_signal_connect (G_OBJECT (p_wnd->rdoDumpRequest), "toggled", G_CALLBACK (rdoDumpLevel_select), p_wnd);

  scrlwndSQLRequest = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (scrlwndSQLRequest);
  gtk_table_attach (GTK_TABLE (table1), scrlwndSQLRequest, 0, 2, 4, 5,
                    (GtkAttachOptions) (GTK_FILL | GTK_EXPAND),
                    (GtkAttachOptions) (GTK_FILL | GTK_EXPAND), 0, 0);
  gtk_container_set_border_width (GTK_CONTAINER (scrlwndSQLRequest), 2);

	sql_buff = gtk_text_buffer_new((GtkTextTagTable *)NULL);
	gtk_text_buffer_set_text (sql_buff, sqlQuery, -1);
  p_wnd->txtSQLRequest = gtk_text_view_new_with_buffer(sql_buff);
  gtk_widget_show (p_wnd->txtSQLRequest);
  gtk_container_add (GTK_CONTAINER (scrlwndSQLRequest), p_wnd->txtSQLRequest);


	p_wnd->lblDumpServerName = gtk_label_new ("");
  gtk_widget_show (p_wnd->lblDumpServerName);
  gtk_table_attach (GTK_TABLE (table1), p_wnd->lblDumpServerName, 1, 2, 0, 1,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (p_wnd->lblDumpServerName), GTK_JUSTIFY_CENTER);
 
  p_wnd->lblDumpTableName = gtk_label_new ("");
  gtk_widget_show (p_wnd->lblDumpTableName);
  gtk_table_attach (GTK_TABLE (table1), p_wnd->lblDumpTableName, 1, 2, 2, 3,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (p_wnd->lblDumpTableName), GTK_JUSTIFY_CENTER);

  p_wnd->lblDumpDatabaseName = gtk_label_new ("");
  gtk_widget_show (p_wnd->lblDumpDatabaseName);
  gtk_table_attach (GTK_TABLE (table1), p_wnd->lblDumpDatabaseName, 1, 2, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (p_wnd->lblDumpDatabaseName), GTK_JUSTIFY_CENTER);

  label5 = gtk_label_new (_("Selection :"));
  gtk_widget_show (label5);
  gtk_frame_set_label_widget (GTK_FRAME (frame1), label5);

  frame2 = gtk_frame_new (NULL);
  gtk_widget_show (frame2);
  gtk_box_pack_start (GTK_BOX (vbox1), frame2, FALSE, TRUE, 0);

  vbox2 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox2);
  gtk_container_add (GTK_CONTAINER (frame2), vbox2);

	p_wnd->chkSeparateFile = gtk_check_button_new_with_mnemonic (_("Selection :"));
  gtk_widget_show (p_wnd->chkSeparateFile);
  gtk_box_pack_start (GTK_BOX (vbox2), p_wnd->chkSeparateFile, FALSE, FALSE, 0);
	g_signal_connect (G_OBJECT (p_wnd->chkSeparateFile), "toggled", G_CALLBACK (chkSeparateFile_toggle), p_wnd);

  hbox2 = gtk_hbox_new (FALSE, 5);
  gtk_widget_show (hbox2);
  gtk_box_pack_start (GTK_BOX (vbox2), hbox2, FALSE, FALSE, 0);
	
	p_wnd->chkGroupInDirectory = gtk_check_button_new_with_mnemonic (_("All files in a directory"));
  gtk_widget_show (p_wnd->chkGroupInDirectory);
  gtk_box_pack_start (GTK_BOX (hbox2), p_wnd->chkGroupInDirectory, FALSE, FALSE, 0);
	
	p_wnd->txtNewDirectory = gtk_entry_new ();
  gtk_widget_show (p_wnd->txtNewDirectory);
  gtk_box_pack_start (GTK_BOX (hbox2), p_wnd->txtNewDirectory, TRUE, TRUE, 0);
	
  p_wnd->chkDropDb = gtk_check_button_new_with_mnemonic (_("Drop database if exist"));
  gtk_widget_show (p_wnd->chkDropDb);
  gtk_box_pack_start (GTK_BOX (vbox2), p_wnd->chkDropDb, FALSE, FALSE, 0);

  p_wnd->chkAddUse = gtk_check_button_new_with_mnemonic (_("Add USE database"));
  gtk_widget_show (p_wnd->chkAddUse);
  gtk_box_pack_start (GTK_BOX (vbox2), p_wnd->chkAddUse, FALSE, FALSE, 0);

  p_wnd->chkDropTable = gtk_check_button_new_with_mnemonic (_("Drop table if exist"));
  gtk_widget_show (p_wnd->chkDropTable);
  gtk_box_pack_start (GTK_BOX (vbox2), p_wnd->chkDropTable, FALSE, FALSE, 0);

  p_wnd->chkCompleteInsert = gtk_check_button_new_with_mnemonic (_("Complete inserts"));
  gtk_widget_show (p_wnd->chkCompleteInsert);
  gtk_box_pack_start (GTK_BOX (vbox2), p_wnd->chkCompleteInsert, FALSE, FALSE, 0);

  frame3 = gtk_frame_new (NULL);
  gtk_widget_show (frame3);
  gtk_box_pack_start (GTK_BOX (vbox2), frame3, FALSE, TRUE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (frame3), 3);

  vbox3 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox3);
  gtk_container_add (GTK_CONTAINER (frame3), vbox3);
  gtk_container_set_border_width (GTK_CONTAINER (vbox3), 3);

  p_wnd->rdoDumpStruct = gtk_radio_button_new_with_mnemonic (NULL, _("Structure"));
  gtk_widget_show (p_wnd->rdoDumpStruct);
  gtk_box_pack_start (GTK_BOX (vbox3), p_wnd->rdoDumpStruct, FALSE, FALSE, 0);
  rdoDumpType_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (p_wnd->rdoDumpStruct));
	g_signal_connect (G_OBJECT (p_wnd->rdoDumpStruct), "toggled", G_CALLBACK (rdoDumpType_select), p_wnd);

  p_wnd->rdoDumpStructData = gtk_radio_button_new_with_mnemonic (rdoDumpType_group, _("Structure and data"));
  gtk_widget_show (p_wnd->rdoDumpStructData);
  gtk_box_pack_start (GTK_BOX (vbox3), p_wnd->rdoDumpStructData, FALSE, FALSE, 0);
  rdoDumpType_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (p_wnd->rdoDumpStructData));
	g_signal_connect (G_OBJECT (p_wnd->rdoDumpStructData), "toggled", G_CALLBACK (rdoDumpType_select), p_wnd);

  hbox1 = gtk_hbox_new (FALSE, 5);
  gtk_widget_show (hbox1);
  gtk_box_pack_start (GTK_BOX (vbox3), hbox1, FALSE, FALSE, 0);

  p_wnd->rdoDumpData = gtk_radio_button_new_with_mnemonic (rdoDumpType_group, _("Data only"));
  gtk_widget_show (p_wnd->rdoDumpData);
  gtk_box_pack_start (GTK_BOX (hbox1), p_wnd->rdoDumpData, FALSE, FALSE, 0);
  rdoDumpType_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (p_wnd->rdoDumpData));
	g_signal_connect (G_OBJECT (p_wnd->rdoDumpData), "toggled", G_CALLBACK (rdoDumpType_select), p_wnd);

  label8 = gtk_label_new (_("Format :"));
  gtk_widget_show (label8);
  gtk_box_pack_start (GTK_BOX (hbox1), label8, TRUE, FALSE, 0);

  p_wnd->cmbDataFormat = gtk_combo_new ();
  gtk_widget_show (p_wnd->cmbDataFormat);
  gtk_box_pack_start (GTK_BOX (hbox1), p_wnd->cmbDataFormat, TRUE, TRUE, 0);
  gtk_combo_set_value_in_list (GTK_COMBO (p_wnd->cmbDataFormat), TRUE, FALSE);
	g_signal_connect (G_OBJECT (GTK_COMBO(p_wnd->cmbDataFormat)->list), "select-child", G_CALLBACK (cmbDataFormat_change), (gpointer)p_wnd);

  label7 = gtk_label_new (_("Table :"));
  gtk_widget_show (label7);
  gtk_frame_set_label_widget (GTK_FRAME (frame3), label7);

  label6 = gtk_label_new (_("Options :"));
  gtk_widget_show (label6);
  gtk_frame_set_label_widget (GTK_FRAME (frame2), label6);

  hbox3 = gtk_hbox_new (FALSE, 5);
  gtk_widget_show (hbox3);
  gtk_box_pack_start (GTK_BOX (vbox1), hbox3, FALSE, FALSE, 0);
	
  label9 = gtk_label_new (_("Output charset :"));
  gtk_widget_show (label9);
  gtk_box_pack_start (GTK_BOX (hbox3), label9, TRUE, FALSE, 0);

  p_wnd->cmbOutputCharset = gtk_combo_new ();
  gtk_widget_show (p_wnd->cmbOutputCharset);
  gtk_box_pack_start (GTK_BOX (hbox3), p_wnd->cmbOutputCharset, TRUE, TRUE, 0);
  gtk_combo_set_value_in_list (GTK_COMBO (p_wnd->cmbOutputCharset), TRUE, FALSE);
	
	hbuttonbox1 = gtk_hbutton_box_new ();
  gtk_widget_show (hbuttonbox1);
  gtk_box_pack_start (GTK_BOX (vbox1), hbuttonbox1, FALSE, TRUE, 3);
  gtk_button_box_set_layout (GTK_BUTTON_BOX (hbuttonbox1), GTK_BUTTONBOX_SPREAD);

	btnCancel = createIconButton("gtk-cancel", _("_Close"));
  gtk_widget_show (btnCancel);
  gtk_container_add (GTK_CONTAINER (hbuttonbox1), btnCancel);
	g_signal_connect (G_OBJECT (btnCancel), "clicked", G_CALLBACK (btnClose_clicked), (gpointer)p_wnd);

 	btnOk = createIconButton("gtk-ok", _("_Dump"));
  gtk_widget_show (btnOk);
  gtk_container_add (GTK_CONTAINER (hbuttonbox1), btnOk);
  GTK_WIDGET_SET_FLAGS (btnOk, GTK_CAN_DEFAULT);
	g_signal_connect (G_OBJECT (btnOk), "clicked", G_CALLBACK (btnDump_clicked), (gpointer)p_wnd);

	initDump(p_wnd);
	
	if (display) {
		gtk_widget_show (p_wnd->wndDump);
		NbrWnd ++;	
	}

  return p_wnd;
}
