
#include "gmysqlcc_gui_all.h"
#include "gmysqlcc_gui_server_evt.h"

void gmysqlcc_gui_server_evt_lstUser_selected (GtkTreeSelection *selection, gpointer user_data) {
	p_gmysqlcc_gui_server gui_server = (p_gmysqlcc_gui_server)user_data;
	GtkTreeIter iter;
	GtkTreeModel *model;
	
	g_print("Changed !\n");
	if (gtk_tree_selection_get_selected (selection, &model, &iter)) {
		gtk_tree_model_get (model, &iter, 1, &gui_server->curr_mysql_usr, -1);
		g_print ("The User is '%s'@'%s'\n", gui_server->curr_mysql_usr->login, gui_server->curr_mysql_usr->host);
		
		gmysqlcc_gui_server_display_current_user(gui_server);
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
	p_gmysqlcc_gui_server gui_server = (p_gmysqlcc_gui_server)user_data;
	
	gmysqlcc_gui_server_fill_user_list(gui_server);
}

void gmysqlcc_gui_server_evt_btnUserUpdate_clicked (GtkWidget *widget, gpointer user_data) {
	p_gmysqlcc_gui_server gui_server = (p_gmysqlcc_gui_server)user_data;
	
	gmysqlcc_gui_server_fill_user_list(gui_server);
}

void gmysqlcc_gui_server_evt_btnUserDelete_clicked (GtkWidget *widget, gpointer user_data) {
	p_gmysqlcc_gui_server gui_server = (p_gmysqlcc_gui_server)user_data;
	
	gmysqlcc_gui_server_fill_user_list(gui_server);
}
