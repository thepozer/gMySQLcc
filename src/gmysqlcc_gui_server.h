
#ifndef __GMYSQLCC_GUI_SERVER_EVT_H__
#define __GMYSQLCC_GUI_SERVER_EVT_H__ 

#include "gmysqlcc_gui_all.h"

void gmysqlcc_gui_server_create_widget_tab_database (p_gmysqlcc_gui_server gui_server, GtkWidget * notebook);
void gmysqlcc_gui_server_init_widget_tab_database (p_gmysqlcc_gui_server gui_server);
void gmysqlcc_gui_server_create_widget_tab_procedure (p_gmysqlcc_gui_server gui_server, GtkWidget * notebook);
void gmysqlcc_gui_server_init_widget_tab_procedure (p_gmysqlcc_gui_server gui_server);
void gmysqlcc_gui_server_create_widget_tab_user (p_gmysqlcc_gui_server gui_server, GtkWidget * notebook);
void gmysqlcc_gui_server_init_widget_tab_user (p_gmysqlcc_gui_server gui_server);
void gmysqlcc_gui_server_create_widget_tab_server (p_gmysqlcc_gui_server gui_server, GtkWidget * notebook);
void gmysqlcc_gui_server_init_widget_tab_server (p_gmysqlcc_gui_server gui_server);
void gmysqlcc_gui_server_create_widget_tab_help (p_gmysqlcc_gui_server gui_server, GtkWidget * notebook);
void gmysqlcc_gui_server_init_widget_tab_help (p_gmysqlcc_gui_server gui_server);

#endif /* __GMYSQLCC_GUI_SERVER_EVT_H__ */
