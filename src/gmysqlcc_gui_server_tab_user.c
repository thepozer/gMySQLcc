
#include "gmysqlcc_gui_all.h"
#include "gmysqlcc_gui_server.h"

void gmysqlcc_gui_server_fill_user_list (p_gmysqlcc_gui_server gui_server, p_mysql_user mysql_usr);
void gmysqlcc_gui_server_fill_user_right_list (p_gmysqlcc_gui_server gui_server);
void gmysqlcc_gui_server_fill_user_right_database_list (p_gmysqlcc_gui_server gui_server, p_mysql_right mysql_rght);
void gmysqlcc_gui_server_fill_user_right_database_right_list (p_gmysqlcc_gui_server gui_server);
void gmysqlcc_gui_server_fill_user_right_table_list (p_gmysqlcc_gui_server gui_server, p_mysql_right mysql_rght);
void gmysqlcc_gui_server_fill_user_right_table_right_list (p_gmysqlcc_gui_server gui_server);

void gmysqlcc_gui_server_display_current_user (p_gmysqlcc_gui_server gui_server);
void gmysqlcc_gui_server_display_current_database_right (p_gmysqlcc_gui_server gui_server);
void gmysqlcc_gui_server_display_current_table_right (p_gmysqlcc_gui_server gui_server);

/* User events */
void gmysqlcc_gui_server_evt_lstUser_selected (GtkTreeSelection *selection, gpointer user_data);
void gmysqlcc_gui_server_evt_lstDRDatabases_selected (GtkTreeSelection *selection, gpointer user_data);
void gmysqlcc_gui_server_evt_btnUserNew_clicked (GtkWidget *widget, gpointer user_data);
void gmysqlcc_gui_server_evt_btnUserAdd_clicked (GtkWidget *widget, gpointer user_data);
void gmysqlcc_gui_server_evt_btnUserUpdate_clicked (GtkWidget *widget, gpointer user_data);
void gmysqlcc_gui_server_evt_btnUserDelete_clicked (GtkWidget *widget, gpointer user_data);
void gmysqlcc_gui_server_evt_btnDRNew_clicked (GtkWidget *widget, gpointer user_data);
void gmysqlcc_gui_server_evt_btnDRAdd_clicked (GtkWidget *widget, gpointer user_data);
void gmysqlcc_gui_server_evt_btnDRApply_clicked (GtkWidget *widget, gpointer user_data);
void gmysqlcc_gui_server_evt_btnDRDelete_clicked (GtkWidget *widget, gpointer user_data);
void gmysqlcc_gui_server_evt_lstURUserRights_edited (GtkCellRendererText *cellrenderertext, gchar *path_string, gchar *new_value, gpointer user_data);
void gmysqlcc_gui_server_evt_lstDRDatabaseRights_edited (GtkCellRendererText *cellrenderertext, gchar *path_string, gchar *new_value, gpointer user_data);
void gmysqlcc_gui_server_evt_btnMaxUpdate_clicked (GtkWidget *widget, gpointer user_data);
void gmysqlcc_gui_server_evt_btnSslUpdate_clicked (GtkWidget *widget, gpointer user_data);


void gmysqlcc_gui_server_create_widget_tab_user (p_gmysqlcc_gui_server gui_server, GtkWidget * notebook) {
	GtkWidget *scrolledwindow;
	GtkWidget *label;
	GtkWidget *table;
	GtkWidget *vbox1, *vbox2;
	GtkWidget *hbox, *hbtnbox;
	GtkWidget *hpaned;
	GtkWidget *notebookRights;
	GtkWidget *frame;
	
	GtkWidget *btnUserNew, *btnUserAdd, *btnUserUpdate, *btnUserDelete;
	GtkWidget *btnDRNew, *btnDRAdd, *btnDRApply, *btnDRDelete;
	GtkWidget *btnMaxUpdate, *btnSslUpdate;
	
	GtkTreeSelection *select;
	
	label = gtk_label_new (_("Users"));
	gtk_widget_show (label);
	hpaned = gtk_hpaned_new ();
	gtk_widget_show (hpaned);
	gtk_paned_set_position (GTK_PANED (hpaned), 200);
	gtk_notebook_append_page(GTK_NOTEBOOK(notebook), hpaned, label);
	
	
	scrolledwindow = gtk_scrolled_window_new (NULL, NULL);
	gtk_widget_show (scrolledwindow);
	gtk_paned_pack1 (GTK_PANED (hpaned), scrolledwindow, FALSE, TRUE);
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow), GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS);
	
	gui_server->lstUser = gtk_tree_view_new ();
	gtk_widget_show (gui_server->lstUser);
	gtk_container_add (GTK_CONTAINER (scrolledwindow), gui_server->lstUser);
	gtk_tree_view_set_rules_hint(GTK_TREE_VIEW (gui_server->lstUser), TRUE);
	select = gtk_tree_view_get_selection (GTK_TREE_VIEW (gui_server->lstUser));
	gtk_tree_selection_set_mode (select, GTK_SELECTION_SINGLE);
	g_signal_connect (G_OBJECT (select), "changed", 
										G_CALLBACK (gmysqlcc_gui_server_evt_lstUser_selected), (gpointer)gui_server);

	vbox1 = gtk_vbox_new (FALSE, 0);
	gtk_widget_show (vbox1);
	gtk_paned_pack2 (GTK_PANED (hpaned), vbox1, TRUE, TRUE);
	
	frame = gtk_frame_new (NULL);
	gtk_widget_show (frame);
	gtk_box_pack_start (GTK_BOX (vbox1), frame, FALSE, TRUE, 0);
	gtk_container_set_border_width (GTK_CONTAINER (frame), 2);
	gtk_frame_set_shadow_type (GTK_FRAME (frame), GTK_SHADOW_IN);
	
	label = gtk_label_new (_("User informations :"));
	gtk_widget_show (label);
	gtk_frame_set_label_widget (GTK_FRAME (frame), label);
	
	table = gtk_table_new (4, 2, FALSE);
	gtk_widget_show (table);
	gtk_container_add (GTK_CONTAINER (frame), table);
	gtk_container_set_border_width (GTK_CONTAINER (table), 2);
	gtk_table_set_row_spacings (GTK_TABLE (table), 2);
	gtk_table_set_col_spacings (GTK_TABLE (table), 2);
	
	label = gtk_label_new (_("Login :"));
	gtk_widget_show (label);
	gtk_table_attach (GTK_TABLE (table), label, 0, 1, 0, 1,
					(GtkAttachOptions) (GTK_FILL),
					(GtkAttachOptions) (0), 0, 0);
	
	label = gtk_label_new (_("Host :"));
	gtk_widget_show (label);
	gtk_table_attach (GTK_TABLE (table), label, 0, 1, 1, 2,
					(GtkAttachOptions) (GTK_FILL),
					(GtkAttachOptions) (0), 0, 0);
	
	label = gtk_label_new (_("Password :"));
	gtk_widget_show (label);
	gtk_table_attach (GTK_TABLE (table), label, 0, 1, 2, 3,
					(GtkAttachOptions) (GTK_FILL),
					(GtkAttachOptions) (0), 0, 0);
	
	gui_server->txtUserPasswd = gtk_entry_new ();
	gtk_widget_show (gui_server->txtUserPasswd);
	gtk_table_attach (GTK_TABLE (table), gui_server->txtUserPasswd, 1, 2, 2, 3,
					(GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
					(GtkAttachOptions) (0), 0, 0);
	
	gui_server->txtUserHost = gtk_entry_new ();
	gtk_widget_show (gui_server->txtUserHost);
	gtk_table_attach (GTK_TABLE (table), gui_server->txtUserHost, 1, 2, 1, 2,
					(GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
					(GtkAttachOptions) (0), 0, 0);
	
	gui_server->txtUserLogin = gtk_entry_new ();
	gtk_widget_show (gui_server->txtUserLogin);
	gtk_table_attach (GTK_TABLE (table), gui_server->txtUserLogin, 1, 2, 0, 1,
					(GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
					(GtkAttachOptions) (0), 0, 0);
	
	gui_server->chkEncryptedPasswd = gtk_check_button_new_with_mnemonic (_("Already encrypted password"));
	gtk_widget_show (gui_server->chkEncryptedPasswd);
	gtk_table_attach (GTK_TABLE (table), gui_server->chkEncryptedPasswd, 1, 2, 3, 4,
					(GtkAttachOptions) (GTK_FILL),
					(GtkAttachOptions) (0), 0, 0);
	
	hbtnbox = gtk_hbutton_box_new ();
	gtk_widget_show (hbtnbox);
	gtk_box_pack_start (GTK_BOX (vbox1), hbtnbox, FALSE, TRUE, 0);
	gtk_button_box_set_layout (GTK_BUTTON_BOX (hbtnbox), GTK_BUTTONBOX_SPREAD);
	gtk_box_set_spacing (GTK_BOX (hbtnbox), 2);
	
	btnUserNew = gtk_button_new_from_stock ("gtk-new");
	gtk_widget_show (btnUserNew);
	gtk_container_add (GTK_CONTAINER (hbtnbox), btnUserNew);
	g_signal_connect (G_OBJECT (btnUserNew), "clicked", 
										G_CALLBACK (gmysqlcc_gui_server_evt_btnUserNew_clicked), (gpointer)gui_server);
	
	btnUserAdd = gtk_button_new_from_stock ("gtk-add");
	gtk_widget_show (btnUserAdd);
	gtk_container_add (GTK_CONTAINER (hbtnbox), btnUserAdd);
	g_signal_connect (G_OBJECT (btnUserAdd), "clicked", 
										G_CALLBACK (gmysqlcc_gui_server_evt_btnUserAdd_clicked), (gpointer)gui_server);
	
	btnUserUpdate = gtk_button_new_from_stock ("gtk-apply");
	gtk_widget_show (btnUserUpdate);
	gtk_container_add (GTK_CONTAINER (hbtnbox), btnUserUpdate);
	g_signal_connect (G_OBJECT (btnUserUpdate), "clicked", 
										G_CALLBACK (gmysqlcc_gui_server_evt_btnUserUpdate_clicked), (gpointer)gui_server);
	
	btnUserDelete = gtk_button_new_from_stock ("gtk-delete");
	gtk_widget_show (btnUserDelete);
	gtk_container_add (GTK_CONTAINER (hbtnbox), btnUserDelete);
	g_signal_connect (G_OBJECT (btnUserDelete), "clicked", 
										G_CALLBACK (gmysqlcc_gui_server_evt_btnUserDelete_clicked), (gpointer)gui_server);
	
	
	
	
	frame = gtk_frame_new (NULL);
	gtk_widget_show (frame);
	gtk_box_pack_start (GTK_BOX (vbox1), frame, TRUE, TRUE, 0);
	gtk_frame_set_shadow_type (GTK_FRAME (frame), GTK_SHADOW_IN);
	
	label = gtk_label_new (_("Rights :"));
	gtk_widget_show (label);
	gtk_frame_set_label_widget (GTK_FRAME (frame), label);
	
	notebookRights = gtk_notebook_new ();
	gtk_widget_show (notebookRights);
	gtk_container_add (GTK_CONTAINER (frame), notebookRights);
	
	
	
	label = gtk_label_new (_("User"));
	gtk_widget_show (label);
	vbox2 = gtk_vbox_new (FALSE, 0);
	gtk_widget_show (vbox2);
	gtk_notebook_append_page(GTK_NOTEBOOK(notebookRights), vbox2, label);
	
	hbox = gtk_hbox_new (FALSE, 0);
	gtk_widget_show (hbox);
	gtk_box_pack_start (GTK_BOX (vbox2), hbox, FALSE, TRUE, 3);
	
	label = gtk_label_new (_("Max"));
	gtk_widget_show (label);
	gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, TRUE, 3);
	
	label = gtk_label_new (_("Queries :"));
	gtk_widget_show (label);
	gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, TRUE, 3);
	
	gui_server->txtMaxQuestions = gtk_entry_new ();
	gtk_widget_show (gui_server->txtMaxQuestions);
	gtk_box_pack_start (GTK_BOX (hbox), gui_server->txtMaxQuestions, FALSE, TRUE, 3);
	gtk_entry_set_max_length(GTK_ENTRY(gui_server->txtMaxQuestions), 11);
	gtk_entry_set_width_chars(GTK_ENTRY(gui_server->txtMaxQuestions), 4);
	
	label = gtk_label_new (_("Updates :"));
	gtk_widget_show (label);
	gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, TRUE, 3);
	
	gui_server->txtMaxUpdates = gtk_entry_new ();
	gtk_widget_show (gui_server->txtMaxUpdates);
	gtk_box_pack_start (GTK_BOX (hbox), gui_server->txtMaxUpdates, FALSE, TRUE, 3);
	gtk_entry_set_max_length(GTK_ENTRY(gui_server->txtMaxUpdates), 11);
	gtk_entry_set_width_chars(GTK_ENTRY(gui_server->txtMaxUpdates), 4);
	
	label = gtk_label_new (_("Connections :"));
	gtk_widget_show (label);
	gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, TRUE, 3);
	
	gui_server->txtMaxConnections = gtk_entry_new ();
	gtk_widget_show (gui_server->txtMaxConnections);
	gtk_box_pack_start (GTK_BOX (hbox), gui_server->txtMaxConnections, FALSE, TRUE, 3);
	gtk_entry_set_max_length(GTK_ENTRY(gui_server->txtMaxConnections), 11);
	gtk_entry_set_width_chars(GTK_ENTRY(gui_server->txtMaxConnections), 4);
	
	btnMaxUpdate = gtk_button_new_from_stock ("gtk-apply");
	gtk_widget_show (btnMaxUpdate);
	gtk_box_pack_start (GTK_BOX (hbox), btnMaxUpdate, FALSE, TRUE, 3);
	g_signal_connect (G_OBJECT (btnMaxUpdate), "clicked", 
										G_CALLBACK (gmysqlcc_gui_server_evt_btnMaxUpdate_clicked), (gpointer)gui_server);
	
	scrolledwindow = gtk_scrolled_window_new (NULL, NULL);
	gtk_widget_show (scrolledwindow);
	gtk_box_pack_start (GTK_BOX (vbox2), scrolledwindow, TRUE, TRUE, 3);
	gtk_container_set_border_width (GTK_CONTAINER (scrolledwindow), 3);
	
	gui_server->lstURUserRights = gtk_tree_view_new ();
	gtk_widget_show (gui_server->lstURUserRights);
	gtk_tree_view_set_rules_hint(GTK_TREE_VIEW (gui_server->lstURUserRights), TRUE);
	gtk_container_add (GTK_CONTAINER (scrolledwindow), gui_server->lstURUserRights);
	
	
	label = gtk_label_new (_("Ssl"));
	gtk_widget_show (label);
	vbox2 = gtk_vbox_new (FALSE, 0);
	gtk_widget_show (vbox2);
	gtk_notebook_append_page(GTK_NOTEBOOK(notebookRights), vbox2, label);
	
	hbox = gtk_hbox_new (FALSE, 0);
	gtk_widget_show (hbox);
	gtk_box_pack_start (GTK_BOX (vbox2), hbox, FALSE, TRUE, 3);
	
	label = gtk_label_new (_("SSL type :"));
	gtk_widget_show (label);
	gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, TRUE, 3);
	
	gui_server->cbx_ssl_type = gtk_combo_box_new_text();
	gtk_widget_show (gui_server->cbx_ssl_type);
	gtk_box_pack_start (GTK_BOX (hbox), gui_server->cbx_ssl_type, FALSE, TRUE, 3);
	
	hbox = gtk_hbox_new (FALSE, 0);
	gtk_widget_show (hbox);
	gtk_box_pack_start (GTK_BOX (vbox2), hbox, TRUE, TRUE, 3);
	
	label = gtk_label_new (_("SSL cypher :"));
	gtk_widget_show (label);
	gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, TRUE, 3);
	
	scrolledwindow = gtk_scrolled_window_new (NULL, NULL);
	gtk_widget_show (scrolledwindow);
	gtk_box_pack_start (GTK_BOX (hbox), scrolledwindow, TRUE, TRUE, 3);
	
	gui_server->txt_ssl_cypher = gtk_text_view_new ();
	gtk_widget_show (gui_server->txt_ssl_cypher);
	gtk_container_add (GTK_CONTAINER (scrolledwindow), gui_server->txt_ssl_cypher);
	
	hbox = gtk_hbox_new (FALSE, 0);
	gtk_widget_show (hbox);
	gtk_box_pack_start (GTK_BOX (vbox2), hbox, TRUE, TRUE, 3);
	
	label = gtk_label_new (_("x509 issuer :"));
	gtk_widget_show (label);
	gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, TRUE, 3);
	
	scrolledwindow = gtk_scrolled_window_new (NULL, NULL);
	gtk_widget_show (scrolledwindow);
	gtk_box_pack_start (GTK_BOX (hbox), scrolledwindow, TRUE, TRUE, 3);
	
	gui_server->txt_x509_issuer = gtk_text_view_new ();
	gtk_widget_show (gui_server->txt_x509_issuer);
	gtk_container_add (GTK_CONTAINER (scrolledwindow), gui_server->txt_x509_issuer);
	
	hbox = gtk_hbox_new (FALSE, 0);
	gtk_widget_show (hbox);
	gtk_box_pack_start (GTK_BOX (vbox2), hbox, TRUE, TRUE, 3);
	
	label = gtk_label_new (_("x509 subject :"));
	gtk_widget_show (label);
	gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, TRUE, 3);
	
	scrolledwindow = gtk_scrolled_window_new (NULL, NULL);
	gtk_widget_show (scrolledwindow);
	gtk_box_pack_start (GTK_BOX (hbox), scrolledwindow, TRUE, TRUE, 3);
	
	gui_server->txt_x509_subject = gtk_text_view_new ();
	gtk_widget_show (gui_server->txt_x509_subject);
	gtk_container_add (GTK_CONTAINER (scrolledwindow), gui_server->txt_x509_subject);
	
	btnSslUpdate = gtk_button_new_from_stock ("gtk-apply");
	gtk_widget_show (btnSslUpdate);
	gtk_box_pack_start (GTK_BOX (vbox2), btnSslUpdate, FALSE, TRUE, 3);
	g_signal_connect (G_OBJECT (btnSslUpdate), "clicked", 
										G_CALLBACK (gmysqlcc_gui_server_evt_btnSslUpdate_clicked), (gpointer)gui_server);
	
	
	
	label = gtk_label_new (_("Database"));
	gtk_widget_show (label);
	hpaned = gtk_hpaned_new ();
	gtk_widget_show (hpaned);
	gtk_paned_set_position (GTK_PANED (hpaned), 200);
	gtk_notebook_append_page(GTK_NOTEBOOK(notebookRights), hpaned, label);
	
	scrolledwindow = gtk_scrolled_window_new (NULL, NULL);
	gtk_widget_show (scrolledwindow);
	gtk_paned_pack1 (GTK_PANED (hpaned), scrolledwindow, FALSE, TRUE);
	
	gui_server->lstDRDatabases = gtk_tree_view_new ();
	gtk_widget_show (gui_server->lstDRDatabases);
	gtk_container_add (GTK_CONTAINER (scrolledwindow), gui_server->lstDRDatabases);
	select = gtk_tree_view_get_selection (GTK_TREE_VIEW (gui_server->lstDRDatabases));
	gtk_tree_selection_set_mode (select, GTK_SELECTION_SINGLE);
	g_signal_connect (G_OBJECT (select), "changed", 
										G_CALLBACK (gmysqlcc_gui_server_evt_lstDRDatabases_selected), (gpointer)gui_server);
	
	vbox2 = gtk_vbox_new (FALSE, 3);
	gtk_widget_show (vbox2);
	gtk_paned_pack2 (GTK_PANED (hpaned), vbox2, TRUE, TRUE);
	
	hbox = gtk_hbox_new (FALSE, 2);
	gtk_widget_show (hbox);
	gtk_box_pack_start (GTK_BOX (vbox2), hbox, FALSE, TRUE, 3);
	
	label = gtk_label_new (_("Database name :"));
	gtk_widget_show (label);
	gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, FALSE, 0);
	
	gui_server->txtDRDbName = gtk_entry_new ();
	gtk_widget_show (gui_server->txtDRDbName);
	gtk_box_pack_start (GTK_BOX (hbox), gui_server->txtDRDbName, TRUE, TRUE, 0);
	
	hbtnbox = gtk_hbutton_box_new ();
	gtk_widget_show (hbtnbox);
	gtk_box_pack_start (GTK_BOX (vbox2), hbtnbox, FALSE, TRUE, 3);
	gtk_button_box_set_layout (GTK_BUTTON_BOX (hbtnbox), GTK_BUTTONBOX_SPREAD);
	
	btnDRNew = gtk_button_new_from_stock ("gtk-new");
	gtk_widget_show (btnDRNew);
	gtk_container_add (GTK_CONTAINER (hbtnbox), btnDRNew);
	g_signal_connect (G_OBJECT (btnDRNew), "clicked", 
										G_CALLBACK (gmysqlcc_gui_server_evt_btnDRNew_clicked), (gpointer)gui_server);
	
	btnDRAdd = gtk_button_new_from_stock ("gtk-add");
	gtk_widget_show (btnDRAdd);
	gtk_container_add (GTK_CONTAINER (hbtnbox), btnDRAdd);
	g_signal_connect (G_OBJECT (btnDRAdd), "clicked", 
										G_CALLBACK (gmysqlcc_gui_server_evt_btnDRAdd_clicked), (gpointer)gui_server);
	
	btnDRApply = gtk_button_new_from_stock ("gtk-apply");
	gtk_widget_show (btnDRApply);
	gtk_container_add (GTK_CONTAINER (hbtnbox), btnDRApply);
	g_signal_connect (G_OBJECT (btnDRApply), "clicked", 
										G_CALLBACK (gmysqlcc_gui_server_evt_btnDRApply_clicked), (gpointer)gui_server);
	
	btnDRDelete = gtk_button_new_from_stock ("gtk-delete");
	gtk_widget_show (btnDRDelete);
	gtk_container_add (GTK_CONTAINER (hbtnbox), btnDRDelete);
	g_signal_connect (G_OBJECT (btnDRDelete), "clicked", 
										G_CALLBACK (gmysqlcc_gui_server_evt_btnDRDelete_clicked), (gpointer)gui_server);
	
	frame = gtk_frame_new (NULL);
	gtk_widget_show (frame);
	gtk_box_pack_start (GTK_BOX (vbox2), frame, TRUE, TRUE, 0);
	gtk_frame_set_shadow_type (GTK_FRAME (frame), GTK_SHADOW_IN);
	
	label = gtk_label_new (_("Database rights :"));
	gtk_widget_show (label);
	gtk_frame_set_label_widget (GTK_FRAME (frame), label);
	
	scrolledwindow = gtk_scrolled_window_new (NULL, NULL);
	gtk_widget_show (scrolledwindow);
	gtk_container_add (GTK_CONTAINER (frame), scrolledwindow);
	
	gui_server->lstDRDatabaseRights = gtk_tree_view_new ();
	gtk_widget_show (gui_server->lstDRDatabaseRights);
	gtk_container_add (GTK_CONTAINER (scrolledwindow), gui_server->lstDRDatabaseRights);
	
}

void gmysqlcc_gui_server_init_widget_tab_user (p_gmysqlcc_gui_server gui_server) {
	GtkTreeViewColumn * currCol;
	GtkCellRenderer * renderer, * renderer_edit;
	GtkListStore * lstEmpty;
	GValue gvalbool = {0, };
	
	g_value_init(&gvalbool, G_TYPE_BOOLEAN);
	g_value_set_boolean(&gvalbool, TRUE);
	
	/* Generate columns for all tree views */
	renderer = gtk_cell_renderer_text_new ();
	
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
	g_signal_connect (G_OBJECT (renderer_edit), "edited", G_CALLBACK (gmysqlcc_gui_server_evt_lstDRDatabaseRights_edited), gui_server);
	currCol = gtk_tree_view_column_new_with_attributes (_("Value"), renderer_edit, "text", 1, NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW (gui_server->lstDRDatabaseRights), currCol);
	
	lstEmpty = gtk_list_store_new(2, G_TYPE_STRING, G_TYPE_STRING);
	gtk_tree_view_set_model(GTK_TREE_VIEW(gui_server->lstDRDatabaseRights), GTK_TREE_MODEL(lstEmpty));
	g_object_unref (G_OBJECT (lstEmpty));
	
	gmysqlcc_gui_server_fill_user_list(gui_server, NULL);
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
	gmysqlcc_gui_server_fill_user_right_database_list(gui_server, NULL);
}

void gmysqlcc_gui_server_fill_user_right_list (p_gmysqlcc_gui_server gui_server) {
	GtkListStore * lstStrUserRights;
	s_bool_right_item * p_userBoolRights;
	GtkTreeIter iter;
	gchar * value;
	
	gboolean sub_ht_key_equal(gpointer key, gpointer value, gpointer user_data) {
		return (strcmp(key, user_data) == 0);
	}
	
	void set_field_value(GtkWidget * entry, gchar * field) {
		gchar * value;
		
		value = g_hash_table_find(gui_server->curr_mysql_usr->user_rights->hsh_rights, &sub_ht_key_equal, field);
		if(GTK_IS_ENTRY(entry)) {
			value = (value == NULL) ? "0" : value ;
			gtk_entry_set_text(GTK_ENTRY(entry), value);
		} else if (GTK_IS_TEXT_VIEW(entry)) {
			GtkTextBuffer * txtBuffer;
			
			value = (value == NULL) ? "" : value ;
			txtBuffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(entry));
			gtk_text_buffer_set_text(GTK_TEXT_BUFFER(txtBuffer), value, -1);
		} else if (GTK_IS_COMBO_BOX(entry)) {
			gint selIdx = -1;
			
			if (value != NULL) {
				if (strcmp(value, "ANY") == 0) {
					selIdx = 1;
				} else if (strcmp(value, "X509") == 0) {
					selIdx = 2;
				} else if (strcmp(value, "SPECIFIED") == 0) {
					selIdx = 3;
				} else {
					selIdx = 0;
				}
			}
			
			gtk_combo_box_set_active(GTK_COMBO_BOX(entry), selIdx);
		}
	}
	
	if (gui_server->curr_mysql_usr == NULL) {
		return;
	}
	
	lstStrUserRights = gtk_list_store_new(2, G_TYPE_STRING, G_TYPE_STRING);
	gtk_tree_sortable_set_sort_column_id (GTK_TREE_SORTABLE(lstStrUserRights), 0, GTK_SORT_ASCENDING);
	
	p_userBoolRights = ar_bri_user;
	while (p_userBoolRights->set_name != NULL) {
		
		value = g_hash_table_find(gui_server->curr_mysql_usr->user_rights->hsh_rights, &sub_ht_key_equal, p_userBoolRights->set_name);
		
		if (value != NULL) {
			gtk_list_store_append (lstStrUserRights, &iter);
			gtk_list_store_set (lstStrUserRights, &iter, 0, p_userBoolRights->set_name, 1, value, -1);
		}
		
		p_userBoolRights++;
	}
	
	gtk_tree_view_set_model(GTK_TREE_VIEW(gui_server->lstURUserRights), GTK_TREE_MODEL(lstStrUserRights));
	g_object_unref (G_OBJECT (lstStrUserRights));
	
	set_field_value(gui_server->txtMaxQuestions, "max_questions");
	set_field_value(gui_server->txtMaxUpdates, "max_updates");
	set_field_value(gui_server->txtMaxConnections, "max_connections");
	
	set_field_value(gui_server->cbx_ssl_type, "ssl_type");
	
	set_field_value(gui_server->txt_ssl_cypher, "ssl_cipher");
	set_field_value(gui_server->txt_x509_issuer, "x509_issuer");
	set_field_value(gui_server->txt_x509_subject, "x509_subject");
}

void gmysqlcc_gui_server_fill_user_right_database_list (p_gmysqlcc_gui_server gui_server, p_mysql_right mysql_rght) {
	GtkListStore * lstStrDatabaseRights;
	
	void sub_ht_fill_user_right_database_list(gpointer key, gpointer value, gpointer user_data) {
		GtkListStore * lstStrDatabaseRights = (GtkListStore *)user_data;
		GtkTreeIter iter;
		
		gtk_list_store_append (lstStrDatabaseRights, &iter);
		gtk_list_store_set (lstStrDatabaseRights, &iter, 0, (gchar *)key, 1, value, -1);
	}
	
	lstStrDatabaseRights = gtk_list_store_new(2, G_TYPE_STRING, G_TYPE_POINTER);
	gtk_tree_sortable_set_sort_column_id (GTK_TREE_SORTABLE(lstStrDatabaseRights), 0, GTK_SORT_ASCENDING);
	
	if (gui_server->curr_mysql_usr != NULL) {
		mysql_user_read_database_rights(gui_server->curr_mysql_usr);
		g_hash_table_foreach(gui_server->curr_mysql_usr->hsh_db_list_rights, &sub_ht_fill_user_right_database_list, lstStrDatabaseRights);
	}
	
	gtk_tree_view_set_model(GTK_TREE_VIEW(gui_server->lstDRDatabases), GTK_TREE_MODEL(lstStrDatabaseRights));
	g_object_unref (G_OBJECT (lstStrDatabaseRights));
	
	gui_server->curr_database_rights = mysql_rght;
	gmysqlcc_gui_server_display_current_database_right(gui_server);
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
	
	void sub_ht_fill_user_right_database_right_list(gpointer key, gpointer value, gpointer user_data) {
		GtkListStore * lstStrRights = (GtkListStore *)user_data;
		GtkTreeIter iter;
		
		gtk_list_store_append (lstStrRights, &iter);
		gtk_list_store_set (lstStrRights, &iter, 0, (gchar *)key, 1, (gchar *)value, -1);
	}
	
	lstStrRights = gtk_list_store_new(2, G_TYPE_STRING, G_TYPE_STRING);
	gtk_tree_sortable_set_sort_column_id (GTK_TREE_SORTABLE(lstStrRights), 0, GTK_SORT_ASCENDING);
	
	if (gui_server->curr_database_rights != NULL) {
		g_hash_table_foreach(gui_server->curr_database_rights->hsh_rights, &sub_ht_fill_user_right_database_right_list, (gpointer)lstStrRights);
	}
	
	gtk_tree_view_set_model(GTK_TREE_VIEW(gui_server->lstDRDatabaseRights), GTK_TREE_MODEL(lstStrRights));
	g_object_unref (G_OBJECT (lstStrRights));
	
}

void gmysqlcc_gui_server_evt_lstUser_selected (GtkTreeSelection *selection, gpointer user_data) {
	p_gmysqlcc_gui_server gui_server = (p_gmysqlcc_gui_server)user_data;
	GtkTreeIter iter;
	GtkTreeModel *model;
	
	/*g_print("Changed !\n");*/
	if (gtk_tree_selection_get_selected (selection, &model, &iter)) {
		gtk_tree_model_get (model, &iter, 1, &gui_server->curr_mysql_usr, -1);
		/*g_print ("The User is '%s'@'%s'\n", gui_server->curr_mysql_usr->login, gui_server->curr_mysql_usr->host);*/
		
		gmysqlcc_gui_server_display_current_user(gui_server);
	}
}

void gmysqlcc_gui_server_evt_lstDRDatabases_selected (GtkTreeSelection *selection, gpointer user_data) {
	p_gmysqlcc_gui_server gui_server = (p_gmysqlcc_gui_server)user_data;
	GtkTreeIter iter;
	GtkTreeModel *model;
	
	/*g_print("Changed !\n");*/
	if (gtk_tree_selection_get_selected (selection, &model, &iter)) {
		gtk_tree_model_get (model, &iter, 1, &gui_server->curr_database_rights, -1);
		/*g_print ("The User is '%s'@'%s'\n", gui_server->curr_database_rights->login, gui_server->curr_database_rights->host);*/
		
		gmysqlcc_gui_server_display_current_database_right(gui_server);
	}
}

void gmysqlcc_gui_server_evt_btnUserNew_clicked (GtkWidget *widget, gpointer user_data) {
	p_gmysqlcc_gui_server gui_server = (p_gmysqlcc_gui_server)user_data;
	GtkTreeSelection *selection;
	
	gui_server->curr_mysql_usr = NULL;
	
	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(gui_server->lstUser));
	gtk_tree_selection_unselect_all(selection);
	
	gmysqlcc_gui_server_display_current_user(gui_server);
}

void gmysqlcc_gui_server_evt_btnUserAdd_clicked (GtkWidget *widget, gpointer user_data) {
	p_data_list mysql_usr_lst;
	p_gmysqlcc_gui_server gui_server = (p_gmysqlcc_gui_server)user_data;
	p_mysql_user mysql_usr;
	const gchar * host, * login, * passwd;
	gboolean crypted_password;
	GString * str_key;
	GtkWidget * msgdlg;
	
	host = gtk_entry_get_text(GTK_ENTRY(gui_server->txtUserHost));
	login = gtk_entry_get_text(GTK_ENTRY(gui_server->txtUserLogin));
	passwd = gtk_entry_get_text(GTK_ENTRY(gui_server->txtUserPasswd));
	crypted_password = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(gui_server->chkEncryptedPasswd));
	
	if (host != NULL && strlen(host) < 1) {
		msgdlg = gtk_message_dialog_new(GTK_WINDOW(gui_server->window), GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, _("Field Host cannot be empty !!!"));
		gtk_dialog_run (GTK_DIALOG (msgdlg));
		gtk_widget_destroy (msgdlg);
		return;
	}
	
	mysql_usr_lst = mysql_server_get_user_list(gui_server->mysql_srv);
	
	str_key = g_string_new("");
	g_string_printf(str_key, "'%s'@'%s'", login, host);
	
	if (mysql_data_list_is_exist(mysql_usr_lst, str_key->str)) {
		msgdlg = gtk_message_dialog_new(GTK_WINDOW(gui_server->window), GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, _("User already exist !!!"));
		gtk_dialog_run (GTK_DIALOG (msgdlg));
		gtk_widget_destroy (msgdlg);
		return;
	}
	
	mysql_usr = mysql_user_create(gui_server->mysql_srv, login, host, passwd, crypted_password);
	
	if (mysql_usr == NULL) {
		msgdlg = gtk_message_dialog_new(GTK_WINDOW(gui_server->window), GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, _("Can't create User !!!"));
		gtk_dialog_run (GTK_DIALOG (msgdlg));
		gtk_widget_destroy (msgdlg);
		return;
	}
	
	g_string_free(str_key, TRUE);
	gmysqlcc_gui_server_fill_user_list(gui_server, mysql_usr);
	
}

void gmysqlcc_gui_server_evt_btnUserUpdate_clicked (GtkWidget *widget, gpointer user_data) {
	p_gmysqlcc_gui_server gui_server = (p_gmysqlcc_gui_server)user_data;
	const gchar * host, * login, * passwd;
	gboolean crypted_password;
	gboolean ret_updt;
	GtkWidget * msgdlg;
	
	host = gtk_entry_get_text(GTK_ENTRY(gui_server->txtUserHost));
	login = gtk_entry_get_text(GTK_ENTRY(gui_server->txtUserLogin));
	passwd = gtk_entry_get_text(GTK_ENTRY(gui_server->txtUserPasswd));
	crypted_password = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(gui_server->chkEncryptedPasswd));
	
	if (gui_server->curr_mysql_usr == NULL) {
		msgdlg = gtk_message_dialog_new(GTK_WINDOW(gui_server->window), GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, _("Please, select a user !!!"));
		gtk_dialog_run (GTK_DIALOG (msgdlg));
		gtk_widget_destroy (msgdlg);
		return;
	}
	
	ret_updt = mysql_user_update_key_values(gui_server->curr_mysql_usr, login, host);
	if (!ret_updt) {
		msgdlg = gtk_message_dialog_new(GTK_WINDOW(gui_server->window), GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, _("Can't update user Login and Host !!!"));
		gtk_dialog_run (GTK_DIALOG (msgdlg));
		gtk_widget_destroy (msgdlg);
		return;
	}

	ret_updt = mysql_user_set_password(gui_server->curr_mysql_usr, passwd, crypted_password);
	if (!ret_updt) {
		msgdlg = gtk_message_dialog_new(GTK_WINDOW(gui_server->window), GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, _("Can't update user Password !!!"));
		gtk_dialog_run (GTK_DIALOG (msgdlg));
		gtk_widget_destroy (msgdlg);
		return;
	}
	
	gmysqlcc_gui_server_fill_user_list(gui_server, NULL);
}

void gmysqlcc_gui_server_evt_btnUserDelete_clicked (GtkWidget *widget, gpointer user_data) {
	p_gmysqlcc_gui_server gui_server = (p_gmysqlcc_gui_server)user_data;
	gboolean ret_del;
	GtkWidget * msgdlg;
	
	if (gui_server->curr_mysql_usr == NULL) {
		msgdlg = gtk_message_dialog_new(GTK_WINDOW(gui_server->window), GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, _("Please, select a user !!!"));
		gtk_dialog_run (GTK_DIALOG (msgdlg));
		gtk_widget_destroy (msgdlg);
		return;
	}
	
	ret_del = mysql_user_remove(gui_server->curr_mysql_usr);
	if (!ret_del) {
		msgdlg = gtk_message_dialog_new(GTK_WINDOW(gui_server->window), GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, _("Can't remove user !!!"));
		gtk_dialog_run (GTK_DIALOG (msgdlg));
		gtk_widget_destroy (msgdlg);
		return;
	}
	
	gmysqlcc_gui_server_fill_user_list(gui_server, NULL);
}

void gmysqlcc_gui_server_evt_btnDRNew_clicked (GtkWidget *widget, gpointer user_data) {
	p_gmysqlcc_gui_server gui_server = (p_gmysqlcc_gui_server)user_data;
	GtkTreeSelection *selection;
	
	gui_server->curr_database_rights = NULL;
	
	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(gui_server->lstDRDatabases));
	gtk_tree_selection_unselect_all(selection);
	
	gmysqlcc_gui_server_display_current_database_right(gui_server);
}

void gmysqlcc_gui_server_evt_btnDRAdd_clicked (GtkWidget *widget, gpointer user_data) {
	p_gmysqlcc_gui_server gui_server = (p_gmysqlcc_gui_server)user_data;
	p_mysql_right mysql_rght;
	const gchar * db;
	GtkWidget * msgdlg;
	
	db = gtk_entry_get_text(GTK_ENTRY(gui_server->txtDRDbName));
	
	if (db != NULL && strlen(db) < 1) {
		msgdlg = gtk_message_dialog_new(GTK_WINDOW(gui_server->window), GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, _("Field Database cannot be empty !!!"));
		gtk_dialog_run (GTK_DIALOG (msgdlg));
		gtk_widget_destroy (msgdlg);
		return;
	}
	
	mysql_rght = mysql_right_new_database(gui_server->mysql_srv, gui_server->curr_mysql_usr->host, gui_server->curr_mysql_usr->login, db);
	
	if (mysql_rght != NULL) {
		mysql_right_delete(mysql_rght);
		msgdlg = gtk_message_dialog_new(GTK_WINDOW(gui_server->window), GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, _("Database right already exist !!!"));
		gtk_dialog_run (GTK_DIALOG (msgdlg));
		gtk_widget_destroy (msgdlg);
		return;
	}
	
	mysql_rght = mysql_right_new_database_create(gui_server->mysql_srv, gui_server->curr_mysql_usr->host, gui_server->curr_mysql_usr->login, db);
	
	if (mysql_rght == NULL) {
		msgdlg = gtk_message_dialog_new(GTK_WINDOW(gui_server->window), GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, _("Can't create DAtabase right !!!"));
		gtk_dialog_run (GTK_DIALOG (msgdlg));
		gtk_widget_destroy (msgdlg);
		return;
	}
	
	gmysqlcc_gui_server_fill_user_right_database_list(gui_server, mysql_rght);
}

void gmysqlcc_gui_server_evt_btnDRApply_clicked (GtkWidget *widget, gpointer user_data) {
	
}

void gmysqlcc_gui_server_evt_btnDRDelete_clicked (GtkWidget *widget, gpointer user_data) {
	p_gmysqlcc_gui_server gui_server = (p_gmysqlcc_gui_server)user_data;
	GtkWidget * msgdlg;
	
	if (gui_server->curr_database_rights == NULL) {
		msgdlg = gtk_message_dialog_new(GTK_WINDOW(gui_server->window), GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, _("Can not remove database right !!!"));
		gtk_dialog_run (GTK_DIALOG (msgdlg));
		gtk_widget_destroy (msgdlg);
		return;
	}
	
	mysql_right_remove(gui_server->curr_database_rights);
	gui_server->curr_database_rights = NULL;
	
	gmysqlcc_gui_server_fill_user_right_database_list(gui_server, NULL);
}

void gmysqlcc_gui_server_evt_lstURUserRights_edited (GtkCellRendererText *cellrenderertext, gchar *path_string, gchar *new_value, gpointer user_data) {
	p_gmysqlcc_gui_server gui_server = (p_gmysqlcc_gui_server)user_data;
	GtkWidget * msgdlg;
	GtkTreeModel * tree_model;
	GtkTreeIter iter;
	gchar * right;
	
	tree_model = gtk_tree_view_get_model(GTK_TREE_VIEW (gui_server->lstURUserRights));
	if (gtk_tree_model_get_iter_from_string(tree_model, &iter, path_string)) {
		gtk_tree_model_get (tree_model, &iter, 0, &right, -1);
		mysql_user_set_right(gui_server->curr_mysql_usr, right, new_value);
		gmysqlcc_gui_server_fill_user_right_list(gui_server);
	} else {
		msgdlg = gtk_message_dialog_new(GTK_WINDOW(gui_server->window), GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, _("Can't found edited right !!!"));
		gtk_dialog_run (GTK_DIALOG (msgdlg));
		gtk_widget_destroy (msgdlg);
	}
}

void gmysqlcc_gui_server_evt_lstDRDatabaseRights_edited (GtkCellRendererText *cellrenderertext, gchar *path_string, gchar *new_value, gpointer user_data) {
	p_gmysqlcc_gui_server gui_server = (p_gmysqlcc_gui_server)user_data;
	GtkWidget * msgdlg;
	GtkTreeModel * tree_model;
	GtkTreeIter iter;
	gchar * right;
	
	tree_model = gtk_tree_view_get_model(GTK_TREE_VIEW (gui_server->lstDRDatabaseRights));
	if (gtk_tree_model_get_iter_from_string(tree_model, &iter, path_string)) {
		gtk_tree_model_get (tree_model, &iter, 0, &right, -1);
		mysql_right_update(gui_server->curr_database_rights, right, new_value);
		gmysqlcc_gui_server_fill_user_right_database_right_list(gui_server);
	} else {
		msgdlg = gtk_message_dialog_new(GTK_WINDOW(gui_server->window), GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, _("Can't found edited right !!!"));
		gtk_dialog_run (GTK_DIALOG (msgdlg));
		gtk_widget_destroy (msgdlg);
	}
}

void gmysqlcc_gui_server_evt_btnMaxUpdate_clicked (GtkWidget *widget, gpointer user_data) {
	p_gmysqlcc_gui_server gui_server = (p_gmysqlcc_gui_server)user_data;
	gchar * pc_newValue = NULL;
	
	if (gui_server->curr_mysql_usr != NULL) {
		pc_newValue = gtk_editable_get_chars(GTK_EDITABLE(gui_server->txtMaxQuestions), 0, -1);
		mysql_right_update(gui_server->curr_mysql_usr->user_rights, "max_questions", pc_newValue);
		g_free(pc_newValue);
		
		pc_newValue = gtk_editable_get_chars(GTK_EDITABLE(gui_server->txtMaxUpdates), 0, -1);
		mysql_right_update(gui_server->curr_mysql_usr->user_rights, "max_updates", pc_newValue);
		g_free(pc_newValue);
		
		pc_newValue = gtk_editable_get_chars(GTK_EDITABLE(gui_server->txtMaxConnections), 0, -1);
		mysql_right_update(gui_server->curr_mysql_usr->user_rights, "max_connections", pc_newValue);
		g_free(pc_newValue);
	}
}

void gmysqlcc_gui_server_evt_btnSslUpdate_clicked (GtkWidget *widget, gpointer user_data) {
	p_gmysqlcc_gui_server gui_server = (p_gmysqlcc_gui_server)user_data;
	GtkTextBuffer* p_buffer = NULL;
	GtkTextIter begin, end;
	gchar * pc_newValue = NULL;
	gint i_idxSelectedItem = 0;
	
	if (gui_server->curr_mysql_usr != NULL) {
		i_idxSelectedItem = gtk_combo_box_get_active(GTK_COMBO_BOX(gui_server->cbx_ssl_type));
		if (i_idxSelectedItem > -1) {
			mysql_right_update(gui_server->curr_mysql_usr->user_rights, "ssl_type", ar_sslType[i_idxSelectedItem]);
		}
		
		p_buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW(gui_server->txt_ssl_cypher));
		gtk_text_buffer_get_start_iter (GTK_TEXT_BUFFER(p_buffer), &begin);
		gtk_text_buffer_get_end_iter (GTK_TEXT_BUFFER(p_buffer), &end);
		pc_newValue = (gchar *)gtk_text_buffer_get_text (GTK_TEXT_BUFFER(p_buffer), &begin, &end, FALSE);
		mysql_right_update(gui_server->curr_mysql_usr->user_rights, "ssl_cipher", pc_newValue);
		g_free(pc_newValue);
		
		p_buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW(gui_server->txt_x509_issuer));
		gtk_text_buffer_get_start_iter (GTK_TEXT_BUFFER(p_buffer), &begin);
		gtk_text_buffer_get_end_iter (GTK_TEXT_BUFFER(p_buffer), &end);
		pc_newValue = (gchar *)gtk_text_buffer_get_text (GTK_TEXT_BUFFER(p_buffer), &begin, &end, FALSE);
		mysql_right_update(gui_server->curr_mysql_usr->user_rights, "x509_issuer", pc_newValue);
		g_free(pc_newValue);
		
		p_buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW(gui_server->txt_x509_subject));
		gtk_text_buffer_get_start_iter (GTK_TEXT_BUFFER(p_buffer), &begin);
		gtk_text_buffer_get_end_iter (GTK_TEXT_BUFFER(p_buffer), &end);
		pc_newValue = (gchar *)gtk_text_buffer_get_text (GTK_TEXT_BUFFER(p_buffer), &begin, &end, FALSE);
		mysql_right_update(gui_server->curr_mysql_usr->user_rights, "x509_subject", pc_newValue);
		g_free(pc_newValue);
		
		
	}
}
