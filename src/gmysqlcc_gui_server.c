
#include "gmysqlcc_gui_all.h"
#include "gmysqlcc_gui_server_evt.h"
	
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
	gui_server->curr_mysql_usr = NULL;
	gui_server->curr_database_rights = NULL;
	
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
	GtkWidget *statusbar;
	GtkWidget *frame;
  GtkWidget *scrolledwindow;
  GtkWidget *label;
	GtkWidget *vbox1, *vbox2, *vbox3, *vbox4, *vbox20, *vbox22;
	GtkWidget *hpaned1, *hpaned3, *hpaned4, *hpaned5, *hpaned6;
	GtkWidget *hbox2, *hbox3, *hbox21, *hbox22, *hbox23, *hbox24, *hbox25, *hbox26, *hbox27;
  GtkWidget *hbuttonbox4, *hbuttonbox8, *hbuttonbox9, *hbuttonbox10;
	GtkWidget *table1;
	GtkWidget *notebook1, *notebook3;
	GtkWidget *frame5, *frame6, *frame7, *frame8, *frame9;
	GtkWidget *toolbar1;
	GtkWidget * imgToolbar;
	GtkToolItem * btnTlbrClose, * btnTlbrSql, * btnTlbrSqlFile;
	GtkWidget *btnDbAdd, *btnDbDel;
	GtkWidget *btnTblAdd, *btnTblEdit, *btnTblDump, *btnTblDel;
  GtkWidget *btnUserNew, *btnUserAdd, *btnUserUpdate, *btnUserDelete;
  GtkWidget *btnDRNew, *btnDRAdd, *btnDRDelete;
  GtkWidget *btnTRNew, *btnTRAdd, *btnTRDelete;
  GtkWidget *btnCRNew, *btnCRAdd, *btnCRDelete;
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
	gtk_window_set_default_size (GTK_WINDOW (gui_server->window), 560, 400);
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

	scrolledwindow = gtk_scrolled_window_new (NULL, NULL);
	gtk_widget_show (scrolledwindow);
	gtk_box_pack_start (GTK_BOX (vbox2), scrolledwindow, TRUE, TRUE, 0);
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow), GTK_POLICY_NEVER, GTK_POLICY_ALWAYS);

	gui_server->lstBase = gtk_tree_view_new ();
	gtk_widget_show (gui_server->lstBase);
	gtk_tree_view_set_rules_hint(GTK_TREE_VIEW (gui_server->lstBase), TRUE);
	gtk_container_add (GTK_CONTAINER (scrolledwindow), gui_server->lstBase);
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

	scrolledwindow = gtk_scrolled_window_new (NULL, NULL);
	gtk_widget_show (scrolledwindow);
	gtk_box_pack_start (GTK_BOX (vbox3), scrolledwindow, TRUE, TRUE, 0);
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow), GTK_POLICY_NEVER, GTK_POLICY_ALWAYS);

	gui_server->lstTable = gtk_tree_view_new ();
	gtk_widget_show (gui_server->lstTable);
	gtk_container_add (GTK_CONTAINER (scrolledwindow), gui_server->lstTable);
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

	label = gtk_label_new (_("Databases"));
	gtk_widget_show (label);
	gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook1), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook1), 0), label);





  hpaned3 = gtk_hpaned_new ();
  gtk_widget_show (hpaned3);
  gtk_container_add (GTK_CONTAINER (notebook1), hpaned3);
  gtk_paned_set_position (GTK_PANED (hpaned3), 180);

  scrolledwindow = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (scrolledwindow);
  gtk_paned_pack1 (GTK_PANED (hpaned3), scrolledwindow, FALSE, TRUE);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow), GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS);

  gui_server->lstUser = gtk_tree_view_new ();
  gtk_widget_show (gui_server->lstUser);
  gtk_container_add (GTK_CONTAINER (scrolledwindow), gui_server->lstUser);
	gtk_tree_view_set_rules_hint(GTK_TREE_VIEW (gui_server->lstUser), TRUE);
	select = gtk_tree_view_get_selection (GTK_TREE_VIEW (gui_server->lstUser));
	gtk_tree_selection_set_mode (select, GTK_SELECTION_SINGLE);
	g_signal_connect (G_OBJECT (select), "changed", 
										G_CALLBACK (gmysqlcc_gui_server_evt_lstUser_selected), (gpointer)gui_server);

  vbox4 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox4);
  gtk_paned_pack2 (GTK_PANED (hpaned3), vbox4, TRUE, TRUE);

  frame = gtk_frame_new (NULL);
  gtk_widget_show (frame);
  gtk_box_pack_start (GTK_BOX (vbox4), frame, FALSE, TRUE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (frame), 2);
  gtk_frame_set_shadow_type (GTK_FRAME (frame), GTK_SHADOW_IN);

  label = gtk_label_new (_("User informations :"));
  gtk_widget_show (label);
  gtk_frame_set_label_widget (GTK_FRAME (frame), label);

  table1 = gtk_table_new (4, 2, FALSE);
  gtk_widget_show (table1);
  gtk_container_add (GTK_CONTAINER (frame), table1);
  gtk_container_set_border_width (GTK_CONTAINER (table1), 2);
  gtk_table_set_row_spacings (GTK_TABLE (table1), 2);
  gtk_table_set_col_spacings (GTK_TABLE (table1), 2);

  label = gtk_label_new (_("Login :"));
  gtk_widget_show (label);
  gtk_table_attach (GTK_TABLE (table1), label, 0, 1, 0, 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  label = gtk_label_new (_("Host :"));
  gtk_widget_show (label);
  gtk_table_attach (GTK_TABLE (table1), label, 0, 1, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  label = gtk_label_new (_("Password :"));
  gtk_widget_show (label);
  gtk_table_attach (GTK_TABLE (table1), label, 0, 1, 2, 3,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  gui_server->txtUserPasswd = gtk_entry_new ();
  gtk_widget_show (gui_server->txtUserPasswd);
  gtk_table_attach (GTK_TABLE (table1), gui_server->txtUserPasswd, 1, 2, 2, 3,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  gui_server->txtUserHost = gtk_entry_new ();
  gtk_widget_show (gui_server->txtUserHost);
  gtk_table_attach (GTK_TABLE (table1), gui_server->txtUserHost, 1, 2, 1, 2,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  gui_server->txtUserLogin = gtk_entry_new ();
  gtk_widget_show (gui_server->txtUserLogin);
  gtk_table_attach (GTK_TABLE (table1), gui_server->txtUserLogin, 1, 2, 0, 1,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  gui_server->chkEncryptedPasswd = gtk_check_button_new_with_mnemonic (_("Already encrypted password"));
  gtk_widget_show (gui_server->chkEncryptedPasswd);
  gtk_table_attach (GTK_TABLE (table1), gui_server->chkEncryptedPasswd, 1, 2, 3, 4,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  hbuttonbox4 = gtk_hbutton_box_new ();
  gtk_widget_show (hbuttonbox4);
  gtk_box_pack_start (GTK_BOX (vbox4), hbuttonbox4, FALSE, TRUE, 0);
  gtk_button_box_set_layout (GTK_BUTTON_BOX (hbuttonbox4), GTK_BUTTONBOX_SPREAD);
  gtk_box_set_spacing (GTK_BOX (hbuttonbox4), 2);

  btnUserNew = gtk_button_new_from_stock ("gtk-new");
  gtk_widget_show (btnUserNew);
  gtk_container_add (GTK_CONTAINER (hbuttonbox4), btnUserNew);
	g_signal_connect (G_OBJECT (btnUserNew), "clicked", 
										G_CALLBACK (gmysqlcc_gui_server_evt_btnUserNew_clicked), (gpointer)gui_server);

  btnUserAdd = gtk_button_new_from_stock ("gtk-add");
  gtk_widget_show (btnUserAdd);
  gtk_container_add (GTK_CONTAINER (hbuttonbox4), btnUserAdd);
	g_signal_connect (G_OBJECT (btnUserAdd), "clicked", 
										G_CALLBACK (gmysqlcc_gui_server_evt_btnUserAdd_clicked), (gpointer)gui_server);

  btnUserUpdate = gtk_button_new_from_stock ("gtk-apply");
  gtk_widget_show (btnUserUpdate);
  gtk_container_add (GTK_CONTAINER (hbuttonbox4), btnUserUpdate);
	g_signal_connect (G_OBJECT (btnUserUpdate), "clicked", 
										G_CALLBACK (gmysqlcc_gui_server_evt_btnUserUpdate_clicked), (gpointer)gui_server);

  btnUserDelete = gtk_button_new_from_stock ("gtk-delete");
  gtk_widget_show (btnUserDelete);
  gtk_container_add (GTK_CONTAINER (hbuttonbox4), btnUserDelete);
	g_signal_connect (G_OBJECT (btnUserDelete), "clicked", 
										G_CALLBACK (gmysqlcc_gui_server_evt_btnUserDelete_clicked), (gpointer)gui_server);

	frame5 = gtk_frame_new (NULL);
  gtk_widget_show (frame5);
  gtk_box_pack_start (GTK_BOX (vbox4), frame5, TRUE, TRUE, 0);
  gtk_frame_set_shadow_type (GTK_FRAME (frame5), GTK_SHADOW_IN);

  notebook3 = gtk_notebook_new ();
  gtk_widget_show (notebook3);
  gtk_container_add (GTK_CONTAINER (frame5), notebook3);

  scrolledwindow = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (scrolledwindow);
  gtk_container_add (GTK_CONTAINER (notebook3), scrolledwindow);
  gtk_container_set_border_width (GTK_CONTAINER (scrolledwindow), 3);
	
  gui_server->lstURUserRights = gtk_tree_view_new ();
  gtk_widget_show (gui_server->lstURUserRights);
	gtk_tree_view_set_rules_hint(GTK_TREE_VIEW (gui_server->lstURUserRights), TRUE);
  gtk_container_add (GTK_CONTAINER (scrolledwindow), gui_server->lstURUserRights);

  label = gtk_label_new (_("User"));
  gtk_widget_show (label);
  gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook3), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook3), 0), label);

  hpaned4 = gtk_hpaned_new ();
  gtk_widget_show (hpaned4);
  gtk_container_add (GTK_CONTAINER (notebook3), hpaned4);
  gtk_paned_set_position (GTK_PANED (hpaned4), 200);

  scrolledwindow = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (scrolledwindow);
  gtk_paned_pack1 (GTK_PANED (hpaned4), scrolledwindow, FALSE, TRUE);

  gui_server->lstDRDatabases = gtk_tree_view_new ();
  gtk_widget_show (gui_server->lstDRDatabases);
  gtk_container_add (GTK_CONTAINER (scrolledwindow), gui_server->lstDRDatabases);
	select = gtk_tree_view_get_selection (GTK_TREE_VIEW (gui_server->lstDRDatabases));
	gtk_tree_selection_set_mode (select, GTK_SELECTION_SINGLE);
	g_signal_connect (G_OBJECT (select), "changed", 
										G_CALLBACK (gmysqlcc_gui_server_evt_lstDRDatabases_selected), (gpointer)gui_server);

  vbox20 = gtk_vbox_new (FALSE, 3);
  gtk_widget_show (vbox20);
  gtk_paned_pack2 (GTK_PANED (hpaned4), vbox20, TRUE, TRUE);

  hbox21 = gtk_hbox_new (FALSE, 2);
  gtk_widget_show (hbox21);
  gtk_box_pack_start (GTK_BOX (vbox20), hbox21, FALSE, TRUE, 3);

  label = gtk_label_new (_("Database name :"));
  gtk_widget_show (label);
  gtk_box_pack_start (GTK_BOX (hbox21), label, FALSE, FALSE, 0);

  gui_server->txtDRDbName = gtk_entry_new ();
  gtk_widget_show (gui_server->txtDRDbName);
  gtk_box_pack_start (GTK_BOX (hbox21), gui_server->txtDRDbName, TRUE, TRUE, 0);

  hbuttonbox8 = gtk_hbutton_box_new ();
  gtk_widget_show (hbuttonbox8);
  gtk_box_pack_start (GTK_BOX (vbox20), hbuttonbox8, FALSE, TRUE, 3);
  gtk_button_box_set_layout (GTK_BUTTON_BOX (hbuttonbox8), GTK_BUTTONBOX_SPREAD);

  btnDRNew = gtk_button_new_from_stock ("gtk-new");
  gtk_widget_show (btnDRNew);
  gtk_container_add (GTK_CONTAINER (hbuttonbox8), btnDRNew);
  GTK_WIDGET_SET_FLAGS (btnDRNew, GTK_CAN_DEFAULT);

  btnDRAdd = gtk_button_new_from_stock ("gtk-add");
  gtk_widget_show (btnDRAdd);
  gtk_container_add (GTK_CONTAINER (hbuttonbox8), btnDRAdd);
  GTK_WIDGET_SET_FLAGS (btnDRAdd, GTK_CAN_DEFAULT);

  btnDRDelete = gtk_button_new_from_stock ("gtk-delete");
  gtk_widget_show (btnDRDelete);
  gtk_container_add (GTK_CONTAINER (hbuttonbox8), btnDRDelete);
  GTK_WIDGET_SET_FLAGS (btnDRDelete, GTK_CAN_DEFAULT);

  frame6 = gtk_frame_new (NULL);
  gtk_widget_show (frame6);
  gtk_box_pack_start (GTK_BOX (vbox20), frame6, TRUE, TRUE, 0);
  gtk_frame_set_shadow_type (GTK_FRAME (frame6), GTK_SHADOW_IN);

  scrolledwindow = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (scrolledwindow);
  gtk_container_add (GTK_CONTAINER (frame6), scrolledwindow);

  gui_server->lstDRDatabaseRights = gtk_tree_view_new ();
  gtk_widget_show (gui_server->lstDRDatabaseRights);
  gtk_container_add (GTK_CONTAINER (scrolledwindow), gui_server->lstDRDatabaseRights);

  label = gtk_label_new (_("Database rights :"));
  gtk_widget_show (label);
  gtk_frame_set_label_widget (GTK_FRAME (frame6), label);

  label = gtk_label_new (_("Database"));
  gtk_widget_show (label);
  gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook3), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook3), 1), label);

  hpaned5 = gtk_hpaned_new ();
  gtk_widget_show (hpaned5);
  gtk_container_add (GTK_CONTAINER (notebook3), hpaned5);
  gtk_paned_set_position (GTK_PANED (hpaned5), 200);

  scrolledwindow = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (scrolledwindow);
  gtk_paned_pack1 (GTK_PANED (hpaned5), scrolledwindow, FALSE, TRUE);
  gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolledwindow), GTK_SHADOW_IN);

  gui_server->lstTRTables = gtk_tree_view_new ();
  gtk_widget_show (gui_server->lstTRTables);
  gtk_container_add (GTK_CONTAINER (scrolledwindow), gui_server->lstTRTables);

  vbox20 = gtk_vbox_new (FALSE, 3);
  gtk_widget_show (vbox20);
  gtk_paned_pack2 (GTK_PANED (hpaned5), vbox20, TRUE, TRUE);

  hbox24 = gtk_hbox_new (FALSE, 3);
  gtk_widget_show (hbox24);
  gtk_box_pack_start (GTK_BOX (vbox20), hbox24, FALSE, TRUE, 0);

  label = gtk_label_new (_("Database name :"));
  gtk_widget_show (label);
  gtk_box_pack_start (GTK_BOX (hbox24), label, FALSE, FALSE, 0);

  gui_server->txtTRDbName = gtk_entry_new ();
  gtk_widget_show (gui_server->txtTRDbName);
  gtk_box_pack_start (GTK_BOX (hbox24), gui_server->txtTRDbName, TRUE, TRUE, 0);

  hbox22 = gtk_hbox_new (FALSE, 3);
  gtk_widget_show (hbox22);
  gtk_box_pack_start (GTK_BOX (vbox20), hbox22, FALSE, TRUE, 0);

  label = gtk_label_new (_("Table name :"));
  gtk_widget_show (label);
  gtk_box_pack_start (GTK_BOX (hbox22), label, FALSE, FALSE, 0);

  gui_server->txtTRTblName = gtk_entry_new ();
  gtk_widget_show (gui_server->txtTRTblName);
  gtk_box_pack_start (GTK_BOX (hbox22), gui_server->txtTRTblName, TRUE, TRUE, 0);

  hbuttonbox9 = gtk_hbutton_box_new ();
  gtk_widget_show (hbuttonbox9);
  gtk_box_pack_start (GTK_BOX (vbox20), hbuttonbox9, FALSE, TRUE, 0);
  gtk_button_box_set_layout (GTK_BUTTON_BOX (hbuttonbox9), GTK_BUTTONBOX_SPREAD);

  btnTRNew = gtk_button_new_from_stock ("gtk-new");
  gtk_widget_show (btnTRNew);
  gtk_container_add (GTK_CONTAINER (hbuttonbox9), btnTRNew);
  GTK_WIDGET_SET_FLAGS (btnTRNew, GTK_CAN_DEFAULT);

  btnTRAdd = gtk_button_new_from_stock ("gtk-add");
  gtk_widget_show (btnTRAdd);
  gtk_container_add (GTK_CONTAINER (hbuttonbox9), btnTRAdd);
  GTK_WIDGET_SET_FLAGS (btnTRAdd, GTK_CAN_DEFAULT);

  btnTRDelete = gtk_button_new_from_stock ("gtk-delete");
  gtk_widget_show (btnTRDelete);
  gtk_container_add (GTK_CONTAINER (hbuttonbox9), btnTRDelete);
  GTK_WIDGET_SET_FLAGS (btnTRDelete, GTK_CAN_DEFAULT);

  hbox23 = gtk_hbox_new (FALSE, 3);
  gtk_widget_show (hbox23);
  gtk_box_pack_start (GTK_BOX (vbox20), hbox23, TRUE, TRUE, 0);

  frame8 = gtk_frame_new (NULL);
  gtk_widget_show (frame8);
  gtk_box_pack_start (GTK_BOX (hbox23), frame8, TRUE, TRUE, 0);
  gtk_frame_set_shadow_type (GTK_FRAME (frame8), GTK_SHADOW_IN);

  scrolledwindow = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (scrolledwindow);
  gtk_container_add (GTK_CONTAINER (frame8), scrolledwindow);
  gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolledwindow), GTK_SHADOW_IN);

  gui_server->lstTRTableRights = gtk_tree_view_new ();
  gtk_widget_show (gui_server->lstTRTableRights);
  gtk_container_add (GTK_CONTAINER (scrolledwindow), gui_server->lstTRTableRights);

  label = gtk_label_new (_("Table Rights :"));
  gtk_widget_show (label);
  gtk_frame_set_label_widget (GTK_FRAME (frame8), label);

  frame9 = gtk_frame_new (NULL);
  gtk_widget_show (frame9);
  gtk_box_pack_start (GTK_BOX (hbox23), frame9, TRUE, TRUE, 0);
  gtk_frame_set_shadow_type (GTK_FRAME (frame9), GTK_SHADOW_IN);

  scrolledwindow = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (scrolledwindow);
  gtk_container_add (GTK_CONTAINER (frame9), scrolledwindow);
  gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolledwindow), GTK_SHADOW_IN);

  gui_server->lstTRColumnRights = gtk_tree_view_new ();
  gtk_widget_show (gui_server->lstTRColumnRights);
  gtk_container_add (GTK_CONTAINER (scrolledwindow), gui_server->lstTRColumnRights);

  label = gtk_label_new (_("Columns Rights :"));
  gtk_widget_show (label);
  gtk_frame_set_label_widget (GTK_FRAME (frame9), label);

  label = gtk_label_new (_("Table"));
  gtk_widget_show (label);
  gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook3), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook3), 2), label);

  hpaned6 = gtk_hpaned_new ();
  gtk_widget_show (hpaned6);
  gtk_container_add (GTK_CONTAINER (notebook3), hpaned6);
  gtk_paned_set_position (GTK_PANED (hpaned6), 200);

  scrolledwindow = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (scrolledwindow);
  gtk_paned_pack1 (GTK_PANED (hpaned6), scrolledwindow, FALSE, TRUE);
  gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolledwindow), GTK_SHADOW_IN);

  gui_server->lstCRColumns = gtk_tree_view_new ();
  gtk_widget_show (gui_server->lstCRColumns);
  gtk_container_add (GTK_CONTAINER (scrolledwindow), gui_server->lstCRColumns);

  vbox22 = gtk_vbox_new (FALSE, 3);
  gtk_widget_show (vbox22);
  gtk_paned_pack2 (GTK_PANED (hpaned6), vbox22, TRUE, TRUE);

  hbox25 = gtk_hbox_new (FALSE, 3);
  gtk_widget_show (hbox25);
  gtk_box_pack_start (GTK_BOX (vbox22), hbox25, FALSE, TRUE, 0);

  label = gtk_label_new (_("Database name :"));
  gtk_widget_show (label);
  gtk_box_pack_start (GTK_BOX (hbox25), label, FALSE, FALSE, 0);

  gui_server->txtCRDbName = gtk_entry_new ();
  gtk_widget_show (gui_server->txtCRDbName);
  gtk_box_pack_start (GTK_BOX (hbox25), gui_server->txtCRDbName, TRUE, TRUE, 0);

  hbox26 = gtk_hbox_new (FALSE, 3);
  gtk_widget_show (hbox26);
  gtk_box_pack_start (GTK_BOX (vbox22), hbox26, FALSE, TRUE, 0);

  label = gtk_label_new (_("Table name :"));
  gtk_widget_show (label);
  gtk_box_pack_start (GTK_BOX (hbox26), label, FALSE, FALSE, 0);

  gui_server->txtCRTblName = gtk_entry_new ();
  gtk_widget_show (gui_server->txtCRTblName);
  gtk_box_pack_start (GTK_BOX (hbox26), gui_server->txtCRTblName, TRUE, TRUE, 0);

  hbox27 = gtk_hbox_new (FALSE, 3);
  gtk_widget_show (hbox27);
  gtk_box_pack_start (GTK_BOX (vbox22), hbox27, FALSE, TRUE, 0);

  label = gtk_label_new (_("Column name :"));
  gtk_widget_show (label);
  gtk_box_pack_start (GTK_BOX (hbox27), label, FALSE, FALSE, 0);

  gui_server->txtCRColName = gtk_entry_new ();
  gtk_widget_show (gui_server->txtCRColName);
  gtk_box_pack_start (GTK_BOX (hbox27), gui_server->txtCRColName, TRUE, TRUE, 0);

  hbuttonbox10 = gtk_hbutton_box_new ();
  gtk_widget_show (hbuttonbox10);
  gtk_box_pack_start (GTK_BOX (vbox22), hbuttonbox10, FALSE, TRUE, 0);
  gtk_button_box_set_layout (GTK_BUTTON_BOX (hbuttonbox10), GTK_BUTTONBOX_SPREAD);

  btnCRNew = gtk_button_new_from_stock ("gtk-new");
  gtk_widget_show (btnCRNew);
  gtk_container_add (GTK_CONTAINER (hbuttonbox10), btnCRNew);
  GTK_WIDGET_SET_FLAGS (btnCRNew, GTK_CAN_DEFAULT);

  btnCRAdd = gtk_button_new_from_stock ("gtk-add");
  gtk_widget_show (btnCRAdd);
  gtk_container_add (GTK_CONTAINER (hbuttonbox10), btnCRAdd);
  GTK_WIDGET_SET_FLAGS (btnCRAdd, GTK_CAN_DEFAULT);

  btnCRDelete = gtk_button_new_from_stock ("gtk-delete");
  gtk_widget_show (btnCRDelete);
  gtk_container_add (GTK_CONTAINER (hbuttonbox10), btnCRDelete);
  GTK_WIDGET_SET_FLAGS (btnCRDelete, GTK_CAN_DEFAULT);

  frame7 = gtk_frame_new (NULL);
  gtk_widget_show (frame7);
  gtk_box_pack_start (GTK_BOX (vbox22), frame7, TRUE, TRUE, 0);
  gtk_frame_set_shadow_type (GTK_FRAME (frame7), GTK_SHADOW_IN);

  scrolledwindow = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (scrolledwindow);
  gtk_container_add (GTK_CONTAINER (frame7), scrolledwindow);
  gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolledwindow), GTK_SHADOW_IN);

  gui_server->lstCRColumnRights = gtk_tree_view_new ();
  gtk_widget_show (gui_server->lstCRColumnRights);
  gtk_container_add (GTK_CONTAINER (scrolledwindow), gui_server->lstCRColumnRights);

  label = gtk_label_new (_("Column rights :"));
  gtk_widget_show (label);
  gtk_frame_set_label_widget (GTK_FRAME (frame7), label);

  label = gtk_label_new (_("Column"));
  gtk_widget_show (label);
  gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook3), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook3), 3), label);

  label = gtk_label_new (_("Rights :"));
  gtk_widget_show (label);
  gtk_frame_set_label_widget (GTK_FRAME (frame5), label);

  label = gtk_label_new (_("Users"));
  gtk_widget_show (label);
  gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook1), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook1), 1), label);



	label = gtk_label_new (_("In construction"));
	gtk_widget_show (label);
	gtk_container_add (GTK_CONTAINER (notebook1), label);
	gtk_label_set_justify (GTK_LABEL (label), GTK_JUSTIFY_LEFT);

	label = gtk_label_new (_("Server"));
	gtk_widget_show (label);
	gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook1), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook1), 2), label);
	gtk_label_set_justify (GTK_LABEL (label), GTK_JUSTIFY_LEFT);



	statusbar = gtk_statusbar_new ();
	gtk_widget_show (statusbar);
	gtk_box_pack_start (GTK_BOX (vbox1), statusbar, FALSE, FALSE, 0);



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
	GtkCellRenderer * renderer, * renderer_edit;
	GtkListStore * lstEmpty;
	GValue gvalbool = {0, };
	
	g_value_init(&gvalbool, G_TYPE_BOOLEAN);
	g_value_set_boolean(&gvalbool, TRUE);
	
	/* Generate columns for all tree views */
	renderer = gtk_cell_renderer_text_new ();
	
	/* Databases columns */
	currCol = gtk_tree_view_column_new_with_attributes (_("Databases"), renderer, "text", 0, NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW (gui_server->lstBase), currCol);
	
	lstEmpty = gtk_list_store_new(1, G_TYPE_STRING);
	gtk_tree_view_set_model(GTK_TREE_VIEW(gui_server->lstBase), GTK_TREE_MODEL(lstEmpty));
	g_object_unref (G_OBJECT (lstEmpty));
	
	/* Table columns */
	currCol = gtk_tree_view_column_new_with_attributes (_("Table name"), renderer, "text", 0, NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW (gui_server->lstTable), currCol);
	currCol = gtk_tree_view_column_new_with_attributes (_("Rows"), renderer, "text", 1, NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW (gui_server->lstTable), currCol);
	currCol = gtk_tree_view_column_new_with_attributes (_("Size"), renderer, "text", 2, NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW (gui_server->lstTable), currCol);
	currCol = gtk_tree_view_column_new_with_attributes (_("Type"), renderer, "text", 3, NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW (gui_server->lstTable), currCol);
	
	lstEmpty = gtk_list_store_new(4, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);
	gtk_tree_view_set_model(GTK_TREE_VIEW(gui_server->lstTable), GTK_TREE_MODEL(lstEmpty));
	g_object_unref (G_OBJECT (lstEmpty));
	
	/* Users columns */
	currCol = gtk_tree_view_column_new_with_attributes (_("Users"), renderer, "text", 0, NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW (gui_server->lstUser), currCol);
	
	lstEmpty = gtk_list_store_new(1, G_TYPE_STRING);
	gtk_tree_view_set_model(GTK_TREE_VIEW(gui_server->lstUser), GTK_TREE_MODEL(lstEmpty));
	g_object_unref (G_OBJECT (lstEmpty));
	
	/* User Rights columns */
	currCol = gtk_tree_view_column_new_with_attributes (_("Name"), renderer, "text", 0, NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW (gui_server->lstURUserRights), currCol);
	renderer_edit = gtk_cell_renderer_text_new ();
	g_object_set_property(G_OBJECT(renderer_edit), "editable", &gvalbool);
	g_signal_connect (G_OBJECT (renderer_edit), "edited", G_CALLBACK (gmysqlcc_gui_server_evt_lstURUserRights_edited), gui_server);
	currCol = gtk_tree_view_column_new_with_attributes (_("Value"), renderer_edit, "text", 1, NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW (gui_server->lstURUserRights), currCol);
	
	lstEmpty = gtk_list_store_new(2, G_TYPE_STRING, G_TYPE_STRING);
	gtk_tree_view_set_model(GTK_TREE_VIEW(gui_server->lstURUserRights), GTK_TREE_MODEL(lstEmpty));
	g_object_unref (G_OBJECT (lstEmpty));
	
	/* Database List Rights columns */
	currCol = gtk_tree_view_column_new_with_attributes (_("Database"), renderer, "text", 0, NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW (gui_server->lstDRDatabases), currCol);
	
	lstEmpty = gtk_list_store_new(1, G_TYPE_STRING);
	gtk_tree_view_set_model(GTK_TREE_VIEW(gui_server->lstDRDatabases), GTK_TREE_MODEL(lstEmpty));
	g_object_unref (G_OBJECT (lstEmpty));
	
	/* Database Rights columns */
	currCol = gtk_tree_view_column_new_with_attributes (_("Name"), renderer, "text", 0, NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW (gui_server->lstDRDatabaseRights), currCol);
	renderer_edit = gtk_cell_renderer_text_new ();
	g_object_set_property(G_OBJECT(renderer_edit), "editable", &gvalbool);
	/*g_signal_connect (G_OBJECT (renderer_edit), "edited", G_CALLBACK (gmysqlcc_gui_server_evt_lstURUserRights_edited), gui_server);*/
	currCol = gtk_tree_view_column_new_with_attributes (_("Value"), renderer_edit, "text", 1, NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW (gui_server->lstDRDatabaseRights), currCol);
	
	lstEmpty = gtk_list_store_new(2, G_TYPE_STRING, G_TYPE_STRING);
	gtk_tree_view_set_model(GTK_TREE_VIEW(gui_server->lstDRDatabaseRights), GTK_TREE_MODEL(lstEmpty));
	g_object_unref (G_OBJECT (lstEmpty));
	
	
	/* Table List Rights columns */
	currCol = gtk_tree_view_column_new_with_attributes (_("Database"), renderer, "text", 0, NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW (gui_server->lstTRTables), currCol);
	currCol = gtk_tree_view_column_new_with_attributes (_("Table"), renderer, "text", 0, NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW (gui_server->lstTRTables), currCol);
	
	lstEmpty = gtk_list_store_new(2, G_TYPE_STRING, G_TYPE_STRING);
	gtk_tree_view_set_model(GTK_TREE_VIEW(gui_server->lstTRTables), GTK_TREE_MODEL(lstEmpty));
	g_object_unref (G_OBJECT (lstEmpty));
	
	/* Table Rights columns */
	currCol = gtk_tree_view_column_new_with_attributes (_("Name"), renderer, "text", 0, NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW (gui_server->lstTRTableRights), currCol);
	renderer_edit = gtk_cell_renderer_text_new ();
	g_object_set_property(G_OBJECT(renderer_edit), "editable", &gvalbool);
	/*g_signal_connect (G_OBJECT (renderer_edit), "edited", G_CALLBACK (gmysqlcc_gui_server_evt_lstURUserRights_edited), gui_server);*/
	currCol = gtk_tree_view_column_new_with_attributes (_("Value"), renderer_edit, "text", 1, NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW (gui_server->lstTRTableRights), currCol);
	
	lstEmpty = gtk_list_store_new(2, G_TYPE_STRING, G_TYPE_STRING);
	gtk_tree_view_set_model(GTK_TREE_VIEW(gui_server->lstTRTableRights), GTK_TREE_MODEL(lstEmpty));
	g_object_unref (G_OBJECT (lstEmpty));
	
	/* Table Column Rights columns */
	currCol = gtk_tree_view_column_new_with_attributes (_("Name"), renderer, "text", 0, NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW (gui_server->lstTRColumnRights), currCol);
	renderer_edit = gtk_cell_renderer_text_new ();
	g_object_set_property(G_OBJECT(renderer_edit), "editable", &gvalbool);
	/*g_signal_connect (G_OBJECT (renderer_edit), "edited", G_CALLBACK (gmysqlcc_gui_server_evt_lstURUserRights_edited), gui_server);*/
	currCol = gtk_tree_view_column_new_with_attributes (_("Value"), renderer_edit, "text", 1, NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW (gui_server->lstTRColumnRights), currCol);
	
	lstEmpty = gtk_list_store_new(2, G_TYPE_STRING, G_TYPE_STRING);
	gtk_tree_view_set_model(GTK_TREE_VIEW(gui_server->lstTRColumnRights), GTK_TREE_MODEL(lstEmpty));
	g_object_unref (G_OBJECT (lstEmpty));
	
	/* Columns List Rights columns */
	currCol = gtk_tree_view_column_new_with_attributes (_("Database"), renderer, "text", 0, NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW (gui_server->lstCRColumns), currCol);
	currCol = gtk_tree_view_column_new_with_attributes (_("Table"), renderer, "text", 0, NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW (gui_server->lstCRColumns), currCol);
	currCol = gtk_tree_view_column_new_with_attributes (_("Column"), renderer, "text", 0, NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW (gui_server->lstCRColumns), currCol);
	
	lstEmpty = gtk_list_store_new(3, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);
	gtk_tree_view_set_model(GTK_TREE_VIEW(gui_server->lstCRColumns), GTK_TREE_MODEL(lstEmpty));
	g_object_unref (G_OBJECT (lstEmpty));
	
	/* Column Rights columns */
	currCol = gtk_tree_view_column_new_with_attributes (_("Name"), renderer, "text", 0, NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW (gui_server->lstCRColumnRights), currCol);
	renderer_edit = gtk_cell_renderer_text_new ();
	g_object_set_property(G_OBJECT(renderer_edit), "editable", &gvalbool);
	/*g_signal_connect (G_OBJECT (renderer_edit), "edited", G_CALLBACK (gmysqlcc_gui_server_evt_lstURUserRights_edited), gui_server);*/
	currCol = gtk_tree_view_column_new_with_attributes (_("Value"), renderer_edit, "text", 1, NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW (gui_server->lstCRColumnRights), currCol);
	
	lstEmpty = gtk_list_store_new(2, G_TYPE_STRING, G_TYPE_STRING);
	gtk_tree_view_set_model(GTK_TREE_VIEW(gui_server->lstCRColumnRights), GTK_TREE_MODEL(lstEmpty));
	g_object_unref (G_OBJECT (lstEmpty));
	
	
	gmysqlcc_gui_server_fill_user_list(gui_server, NULL);
	gmysqlcc_gui_server_fill_database_list(gui_server);

}

void gmysqlcc_gui_server_fill_database_list (p_gmysqlcc_gui_server gui_server) {
	GtkListStore * lstStrBase;

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
	
	gui_server->curr_mysql_db = NULL;
}

void gmysqlcc_gui_server_fill_table_list (p_gmysqlcc_gui_server gui_server) {
	GtkListStore * lstStrTable;

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
	
	gui_server->curr_mysql_tbl = NULL;
}

void gmysqlcc_gui_server_fill_user_list (p_gmysqlcc_gui_server gui_server, p_mysql_user mysql_usr) {
	p_data_list mysql_usr_lst;
	GtkListStore * lstStrUsers;
	
	void sub_ht_fill_user_list(gpointer key, gpointer value, gpointer user_data) {
		p_mysql_user mysql_usr = (p_mysql_user)value;
		GtkListStore * lstStrUsers = (GtkListStore *)user_data;
		GtkTreeIter iter;
		
		gtk_list_store_append (lstStrUsers, &iter);
		gtk_list_store_set (lstStrUsers, &iter, 0, (gchar *)key, 1, mysql_usr, -1);
	}
	
	mysql_usr_lst = mysql_server_get_user_list(gui_server->mysql_srv);
	
	if (mysql_usr_lst == NULL) {
		return;
	}
	
	lstStrUsers = gtk_list_store_new(2, G_TYPE_STRING, G_TYPE_POINTER);
	gtk_tree_sortable_set_sort_column_id (GTK_TREE_SORTABLE(lstStrUsers), 0, GTK_SORT_ASCENDING);
	
	mysql_data_list_foreach(mysql_usr_lst, &sub_ht_fill_user_list, (gpointer)lstStrUsers);
	
	gtk_tree_view_set_model(GTK_TREE_VIEW(gui_server->lstUser), GTK_TREE_MODEL(lstStrUsers));
	g_object_unref (G_OBJECT (lstStrUsers));
	
	gui_server->curr_mysql_usr = mysql_usr;
	gui_server->curr_database_rights = NULL;
	
	gmysqlcc_gui_server_display_current_user(gui_server);
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

void gmysqlcc_gui_server_display_current_user (p_gmysqlcc_gui_server gui_server) {
	if (gui_server->curr_mysql_usr != NULL) {
		gtk_entry_set_text(GTK_ENTRY(gui_server->txtUserLogin), gui_server->curr_mysql_usr->login);
		gtk_entry_set_text(GTK_ENTRY(gui_server->txtUserHost), gui_server->curr_mysql_usr->host);
		if (gui_server->curr_mysql_usr->passwd != NULL) {
			gtk_entry_set_text(GTK_ENTRY(gui_server->txtUserPasswd), gui_server->curr_mysql_usr->passwd);
		}
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gui_server->chkEncryptedPasswd), TRUE);
	} else {
		gtk_entry_set_text(GTK_ENTRY(gui_server->txtUserLogin), "");
		gtk_entry_set_text(GTK_ENTRY(gui_server->txtUserHost), "");
		gtk_entry_set_text(GTK_ENTRY(gui_server->txtUserPasswd), "");
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gui_server->chkEncryptedPasswd), FALSE);
		
	}
	
	gmysqlcc_gui_server_fill_user_right_list(gui_server);
	gmysqlcc_gui_server_fill_user_right_database_list(gui_server);
}

void gmysqlcc_gui_server_fill_user_right_list (p_gmysqlcc_gui_server gui_server) {
	GtkListStore * lstStrUserRights;
	
	void sub_ht_fill_user_right_list(gpointer key, gpointer value, gpointer user_data) {
		GtkListStore * lstStrUserRights = (GtkListStore *)user_data;
		GtkTreeIter iter;
		
		gtk_list_store_append (lstStrUserRights, &iter);
		gtk_list_store_set (lstStrUserRights, &iter, 0, (gchar *)key, 1, (gchar *)value, -1);
	}
	
	lstStrUserRights = gtk_list_store_new(2, G_TYPE_STRING, G_TYPE_STRING);
	gtk_tree_sortable_set_sort_column_id (GTK_TREE_SORTABLE(lstStrUserRights), 0, GTK_SORT_ASCENDING);
	
	if (gui_server->curr_mysql_usr != NULL) {
		g_hash_table_foreach(gui_server->curr_mysql_usr->user_rights->hsh_rights, &sub_ht_fill_user_right_list, (gpointer)lstStrUserRights);
	}
	
	gtk_tree_view_set_model(GTK_TREE_VIEW(gui_server->lstURUserRights), GTK_TREE_MODEL(lstStrUserRights));
	g_object_unref (G_OBJECT (lstStrUserRights));
	
}

void gmysqlcc_gui_server_fill_user_right_database_list (p_gmysqlcc_gui_server gui_server) {
	GtkListStore * lstStrDatabaseRights;
	
	void sub_ht_fill_user_right_list(gpointer key, gpointer value, gpointer user_data) {
		GtkListStore * lstStrDatabaseRights = (GtkListStore *)user_data;
		GtkTreeIter iter;
		
		/*g_print("Database : '%s'\n", (gchar *)key);*/
		
		gtk_list_store_append (lstStrDatabaseRights, &iter);
		gtk_list_store_set (lstStrDatabaseRights, &iter, 0, (gchar *)key, 1, value, -1);
	}
	
	lstStrDatabaseRights = gtk_list_store_new(2, G_TYPE_STRING, G_TYPE_POINTER);
	gtk_tree_sortable_set_sort_column_id (GTK_TREE_SORTABLE(lstStrDatabaseRights), 0, GTK_SORT_ASCENDING);
	
	if (gui_server->curr_mysql_usr != NULL) {
		g_hash_table_foreach(gui_server->curr_mysql_usr->hsh_db_list_rights, &sub_ht_fill_user_right_list, (gpointer)lstStrDatabaseRights);
	}
	
	gtk_tree_view_set_model(GTK_TREE_VIEW(gui_server->lstDRDatabases), GTK_TREE_MODEL(lstStrDatabaseRights));
	g_object_unref (G_OBJECT (lstStrDatabaseRights));
	
}

void gmysqlcc_gui_server_display_current_database_right (p_gmysqlcc_gui_server gui_server) {
	if (gui_server->curr_database_rights != NULL) {
		gtk_entry_set_text(GTK_ENTRY(gui_server->txtDRDbName), gui_server->curr_database_rights->db);
	} else {
		gtk_entry_set_text(GTK_ENTRY(gui_server->txtDRDbName), "");
	}
	
	gmysqlcc_gui_server_fill_user_right_database_right_list(gui_server);
}

void gmysqlcc_gui_server_fill_user_right_database_right_list (p_gmysqlcc_gui_server gui_server) {
	GtkListStore * lstStrRights;
	
	void sub_ht_fill_user_right_list(gpointer key, gpointer value, gpointer user_data) {
		GtkListStore * lstStrRights = (GtkListStore *)user_data;
		GtkTreeIter iter;
		
		gtk_list_store_append (lstStrRights, &iter);
		gtk_list_store_set (lstStrRights, &iter, 0, (gchar *)key, 1, (gchar *)value, -1);
	}
	
	lstStrRights = gtk_list_store_new(2, G_TYPE_STRING, G_TYPE_STRING);
	gtk_tree_sortable_set_sort_column_id (GTK_TREE_SORTABLE(lstStrRights), 0, GTK_SORT_ASCENDING);
	
	if (gui_server->curr_mysql_usr != NULL) {
		g_hash_table_foreach(gui_server->curr_database_rights->hsh_rights, &sub_ht_fill_user_right_list, (gpointer)lstStrRights);
	}
	
	gtk_tree_view_set_model(GTK_TREE_VIEW(gui_server->lstDRDatabaseRights), GTK_TREE_MODEL(lstStrRights));
	g_object_unref (G_OBJECT (lstStrRights));
	
}
