
#ifndef __GMYSQLCC_GUI_SERVER_EVT_H__
#define __GMYSQLCC_GUI_SERVER_EVT_H__ 

#include "gmysqlcc_gui_all.h"

void gmysqlcc_gui_server_create_widget (p_gmysqlcc_gui_server gui_server);
void gmysqlcc_gui_server_init_widget (p_gmysqlcc_gui_server gui_server);

void gmysqlcc_gui_server_fill_database_list (p_gmysqlcc_gui_server gui_server);
void gmysqlcc_gui_server_fill_table_list (p_gmysqlcc_gui_server gui_server);
void gmysqlcc_gui_server_fill_user_list (p_gmysqlcc_gui_server gui_server, p_mysql_user mysql_usr);
void gmysqlcc_gui_server_display_current_user (p_gmysqlcc_gui_server gui_server);
void gmysqlcc_gui_server_fill_user_right_list (p_gmysqlcc_gui_server gui_server);
void gmysqlcc_gui_server_open_query_window (p_gmysqlcc_gui_server gui_server, gboolean use_table);

void gmysqlcc_gui_server_evt_destroy (GtkWidget *widget, gpointer user_data);

/* Database events */
void gmysqlcc_gui_server_evt_mnuDBOpsRefresh_activate (GtkWidget *widget, gpointer user_data);
void gmysqlcc_gui_server_evt_mnuDBOpsShowCreate_activate (GtkWidget *widget, gpointer user_data);
void gmysqlcc_gui_server_evt_mnuTBLOpsShowCreate_activate (GtkWidget *widget, gpointer user_data);
gboolean gmysqlcc_gui_server_evt_lstBase_btnpress (GtkWidget *widget, GdkEventButton *event, gpointer user_data);
gboolean gmysqlcc_gui_server_evt_lstTable_btnpress (GtkWidget *widget, GdkEventButton *event, gpointer user_data);
void gmysqlcc_gui_server_evt_lstBase_selected (GtkTreeSelection *selection, gpointer user_data);
void gmysqlcc_gui_server_evt_lstTable_selected (GtkTreeSelection *selection, gpointer user_data);
void gmysqlcc_gui_server_evt_btnTlbrSql_clicked (GtkWidget *widget, gpointer user_data);
void gmysqlcc_gui_server_evt_btnTlbrSqlFile_clicked (GtkWidget *widget, gpointer user_data);
void gmysqlcc_gui_server_evt_btnTlbrClose_clicked (GtkWidget *widget, gpointer user_data);
void gmysqlcc_gui_server_evt_btnDbAdd_clicked (GtkWidget *widget, gpointer user_data);
void gmysqlcc_gui_server_evt_btnDbDel_clicked (GtkWidget *widget, gpointer user_data);
void gmysqlcc_gui_server_evt_btnTblAdd_clicked (GtkWidget *widget, gpointer user_data);
void gmysqlcc_gui_server_evt_btnTblEdit_clicked (GtkWidget *widget, gpointer user_data);
void gmysqlcc_gui_server_evt_btnTblDump_clicked (GtkWidget *widget, gpointer user_data);
void gmysqlcc_gui_server_evt_btnTblDel_clicked (GtkWidget *widget, gpointer user_data);

/* User events */
void gmysqlcc_gui_server_evt_lstUser_selected (GtkTreeSelection *selection, gpointer user_data);
void gmysqlcc_gui_server_evt_btnUserNew_clicked (GtkWidget *widget, gpointer user_data);
void gmysqlcc_gui_server_evt_btnUserAdd_clicked (GtkWidget *widget, gpointer user_data);
void gmysqlcc_gui_server_evt_btnUserUpdate_clicked (GtkWidget *widget, gpointer user_data);
void gmysqlcc_gui_server_evt_btnUserDelete_clicked (GtkWidget *widget, gpointer user_data);
void gmysqlcc_gui_server_evt_lstUserRights_edited (GtkCellRendererText *cellrenderertext, gchar *path_string, gchar *new_value, gpointer user_data);

#endif /* __GMYSQLCC_GUI_SERVER_EVT_H__ */
