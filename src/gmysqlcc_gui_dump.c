
#include "gmysql_gui.h"

#include "gmysqlcc_gui_all.h"


void gmysqlcc_gui_dump_init_widget (p_gmysqlcc_gui_dump gui_dump);
void gmysqlcc_gui_dump_create_widget (p_gmysqlcc_gui_dump gui_dump);
void gmysqlcc_gui_dump_to_file (p_gmysqlcc_gui_dump gui_dump, gchar * sqlFilename);

void gmysqlcc_gui_dump_evt_destroy (GtkWidget *widget, gpointer user_data);
void gmysqlcc_gui_dump_evt_dump_clicked (GtkWidget *widget, gpointer user_data);
void gmysqlcc_gui_dump_evt_close_clicked (GtkWidget *widget, gpointer user_data);
void gmysqlcc_gui_dump_evt_format_change (GtkList *list, GtkWidget *widget, gpointer user_data);
void gmysqlcc_gui_dump_evt_level_select (GtkRadioButton *radiobutton, gpointer user_data);
void gmysqlcc_gui_dump_evt_separate_file_toggle (GtkToggleButton *togglebutton, gpointer user_data);
void gmysqlcc_gui_dump_evt_type_select (GtkRadioButton *radiobutton, gpointer user_data);

p_gmysqlcc_gui_dump gmysqlcc_gui_dump_new (p_mysql_server mysql_srv, p_mysql_database mysql_db, p_mysql_table mysql_tbl, gchar * sqlQuery) {

	p_gmysqlcc_gui_dump gui_dump;
	
	/* Init structure */
	gui_dump = (p_gmysqlcc_gui_dump) g_try_malloc(sizeof(s_gmysqlcc_gui_dump));
	
	if (gui_dump == NULL) {
		return NULL; /* return NULL pointer */
	}

	NbrWnd ++; /* A new window is created */
		
	gui_dump->mysql_srv = mysql_srv;
	gui_dump->mysql_db = mysql_db;
	gui_dump->mysql_tbl = mysql_tbl;
	gui_dump->sqlQuery = g_strdup(sqlQuery);
	gui_dump->dumpLevel = DumpLevel_Table;
	gui_dump->dumpType = 0;
	gui_dump->dumpFormat = DumpFormat_Sql;

	gmysqlcc_gui_dump_create_widget(gui_dump);
	gmysqlcc_gui_dump_init_widget(gui_dump);
	
	return gui_dump;
}

gboolean gmysqlcc_gui_dump_delete (p_gmysqlcc_gui_dump gui_dump) {

	/* Destroy Datas */
	g_free(gui_dump);
	
	/* Destroy Application if needed */
	NbrWnd--;
	g_printerr("Destruction Dump window - nbrWnd : %d\n", NbrWnd);
	if (NbrWnd <= 0) {
		g_printerr("Destroy App\n");
		gtk_main_quit();
	}
	
	return TRUE;	
}

gboolean gmysqlcc_gui_dump_display (p_gmysqlcc_gui_dump gui_dump, gboolean display) {
	if (display) {
		gtk_widget_show (gui_dump->window);
	} else {
		gtk_widget_hide (gui_dump->window);
	}
	return TRUE;	
}

void gmysqlcc_gui_dump_create_widget (p_gmysqlcc_gui_dump gui_dump) {
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

	gui_dump->window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_container_set_border_width (GTK_CONTAINER (gui_dump->window), 2);
  gtk_window_set_title (GTK_WINDOW (gui_dump->window), _("Dump SQL"));
	gtk_window_set_default_size (GTK_WINDOW (gui_dump->window), 400, 300);
	g_signal_connect (G_OBJECT (gui_dump->window), "destroy", G_CALLBACK (gmysqlcc_gui_dump_evt_destroy), gui_dump);

  vbox1 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox1);
  gtk_container_add (GTK_CONTAINER (gui_dump->window), vbox1);

  frame1 = gtk_frame_new (NULL);
  gtk_widget_show (frame1);
  gtk_box_pack_start (GTK_BOX (vbox1), frame1, TRUE, TRUE, 3);

  table1 = gtk_table_new (5, 2, FALSE);
  gtk_widget_show (table1);
  gtk_container_add (GTK_CONTAINER (frame1), table1);

  gui_dump->rdoDumpServer = gtk_radio_button_new_with_mnemonic (NULL, _("Server name :"));
  gtk_widget_show (gui_dump->rdoDumpServer);
  gtk_table_attach (GTK_TABLE (table1), gui_dump->rdoDumpServer, 0, 1, 0, 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_container_set_border_width (GTK_CONTAINER (gui_dump->rdoDumpServer), 2);
  rdoDumpLevel_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (gui_dump->rdoDumpServer));
	g_signal_connect (G_OBJECT (gui_dump->rdoDumpServer), "toggled", G_CALLBACK (gmysqlcc_gui_dump_evt_level_select), gui_dump);

  gui_dump->rdoDumpDatabase = gtk_radio_button_new_with_mnemonic (rdoDumpLevel_group, _("Database name :"));
  gtk_widget_show (gui_dump->rdoDumpDatabase);
  gtk_table_attach (GTK_TABLE (table1), gui_dump->rdoDumpDatabase, 0, 1, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_container_set_border_width (GTK_CONTAINER (gui_dump->rdoDumpDatabase), 2);
  rdoDumpLevel_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (gui_dump->rdoDumpDatabase));
	g_signal_connect (G_OBJECT (gui_dump->rdoDumpDatabase), "toggled", G_CALLBACK (gmysqlcc_gui_dump_evt_level_select), gui_dump);

  gui_dump->rdoDumpTable = gtk_radio_button_new_with_mnemonic (rdoDumpLevel_group, _("Table name :"));
  gtk_widget_show (gui_dump->rdoDumpTable);
  gtk_table_attach (GTK_TABLE (table1), gui_dump->rdoDumpTable, 0, 1, 2, 3,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_container_set_border_width (GTK_CONTAINER (gui_dump->rdoDumpTable), 2);
  rdoDumpLevel_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (gui_dump->rdoDumpTable));
	g_signal_connect (G_OBJECT (gui_dump->rdoDumpTable), "toggled", G_CALLBACK (gmysqlcc_gui_dump_evt_level_select), gui_dump);

  gui_dump->rdoDumpRequest = gtk_radio_button_new_with_mnemonic (rdoDumpLevel_group, _("SQL query :"));
  gtk_widget_show (gui_dump->rdoDumpRequest);
  gtk_table_attach (GTK_TABLE (table1), gui_dump->rdoDumpRequest, 0, 2, 3, 4,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_container_set_border_width (GTK_CONTAINER (gui_dump->rdoDumpRequest), 2);
  rdoDumpLevel_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (gui_dump->rdoDumpRequest));
	g_signal_connect (G_OBJECT (gui_dump->rdoDumpRequest), "toggled", G_CALLBACK (gmysqlcc_gui_dump_evt_level_select), gui_dump);

  scrlwndSQLRequest = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (scrlwndSQLRequest);
  gtk_table_attach (GTK_TABLE (table1), scrlwndSQLRequest, 0, 2, 4, 5,
                    (GtkAttachOptions) (GTK_FILL | GTK_EXPAND),
                    (GtkAttachOptions) (GTK_FILL | GTK_EXPAND), 0, 0);
  gtk_container_set_border_width (GTK_CONTAINER (scrlwndSQLRequest), 2);

	sql_buff = gtk_text_buffer_new((GtkTextTagTable *)NULL);
	gtk_text_buffer_set_text (sql_buff, gui_dump->sqlQuery, -1);
  gui_dump->txtSQLRequest = gtk_text_view_new_with_buffer(sql_buff);
  gtk_widget_show (gui_dump->txtSQLRequest);
  gtk_container_add (GTK_CONTAINER (scrlwndSQLRequest), gui_dump->txtSQLRequest);


	gui_dump->lblDumpServerName = gtk_label_new ("");
  gtk_widget_show (gui_dump->lblDumpServerName);
  gtk_table_attach (GTK_TABLE (table1), gui_dump->lblDumpServerName, 1, 2, 0, 1,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (gui_dump->lblDumpServerName), GTK_JUSTIFY_CENTER);
 
  gui_dump->lblDumpTableName = gtk_label_new ("");
  gtk_widget_show (gui_dump->lblDumpTableName);
  gtk_table_attach (GTK_TABLE (table1), gui_dump->lblDumpTableName, 1, 2, 2, 3,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (gui_dump->lblDumpTableName), GTK_JUSTIFY_CENTER);

  gui_dump->lblDumpDatabaseName = gtk_label_new ("");
  gtk_widget_show (gui_dump->lblDumpDatabaseName);
  gtk_table_attach (GTK_TABLE (table1), gui_dump->lblDumpDatabaseName, 1, 2, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (gui_dump->lblDumpDatabaseName), GTK_JUSTIFY_CENTER);

  label5 = gtk_label_new (_("Selection :"));
  gtk_widget_show (label5);
  gtk_frame_set_label_widget (GTK_FRAME (frame1), label5);

  frame2 = gtk_frame_new (NULL);
  gtk_widget_show (frame2);
  gtk_box_pack_start (GTK_BOX (vbox1), frame2, FALSE, TRUE, 0);

  vbox2 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox2);
  gtk_container_add (GTK_CONTAINER (frame2), vbox2);

	gui_dump->chkSeparateFile = gtk_check_button_new_with_mnemonic (_("Selection :"));
  gtk_widget_show (gui_dump->chkSeparateFile);
  gtk_box_pack_start (GTK_BOX (vbox2), gui_dump->chkSeparateFile, FALSE, FALSE, 0);
	g_signal_connect (G_OBJECT (gui_dump->chkSeparateFile), "toggled", G_CALLBACK (gmysqlcc_gui_dump_evt_separate_file_toggle), gui_dump);

  hbox2 = gtk_hbox_new (FALSE, 5);
  gtk_widget_show (hbox2);
  gtk_box_pack_start (GTK_BOX (vbox2), hbox2, FALSE, FALSE, 0);
	
	gui_dump->chkGroupInDirectory = gtk_check_button_new_with_mnemonic (_("All files in a directory"));
  gtk_widget_show (gui_dump->chkGroupInDirectory);
  gtk_box_pack_start (GTK_BOX (hbox2), gui_dump->chkGroupInDirectory, FALSE, FALSE, 0);
	
	gui_dump->txtNewDirectory = gtk_entry_new ();
  gtk_widget_show (gui_dump->txtNewDirectory);
  gtk_box_pack_start (GTK_BOX (hbox2), gui_dump->txtNewDirectory, TRUE, TRUE, 0);
	
  gui_dump->chkDropDb = gtk_check_button_new_with_mnemonic (_("Drop database if exist"));
  gtk_widget_show (gui_dump->chkDropDb);
  gtk_box_pack_start (GTK_BOX (vbox2), gui_dump->chkDropDb, FALSE, FALSE, 0);

  gui_dump->chkAddUse = gtk_check_button_new_with_mnemonic (_("Add USE database"));
  gtk_widget_show (gui_dump->chkAddUse);
  gtk_box_pack_start (GTK_BOX (vbox2), gui_dump->chkAddUse, FALSE, FALSE, 0);

  gui_dump->chkDropTable = gtk_check_button_new_with_mnemonic (_("Drop table if exist"));
  gtk_widget_show (gui_dump->chkDropTable);
  gtk_box_pack_start (GTK_BOX (vbox2), gui_dump->chkDropTable, FALSE, FALSE, 0);

  gui_dump->chkCompleteInsert = gtk_check_button_new_with_mnemonic (_("Complete inserts"));
  gtk_widget_show (gui_dump->chkCompleteInsert);
  gtk_box_pack_start (GTK_BOX (vbox2), gui_dump->chkCompleteInsert, FALSE, FALSE, 0);

  frame3 = gtk_frame_new (NULL);
  gtk_widget_show (frame3);
  gtk_box_pack_start (GTK_BOX (vbox2), frame3, FALSE, TRUE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (frame3), 3);

  vbox3 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox3);
  gtk_container_add (GTK_CONTAINER (frame3), vbox3);
  gtk_container_set_border_width (GTK_CONTAINER (vbox3), 3);

  gui_dump->rdoDumpStruct = gtk_radio_button_new_with_mnemonic (NULL, _("Structure"));
  gtk_widget_show (gui_dump->rdoDumpStruct);
  gtk_box_pack_start (GTK_BOX (vbox3), gui_dump->rdoDumpStruct, FALSE, FALSE, 0);
  rdoDumpType_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (gui_dump->rdoDumpStruct));
	g_signal_connect (G_OBJECT (gui_dump->rdoDumpStruct), "toggled", G_CALLBACK (gmysqlcc_gui_dump_evt_type_select), gui_dump);

  gui_dump->rdoDumpStructData = gtk_radio_button_new_with_mnemonic (rdoDumpType_group, _("Structure and data"));
  gtk_widget_show (gui_dump->rdoDumpStructData);
  gtk_box_pack_start (GTK_BOX (vbox3), gui_dump->rdoDumpStructData, FALSE, FALSE, 0);
  rdoDumpType_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (gui_dump->rdoDumpStructData));
	g_signal_connect (G_OBJECT (gui_dump->rdoDumpStructData), "toggled", G_CALLBACK (gmysqlcc_gui_dump_evt_type_select), gui_dump);

  hbox1 = gtk_hbox_new (FALSE, 5);
  gtk_widget_show (hbox1);
  gtk_box_pack_start (GTK_BOX (vbox3), hbox1, FALSE, FALSE, 0);

  gui_dump->rdoDumpData = gtk_radio_button_new_with_mnemonic (rdoDumpType_group, _("Data only"));
  gtk_widget_show (gui_dump->rdoDumpData);
  gtk_box_pack_start (GTK_BOX (hbox1), gui_dump->rdoDumpData, FALSE, FALSE, 0);
  rdoDumpType_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (gui_dump->rdoDumpData));
	g_signal_connect (G_OBJECT (gui_dump->rdoDumpData), "toggled", G_CALLBACK (gmysqlcc_gui_dump_evt_type_select), gui_dump);

  label8 = gtk_label_new (_("Format :"));
  gtk_widget_show (label8);
  gtk_box_pack_start (GTK_BOX (hbox1), label8, TRUE, FALSE, 0);

  gui_dump->cmbDataFormat = gtk_combo_new ();
  gtk_widget_show (gui_dump->cmbDataFormat);
  gtk_box_pack_start (GTK_BOX (hbox1), gui_dump->cmbDataFormat, TRUE, TRUE, 0);
  gtk_combo_set_value_in_list (GTK_COMBO (gui_dump->cmbDataFormat), TRUE, FALSE);
	g_signal_connect (G_OBJECT (GTK_COMBO(gui_dump->cmbDataFormat)->list), "select-child", G_CALLBACK (gmysqlcc_gui_dump_evt_format_change), gui_dump);

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

  gui_dump->cmbOutputCharset = gtk_combo_new ();
  gtk_widget_show (gui_dump->cmbOutputCharset);
  gtk_box_pack_start (GTK_BOX (hbox3), gui_dump->cmbOutputCharset, TRUE, TRUE, 0);
  gtk_combo_set_value_in_list (GTK_COMBO (gui_dump->cmbOutputCharset), TRUE, FALSE);
	
	hbuttonbox1 = gtk_hbutton_box_new ();
  gtk_widget_show (hbuttonbox1);
  gtk_box_pack_start (GTK_BOX (vbox1), hbuttonbox1, FALSE, TRUE, 3);
  gtk_button_box_set_layout (GTK_BUTTON_BOX (hbuttonbox1), GTK_BUTTONBOX_SPREAD);

	btnCancel = createIconButton("gtk-cancel", _("_Close"));
  gtk_widget_show (btnCancel);
  gtk_container_add (GTK_CONTAINER (hbuttonbox1), btnCancel);
	g_signal_connect (G_OBJECT (btnCancel), "clicked", G_CALLBACK (gmysqlcc_gui_dump_evt_close_clicked), gui_dump);

 	btnOk = createIconButton("gtk-ok", _("_Dump"));
  gtk_widget_show (btnOk);
  gtk_container_add (GTK_CONTAINER (hbuttonbox1), btnOk);
  GTK_WIDGET_SET_FLAGS (btnOk, GTK_CAN_DEFAULT);
	g_signal_connect (G_OBJECT (btnOk), "clicked", G_CALLBACK (gmysqlcc_gui_dump_evt_dump_clicked), gui_dump);

}

void gmysqlcc_gui_dump_init_widget (p_gmysqlcc_gui_dump gui_dump) {
	GList * cmbDataFormat_items = (GList *)NULL;
	GList * cmbOutputCharset_items = (GList *)NULL;
	
	gui_dump->dumpLevel = DumpLevel_Table;
	gui_dump->dumpType = 0;
	gui_dump->dumpFormat = DumpFormat_Sql;

	/* Fill format combo box */
	cmbDataFormat_items = g_list_append (cmbDataFormat_items, (gpointer) _("SQL"));
	cmbDataFormat_items = g_list_append (cmbDataFormat_items, (gpointer) _("CSV"));
	cmbDataFormat_items = g_list_append (cmbDataFormat_items, (gpointer) _("XML"));
	
	gtk_combo_set_popdown_strings (GTK_COMBO (gui_dump->cmbDataFormat), cmbDataFormat_items);
  g_list_free (cmbDataFormat_items);
	
	/* Fill output charset combo box */
	cmbOutputCharset_items = gmysql_charset_list_new();
	gtk_combo_set_popdown_strings (GTK_COMBO (gui_dump->cmbOutputCharset), cmbOutputCharset_items);
  g_list_free (cmbOutputCharset_items);
	
	/* Check datas */
	if (gui_dump->mysql_tbl != NULL) {
		gui_dump->mysql_db = gui_dump->mysql_tbl->mysql_db;
		gui_dump->mysql_srv = gui_dump->mysql_db->mysql_srv;
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gui_dump->rdoDumpTable), TRUE);
	} else if (gui_dump->mysql_db != NULL) {
		gui_dump->mysql_tbl = NULL;
		gui_dump->mysql_srv = gui_dump->mysql_db->mysql_srv;
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gui_dump->rdoDumpDatabase), TRUE);
	} else if (gui_dump->mysql_srv != NULL) {
		gui_dump->mysql_tbl = NULL;
		gui_dump->mysql_db = NULL;
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gui_dump->rdoDumpRequest), TRUE);
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gui_dump->rdoDumpServer), TRUE);
	} else {
		gui_dump->mysql_tbl = NULL;
		gui_dump->mysql_db = NULL;
		gui_dump->mysql_srv = NULL;
	}
	
	/* Fill labels and (des)activate radio box */
	if (gui_dump->mysql_srv != NULL) {
		gtk_label_set_text(GTK_LABEL(gui_dump->lblDumpServerName), gui_dump->mysql_srv->name);
	} else {
		gtk_label_set_text(GTK_LABEL(gui_dump->lblDumpServerName), _("-"));
		gtk_widget_set_sensitive(GTK_WIDGET(gui_dump->rdoDumpServer), FALSE);
	}
	
	if (gui_dump->mysql_db != NULL) {
		gtk_label_set_text(GTK_LABEL(gui_dump->lblDumpDatabaseName), gui_dump->mysql_db->name);
	} else {
		gtk_label_set_text(GTK_LABEL(gui_dump->lblDumpDatabaseName), _("-"));
		gtk_widget_set_sensitive(GTK_WIDGET(gui_dump->rdoDumpDatabase), FALSE);
	}
	
	if (gui_dump->mysql_tbl != NULL) {
		gtk_label_set_text(GTK_LABEL(gui_dump->lblDumpTableName), gui_dump->mysql_tbl->name);
	} else {
		gtk_label_set_text(GTK_LABEL(gui_dump->lblDumpTableName), _("-"));
		gtk_widget_set_sensitive(GTK_WIDGET(gui_dump->rdoDumpTable), FALSE);
	}

	if (gui_dump->sqlQuery != NULL) {
		gtk_widget_set_sensitive(GTK_WIDGET(gui_dump->rdoDumpServer), FALSE);
		gtk_widget_set_sensitive(GTK_WIDGET(gui_dump->rdoDumpDatabase), FALSE);
		gtk_widget_set_sensitive(GTK_WIDGET(gui_dump->rdoDumpTable), FALSE);
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gui_dump->rdoDumpRequest), TRUE);
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gui_dump->rdoDumpData), TRUE);
	} else {
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gui_dump->rdoDumpStructData), TRUE);
	}
}

void gmysqlcc_gui_dump_evt_level_select (GtkRadioButton *radiobutton, gpointer user_data) {
	p_gmysqlcc_gui_dump gui_dump = (p_gmysqlcc_gui_dump)user_data;

	if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON (gui_dump->rdoDumpServer))) {
		gtk_widget_set_sensitive(GTK_WIDGET(gui_dump->chkSeparateFile), TRUE);
		gtk_widget_set_sensitive(GTK_WIDGET(gui_dump->chkGroupInDirectory), FALSE);
		gtk_widget_set_sensitive(GTK_WIDGET(gui_dump->txtNewDirectory), FALSE);
		gmysqlcc_gui_dump_evt_separate_file_toggle (GTK_TOGGLE_BUTTON(gui_dump->chkSeparateFile), (gpointer)gui_dump);
		gtk_widget_set_sensitive(GTK_WIDGET(gui_dump->chkDropDb), TRUE);
		gtk_widget_set_sensitive(GTK_WIDGET(gui_dump->chkAddUse), TRUE);
		gtk_widget_set_sensitive(GTK_WIDGET(gui_dump->chkDropTable), TRUE);
		gtk_widget_set_sensitive(GTK_WIDGET(gui_dump->rdoDumpStruct), TRUE);
		gtk_widget_set_sensitive(GTK_WIDGET(gui_dump->rdoDumpStructData), TRUE);
		gui_dump->dumpLevel = DUMP_LEVEL_SERVER;
	} else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON (gui_dump->rdoDumpDatabase))) {
		gtk_widget_set_sensitive(GTK_WIDGET(gui_dump->chkSeparateFile), FALSE);
		gtk_widget_set_sensitive(GTK_WIDGET(gui_dump->chkGroupInDirectory), FALSE);
		gtk_widget_set_sensitive(GTK_WIDGET(gui_dump->txtNewDirectory), FALSE);
		gtk_widget_set_sensitive(GTK_WIDGET(gui_dump->chkDropDb), TRUE);
		gtk_widget_set_sensitive(GTK_WIDGET(gui_dump->chkAddUse), TRUE);
		gtk_widget_set_sensitive(GTK_WIDGET(gui_dump->chkDropTable), TRUE);
		gtk_widget_set_sensitive(GTK_WIDGET(gui_dump->rdoDumpStruct), TRUE);
		gtk_widget_set_sensitive(GTK_WIDGET(gui_dump->rdoDumpStructData), TRUE);
		gui_dump->dumpLevel = DUMP_LEVEL_DATABASE;
	} else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON (gui_dump->rdoDumpTable))) {
		gtk_widget_set_sensitive(GTK_WIDGET(gui_dump->chkSeparateFile), FALSE);
		gtk_widget_set_sensitive(GTK_WIDGET(gui_dump->chkGroupInDirectory), FALSE);
		gtk_widget_set_sensitive(GTK_WIDGET(gui_dump->txtNewDirectory), FALSE);
		gtk_widget_set_sensitive(GTK_WIDGET(gui_dump->chkDropDb), FALSE);
		gtk_widget_set_sensitive(GTK_WIDGET(gui_dump->chkAddUse), FALSE);
		gtk_widget_set_sensitive(GTK_WIDGET(gui_dump->chkDropTable), TRUE);
		gtk_widget_set_sensitive(GTK_WIDGET(gui_dump->rdoDumpStruct), TRUE);
		gtk_widget_set_sensitive(GTK_WIDGET(gui_dump->rdoDumpStructData), TRUE);
		gui_dump->dumpLevel = DUMP_LEVEL_TABLE;
	} else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON (gui_dump->rdoDumpRequest))) {
		gtk_widget_set_sensitive(GTK_WIDGET(gui_dump->chkSeparateFile), FALSE);
		gtk_widget_set_sensitive(GTK_WIDGET(gui_dump->chkGroupInDirectory), FALSE);
		gtk_widget_set_sensitive(GTK_WIDGET(gui_dump->txtNewDirectory), FALSE);
		gtk_widget_set_sensitive(GTK_WIDGET(gui_dump->chkDropDb), FALSE);
		gtk_widget_set_sensitive(GTK_WIDGET(gui_dump->chkAddUse), FALSE);
		gtk_widget_set_sensitive(GTK_WIDGET(gui_dump->chkDropTable), FALSE);
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gui_dump->rdoDumpData), TRUE);
		gtk_widget_set_sensitive(GTK_WIDGET(gui_dump->rdoDumpStruct), FALSE);
		gtk_widget_set_sensitive(GTK_WIDGET(gui_dump->rdoDumpStructData), FALSE);
		gui_dump->dumpLevel = DUMP_LEVEL_REQUEST;
	} 
}

void gmysqlcc_gui_dump_evt_type_select (GtkRadioButton *radiobutton, gpointer user_data) {
	p_gmysqlcc_gui_dump gui_dump = (p_gmysqlcc_gui_dump)user_data;

	if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON (gui_dump->rdoDumpStruct))) {
		gtk_widget_set_sensitive(GTK_WIDGET(gui_dump->cmbDataFormat), FALSE);
		gtk_widget_set_sensitive(GTK_WIDGET(gui_dump->chkCompleteInsert), FALSE);
		gui_dump->dumpType = DUMP_TYPE_STRUCT;
	} else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON (gui_dump->rdoDumpStructData))) {
		gtk_widget_set_sensitive(GTK_WIDGET(gui_dump->cmbDataFormat), FALSE);
		gtk_widget_set_sensitive(GTK_WIDGET(gui_dump->chkCompleteInsert), TRUE);
		gui_dump->dumpType = DUMP_TYPE_STRUCT_DATA;
	} else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON (gui_dump->rdoDumpData))) {
		gtk_widget_set_sensitive(GTK_WIDGET(gui_dump->cmbDataFormat), TRUE);
		gtk_widget_set_sensitive(GTK_WIDGET(gui_dump->chkCompleteInsert), TRUE);
		gui_dump->dumpType = DUMP_TYPE_DATA;
	}
}

void gmysqlcc_gui_dump_evt_format_change (GtkList *list, GtkWidget *widget, gpointer user_data) {
	p_gmysqlcc_gui_dump gui_dump = (p_gmysqlcc_gui_dump)user_data;

	gui_dump->dumpFormat = gtk_list_child_position(list, widget) + 1;
	g_print("format selected : %d\n", gui_dump->dumpFormat);
}

void gmysqlcc_gui_dump_evt_separate_file_toggle (GtkToggleButton *togglebutton, gpointer user_data) {
	p_gmysqlcc_gui_dump gui_dump = (p_gmysqlcc_gui_dump)user_data;

	if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON (gui_dump->chkSeparateFile))) {
		gtk_widget_set_sensitive(GTK_WIDGET(gui_dump->chkGroupInDirectory), TRUE);
		gtk_widget_set_sensitive(GTK_WIDGET(gui_dump->txtNewDirectory), TRUE);
	} else {
		gtk_widget_set_sensitive(GTK_WIDGET(gui_dump->chkGroupInDirectory), FALSE);
		gtk_widget_set_sensitive(GTK_WIDGET(gui_dump->txtNewDirectory), FALSE);
	}
}

void gmysqlcc_gui_dump_evt_dump_clicked (GtkWidget *widget, gpointer user_data) {
	p_gmysqlcc_gui_dump gui_dump = (p_gmysqlcc_gui_dump)user_data;
	GtkWidget *chooser;
	gint response;
	gchar *filename;
	
	chooser = gtk_file_chooser_dialog_new (_("Dump file name"), GTK_WINDOW(gui_dump->window), GTK_FILE_CHOOSER_ACTION_SAVE,
		GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, GTK_STOCK_SAVE, GTK_RESPONSE_OK, NULL);
	
	response = gtk_dialog_run (GTK_DIALOG (chooser));
	if (response == GTK_RESPONSE_OK) {
		filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (chooser));
	} else {
		filename = NULL;
	}
	
	gtk_widget_destroy (chooser);
		
	if (filename != NULL) {
		gmysqlcc_gui_dump_to_file (gui_dump, filename);
		g_free (filename);
	}
}

void gmysqlcc_gui_dump_evt_close_clicked (GtkWidget *widget, gpointer user_data) {
	p_gmysqlcc_gui_dump gui_dump = (p_gmysqlcc_gui_dump)user_data;
	
	gtk_widget_destroy(GTK_WIDGET(gui_dump->window));
}

void gmysqlcc_gui_dump_to_file (p_gmysqlcc_gui_dump gui_dump, gchar * sqlFilename) {
	p_mysql_dump mysql_dmp;
	GtkTextBuffer * txtBuffer;
	GtkTextIter begin, end;
	gchar * sqlRequest = (gchar *)NULL;
	gint selOutputCharset;
	e_dumpFormat format;


	format = (gui_dump->dumpFormat != 0) 
						? gui_dump->dumpFormat 
						: DumpFormat_Sql ;
	
	txtBuffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(gui_dump->txtSQLRequest));
	gtk_text_buffer_get_start_iter (GTK_TEXT_BUFFER(txtBuffer), &begin);
	gtk_text_buffer_get_end_iter (GTK_TEXT_BUFFER(txtBuffer), &end);
	sqlRequest = (gchar *)gtk_text_buffer_get_text (GTK_TEXT_BUFFER(txtBuffer), &begin, &end, FALSE);
	
	selOutputCharset = gtk_combo_box_get_active(GTK_COMBO_BOX(gui_dump->cmbOutputCharset));
	g_print("Selected charset : '%s'\n", gmysql_charset_list_get_by_index(selOutputCharset));
	

	mysql_dmp = mysql_dump_new(format, gui_dump->dumpLevel);
	
	mysql_dump_set_filename(mysql_dmp, sqlFilename, gmysql_charset_list_get_by_index(selOutputCharset));
	
	mysql_dump_set_params_server(mysql_dmp, gui_dump->mysql_srv, 
					gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(gui_dump->chkSeparateFile)), /* separate_file */
					(g_file_test(sqlFilename, G_FILE_TEST_IS_DIR)) ? sqlFilename : g_path_get_dirname(sqlFilename), /* base_directory */
					gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(gui_dump->chkGroupInDirectory)), /* group_in_directory */
					gtk_editable_get_chars(GTK_EDITABLE(gui_dump->txtNewDirectory), 0, -1)); /* group_directory */
	
	mysql_dump_set_params_database(mysql_dmp, gui_dump->mysql_db,
					gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(gui_dump->chkDropDb)), /* drop_database */
					gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(gui_dump->chkAddUse))); /* use_database */
	
	mysql_dump_set_params_table(mysql_dmp, gui_dump->mysql_tbl,
					gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(gui_dump->chkDropTable)), /* drop_table */
					(gui_dump->dumpType == DUMP_TYPE_STRUCT || gui_dump->dumpType == DUMP_TYPE_STRUCT_DATA), /* structure */
					(gui_dump->dumpType == DUMP_TYPE_DATA || gui_dump->dumpType == DUMP_TYPE_STRUCT_DATA), /* data */
					gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(gui_dump->chkCompleteInsert))); /* data_complete_insert */
	
	mysql_dump_set_query_string(mysql_dmp, sqlRequest);
	
	mysql_dump_do_to_disk(mysql_dmp);
	
	mysql_dump_free(mysql_dmp);
}

void gmysqlcc_gui_dump_evt_destroy (GtkWidget *widget, gpointer user_data) {
	p_gmysqlcc_gui_dump gui_dump = (p_gmysqlcc_gui_dump)user_data;
	
	gmysqlcc_gui_dump_display(gui_dump, FALSE);
	gmysqlcc_gui_dump_delete(gui_dump);
}

