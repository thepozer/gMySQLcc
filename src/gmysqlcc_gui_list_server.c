
#include "gmysqlcc_gui_all.h"

char * arFieldNames[] = {"Name", "Host", "Port", "Login", "Password"};

void gmysqlcc_gui_list_server_create_widget (p_gmysqlcc_gui_list_server gui_list_server);
void gmysqlcc_gui_list_server_init_widget (p_gmysqlcc_gui_list_server gui_list_server);
void gmysqlcc_gui_list_server_dislpay_current_server (p_gmysqlcc_gui_list_server gui_list_server);

void gmysqlcc_gui_list_server_evt_destroy (GtkWidget *widget, gpointer user_data);
void gmysqlcc_gui_list_server_evt_btnNew_clicked (GtkWidget *widget, gpointer user_data);
void gmysqlcc_gui_list_server_evt_btnAdd_clicked (GtkWidget *widget, gpointer user_data);
void gmysqlcc_gui_list_server_evt_btnEdit_clicked (GtkWidget *widget, gpointer user_data);
void gmysqlcc_gui_list_server_evt_btnDel_clicked (GtkWidget *widget, gpointer user_data);
void gmysqlcc_gui_list_server_evt_btnServerUp_clicked (GtkWidget *widget, gpointer user_data);
void gmysqlcc_gui_list_server_evt_btnServerDown_clicked (GtkWidget *widget, gpointer user_data);
void gmysqlcc_gui_list_server_evt_btnTlbrEdit_clicked (GtkWidget *widget, gpointer user_data);
void gmysqlcc_gui_list_server_evt_btnTlbrLoad_clicked (GtkWidget *widget, gpointer user_data);
void gmysqlcc_gui_list_server_evt_btnTlbrConnect_clicked (GtkWidget *widget, gpointer user_data);
void gmysqlcc_gui_list_server_evt_btnTlbrClose_clicked (GtkWidget *widget, gpointer user_data);
void gmysqlcc_gui_list_server_evt_lstListHosts_selected (GtkTreeSelection *selection, gpointer user_data);
gboolean gmysqlcc_gui_list_server_evt_lstListHosts_btnpress(GtkWidget *widget, GdkEventButton *event, gpointer user_data);

p_gmysqlcc_gui_list_server gmysqlcc_gui_list_server_new (p_gmysqlcc_config gmysqlcc_conf) {
	p_gmysqlcc_gui_list_server gui_list_server;
	
	/* Init structure */
	gui_list_server = (p_gmysqlcc_gui_list_server) g_try_malloc(sizeof(s_gmysqlcc_gui_list_server));
	
	if (gui_list_server == NULL) {
		return NULL; /* return NULL pointer */
	}
	
	gui_list_server->curr_mysql_srv = NULL;
	gui_list_server->gmysqlcc_conf = gmysqlcc_conf;
	gui_list_server->show_edit_part = TRUE;
	
	gmysqlcc_gui_list_server_create_widget(gui_list_server);
	gmysqlcc_gui_list_server_init_widget(gui_list_server);
	
	/* To hide edition server part */
	gmysqlcc_gui_list_server_evt_btnTlbrEdit_clicked(NULL, gui_list_server);
	
	NbrWnd ++;	
	
	return gui_list_server;
}

gboolean gmysqlcc_gui_list_server_display (p_gmysqlcc_gui_list_server gui_list_server, gboolean display) {
	if (display) {
		gtk_widget_show (gui_list_server->window);
	} else {
		gtk_widget_hide (gui_list_server->window);
	}
	
	return TRUE;
}

gboolean gmysqlcc_gui_list_server_delete (p_gmysqlcc_gui_list_server gui_list_server) {
	/* Destroy Datas */
	g_free(gui_list_server);
	
	/* Destroy Application if needed */
	NbrWnd--;
	g_printerr("Destruction List Server window - nbrWnd : %d\n", NbrWnd);
	if (NbrWnd <= 0) {
		g_printerr("Destroy App\n");
		gtk_main_quit();
	}
	
	return TRUE;
}

void gmysqlcc_gui_list_server_create_widget (p_gmysqlcc_gui_list_server gui_list_server) {
	GtkWidget *frame1;
	GtkWidget *vbox4, *vbox5, *vbox7;
	GtkWidget *hbox5, *hbox6, *hbox7, *hbox8, *hbox12, *hbox13, *hbox14, *hbox15, *hbox16, *hbox17;
	GtkWidget *label9, *label10, *label11, *label12, *label13, *label14, *label17;
	GtkWidget *scrolledwindow3;
	GtkWidget *btnNew, *btnAdd, *btnEdit, *btnDel, *btnServerUp, *btnServerDown;
	
	GtkWidget *hpaned1;
	GtkWidget *toolbar1;
	GtkWidget * imgToolbar;
	GtkToolItem * btnTlbrConnect, * btnTlbrEdit, * btnTlbrClose, * btnTlbrLoad;

	GtkTreeSelection *select;
	GtkTooltips * tooltips;
	
	tooltips = gtk_tooltips_new();
	
	gui_list_server->window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title (GTK_WINDOW (gui_list_server->window), _("Servers list"));
	g_signal_connect (G_OBJECT (gui_list_server->window), "destroy", 
										G_CALLBACK (gmysqlcc_gui_list_server_evt_destroy), gui_list_server);
	gtk_window_set_default_size (GTK_WINDOW (gui_list_server->window), 500, 300);

	vbox4 = gtk_vbox_new (FALSE, 0);
	gtk_widget_show (vbox4);
	gtk_container_add (GTK_CONTAINER (gui_list_server->window), vbox4);

	toolbar1 = gtk_toolbar_new ();
	gtk_widget_show (toolbar1);
	gtk_box_pack_start (GTK_BOX (vbox4), toolbar1, FALSE, FALSE, 0);
	gtk_toolbar_set_style (GTK_TOOLBAR (toolbar1), GTK_TOOLBAR_BOTH_HORIZ);

	imgToolbar = gtk_image_new_from_stock(GTK_STOCK_EXECUTE, GTK_ICON_SIZE_LARGE_TOOLBAR);
	gtk_widget_show(imgToolbar);
	btnTlbrConnect = gtk_tool_button_new (imgToolbar, _("Connection"));
	gtk_tool_item_set_is_important (GTK_TOOL_ITEM(btnTlbrConnect), TRUE);
	g_signal_connect (G_OBJECT (btnTlbrConnect), "clicked", 
										G_CALLBACK (gmysqlcc_gui_list_server_evt_btnTlbrConnect_clicked), gui_list_server);
	gtk_widget_show(GTK_WIDGET(btnTlbrConnect));
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar1), GTK_TOOL_ITEM(btnTlbrConnect), -1);
	gtk_tool_item_set_tooltip (GTK_TOOL_ITEM(btnTlbrConnect), tooltips, _("Connect to a server"), NULL);
	
	imgToolbar = gtk_image_new_from_stock(GTK_STOCK_EXECUTE, GTK_ICON_SIZE_LARGE_TOOLBAR);
	gtk_widget_show(imgToolbar);
	btnTlbrEdit = gtk_tool_button_new (imgToolbar, _("Edit Server"));
	gtk_tool_item_set_is_important (GTK_TOOL_ITEM(btnTlbrEdit), TRUE);
	g_signal_connect (G_OBJECT (btnTlbrEdit), "clicked", 
										G_CALLBACK (gmysqlcc_gui_list_server_evt_btnTlbrEdit_clicked), gui_list_server);
	gtk_widget_show(GTK_WIDGET(btnTlbrEdit));
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar1), GTK_TOOL_ITEM(btnTlbrEdit), -1);
	gtk_tool_item_set_tooltip (GTK_TOOL_ITEM(btnTlbrEdit), tooltips, _("Display edit server part"), NULL);
	
	imgToolbar = gtk_image_new_from_stock(GTK_STOCK_OPEN, GTK_ICON_SIZE_LARGE_TOOLBAR);
	gtk_widget_show(imgToolbar);
	btnTlbrLoad = gtk_tool_button_new (imgToolbar, _("Execute SQL file"));
	gtk_tool_item_set_is_important (GTK_TOOL_ITEM(btnTlbrLoad), TRUE);
	g_signal_connect (G_OBJECT (btnTlbrLoad), "clicked", 
										G_CALLBACK (gmysqlcc_gui_list_server_evt_btnTlbrLoad_clicked), gui_list_server);
	gtk_widget_show(GTK_WIDGET(btnTlbrLoad));
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar1), GTK_TOOL_ITEM(btnTlbrLoad), -1);
	gtk_tool_item_set_tooltip (GTK_TOOL_ITEM(btnTlbrLoad), tooltips, _("Execute an SQL file on a MySQL server"), NULL);
	
	imgToolbar = gtk_image_new_from_stock(GTK_STOCK_CLOSE, GTK_ICON_SIZE_LARGE_TOOLBAR);
	gtk_widget_show(imgToolbar);
	btnTlbrClose = gtk_tool_button_new (imgToolbar, _("Close"));
	gtk_tool_item_set_is_important (GTK_TOOL_ITEM(btnTlbrClose), TRUE);
	g_signal_connect (G_OBJECT (btnTlbrClose), "clicked", 
										G_CALLBACK (gmysqlcc_gui_list_server_evt_btnTlbrClose_clicked), gui_list_server);
	gtk_widget_show(GTK_WIDGET(btnTlbrClose));
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar1), GTK_TOOL_ITEM(btnTlbrClose), -1);
	gtk_tool_item_set_tooltip (GTK_TOOL_ITEM(btnTlbrClose), tooltips, _("Close window"), NULL);
	
  hpaned1 = gtk_hpaned_new ();
  gtk_widget_show (hpaned1);
  gtk_container_add (GTK_CONTAINER (vbox4), hpaned1);
  gtk_paned_set_position (GTK_PANED (hpaned1), 200);

  vbox7 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox7);
  gtk_paned_pack1 (GTK_PANED (hpaned1), vbox7, TRUE, TRUE);

  scrolledwindow3 = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (scrolledwindow3);
  gtk_box_pack_start (GTK_BOX (vbox7), scrolledwindow3, TRUE, TRUE, 0);

  gui_list_server->lstListHosts = gtk_tree_view_new ();
  gtk_widget_show (gui_list_server->lstListHosts);
  gtk_container_add (GTK_CONTAINER (scrolledwindow3), gui_list_server->lstListHosts);
	g_signal_connect (G_OBJECT (gui_list_server->lstListHosts), "button-press-event", 
										G_CALLBACK (gmysqlcc_gui_list_server_evt_lstListHosts_btnpress), gui_list_server);
	select = gtk_tree_view_get_selection (GTK_TREE_VIEW (gui_list_server->lstListHosts));
	gtk_tree_selection_set_mode (select, GTK_SELECTION_SINGLE);
	g_signal_connect (G_OBJECT (select), "changed", 
										G_CALLBACK (gmysqlcc_gui_list_server_evt_lstListHosts_selected), gui_list_server);

  hbox14 = gtk_hbox_new (TRUE, 0);
  gtk_widget_show (hbox14);
  gtk_box_pack_start (GTK_BOX (vbox7), hbox14, FALSE, TRUE, 0);

	btnServerUp = createIconButton("gtk-go-up", _("_Up"));
  gtk_box_pack_start (GTK_BOX (hbox14), btnServerUp, FALSE, FALSE, 0);
	g_signal_connect (G_OBJECT (btnServerUp), "clicked", 
										G_CALLBACK (gmysqlcc_gui_list_server_evt_btnServerUp_clicked), gui_list_server);

	btnServerDown = createIconButton("gtk-go-down", _("_Down"));
  gtk_box_pack_start (GTK_BOX (hbox14), btnServerDown, FALSE, FALSE, 0);
	g_signal_connect (G_OBJECT (btnServerDown), "clicked", 
										G_CALLBACK (gmysqlcc_gui_list_server_evt_btnServerDown_clicked), gui_list_server);

  gui_list_server->vbxEditPart = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (gui_list_server->vbxEditPart);
  gtk_paned_pack2 (GTK_PANED (hpaned1), gui_list_server->vbxEditPart, TRUE, TRUE);

  frame1 = gtk_frame_new (NULL);
  gtk_widget_show (frame1);
  gtk_box_pack_start (GTK_BOX (gui_list_server->vbxEditPart), frame1, FALSE, FALSE, 0);

  label9 = gtk_label_new (_("Server"));
  gtk_widget_show (label9);
  gtk_frame_set_label_widget (GTK_FRAME (frame1), label9);
  gtk_label_set_justify (GTK_LABEL (label9), GTK_JUSTIFY_LEFT);

  vbox5 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox5);
  gtk_container_add (GTK_CONTAINER (frame1), vbox5);
  gtk_container_set_border_width (GTK_CONTAINER (vbox5), 4);

  hbox6 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox6);
  gtk_box_pack_start (GTK_BOX (vbox5), hbox6, TRUE, TRUE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (hbox6), 2);

  label10 = gtk_label_new (_("Name :"));
  gtk_widget_show (label10);
  gtk_box_pack_start (GTK_BOX (hbox6), label10, FALSE, FALSE, 0);
  gtk_label_set_justify (GTK_LABEL (label10), GTK_JUSTIFY_LEFT);

  gui_list_server->txtName = gtk_entry_new ();
  gtk_widget_show (gui_list_server->txtName);
  gtk_box_pack_start (GTK_BOX (hbox6), gui_list_server->txtName, TRUE, TRUE, 2);

  hbox7 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox7);
  gtk_box_pack_start (GTK_BOX (vbox5), hbox7, TRUE, TRUE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (hbox7), 2);

  label11 = gtk_label_new (_("Host :"));
  gtk_widget_show (label11);
  gtk_box_pack_start (GTK_BOX (hbox7), label11, FALSE, FALSE, 0);
  gtk_label_set_justify (GTK_LABEL (label11), GTK_JUSTIFY_LEFT);

  gui_list_server->txtHost = gtk_entry_new ();
  gtk_widget_show (gui_list_server->txtHost);
  gtk_box_pack_start (GTK_BOX (hbox7), gui_list_server->txtHost, TRUE, TRUE, 2);

  hbox12 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox12);
  gtk_box_pack_start (GTK_BOX (vbox5), hbox12, TRUE, TRUE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (hbox12), 2);

  label12 = gtk_label_new (_("Port :"));
  gtk_widget_show (label12);
  gtk_box_pack_start (GTK_BOX (hbox12), label12, FALSE, FALSE, 0);
  gtk_label_set_justify (GTK_LABEL (label12), GTK_JUSTIFY_LEFT);

  gui_list_server->txtPort = gtk_entry_new ();
  gtk_widget_show (gui_list_server->txtPort);
  gtk_box_pack_start (GTK_BOX (hbox12), gui_list_server->txtPort, TRUE, TRUE, 2);
  gtk_entry_set_max_length (GTK_ENTRY (gui_list_server->txtPort), 5);

  hbox8 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox8);
  gtk_box_pack_start (GTK_BOX (vbox5), hbox8, TRUE, TRUE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (hbox8), 2);

  label13 = gtk_label_new (_("User name :"));
  gtk_widget_show (label13);
  gtk_box_pack_start (GTK_BOX (hbox8), label13, FALSE, FALSE, 0);
  gtk_label_set_justify (GTK_LABEL (label13), GTK_JUSTIFY_LEFT);

  gui_list_server->txtLogin = gtk_entry_new ();
  gtk_widget_show (gui_list_server->txtLogin);
  gtk_box_pack_start (GTK_BOX (hbox8), gui_list_server->txtLogin, TRUE, TRUE, 2);

  hbox13 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox13);
  gtk_box_pack_start (GTK_BOX (vbox5), hbox13, TRUE, TRUE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (hbox13), 2);

  label14 = gtk_label_new (_("Password :"));
  gtk_widget_show (label14);
  gtk_box_pack_start (GTK_BOX (hbox13), label14, FALSE, FALSE, 0);
  gtk_label_set_justify (GTK_LABEL (label14), GTK_JUSTIFY_LEFT);

  gui_list_server->txtPasswd = gtk_entry_new ();
  gtk_widget_show (gui_list_server->txtPasswd);
  gtk_box_pack_start (GTK_BOX (hbox13), gui_list_server->txtPasswd, TRUE, TRUE, 2);

  hbox15 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox15);
  gtk_box_pack_start (GTK_BOX (vbox5), hbox15, TRUE, TRUE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (hbox15), 2);

  label17 = gtk_label_new (_("Local socket file :"));
  gtk_widget_show (label17);
  gtk_box_pack_start (GTK_BOX (hbox15), label17, FALSE, FALSE, 0);
  gtk_label_set_justify (GTK_LABEL (label17), GTK_JUSTIFY_LEFT);

  gui_list_server->txtLocalSock = gtk_entry_new ();
  gtk_widget_show (gui_list_server->txtLocalSock);
  gtk_box_pack_start (GTK_BOX (hbox15), gui_list_server->txtLocalSock, TRUE, TRUE, 2);

	hbox16 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox16);
  gtk_box_pack_start (GTK_BOX (vbox5), hbox16, TRUE, TRUE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (hbox16), 2);

  gui_list_server->chkReadOnly = gtk_check_button_new_with_label (_("Read only server"));
  gtk_widget_show (gui_list_server->chkReadOnly);
  gtk_box_pack_start (GTK_BOX (hbox16), gui_list_server->chkReadOnly, TRUE, TRUE, 2);

	hbox17 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox17);
  gtk_box_pack_start (GTK_BOX (vbox5), hbox17, TRUE, TRUE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (hbox17), 2);

  gui_list_server->chkWriteWarning = gtk_check_button_new_with_label (_("Warning about write query on this server"));
  gtk_widget_show (gui_list_server->chkWriteWarning);
  gtk_box_pack_start (GTK_BOX (hbox17), gui_list_server->chkWriteWarning, TRUE, TRUE, 2);

  hbox5 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox5);
  gtk_box_pack_start (GTK_BOX (gui_list_server->vbxEditPart), hbox5, FALSE, TRUE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (hbox5), 1);

	btnNew = createIconButton("gtk-new", _("_New"));
  gtk_box_pack_start (GTK_BOX (hbox5), btnNew, TRUE, TRUE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (btnNew), 3);
	g_signal_connect (G_OBJECT (btnNew), "clicked", 
										G_CALLBACK (gmysqlcc_gui_list_server_evt_btnNew_clicked), gui_list_server);

	btnAdd = createIconButton("gtk-add", _("_Add"));
  gtk_box_pack_start (GTK_BOX (hbox5), btnAdd, TRUE, TRUE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (btnAdd), 2);
	g_signal_connect (G_OBJECT (btnAdd), "clicked", 
										G_CALLBACK (gmysqlcc_gui_list_server_evt_btnAdd_clicked), gui_list_server);

	btnEdit = createIconButton("gtk-apply", _("_Modify"));
  gtk_box_pack_start (GTK_BOX (hbox5), btnEdit, TRUE, TRUE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (btnEdit), 2);
	g_signal_connect (G_OBJECT (btnEdit), "clicked", 
										G_CALLBACK (gmysqlcc_gui_list_server_evt_btnEdit_clicked), gui_list_server);
	
	btnDel = createIconButton("gtk-del", _("D_elete"));
  gtk_box_pack_start (GTK_BOX (hbox5), btnDel, TRUE, TRUE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (btnDel), 2);
	g_signal_connect (G_OBJECT (btnDel), "clicked", 
										G_CALLBACK (gmysqlcc_gui_list_server_evt_btnDel_clicked), gui_list_server);
	
}

void gmysqlcc_gui_list_server_init_widget (p_gmysqlcc_gui_list_server gui_list_server) {
	p_mysql_server mysql_srv;
  GList * items = NULL;
	GtkListStore * lstStrBase;
	GtkTreeIter iter, * itSel;
	GtkTreeViewColumn * currCol;
	GtkCellRenderer * renderer;
	GtkTreeSelection* selection;
	GtkTreePath * pathToSelRow;
	
	itSel = NULL;
	lstStrBase = gtk_list_store_new(2, G_TYPE_STRING, G_TYPE_POINTER);
	items = g_list_first(gui_list_server->gmysqlcc_conf->lst_servers);
	while (items != NULL) {
		mysql_srv = (p_mysql_server)items->data;
		
		/* Display if needed the field */
		if (mysql_srv != NULL) {
			gtk_list_store_append (lstStrBase, &iter);
			gtk_list_store_set (lstStrBase, &iter, 0, mysql_srv->name, -1);
			gtk_list_store_set (lstStrBase, &iter, 1, mysql_srv, -1);
			
			if (mysql_srv == gui_list_server->curr_mysql_srv) {
				itSel = gtk_tree_iter_copy(&iter);
			}
		}
		
		/* Go to next item */
		items = g_list_next(items);
	}
	
	gtk_tree_view_set_model(GTK_TREE_VIEW(gui_list_server->lstListHosts), GTK_TREE_MODEL(lstStrBase));
		
	currCol = gtk_tree_view_get_column(GTK_TREE_VIEW(gui_list_server->lstListHosts), 0);
	if (currCol != NULL) {
		gtk_tree_view_remove_column(GTK_TREE_VIEW(gui_list_server->lstListHosts), currCol);
	}

	renderer = gtk_cell_renderer_text_new ();
	currCol = gtk_tree_view_column_new_with_attributes (_("Server name"), renderer, "text", 0, NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW (gui_list_server->lstListHosts), currCol);

	if (itSel != NULL) {
		selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(gui_list_server->lstListHosts));
		gtk_tree_selection_select_iter(GTK_TREE_SELECTION(selection), itSel);
		
		pathToSelRow = gtk_tree_model_get_path (GTK_TREE_MODEL(lstStrBase), itSel);
		gtk_tree_view_scroll_to_cell (GTK_TREE_VIEW(gui_list_server->lstListHosts), pathToSelRow, NULL, TRUE, 0.5, 0.0);
		
		gtk_tree_path_free (pathToSelRow);
		gtk_tree_iter_free (itSel);
		
		gmysqlcc_gui_list_server_dislpay_current_server (gui_list_server);
	}

	g_object_unref (G_OBJECT (lstStrBase));
	
}

void gmysqlcc_gui_list_server_dislpay_current_server (p_gmysqlcc_gui_list_server gui_list_server) {
	p_mysql_server mysql_srv = gui_list_server->curr_mysql_srv;
	char tmpPort [G_ASCII_DTOSTR_BUF_SIZE + 1];
	
	if (mysql_srv != NULL) {
		g_ascii_dtostr(tmpPort, G_ASCII_DTOSTR_BUF_SIZE, mysql_srv->port);
		
		gtk_entry_set_text(GTK_ENTRY(gui_list_server->txtName), mysql_srv->name);
		gtk_entry_set_text(GTK_ENTRY(gui_list_server->txtHost), mysql_srv->host);
		gtk_entry_set_text(GTK_ENTRY(gui_list_server->txtPort), tmpPort);
		gtk_entry_set_text(GTK_ENTRY(gui_list_server->txtLogin), mysql_srv->user);
		gtk_entry_set_text(GTK_ENTRY(gui_list_server->txtPasswd), mysql_srv->passwd);
		gtk_entry_set_text(GTK_ENTRY(gui_list_server->txtLocalSock), 
											((mysql_srv->localSock != NULL) ? mysql_srv->localSock : ""));
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gui_list_server->chkReadOnly), mysql_srv->read_only);
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gui_list_server->chkWriteWarning), mysql_srv->write_warning);
	} else {
		gtk_entry_set_text(GTK_ENTRY(gui_list_server->txtName), "");
		gtk_entry_set_text(GTK_ENTRY(gui_list_server->txtHost), "");
		gtk_entry_set_text(GTK_ENTRY(gui_list_server->txtPort), "3306");
		gtk_entry_set_text(GTK_ENTRY(gui_list_server->txtLogin), "");
		gtk_entry_set_text(GTK_ENTRY(gui_list_server->txtPasswd), "");
		gtk_entry_set_text(GTK_ENTRY(gui_list_server->txtLocalSock), "/var/run/mysqld/mysqld.sock");
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gui_list_server->chkReadOnly), FALSE);
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gui_list_server->chkWriteWarning), FALSE);
	}
}

void gmysqlcc_gui_list_server_evt_destroy (GtkWidget *widget, gpointer user_data) {
	p_gmysqlcc_gui_list_server gui_list_server = (p_gmysqlcc_gui_list_server)user_data;
	
	gmysqlcc_gui_list_server_display(gui_list_server, FALSE);
	gmysqlcc_gui_list_server_delete(gui_list_server);
}

void gmysqlcc_gui_list_server_evt_btnNew_clicked (GtkWidget *widget, gpointer user_data) {
	p_gmysqlcc_gui_list_server gui_list_server = (p_gmysqlcc_gui_list_server)user_data;
	
	gui_list_server->curr_mysql_srv = NULL;
	gmysqlcc_gui_list_server_init_widget(gui_list_server);
}

void gmysqlcc_gui_list_server_evt_btnAdd_clicked (GtkWidget *widget, gpointer user_data) {
	p_gmysqlcc_gui_list_server gui_list_server = (p_gmysqlcc_gui_list_server)user_data;
	GString * infos[6];
	GtkWidget * msgdlg;
	gboolean read_only, write_warning;
	int i, j, port;
	
	/* Get data */
	infos[0] = g_string_new(gtk_entry_get_text(GTK_ENTRY(gui_list_server->txtName)));
	infos[1] = g_string_new(gtk_entry_get_text(GTK_ENTRY(gui_list_server->txtHost)));
	infos[2] = g_string_new(gtk_entry_get_text(GTK_ENTRY(gui_list_server->txtPort)));
	infos[3] = g_string_new(gtk_entry_get_text(GTK_ENTRY(gui_list_server->txtLogin)));
	infos[4] = g_string_new(gtk_entry_get_text(GTK_ENTRY(gui_list_server->txtPasswd)));
	infos[5] = g_string_new(gtk_entry_get_text(GTK_ENTRY(gui_list_server->txtLocalSock)));
	read_only = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(gui_list_server->chkReadOnly));
	write_warning = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(gui_list_server->chkWriteWarning));
	
	/* Check data */
	for (i = 0; i < 4; i++) {
		if (infos[i]->len == 0) { /* check if not empty */
			msgdlg = gtk_message_dialog_new(GTK_WINDOW(gui_list_server->window), GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, _("Field %s cannot be empty !!!"), arFieldNames[i]);
			gtk_dialog_run (GTK_DIALOG (msgdlg));
			gtk_widget_destroy (msgdlg);
			return;
		} else if (i == 2) { /* check if port is a number */
			for (j = 0; j < infos[i]->len; j++) {
				if (!g_ascii_isdigit(infos[i]->str[j])) {
					msgdlg = gtk_message_dialog_new(GTK_WINDOW(gui_list_server->window), GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, _("Field %s can only contain digits !!!"), arFieldNames[i]);
					gtk_dialog_run (GTK_DIALOG (msgdlg));
					gtk_widget_destroy (msgdlg);
					return;
				}
			}
		}
	}
	
	/* Check if name is unique */
	if (gmysqlcc_config_get_server(gui_list_server->gmysqlcc_conf, infos[0]->str) != NULL) {
		msgdlg = gtk_message_dialog_new(GTK_WINDOW(gui_list_server->window), GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, _("Name already taken !\nPlease set another one."));
		gtk_dialog_run (GTK_DIALOG (msgdlg));
		gtk_widget_destroy (msgdlg);
		return;
	}
	
	/* Insert data in configuration */
	port = (int)g_ascii_strtoull(infos[2]->str, NULL, 10);
	if (gmysqlcc_config_add_server(gui_list_server->gmysqlcc_conf, infos[0]->str, infos[1]->str, port, infos[3]->str, infos[4]->str, NULL, infos[5]->str, read_only, write_warning)) {
		gui_list_server->curr_mysql_srv = gmysqlcc_config_get_server(gui_list_server->gmysqlcc_conf, infos[0]->str);
		gmysqlcc_gui_list_server_init_widget(gui_list_server);
	}
	
	for (i = 0; i < 6; i++) {
		if (infos[i] != NULL) {
			g_string_free(infos[i], TRUE);
		}
	}
}

void gmysqlcc_gui_list_server_evt_btnEdit_clicked (GtkWidget *widget, gpointer user_data) {
	p_gmysqlcc_gui_list_server gui_list_server = (p_gmysqlcc_gui_list_server)user_data;
	p_mysql_server newSrvr;
	GString * infos[6];
	GtkWidget * msgdlg;
	gboolean read_only, write_warning;
	int i, j, port;
	
	if (gui_list_server->curr_mysql_srv == NULL) {
		msgdlg = gtk_message_dialog_new(GTK_WINDOW(gui_list_server->window), GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, _("Please select a database"));
		gtk_dialog_run (GTK_DIALOG (msgdlg));
		gtk_widget_destroy (msgdlg);
		return;
	}
	
	/* Get data */
	infos[0] = g_string_new(gtk_entry_get_text(GTK_ENTRY(gui_list_server->txtName)));
	infos[1] = g_string_new(gtk_entry_get_text(GTK_ENTRY(gui_list_server->txtHost)));
	infos[2] = g_string_new(gtk_entry_get_text(GTK_ENTRY(gui_list_server->txtPort)));
	infos[3] = g_string_new(gtk_entry_get_text(GTK_ENTRY(gui_list_server->txtLogin)));
	infos[4] = g_string_new(gtk_entry_get_text(GTK_ENTRY(gui_list_server->txtPasswd)));
	infos[5] = g_string_new(gtk_entry_get_text(GTK_ENTRY(gui_list_server->txtLocalSock)));
	read_only = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(gui_list_server->chkReadOnly));
	write_warning = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(gui_list_server->chkWriteWarning));
	
	/* Check data */
	for (i = 0; i < 4; i++) {
		if (infos[i]->len == 0) { /* check if not empty */
			msgdlg = gtk_message_dialog_new(GTK_WINDOW(gui_list_server->window), GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, _("Field %s cannot be empty !!!"), arFieldNames[i]);
			gtk_dialog_run (GTK_DIALOG (msgdlg));
			gtk_widget_destroy (msgdlg);
			return;		
		} else if (i == 2) { /* check if port is a number */
			for (j = 0; j < infos[i]->len; j++) {
				if (!g_ascii_isdigit(infos[i]->str[j])) {
					msgdlg = gtk_message_dialog_new(GTK_WINDOW(gui_list_server->window), GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, _("Field %s can only contain digits !!!"), arFieldNames[i]);
					gtk_dialog_run (GTK_DIALOG (msgdlg));
					gtk_widget_destroy (msgdlg);
					return;		
				}
			}
		}
	}
	
	/* Check if name is unique */
	newSrvr = gmysqlcc_config_get_server(gui_list_server->gmysqlcc_conf, infos[0]->str);
	if (newSrvr != NULL && newSrvr != gui_list_server->curr_mysql_srv) {
		msgdlg = gtk_message_dialog_new(GTK_WINDOW(gui_list_server->window), GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, _("Name already taken !\nPlease set another one."));
		gtk_dialog_run (GTK_DIALOG (msgdlg));
		gtk_widget_destroy (msgdlg);
		return;		
	}
	
	/* Update data in configuration */
	port = (int)g_ascii_strtoull(infos[2]->str, NULL, 10);
	if (gmysqlcc_config_update_server(gui_list_server->gmysqlcc_conf, gui_list_server->curr_mysql_srv->name, infos[0]->str, infos[1]->str, port, infos[3]->str, infos[4]->str, NULL, infos[5]->str, read_only, write_warning)) {
		gmysqlcc_gui_list_server_init_widget(gui_list_server);
	}
	
	for (i = 0; i < 6; i++) {
		if (infos[i] != NULL) {
			g_string_free(infos[i], 1);
		}
	}
}

void gmysqlcc_gui_list_server_evt_btnDel_clicked (GtkWidget *widget, gpointer user_data) {
	p_gmysqlcc_gui_list_server gui_list_server = (p_gmysqlcc_gui_list_server)user_data;
	GtkWidget * msgdlg;
	
	if (gui_list_server->curr_mysql_srv != NULL) {
		if (!gmysqlcc_config_del_server (gui_list_server->gmysqlcc_conf, gui_list_server->curr_mysql_srv->name)) {
			msgdlg = gtk_message_dialog_new(GTK_WINDOW(gui_list_server->window), GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, _("Can't delete this server !!!"));
			gtk_dialog_run (GTK_DIALOG (msgdlg));
			gtk_widget_destroy (msgdlg);
		} else {
			gmysqlcc_gui_list_server_init_widget(gui_list_server);
		}
	} else {
		msgdlg = gtk_message_dialog_new(GTK_WINDOW(gui_list_server->window), GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, _("Please select a database"));
		gtk_dialog_run (GTK_DIALOG (msgdlg));
		gtk_widget_destroy (msgdlg);
	}
}

void gmysqlcc_gui_list_server_evt_btnServerUp_clicked (GtkWidget *widget, gpointer user_data) {
	p_gmysqlcc_gui_list_server gui_list_server = (p_gmysqlcc_gui_list_server)user_data;
	GtkTreeIter itCurr;
	GtkTreeModel *model;
	GtkTreeSelection* selection;
	p_mysql_server currSrvr;
	GList * lstCurr, * lstTrgt;
	GtkWidget * msgdlg;
	gboolean error = FALSE;
	
	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(gui_list_server->lstListHosts));
	if (gtk_tree_selection_get_selected (selection, &model, &itCurr)) {
		gtk_tree_model_get (model, &itCurr, 1, &currSrvr, -1);
		lstCurr = g_list_find (gui_list_server->gmysqlcc_conf->lst_servers, currSrvr);
		
		if (lstCurr != NULL && lstCurr->prev != NULL) {
			lstTrgt = lstCurr->prev;
			
			gui_list_server->gmysqlcc_conf->lst_servers = g_list_insert_before(gui_list_server->gmysqlcc_conf->lst_servers, lstTrgt, currSrvr);
			gui_list_server->gmysqlcc_conf->lst_servers = g_list_delete_link(gui_list_server->gmysqlcc_conf->lst_servers, lstCurr);
			
			gmysqlcc_gui_list_server_init_widget(gui_list_server);
		} else {
			error = TRUE;
		}
	} else {
		error = TRUE;
	}
	
	if (error) {
		msgdlg = gtk_message_dialog_new(GTK_WINDOW(gui_list_server->window), GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, _("Can't move up more !!!"));
		gtk_dialog_run (GTK_DIALOG (msgdlg));
		gtk_widget_destroy (msgdlg);
	}
}

void gmysqlcc_gui_list_server_evt_btnServerDown_clicked (GtkWidget *widget, gpointer user_data) {
	p_gmysqlcc_gui_list_server gui_list_server = (p_gmysqlcc_gui_list_server)user_data;
	GtkTreeIter itCurr;
	GtkTreeModel *model;
	GtkTreeSelection* selection;
	p_mysql_server currSrvr;
	GList * lstCurr, * lstTrgt;
	GtkWidget * msgdlg;
	gboolean error = FALSE;
	
	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(gui_list_server->lstListHosts));
	if (gtk_tree_selection_get_selected (selection, &model, &itCurr)) {
		gtk_tree_model_get (model, &itCurr, 1, &currSrvr, -1);
		lstCurr = g_list_find (gui_list_server->gmysqlcc_conf->lst_servers, currSrvr);
		
		if (lstCurr != NULL && lstCurr->next != NULL) {
			lstTrgt = lstCurr->next->next;
			
			gui_list_server->gmysqlcc_conf->lst_servers = g_list_insert_before(gui_list_server->gmysqlcc_conf->lst_servers, lstTrgt, currSrvr);
			gui_list_server->gmysqlcc_conf->lst_servers = g_list_delete_link(gui_list_server->gmysqlcc_conf->lst_servers, lstCurr);
			
			gmysqlcc_gui_list_server_init_widget(gui_list_server);
		} else {
			error = TRUE;
		}
	} else {
		error = TRUE;
	}
	
	if (error) {
		msgdlg = gtk_message_dialog_new(GTK_WINDOW(gui_list_server->window), GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, _("Can't move down more !!!"));
		gtk_dialog_run (GTK_DIALOG (msgdlg));
		gtk_widget_destroy (msgdlg);
	}
}

void gmysqlcc_gui_list_server_evt_btnTlbrEdit_clicked (GtkWidget *widget, gpointer user_data) {
	p_gmysqlcc_gui_list_server gui_list_server = (p_gmysqlcc_gui_list_server)user_data;	
	
	gui_list_server->show_edit_part = (gui_list_server->show_edit_part) ? FALSE : TRUE ;
	
	if (gui_list_server->show_edit_part) {
		gtk_widget_show(GTK_WIDGET(gui_list_server->vbxEditPart));
	} else {
		gtk_widget_hide(GTK_WIDGET(gui_list_server->vbxEditPart));
	}
}

void gmysqlcc_gui_list_server_evt_btnTlbrLoad_clicked (GtkWidget *widget, gpointer user_data) {
	p_gmysqlcc_gui_exec_file gui_xcfl = NULL;
	
	gui_xcfl = gmysqlcc_gui_exec_file_new_open_file();
	
	if (gui_xcfl != NULL) {
		gmysqlcc_gui_exec_file_display(gui_xcfl, TRUE);
	}
}

void gmysqlcc_gui_list_server_evt_btnTlbrConnect_clicked (GtkWidget *widget, gpointer user_data) {
	p_gmysqlcc_gui_list_server gui_list_server = (p_gmysqlcc_gui_list_server)user_data;
	p_gmysqlcc_gui_server gui_server;
	GtkWidget * msgdlg;
	
	if (gui_list_server->curr_mysql_srv != NULL) {
		gui_server = gmysqlcc_gui_server_new(gui_list_server->curr_mysql_srv);
		gmysqlcc_gui_server_display(gui_server, TRUE);
	} else {
		msgdlg = gtk_message_dialog_new(GTK_WINDOW(gui_list_server->window), GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, _("Please select a database"));
		gtk_dialog_run (GTK_DIALOG (msgdlg));
		gtk_widget_destroy (msgdlg);
	}	
}

void gmysqlcc_gui_list_server_evt_btnTlbrClose_clicked (GtkWidget *widget, gpointer user_data) {
	p_gmysqlcc_gui_list_server gui_list_server = (p_gmysqlcc_gui_list_server)user_data;
	
	gtk_widget_destroy(GTK_WIDGET(gui_list_server->window));
}

void gmysqlcc_gui_list_server_evt_lstListHosts_selected (GtkTreeSelection *selection, gpointer user_data) {
	p_gmysqlcc_gui_list_server gui_list_server = (p_gmysqlcc_gui_list_server)user_data;
	GtkTreeIter iter;
	GtkTreeModel *model;
	
	if (gtk_tree_selection_get_selected (selection, &model, &iter)) {
		gtk_tree_model_get (model, &iter, 1, &gui_list_server->curr_mysql_srv, -1);
		
		gmysqlcc_gui_list_server_dislpay_current_server(gui_list_server);
	}
}

gboolean gmysqlcc_gui_list_server_evt_lstListHosts_btnpress(GtkWidget *widget, GdkEventButton *event, gpointer user_data) {
	if (event->type == GDK_2BUTTON_PRESS) {
		gmysqlcc_gui_list_server_evt_btnTlbrConnect_clicked(widget, user_data);
	}
	return FALSE;
}
