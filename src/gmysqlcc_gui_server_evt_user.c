
#include "gmysqlcc_gui_all.h"
#include "gmysqlcc_gui_server_evt.h"

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
