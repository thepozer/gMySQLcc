

#include "gmysqlcc_gui_all.h"


void gmysqlcc_gui_dump_init_widget (p_gmysqlcc_gui_dump gui_dump);
void gmysqlcc_gui_dump_create_widget (p_gmysqlcc_gui_dump gui_dump);
void gmysqlcc_gui_dump_to_file (p_gmysqlcc_gui_dump gui_dump, gchar * sqlFilename);

void gmysqlcc_gui_dump_evt_destroy (GtkWidget *widget, gpointer user_data);
void gmysqlcc_gui_dump_evt_dump_clicked (GtkWidget *widget, gpointer user_data);
void gmysqlcc_gui_dump_evt_close_clicked (GtkWidget *widget, gpointer user_data);
void gmysqlcc_gui_dump_evt_getOutputFilename_clicked (GtkWidget *widget, gpointer user_data);
void gmysqlcc_gui_dump_evt_format_change (GtkWidget *widget, gpointer user_data);
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
	GtkWidget *vbox14, *vbox15, *vbox16;
  GtkWidget *hbox15,  *hbox18;
  GtkWidget *frame5, *frame6, *frame7, *frame8;
  GtkWidget *table2, *table3;
  GtkWidget *label31, *label32, *label33, *label34, *label35, *label36, *label37;
  GtkWidget *scrolledwindow8;
  GtkWidget *action_area;
  GtkWidget *btnCancel, *btnOk;
  GtkWidget *btnGetOutputFilename;
	
  GSList *grp_rdoBaseDump = NULL;
  GSList *grp_rdoTypeDump = NULL;

  gui_dump->window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title (GTK_WINDOW (gui_dump->window), _("Dump Query"));
  /*gtk_window_set_type_hint (GTK_WINDOW (gui_dump->window), GDK_WINDOW_TYPE_HINT_DIALOG);*/
	g_signal_connect (G_OBJECT (gui_dump->window), "destroy", G_CALLBACK (gmysqlcc_gui_dump_evt_destroy), gui_dump);

  vbox14 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox14);
	gtk_container_add (GTK_CONTAINER (gui_dump->window), vbox14);
  gtk_container_set_border_width (GTK_CONTAINER (vbox14), 2);

  frame5 = gtk_frame_new (NULL);
  gtk_widget_show (frame5);
  gtk_box_pack_start (GTK_BOX (vbox14), frame5, FALSE, TRUE, 0);

  table2 = gtk_table_new (5, 2, FALSE);
  gtk_widget_show (table2);
  gtk_container_add (GTK_CONTAINER (frame5), table2);
  gtk_table_set_row_spacings (GTK_TABLE (table2), 3);
  gtk_table_set_col_spacings (GTK_TABLE (table2), 3);

  gui_dump->lblDumpServerName = gtk_label_new (_("-"));
  gtk_widget_show (gui_dump->lblDumpServerName);
  gtk_table_attach (GTK_TABLE (table2), gui_dump->lblDumpServerName, 1, 2, 0, 1,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_SHRINK | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (gui_dump->lblDumpServerName), GTK_JUSTIFY_CENTER);
  gtk_misc_set_alignment (GTK_MISC (gui_dump->lblDumpServerName), 0, 0.5);

  gui_dump->lblDumpDatabaseName = gtk_label_new (_("-"));
  gtk_widget_show (gui_dump->lblDumpDatabaseName);
  gtk_table_attach (GTK_TABLE (table2), gui_dump->lblDumpDatabaseName, 1, 2, 1, 2,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_SHRINK | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (gui_dump->lblDumpDatabaseName), GTK_JUSTIFY_CENTER);
  gtk_misc_set_alignment (GTK_MISC (gui_dump->lblDumpDatabaseName), 0, 0.5);

  gui_dump->lblDumpTableName = gtk_label_new (_("-"));
  gtk_widget_show (gui_dump->lblDumpTableName);
  gtk_table_attach (GTK_TABLE (table2), gui_dump->lblDumpTableName, 1, 2, 2, 3,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_SHRINK | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (gui_dump->lblDumpTableName), GTK_JUSTIFY_CENTER);
  gtk_misc_set_alignment (GTK_MISC (gui_dump->lblDumpTableName), 0, 0.5);

  gui_dump->rdoDumpServer = gtk_radio_button_new_with_mnemonic (NULL, _("Server name :"));
  gtk_widget_show (gui_dump->rdoDumpServer);
  gtk_table_attach (GTK_TABLE (table2), gui_dump->rdoDumpServer, 0, 1, 0, 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_radio_button_set_group (GTK_RADIO_BUTTON (gui_dump->rdoDumpServer), grp_rdoBaseDump);
  grp_rdoBaseDump = gtk_radio_button_get_group (GTK_RADIO_BUTTON (gui_dump->rdoDumpServer));
	g_signal_connect (G_OBJECT (gui_dump->rdoDumpServer), "toggled", G_CALLBACK (gmysqlcc_gui_dump_evt_level_select), gui_dump);
	
  gui_dump->rdoDumpDatabase = gtk_radio_button_new_with_mnemonic (NULL, _("Database name :"));
  gtk_widget_show (gui_dump->rdoDumpDatabase);
  gtk_table_attach (GTK_TABLE (table2), gui_dump->rdoDumpDatabase, 0, 1, 1, 2,
                    (GtkAttachOptions) (GTK_SHRINK | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_radio_button_set_group (GTK_RADIO_BUTTON (gui_dump->rdoDumpDatabase), grp_rdoBaseDump);
  grp_rdoBaseDump = gtk_radio_button_get_group (GTK_RADIO_BUTTON (gui_dump->rdoDumpDatabase));
	g_signal_connect (G_OBJECT (gui_dump->rdoDumpDatabase), "toggled", G_CALLBACK (gmysqlcc_gui_dump_evt_level_select), gui_dump);

  gui_dump->rdoDumpTable = gtk_radio_button_new_with_mnemonic (NULL, _("Table name :"));
  gtk_widget_show (gui_dump->rdoDumpTable);
  gtk_table_attach (GTK_TABLE (table2), gui_dump->rdoDumpTable, 0, 1, 2, 3,
                    (GtkAttachOptions) (GTK_SHRINK | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_radio_button_set_group (GTK_RADIO_BUTTON (gui_dump->rdoDumpTable), grp_rdoBaseDump);
  grp_rdoBaseDump = gtk_radio_button_get_group (GTK_RADIO_BUTTON (gui_dump->rdoDumpTable));
	g_signal_connect (G_OBJECT (gui_dump->rdoDumpTable), "toggled", G_CALLBACK (gmysqlcc_gui_dump_evt_level_select), gui_dump);

  gui_dump->rdoDumpRequest = gtk_radio_button_new_with_mnemonic (NULL, _("SQL query :"));
  gtk_widget_show (gui_dump->rdoDumpRequest);
  gtk_table_attach (GTK_TABLE (table2), gui_dump->rdoDumpRequest, 0, 2, 3, 4,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_SHRINK | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_radio_button_set_group (GTK_RADIO_BUTTON (gui_dump->rdoDumpRequest), grp_rdoBaseDump);
  grp_rdoBaseDump = gtk_radio_button_get_group (GTK_RADIO_BUTTON (gui_dump->rdoDumpRequest));
	g_signal_connect (G_OBJECT (gui_dump->rdoDumpRequest), "toggled", G_CALLBACK (gmysqlcc_gui_dump_evt_level_select), gui_dump);

  scrolledwindow8 = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (scrolledwindow8);
  gtk_table_attach (GTK_TABLE (table2), scrolledwindow8, 0, 2, 4, 5,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_SHRINK | GTK_FILL),
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), 0, 0);

  gui_dump->txtSQLRequest = gtk_text_view_new ();
  gtk_widget_show (gui_dump->txtSQLRequest);
  gtk_container_add (GTK_CONTAINER (scrolledwindow8), gui_dump->txtSQLRequest);



  label31 = gtk_label_new (_("Selection :"));
  gtk_widget_show (label31);
  gtk_frame_set_label_widget (GTK_FRAME (frame5), label31);
  gtk_label_set_use_markup (GTK_LABEL (label31), TRUE);

  frame6 = gtk_frame_new (NULL);
  gtk_widget_show (frame6);
  gtk_box_pack_start (GTK_BOX (vbox14), frame6, FALSE, TRUE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (frame6), 3);

  vbox15 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox15);
  gtk_container_add (GTK_CONTAINER (frame6), vbox15);
  gtk_container_set_border_width (GTK_CONTAINER (vbox15), 3);

  gui_dump->chkSeparateFile = gtk_check_button_new_with_mnemonic (_("One file per database"));
  gtk_widget_show (gui_dump->chkSeparateFile);
  gtk_box_pack_start (GTK_BOX (vbox15), gui_dump->chkSeparateFile, FALSE, FALSE, 0);
	g_signal_connect (G_OBJECT (gui_dump->chkSeparateFile), "toggled", G_CALLBACK (gmysqlcc_gui_dump_evt_separate_file_toggle), gui_dump);

  hbox15 = gtk_hbox_new (FALSE, 3);
  gtk_widget_show (hbox15);
  gtk_box_pack_start (GTK_BOX (vbox15), hbox15, FALSE, FALSE, 0);

  gui_dump->chkGroupInDirectory = gtk_check_button_new_with_mnemonic (_("All files in a directory"));
  gtk_widget_show (gui_dump->chkGroupInDirectory);
  gtk_box_pack_start (GTK_BOX (hbox15), gui_dump->chkGroupInDirectory, FALSE, FALSE, 0);

  gui_dump->txtNewDirectory = gtk_entry_new ();
  gtk_widget_show (gui_dump->txtNewDirectory);
  gtk_box_pack_start (GTK_BOX (hbox15), gui_dump->txtNewDirectory, TRUE, TRUE, 0);

  gui_dump->chkDropDb = gtk_check_button_new_with_mnemonic (_("Drop database if exist"));
  gtk_widget_show (gui_dump->chkDropDb);
  gtk_box_pack_start (GTK_BOX (vbox15), gui_dump->chkDropDb, FALSE, FALSE, 0);

  gui_dump->chkAddUse = gtk_check_button_new_with_mnemonic (_("Add USE database"));
  gtk_widget_show (gui_dump->chkAddUse);
  gtk_box_pack_start (GTK_BOX (vbox15), gui_dump->chkAddUse, FALSE, FALSE, 0);

  gui_dump->chkDropTable = gtk_check_button_new_with_mnemonic (_("Drop table if exist"));
  gtk_widget_show (gui_dump->chkDropTable);
  gtk_box_pack_start (GTK_BOX (vbox15), gui_dump->chkDropTable, FALSE, FALSE, 0);

  gui_dump->chkCompleteInsert = gtk_check_button_new_with_mnemonic (_("Complete inserts"));
  gtk_widget_show (gui_dump->chkCompleteInsert);
  gtk_box_pack_start (GTK_BOX (vbox15), gui_dump->chkCompleteInsert, FALSE, FALSE, 0);



  frame7 = gtk_frame_new (NULL);
  gtk_widget_show (frame7);
  gtk_box_pack_start (GTK_BOX (vbox15), frame7, FALSE, TRUE, 0);

  vbox16 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox16);
  gtk_container_add (GTK_CONTAINER (frame7), vbox16);

  gui_dump->rdoDumpStruct = gtk_radio_button_new_with_mnemonic (NULL, _("Structure"));
  gtk_widget_show (gui_dump->rdoDumpStruct);
  gtk_box_pack_start (GTK_BOX (vbox16), gui_dump->rdoDumpStruct, FALSE, FALSE, 0);
  gtk_radio_button_set_group (GTK_RADIO_BUTTON (gui_dump->rdoDumpStruct), grp_rdoTypeDump);
  grp_rdoTypeDump = gtk_radio_button_get_group (GTK_RADIO_BUTTON (gui_dump->rdoDumpStruct));
	g_signal_connect (G_OBJECT (gui_dump->rdoDumpStruct), "toggled", G_CALLBACK (gmysqlcc_gui_dump_evt_type_select), gui_dump);

  gui_dump->rdoDumpStructData = gtk_radio_button_new_with_mnemonic (NULL, _("Structure and data"));
  gtk_widget_show (gui_dump->rdoDumpStructData);
  gtk_box_pack_start (GTK_BOX (vbox16), gui_dump->rdoDumpStructData, FALSE, FALSE, 0);
  gtk_radio_button_set_group (GTK_RADIO_BUTTON (gui_dump->rdoDumpStructData), grp_rdoTypeDump);
  grp_rdoTypeDump = gtk_radio_button_get_group (GTK_RADIO_BUTTON (gui_dump->rdoDumpStructData));
	g_signal_connect (G_OBJECT (gui_dump->rdoDumpStructData), "toggled", G_CALLBACK (gmysqlcc_gui_dump_evt_type_select), gui_dump);

  gui_dump->rdoDumpData = gtk_radio_button_new_with_mnemonic (NULL, _("Data only"));
  gtk_widget_show (gui_dump->rdoDumpData);
  gtk_box_pack_start (GTK_BOX (vbox16), gui_dump->rdoDumpData, FALSE, FALSE, 0);
  gtk_radio_button_set_group (GTK_RADIO_BUTTON (gui_dump->rdoDumpData), grp_rdoTypeDump);
  grp_rdoTypeDump = gtk_radio_button_get_group (GTK_RADIO_BUTTON (gui_dump->rdoDumpData));
	g_signal_connect (G_OBJECT (gui_dump->rdoDumpData), "toggled", G_CALLBACK (gmysqlcc_gui_dump_evt_type_select), gui_dump);
	
	
	
  label33 = gtk_label_new (_("Table :"));
  gtk_widget_show (label33);
  gtk_frame_set_label_widget (GTK_FRAME (frame7), label33);
  gtk_label_set_use_markup (GTK_LABEL (label33), TRUE);

  label35 = gtk_label_new (_("Options :"));
  gtk_widget_show (label35);
  gtk_frame_set_label_widget (GTK_FRAME (frame6), label35);
  gtk_label_set_use_markup (GTK_LABEL (label35), TRUE);

  frame8 = gtk_frame_new (NULL);
  gtk_widget_show (frame8);
  gtk_box_pack_start (GTK_BOX (vbox14), frame8, FALSE, TRUE, 0);

  table3 = gtk_table_new (3, 2, FALSE);
  gtk_widget_show (table3);
  gtk_container_add (GTK_CONTAINER (frame8), table3);

  label37 = gtk_label_new (_("File name :"));
  gtk_widget_show (label37);
  gtk_table_attach (GTK_TABLE (table3), label37, 0, 1, 0, 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label37), 0, 0.5);

  hbox18 = gtk_hbox_new (FALSE, 2);
  gtk_widget_show (hbox18);
  gtk_table_attach (GTK_TABLE (table3), hbox18, 1, 2, 0, 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (GTK_FILL), 0, 0);

  gui_dump->txtOutputFilename = gtk_entry_new ();
  gtk_widget_show (gui_dump->txtOutputFilename);
  gtk_box_pack_start (GTK_BOX (hbox18), gui_dump->txtOutputFilename, TRUE, TRUE, 0);

  btnGetOutputFilename = gtk_button_new_with_mnemonic (_("..."));
  gtk_widget_show (btnGetOutputFilename);
  gtk_box_pack_start (GTK_BOX (hbox18), btnGetOutputFilename, FALSE, FALSE, 0);
	g_signal_connect (G_OBJECT (btnGetOutputFilename), "clicked", G_CALLBACK (gmysqlcc_gui_dump_evt_getOutputFilename_clicked), gui_dump);

  label32 = gtk_label_new (_("Format :"));
  gtk_widget_show (label32);
  gtk_table_attach (GTK_TABLE (table3), label32, 0, 1, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  gui_dump->cmbOutputFormat = gtk_combo_box_entry_new_text ();
  gtk_widget_show (gui_dump->cmbOutputFormat);
  gtk_table_attach (GTK_TABLE (table3), gui_dump->cmbOutputFormat, 1, 2, 1, 2,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (GTK_FILL), 0, 0);
	g_signal_connect (G_OBJECT (gui_dump->cmbOutputFormat), "changed", G_CALLBACK (gmysqlcc_gui_dump_evt_format_change), gui_dump);

	
  label34 = gtk_label_new (_("Charset :"));
  gtk_widget_show (label34);
  gtk_table_attach (GTK_TABLE (table3), label34, 0, 1, 2, 3,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  gui_dump->cmbOutputCharset = gtk_combo_box_entry_new_text ();
  gtk_widget_show (gui_dump->cmbOutputCharset);
  gtk_table_attach (GTK_TABLE (table3), gui_dump->cmbOutputCharset, 1, 2, 2, 3,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (GTK_FILL), 0, 0);

  label36 = gtk_label_new (_("Output :"));
  gtk_widget_show (label36);
  gtk_frame_set_label_widget (GTK_FRAME (frame8), label36);
	
	
	
  action_area = gtk_hbutton_box_new ();
  gtk_widget_show (action_area);
	gtk_box_pack_start (GTK_BOX (vbox14), action_area, FALSE, TRUE, 0);
  gtk_button_box_set_layout (GTK_BUTTON_BOX (action_area), GTK_BUTTONBOX_SPREAD);

  btnCancel = gtk_button_new_from_stock ("gtk-cancel");
  gtk_widget_show (btnCancel);
	gtk_box_pack_start (GTK_BOX (action_area), btnCancel, FALSE, TRUE, 0);
	g_signal_connect (G_OBJECT (btnCancel), "clicked", G_CALLBACK (gmysqlcc_gui_dump_evt_close_clicked), gui_dump);

  btnOk = gtk_button_new_from_stock ("gtk-ok");
  gtk_widget_show (btnOk);
	gtk_box_pack_start (GTK_BOX (action_area), btnOk, FALSE, TRUE, 0);
	g_signal_connect (G_OBJECT (btnOk), "clicked", G_CALLBACK (gmysqlcc_gui_dump_evt_dump_clicked), gui_dump);
	
}

void gmysqlcc_gui_dump_init_widget (p_gmysqlcc_gui_dump gui_dump) {
	GList * lstCharset_items = NULL;
	GList * lstCharset_idx = NULL;
	
	/* Fill format combo box */
	gui_dump->dumpFormat = DumpFormat_Sql;
	gtk_combo_box_append_text(GTK_COMBO_BOX(gui_dump->cmbOutputFormat), _("SQL"));
	gtk_combo_box_append_text(GTK_COMBO_BOX(gui_dump->cmbOutputFormat), _("CSV"));
	gtk_combo_box_append_text(GTK_COMBO_BOX(gui_dump->cmbOutputFormat), _("XML"));
	gtk_combo_box_set_active(GTK_COMBO_BOX(gui_dump->cmbOutputFormat), 0);
	
	/* Fill output charset combo box */
	lstCharset_items = gmysqlcc_helpers_charset_list_new();
	lstCharset_idx = lstCharset_items;
	while (lstCharset_idx != NULL) {
		gtk_combo_box_append_text(GTK_COMBO_BOX(gui_dump->cmbOutputCharset), (gchar *)lstCharset_idx->data);
		lstCharset_idx = g_list_next(lstCharset_idx);
	}
  g_list_free (lstCharset_items);
	gtk_combo_box_set_active(GTK_COMBO_BOX(gui_dump->cmbOutputCharset), 0);
	
	/* Check datas */
	gui_dump->dumpLevel = DumpLevel_Table;
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
	gui_dump->dumpType = DUMP_TYPE_STRUCT_DATA;
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
		gui_dump->dumpLevel = DumpLevel_Server;
	} else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON (gui_dump->rdoDumpDatabase))) {
		gtk_widget_set_sensitive(GTK_WIDGET(gui_dump->chkSeparateFile), FALSE);
		gtk_widget_set_sensitive(GTK_WIDGET(gui_dump->chkGroupInDirectory), FALSE);
		gtk_widget_set_sensitive(GTK_WIDGET(gui_dump->txtNewDirectory), FALSE);
		gtk_widget_set_sensitive(GTK_WIDGET(gui_dump->chkDropDb), TRUE);
		gtk_widget_set_sensitive(GTK_WIDGET(gui_dump->chkAddUse), TRUE);
		gtk_widget_set_sensitive(GTK_WIDGET(gui_dump->chkDropTable), TRUE);
		gtk_widget_set_sensitive(GTK_WIDGET(gui_dump->rdoDumpStruct), TRUE);
		gtk_widget_set_sensitive(GTK_WIDGET(gui_dump->rdoDumpStructData), TRUE);
		gui_dump->dumpLevel = DumpLevel_Database;
	} else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON (gui_dump->rdoDumpTable))) {
		gtk_widget_set_sensitive(GTK_WIDGET(gui_dump->chkSeparateFile), FALSE);
		gtk_widget_set_sensitive(GTK_WIDGET(gui_dump->chkGroupInDirectory), FALSE);
		gtk_widget_set_sensitive(GTK_WIDGET(gui_dump->txtNewDirectory), FALSE);
		gtk_widget_set_sensitive(GTK_WIDGET(gui_dump->chkDropDb), FALSE);
		gtk_widget_set_sensitive(GTK_WIDGET(gui_dump->chkAddUse), FALSE);
		gtk_widget_set_sensitive(GTK_WIDGET(gui_dump->chkDropTable), TRUE);
		gtk_widget_set_sensitive(GTK_WIDGET(gui_dump->rdoDumpStruct), TRUE);
		gtk_widget_set_sensitive(GTK_WIDGET(gui_dump->rdoDumpStructData), TRUE);
		gui_dump->dumpLevel = DumpLevel_Table;
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
		gui_dump->dumpLevel = DumpLevel_Query;
	} 
}

void gmysqlcc_gui_dump_evt_type_select (GtkRadioButton *radiobutton, gpointer user_data) {
	p_gmysqlcc_gui_dump gui_dump = (p_gmysqlcc_gui_dump)user_data;

	if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON (gui_dump->rdoDumpStruct))) {
		gtk_combo_box_set_active(GTK_COMBO_BOX(gui_dump->cmbOutputFormat), 0);
		gtk_widget_set_sensitive(GTK_WIDGET(gui_dump->cmbOutputFormat), FALSE);
		gtk_widget_set_sensitive(GTK_WIDGET(gui_dump->chkCompleteInsert), FALSE);
		gui_dump->dumpType = DUMP_TYPE_STRUCT;
	} else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON (gui_dump->rdoDumpStructData))) {
		gtk_combo_box_set_active(GTK_COMBO_BOX(gui_dump->cmbOutputFormat), 0);
		gtk_widget_set_sensitive(GTK_WIDGET(gui_dump->cmbOutputFormat), FALSE);
		gtk_widget_set_sensitive(GTK_WIDGET(gui_dump->chkCompleteInsert), TRUE);
		gui_dump->dumpType = DUMP_TYPE_STRUCT_DATA;
	} else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON (gui_dump->rdoDumpData))) {
		gtk_widget_set_sensitive(GTK_WIDGET(gui_dump->cmbOutputFormat), TRUE);
		gtk_widget_set_sensitive(GTK_WIDGET(gui_dump->chkCompleteInsert), TRUE);
		gui_dump->dumpType = DUMP_TYPE_DATA;
	}
}

void gmysqlcc_gui_dump_evt_format_change (GtkWidget *widget, gpointer user_data) {
	p_gmysqlcc_gui_dump gui_dump = (p_gmysqlcc_gui_dump)user_data;

	gui_dump->dumpFormat = gtk_combo_box_get_active(GTK_COMBO_BOX(gui_dump->cmbOutputFormat)) + 1;
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
	gchar *filename;
	
	filename = gtk_editable_get_chars(GTK_EDITABLE(gui_dump->txtOutputFilename), 0, -1);
	
	if (filename != NULL) {
		gmysqlcc_gui_dump_to_file (gui_dump, filename);
		g_free (filename);
		gtk_widget_destroy(GTK_WIDGET(gui_dump->window));
	}
}

void gmysqlcc_gui_dump_evt_close_clicked (GtkWidget *widget, gpointer user_data) {
	p_gmysqlcc_gui_dump gui_dump = (p_gmysqlcc_gui_dump)user_data;
	
	gtk_widget_destroy(GTK_WIDGET(gui_dump->window));
}

void gmysqlcc_gui_dump_evt_getOutputFilename_clicked (GtkWidget *widget, gpointer user_data) {
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
		gtk_entry_set_text(GTK_ENTRY(gui_dump->txtOutputFilename), filename);
		g_free (filename);
	}
}

void gmysqlcc_gui_dump_to_file (p_gmysqlcc_gui_dump gui_dump, gchar * sqlFilename) {
	p_mysql_dump mysql_dmp;
	GtkTextBuffer * txtBuffer;
	GtkTextIter begin, end;
	gchar * sqlRequest = (gchar *)NULL;
	gint selOutputCharset;
	e_dumpFormat format;


	format = (gui_dump->dumpFormat != 0) ? gui_dump->dumpFormat : DumpFormat_Sql ;
	
	txtBuffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(gui_dump->txtSQLRequest));
	gtk_text_buffer_get_start_iter (GTK_TEXT_BUFFER(txtBuffer), &begin);
	gtk_text_buffer_get_end_iter (GTK_TEXT_BUFFER(txtBuffer), &end);
	sqlRequest = (gchar *)gtk_text_buffer_get_text (GTK_TEXT_BUFFER(txtBuffer), &begin, &end, FALSE);
	
	selOutputCharset = gtk_combo_box_get_active(GTK_COMBO_BOX(gui_dump->cmbOutputCharset));
	g_print("Selected charset : '%s'\n", gmysqlcc_helpers_charset_list_get_by_index(selOutputCharset));
	
	mysql_dmp = mysql_dump_new(format, gui_dump->dumpLevel);
	
	mysql_dump_set_filename(mysql_dmp, sqlFilename, gmysqlcc_helpers_charset_list_get_by_index(selOutputCharset));
	
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
					(gui_dump->dumpType & DUMP_TYPE_STRUCT), /* structure */
					(gui_dump->dumpType & DUMP_TYPE_DATA), /* data */
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

