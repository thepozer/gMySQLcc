
#include "gListServer.h"

extern int NbrWnd;

char * arFieldNames[] = {"Name", "Host", "Port", "Login", "Password"};

void displayCurrentDB (p_listServWnd pLstSrvWnd);
void showListServer (p_listServWnd pLstSrvWnd, p_mysql_server currSrvr);

static void lstlisthosts_selected (GtkTreeSelection *selection, gpointer user_data);
static gboolean lstlisthosts_btnpress(GtkWidget *widget, GdkEventButton *event, gpointer user_data);
static void btnconnect_clicked (GtkWidget *widget, gpointer user_data);
static void btnnew_clicked (GtkWidget *widget, gpointer user_data);
static void btnadd_clicked (GtkWidget *widget, gpointer user_data);
static void btnedit_clicked (GtkWidget *widget, gpointer user_data);
static void btndel_clicked (GtkWidget *widget, gpointer user_data);
static void btnclose_clicked (GtkWidget *widget, gpointer user_data);
static void btnserverup_clicked (GtkWidget *widget, gpointer user_data);
static void btnserverdown_clicked (GtkWidget *widget, gpointer user_data);
static void destroy(GtkWidget *widget, gpointer user_data);

void displayCurrentDB (p_listServWnd pLstSrvWnd) {
	p_mysql_server currSrvr = pLstSrvWnd->selSrvr;
	char tmpPort [G_ASCII_DTOSTR_BUF_SIZE + 1];
	
	if (currSrvr != (p_mysql_server)NULL) {
		g_ascii_dtostr(tmpPort, G_ASCII_DTOSTR_BUF_SIZE, currSrvr->port);
		gtk_entry_set_text(GTK_ENTRY(pLstSrvWnd->txtName), currSrvr->name);
		gtk_entry_set_text(GTK_ENTRY(pLstSrvWnd->txtHost), currSrvr->host);
		gtk_entry_set_text(GTK_ENTRY(pLstSrvWnd->txtPort), tmpPort);
		gtk_entry_set_text(GTK_ENTRY(pLstSrvWnd->txtLogin), currSrvr->user);
		gtk_entry_set_text(GTK_ENTRY(pLstSrvWnd->txtPasswd), currSrvr->passwd);
		gtk_entry_set_text(GTK_ENTRY(pLstSrvWnd->txtLocalSock), ((currSrvr->localSock != (gchar *)NULL) ? currSrvr->localSock : ""));
	} else {
		gtk_entry_set_text(GTK_ENTRY(pLstSrvWnd->txtName), "");
		gtk_entry_set_text(GTK_ENTRY(pLstSrvWnd->txtHost), "");
		gtk_entry_set_text(GTK_ENTRY(pLstSrvWnd->txtPort), "3306");
		gtk_entry_set_text(GTK_ENTRY(pLstSrvWnd->txtLogin), "");
		gtk_entry_set_text(GTK_ENTRY(pLstSrvWnd->txtPasswd), "");
		gtk_entry_set_text(GTK_ENTRY(pLstSrvWnd->txtLocalSock), "");
	}
}

void showListServer (p_listServWnd pLstSrvWnd, p_mysql_server currSrvr) {
	p_mysql_server msql_srv;
  GList * items = NULL;
	GtkListStore * lstStrBase;
	GtkTreeIter iter, * itSel;
	GtkTreeViewColumn * currCol;
	GtkCellRenderer * renderer;
	GtkTreeSelection* selection;
	GtkTreePath * pathToSelRow;
	
	itSel = (GtkTreeIter *)NULL;
	lstStrBase = gtk_list_store_new(2, G_TYPE_STRING, G_TYPE_POINTER);
	items = g_list_first(pLstSrvWnd->gmsql_conf->lstServers);
	while (items != (GList *)NULL) {
		msql_srv = (p_mysql_server)items->data;
		
		/* Display if needed the field */
		if (msql_srv != (p_mysql_server)NULL) {
			gtk_list_store_append (lstStrBase, &iter);
			gtk_list_store_set (lstStrBase, &iter, 0, msql_srv->name, -1);
			gtk_list_store_set (lstStrBase, &iter, 1, msql_srv, -1);
			
			if (msql_srv == currSrvr) {
				itSel = gtk_tree_iter_copy(&iter);
			}
		}
		
		/* Go to next item */
		items = g_list_next(items);
	}
	
	gtk_tree_view_set_model(GTK_TREE_VIEW(pLstSrvWnd->lstListHosts), GTK_TREE_MODEL(lstStrBase));
		
	currCol = gtk_tree_view_get_column(GTK_TREE_VIEW(pLstSrvWnd->lstListHosts), 0);
	if (currCol != (GtkTreeViewColumn *)NULL) {
		gtk_tree_view_remove_column(GTK_TREE_VIEW(pLstSrvWnd->lstListHosts), currCol);
	}

	renderer = gtk_cell_renderer_text_new ();
	currCol = gtk_tree_view_column_new_with_attributes (_("Server name"), renderer, "text", 0, NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW (pLstSrvWnd->lstListHosts), currCol);

	if (itSel != (GtkTreeIter *)NULL) {
		selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(pLstSrvWnd->lstListHosts));
		gtk_tree_selection_select_iter(GTK_TREE_SELECTION(selection), itSel);
		
		pathToSelRow = gtk_tree_model_get_path (GTK_TREE_MODEL(lstStrBase), itSel);
		gtk_tree_view_scroll_to_cell (GTK_TREE_VIEW(pLstSrvWnd->lstListHosts), pathToSelRow, (GtkTreeViewColumn *)NULL, TRUE, 0.5, 0.0);
		
		gtk_tree_path_free (pathToSelRow);
		gtk_tree_iter_free (itSel);
	}

	g_object_unref (G_OBJECT (lstStrBase));
	
	pLstSrvWnd->selSrvr = currSrvr;
	displayCurrentDB(pLstSrvWnd);
}

static void lstlisthosts_selected (GtkTreeSelection *selection, gpointer user_data) {
	GtkTreeIter iter;
	GtkTreeModel *model;
	p_listServWnd pLstSrvWnd = (p_listServWnd)user_data;
	
	if (gtk_tree_selection_get_selected (selection, &model, &iter)) {
		gtk_tree_model_get (model, &iter, 1, &pLstSrvWnd->selSrvr, -1);
		
		displayCurrentDB(pLstSrvWnd);
	}
}

static gboolean lstlisthosts_btnpress(GtkWidget *widget, GdkEventButton *event, gpointer user_data) {
	if (event->type == GDK_2BUTTON_PRESS) {
		btnconnect_clicked(widget, user_data);
	}
	return FALSE;
}

static void btnconnect_clicked (GtkWidget *widget, gpointer user_data) {
	p_listServWnd pLstSrvWnd = (p_listServWnd)user_data;
	GtkWidget * msgdlg;
	
	if (pLstSrvWnd->selSrvr != (p_mysql_server)NULL) {
		create_wndServer(TRUE, pLstSrvWnd->selSrvr);
	} else {
		msgdlg = gtk_message_dialog_new(GTK_WINDOW(NULL), GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, _("Please select a database"));
		gtk_dialog_run (GTK_DIALOG (msgdlg));
		gtk_widget_destroy (msgdlg);
	}
}

static void btnnew_clicked (GtkWidget *widget, gpointer user_data) {
	p_listServWnd pLstSrvWnd = (p_listServWnd)user_data;
	
	pLstSrvWnd->selSrvr = (p_mysql_server)NULL;
	displayCurrentDB(pLstSrvWnd);
}

static void btnadd_clicked (GtkWidget *widget, gpointer user_data) {
	p_listServWnd pLstSrvWnd = (p_listServWnd)user_data;
	GString * infos[6];
	GtkWidget * msgdlg;
	int i, j, port;
	p_mysql_server newSrvr;
	
	/* Get data */
	infos[0] = g_string_new(gtk_entry_get_text(GTK_ENTRY(pLstSrvWnd->txtName)));
	infos[1] = g_string_new(gtk_entry_get_text(GTK_ENTRY(pLstSrvWnd->txtHost)));
	infos[2] = g_string_new(gtk_entry_get_text(GTK_ENTRY(pLstSrvWnd->txtPort)));
	infos[3] = g_string_new(gtk_entry_get_text(GTK_ENTRY(pLstSrvWnd->txtLogin)));
	infos[4] = g_string_new(gtk_entry_get_text(GTK_ENTRY(pLstSrvWnd->txtPasswd)));
	infos[5] = g_string_new(gtk_entry_get_text(GTK_ENTRY(pLstSrvWnd->txtLocalSock)));
	
	/* Check data */
	for (i = 0; i < 4; i++) {
		if (infos[i]->len == 0) { /* check if not empty */
			msgdlg = gtk_message_dialog_new(GTK_WINDOW(NULL), GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, _("Field %s cannot be empty !!!"), arFieldNames[i]);
			gtk_dialog_run (GTK_DIALOG (msgdlg));
			gtk_widget_destroy (msgdlg);
			return;		
		} else if (i == 2) { /* check if port is a number */
			for (j = 0; j < infos[i]->len; j++) {
				if (!g_ascii_isdigit(infos[i]->str[j])) {
					msgdlg = gtk_message_dialog_new(GTK_WINDOW(NULL), GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, _("Field %s can only contain digits !!!"), arFieldNames[i]);
					gtk_dialog_run (GTK_DIALOG (msgdlg));
					gtk_widget_destroy (msgdlg);
					return;		
				}
			}
		}
	}
	
	/* Check if name is unique */
	if (gmysql_config_get_server(pLstSrvWnd->gmsql_conf, infos[0]->str) != (p_mysql_server)NULL) {
		msgdlg = gtk_message_dialog_new(GTK_WINDOW(NULL), GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, _("Name already taken !\nPlease set another one."));
		gtk_dialog_run (GTK_DIALOG (msgdlg));
		gtk_widget_destroy (msgdlg);
		return;		
	}
	
	/* Insert data in configuration */
	port = (int)g_ascii_strtoull(infos[2]->str, (gchar **)NULL, 10);
	if (gmysql_config_add_server(pLstSrvWnd->gmsql_conf, infos[0]->str, infos[1]->str, port, infos[3]->str, infos[4]->str, (gchar *) NULL, infos[5]->str)) {
		newSrvr = gmysql_config_get_server(pLstSrvWnd->gmsql_conf, infos[0]->str);
		showListServer(pLstSrvWnd, newSrvr);
	}
	
	for (i = 0; i < 6; i++) {
		if (infos[i] != (GString *)NULL) {
			g_string_free(infos[i], TRUE);
		}
	}	
}

static void btnedit_clicked (GtkWidget *widget, gpointer user_data) {
	p_listServWnd pLstSrvWnd = (p_listServWnd)user_data;
	GString * infos[6];
	GtkWidget * msgdlg;
	int i, j, port;
	p_mysql_server newSrvr;
	
	if (pLstSrvWnd->selSrvr == (p_mysql_server)NULL) {
		msgdlg = gtk_message_dialog_new(GTK_WINDOW(NULL), GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, _("Please select a database"));
		gtk_dialog_run (GTK_DIALOG (msgdlg));
		gtk_widget_destroy (msgdlg);
		return;
	}
	
	/* Get data */
	infos[0] = g_string_new(gtk_entry_get_text(GTK_ENTRY(pLstSrvWnd->txtName)));
	infos[1] = g_string_new(gtk_entry_get_text(GTK_ENTRY(pLstSrvWnd->txtHost)));
	infos[2] = g_string_new(gtk_entry_get_text(GTK_ENTRY(pLstSrvWnd->txtPort)));
	infos[3] = g_string_new(gtk_entry_get_text(GTK_ENTRY(pLstSrvWnd->txtLogin)));
	infos[4] = g_string_new(gtk_entry_get_text(GTK_ENTRY(pLstSrvWnd->txtPasswd)));
	infos[5] = g_string_new(gtk_entry_get_text(GTK_ENTRY(pLstSrvWnd->txtLocalSock)));
	
	/* Check data */
	for (i = 0; i < 4; i++) {
		if (infos[i]->len == 0) { /* check if not empty */
			msgdlg = gtk_message_dialog_new(GTK_WINDOW(NULL), GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, _("Field %s cannot be empty !!!"), arFieldNames[i]);
			gtk_dialog_run (GTK_DIALOG (msgdlg));
			gtk_widget_destroy (msgdlg);
			return;		
		} else if (i == 2) { /* check if port is a number */
			for (j = 0; j < infos[i]->len; j++) {
				if (!g_ascii_isdigit(infos[i]->str[j])) {
					msgdlg = gtk_message_dialog_new(GTK_WINDOW(NULL), GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, _("Field %s can only contain digits !!!"), arFieldNames[i]);
					gtk_dialog_run (GTK_DIALOG (msgdlg));
					gtk_widget_destroy (msgdlg);
					return;		
				}
			}
		}
	}
	
	/* Check if name is unique */
	newSrvr = gmysql_config_get_server(pLstSrvWnd->gmsql_conf, infos[0]->str);
	if (newSrvr != (p_mysql_server)NULL && newSrvr != pLstSrvWnd->selSrvr) {
		msgdlg = gtk_message_dialog_new(GTK_WINDOW(NULL), GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, _("Name already taken !\nPlease set another one."));
		gtk_dialog_run (GTK_DIALOG (msgdlg));
		gtk_widget_destroy (msgdlg);
		return;		
	}
	
	/* Update data in configuration */
	port = (int)g_ascii_strtoull(infos[2]->str, (gchar **)NULL, 10);
	if (gmysql_config_update_server(pLstSrvWnd->gmsql_conf, pLstSrvWnd->selSrvr->name, infos[0]->str, infos[1]->str, port, infos[3]->str, infos[4]->str, (gchar *)NULL, infos[5]->str)) {
		showListServer(pLstSrvWnd, pLstSrvWnd->selSrvr);
	}
	
	for (i = 0; i < 6; i++) {
		if (infos[i] != (GString *)NULL) {
			g_string_free(infos[i], 1);
		}
	}	
}

static void btndel_clicked (GtkWidget *widget, gpointer user_data) {
	p_listServWnd pLstSrvWnd = (p_listServWnd)user_data;
	GtkWidget * msgdlg;
	
	if (pLstSrvWnd->selSrvr != (p_mysql_server)NULL) {
		if (!gmysql_config_del_server (pLstSrvWnd->gmsql_conf, pLstSrvWnd->selSrvr->name)) {
			msgdlg = gtk_message_dialog_new(GTK_WINDOW(NULL), GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, _("Can't delete this server !!!"));
			gtk_dialog_run (GTK_DIALOG (msgdlg));
			gtk_widget_destroy (msgdlg);
		} else {
			showListServer(pLstSrvWnd, (p_mysql_server)NULL);
		}
	} else {
		msgdlg = gtk_message_dialog_new(GTK_WINDOW(NULL), GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, _("Please select a database"));
		gtk_dialog_run (GTK_DIALOG (msgdlg));
		gtk_widget_destroy (msgdlg);
	}
}

static void btnserverup_clicked (GtkWidget *widget, gpointer user_data) {
	p_listServWnd pLstSrvWnd = (p_listServWnd)user_data;
	GtkTreeIter itCurr;
	GtkTreeModel *model;
	GtkTreeSelection* selection;
	p_mysql_server currSrvr;
	GList * lstCurr, * lstTrgt;
	GtkWidget * msgdlg;
	gboolean error = FALSE;
	
	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(pLstSrvWnd->lstListHosts));
	if (gtk_tree_selection_get_selected (selection, &model, &itCurr)) {
		gtk_tree_model_get (model, &itCurr, 1, &currSrvr, -1);
		lstCurr = g_list_find (pLstSrvWnd->gmsql_conf->lstServers, currSrvr);
		
		if (lstCurr != (GList *)NULL) {
			lstTrgt = lstCurr->prev;
			
			if (lstTrgt != (GList *)NULL) {
				pLstSrvWnd->gmsql_conf->lstServers = g_list_insert_before(pLstSrvWnd->gmsql_conf->lstServers, lstTrgt, currSrvr);
				pLstSrvWnd->gmsql_conf->lstServers = g_list_delete_link(pLstSrvWnd->gmsql_conf->lstServers, lstCurr);
				showListServer(pLstSrvWnd, currSrvr);
			} else {
				error = TRUE;
			}
		} else {
			error = TRUE;
		}
	} else {
		error = TRUE;
	}
	
	if (error) {
		msgdlg = gtk_message_dialog_new(GTK_WINDOW(NULL), GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, _("Can't move up more !!!"));
		gtk_dialog_run (GTK_DIALOG (msgdlg));
		gtk_widget_destroy (msgdlg);
	}
}

static void btnserverdown_clicked (GtkWidget *widget, gpointer user_data) {
	p_listServWnd pLstSrvWnd = (p_listServWnd)user_data;
	GtkTreeIter itCurr;
	GtkTreeModel *model;
	GtkTreeSelection* selection;
	p_mysql_server currSrvr;
	GList * lstCurr, * lstTrgt;
	GtkWidget * msgdlg;
	gboolean error = FALSE;
	
	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(pLstSrvWnd->lstListHosts));
	if (gtk_tree_selection_get_selected (selection, &model, &itCurr)) {
		gtk_tree_model_get (model, &itCurr, 1, &currSrvr, -1);
		lstCurr = g_list_find (pLstSrvWnd->gmsql_conf->lstServers, currSrvr);
		
		if (lstCurr != (GList *)NULL && lstCurr->next != (GList *)NULL) {
			lstTrgt = lstCurr->next->next;
			
			pLstSrvWnd->gmsql_conf->lstServers = g_list_insert_before(pLstSrvWnd->gmsql_conf->lstServers, lstTrgt, currSrvr);
			pLstSrvWnd->gmsql_conf->lstServers = g_list_delete_link(pLstSrvWnd->gmsql_conf->lstServers, lstCurr);
			showListServer(pLstSrvWnd, currSrvr);
		} else {
			error = TRUE;
		}
	} else {
		error = TRUE;
	}
	
	if (error) {
		msgdlg = gtk_message_dialog_new(GTK_WINDOW(NULL), GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, _("Can't move down more !!!"));
		gtk_dialog_run (GTK_DIALOG (msgdlg));
		gtk_widget_destroy (msgdlg);
	}
}

static void btnclose_clicked (GtkWidget *widget, gpointer user_data) {
	p_listServWnd pLstSrvWnd = (p_listServWnd)user_data;
	
	gtk_widget_destroy(GTK_WIDGET(pLstSrvWnd->wndListServer));
}

static void destroy(GtkWidget *widget, gpointer user_data) {
	p_listServWnd pLstSrvWnd = (p_listServWnd)user_data;
	
	/* Destroy GUI */
	gtk_widget_hide (GTK_WIDGET(pLstSrvWnd->wndListServer));
	gtk_widget_destroy (GTK_WIDGET(pLstSrvWnd->wndListServer));
	
	/* Destroy Datas */
	g_free(pLstSrvWnd);
	
	/* Destroy Application if needed */
	NbrWnd--;
	g_printerr("Destruction List Server window - nbrWnd : %d\n", NbrWnd);
	if (NbrWnd <= 0) {
		g_printerr("Destroy App\n");
		gtk_main_quit();
	}
}

listServWnd * create_wndListServer (gboolean display, p_gmysql_config gmsql_conf) {
  GtkWidget *frame1;
  GtkWidget *vbox4, *vbox5, *vbox6, *vbox7;
  GtkWidget *hbox5, *hbox6, *hbox7, *hbox8, *hbox12, *hbox13, *hbox14, *hbox15;
  GtkWidget *label9, *label10, *label11, *label12, *label13, *label14, *label17;
	GtkWidget *scrolledwindow3;
  GtkWidget *btnNew, *btnAdd, *btnEdit, *btnDel, *btnServerUp, *btnServerDown;
	
	GtkWidget *hpaned1;
  GtkWidget *toolbar1;
	GtkWidget * imgToolbar;
  GtkToolItem * btnTlbrApply, * btnTlbrClose;

	GtkTreeSelection *select;
	GtkTooltips * tooltips;
	p_listServWnd p_lstsvr;
	
	/* Init structure */
	p_lstsvr = (p_listServWnd) g_try_malloc(sizeof(listServWnd));
	
	if (p_lstsvr == (p_listServWnd)NULL) {
		return (p_listServWnd)NULL; /* return NULL pointer */
	}
	
	p_lstsvr->gmsql_conf = gmsql_conf;
	
	tooltips = gtk_tooltips_new();
	
  p_lstsvr->wndListServer = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title (GTK_WINDOW (p_lstsvr->wndListServer), _("Servers list"));
	g_signal_connect (G_OBJECT (p_lstsvr->wndListServer), "destroy", G_CALLBACK (destroy), p_lstsvr);
	gtk_window_set_default_size (GTK_WINDOW (p_lstsvr->wndListServer), 500, 300);

  vbox4 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox4);
  gtk_container_add (GTK_CONTAINER (p_lstsvr->wndListServer), vbox4);

  toolbar1 = gtk_toolbar_new ();
  gtk_widget_show (toolbar1);
  gtk_box_pack_start (GTK_BOX (vbox4), toolbar1, FALSE, FALSE, 0);
	gtk_toolbar_set_style (GTK_TOOLBAR (toolbar1), GTK_TOOLBAR_BOTH_HORIZ);

	imgToolbar = gtk_image_new_from_stock(GTK_STOCK_EXECUTE, GTK_ICON_SIZE_LARGE_TOOLBAR);
	gtk_widget_show(imgToolbar);
	btnTlbrApply = gtk_tool_button_new (imgToolbar, _("Connection"));
	gtk_tool_item_set_is_important (GTK_TOOL_ITEM(btnTlbrApply), TRUE);
	g_signal_connect (G_OBJECT (btnTlbrApply), "clicked", G_CALLBACK (btnconnect_clicked), p_lstsvr);
	gtk_widget_show(GTK_WIDGET(btnTlbrApply));
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar1), GTK_TOOL_ITEM(btnTlbrApply), -1);
	gtk_tool_item_set_tooltip (GTK_TOOL_ITEM(btnTlbrApply), tooltips, _("Connect to a server"), "glop");
	
	imgToolbar = gtk_image_new_from_stock(GTK_STOCK_CLOSE, GTK_ICON_SIZE_LARGE_TOOLBAR);
	gtk_widget_show(imgToolbar);
	btnTlbrClose = gtk_tool_button_new (imgToolbar, _("Close"));
	gtk_tool_item_set_is_important (GTK_TOOL_ITEM(btnTlbrClose), TRUE);
	g_signal_connect (G_OBJECT (btnTlbrClose), "clicked", G_CALLBACK (btnclose_clicked), p_lstsvr);
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

  p_lstsvr->lstListHosts = gtk_tree_view_new ();
  gtk_widget_show (p_lstsvr->lstListHosts);
  gtk_container_add (GTK_CONTAINER (scrolledwindow3), p_lstsvr->lstListHosts);
	g_signal_connect (G_OBJECT (p_lstsvr->lstListHosts), "button-press-event", G_CALLBACK (lstlisthosts_btnpress), (gpointer)p_lstsvr);
	select = gtk_tree_view_get_selection (GTK_TREE_VIEW ( p_lstsvr->lstListHosts));
	gtk_tree_selection_set_mode (select, GTK_SELECTION_SINGLE);
	g_signal_connect (G_OBJECT (select), "changed", G_CALLBACK (lstlisthosts_selected), (gpointer)p_lstsvr);

  hbox14 = gtk_hbox_new (TRUE, 0);
  gtk_widget_show (hbox14);
  gtk_box_pack_start (GTK_BOX (vbox7), hbox14, FALSE, TRUE, 0);

	btnServerUp = createIconButton("gtk-go-up", _("_Up"));
  gtk_box_pack_start (GTK_BOX (hbox14), btnServerUp, FALSE, FALSE, 0);
	g_signal_connect (G_OBJECT (btnServerUp), "clicked", G_CALLBACK (btnserverup_clicked), p_lstsvr);

	btnServerDown = createIconButton("gtk-go-down", _("_Down"));
  gtk_box_pack_start (GTK_BOX (hbox14), btnServerDown, FALSE, FALSE, 0);
	g_signal_connect (G_OBJECT (btnServerDown), "clicked", G_CALLBACK (btnserverdown_clicked), p_lstsvr);

  vbox6 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox6);
  gtk_paned_pack2 (GTK_PANED (hpaned1), vbox6, TRUE, TRUE);

  frame1 = gtk_frame_new (NULL);
  gtk_widget_show (frame1);
  gtk_box_pack_start (GTK_BOX (vbox6), frame1, FALSE, FALSE, 0);

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

  p_lstsvr->txtName = gtk_entry_new ();
  gtk_widget_show (p_lstsvr->txtName);
  gtk_box_pack_start (GTK_BOX (hbox6), p_lstsvr->txtName, TRUE, TRUE, 2);

  hbox7 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox7);
  gtk_box_pack_start (GTK_BOX (vbox5), hbox7, TRUE, TRUE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (hbox7), 2);

  label11 = gtk_label_new (_("Host :"));
  gtk_widget_show (label11);
  gtk_box_pack_start (GTK_BOX (hbox7), label11, FALSE, FALSE, 0);
  gtk_label_set_justify (GTK_LABEL (label11), GTK_JUSTIFY_LEFT);

  p_lstsvr->txtHost = gtk_entry_new ();
  gtk_widget_show (p_lstsvr->txtHost);
  gtk_box_pack_start (GTK_BOX (hbox7), p_lstsvr->txtHost, TRUE, TRUE, 2);

  hbox12 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox12);
  gtk_box_pack_start (GTK_BOX (vbox5), hbox12, TRUE, TRUE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (hbox12), 2);

  label12 = gtk_label_new (_("Port :"));
  gtk_widget_show (label12);
  gtk_box_pack_start (GTK_BOX (hbox12), label12, FALSE, FALSE, 0);
  gtk_label_set_justify (GTK_LABEL (label12), GTK_JUSTIFY_LEFT);

  p_lstsvr->txtPort = gtk_entry_new ();
  gtk_widget_show (p_lstsvr->txtPort);
  gtk_box_pack_start (GTK_BOX (hbox12), p_lstsvr->txtPort, TRUE, TRUE, 2);
  gtk_entry_set_max_length (GTK_ENTRY (p_lstsvr->txtPort), 5);

  hbox8 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox8);
  gtk_box_pack_start (GTK_BOX (vbox5), hbox8, TRUE, TRUE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (hbox8), 2);

  label13 = gtk_label_new (_("User name :"));
  gtk_widget_show (label13);
  gtk_box_pack_start (GTK_BOX (hbox8), label13, FALSE, FALSE, 0);
  gtk_label_set_justify (GTK_LABEL (label13), GTK_JUSTIFY_LEFT);

  p_lstsvr->txtLogin = gtk_entry_new ();
  gtk_widget_show (p_lstsvr->txtLogin);
  gtk_box_pack_start (GTK_BOX (hbox8), p_lstsvr->txtLogin, TRUE, TRUE, 2);

  hbox13 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox13);
  gtk_box_pack_start (GTK_BOX (vbox5), hbox13, TRUE, TRUE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (hbox13), 2);

  label14 = gtk_label_new (_("Password :"));
  gtk_widget_show (label14);
  gtk_box_pack_start (GTK_BOX (hbox13), label14, FALSE, FALSE, 0);
  gtk_label_set_justify (GTK_LABEL (label14), GTK_JUSTIFY_LEFT);

  p_lstsvr->txtPasswd = gtk_entry_new ();
  gtk_widget_show (p_lstsvr->txtPasswd);
  gtk_box_pack_start (GTK_BOX (hbox13), p_lstsvr->txtPasswd, TRUE, TRUE, 2);

  hbox15 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox15);
  gtk_box_pack_start (GTK_BOX (vbox5), hbox15, TRUE, TRUE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (hbox15), 2);

  label17 = gtk_label_new (_("Local socket file :"));
  gtk_widget_show (label17);
  gtk_box_pack_start (GTK_BOX (hbox15), label17, FALSE, FALSE, 0);
  gtk_label_set_justify (GTK_LABEL (label17), GTK_JUSTIFY_LEFT);

  p_lstsvr->txtLocalSock = gtk_entry_new ();
  gtk_widget_show (p_lstsvr->txtLocalSock);
  gtk_box_pack_start (GTK_BOX (hbox15), p_lstsvr->txtLocalSock, TRUE, TRUE, 2);

  hbox5 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox5);
  gtk_box_pack_start (GTK_BOX (vbox6), hbox5, FALSE, TRUE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (hbox5), 1);

	btnNew = createIconButton("gtk-new", _("_New"));
  gtk_box_pack_start (GTK_BOX (hbox5), btnNew, TRUE, TRUE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (btnNew), 3);
	g_signal_connect (G_OBJECT (btnNew), "clicked", G_CALLBACK (btnnew_clicked), p_lstsvr);

	btnAdd = createIconButton("gtk-add", _("_Add"));
  gtk_box_pack_start (GTK_BOX (hbox5), btnAdd, TRUE, TRUE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (btnAdd), 2);
	g_signal_connect (G_OBJECT (btnAdd), "clicked", G_CALLBACK (btnadd_clicked), p_lstsvr);

	btnEdit = createIconButton("gtk-apply", _("_Modify"));
  gtk_box_pack_start (GTK_BOX (hbox5), btnEdit, TRUE, TRUE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (btnEdit), 2);
	g_signal_connect (G_OBJECT (btnEdit), "clicked", G_CALLBACK (btnedit_clicked), p_lstsvr);

	btnDel = createIconButton("gtk-del", _("D_elete"));
  gtk_box_pack_start (GTK_BOX (hbox5), btnDel, TRUE, TRUE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (btnDel), 2);
	g_signal_connect (G_OBJECT (btnDel), "clicked", G_CALLBACK (btndel_clicked), p_lstsvr);

	showListServer(p_lstsvr, (p_mysql_server)NULL);
	
	if (display) {
		gtk_widget_show (p_lstsvr->wndListServer);
		NbrWnd ++;	
	}

  return p_lstsvr;
}
