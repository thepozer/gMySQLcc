
#include "gmysqlcc_gui_all.h"
#include "gmysqlcc_gui_server.h"


void gmysqlcc_gui_server_create_widget (p_gmysqlcc_gui_server gui_server);
void gmysqlcc_gui_server_init_widget (p_gmysqlcc_gui_server gui_server);

void gmysqlcc_gui_server_evt_destroy (GtkWidget *widget, gpointer user_data);
void gmysqlcc_gui_server_evt_btnTlbrSql_clicked (GtkWidget *widget, gpointer user_data);
void gmysqlcc_gui_server_evt_btnTlbrSqlFile_clicked (GtkWidget *widget, gpointer user_data);
void gmysqlcc_gui_server_evt_btnTlbrSqlServerList_clicked (GtkWidget *widget, gpointer user_data);
void gmysqlcc_gui_server_evt_btnTlbrClose_clicked (GtkWidget *widget, gpointer user_data);

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
	gui_server->curr_table_rights = NULL;
	
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
	if (NbrWnd <= 1) { /* There is always the window server list */
		g_printerr("Destroy App\n");
		gtk_main_quit();
	}
	
	return TRUE;
}

void gmysqlcc_gui_server_create_widget (p_gmysqlcc_gui_server gui_server) {
	GtkWidget *statusbar;
	GtkWidget *vbox;
	GtkWidget *notebook;
	GtkWidget *toolbar;
	GtkWidget * imgToolbar;
	GtkToolItem * btnTlbrClose, * btnTlbrSql, * btnTlbrSqlFile, * btnTlbrSqlServerList;
	
	GtkTooltips * tooltips;
	GString * sTitle;
	gint iServerVersion = 0;
	
	tooltips = gtk_tooltips_new();

	gui_server->window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	sTitle = g_string_new("");
	g_string_printf(sTitle, _("Server : %s"), gui_server->mysql_srv->name);
	gtk_window_set_title (GTK_WINDOW (gui_server->window), sTitle->str);
	g_string_free(sTitle, TRUE);
	gtk_window_set_default_size (GTK_WINDOW (gui_server->window), 640, 480);
	g_signal_connect (G_OBJECT (gui_server->window), "destroy", 
										G_CALLBACK (gmysqlcc_gui_server_evt_destroy), (gpointer)gui_server);

	vbox = gtk_vbox_new (FALSE, 0);
	gtk_widget_show (vbox);
	gtk_container_add (GTK_CONTAINER (gui_server->window), vbox);

	toolbar = gtk_toolbar_new ();
	gtk_widget_show (toolbar);
	gtk_box_pack_start (GTK_BOX (vbox), toolbar, FALSE, FALSE, 0);
	gtk_toolbar_set_style (GTK_TOOLBAR (toolbar), GTK_TOOLBAR_BOTH_HORIZ);

	imgToolbar = gtk_image_new_from_stock(GTK_STOCK_EXECUTE, GTK_ICON_SIZE_LARGE_TOOLBAR);
	gtk_widget_show(imgToolbar);
	btnTlbrSql = gtk_tool_button_new (imgToolbar, _("SQL"));
	gtk_tool_item_set_is_important (GTK_TOOL_ITEM(btnTlbrSql), TRUE);
	g_signal_connect (G_OBJECT (btnTlbrSql), "clicked", 
										G_CALLBACK (gmysqlcc_gui_server_evt_btnTlbrSql_clicked), (gpointer)gui_server);
	gtk_widget_show(GTK_WIDGET(btnTlbrSql));
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar), GTK_TOOL_ITEM(btnTlbrSql), -1);
	gtk_tool_item_set_tooltip (GTK_TOOL_ITEM(btnTlbrSql), tooltips, _("Exec SQL Query"), NULL);

	imgToolbar = gtk_image_new_from_stock(GTK_STOCK_EXECUTE, GTK_ICON_SIZE_LARGE_TOOLBAR);
	gtk_widget_show(imgToolbar);
	btnTlbrSqlFile = gtk_tool_button_new (imgToolbar, _("SQL File"));
	gtk_tool_item_set_is_important (GTK_TOOL_ITEM(btnTlbrSqlFile), TRUE);
	g_signal_connect (G_OBJECT (btnTlbrSqlFile), "clicked", 
										G_CALLBACK (gmysqlcc_gui_server_evt_btnTlbrSqlFile_clicked), (gpointer)gui_server);
	gtk_widget_show(GTK_WIDGET(btnTlbrSqlFile));
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar), GTK_TOOL_ITEM(btnTlbrSqlFile), -1);
	gtk_tool_item_set_tooltip (GTK_TOOL_ITEM(btnTlbrSqlFile), tooltips, _("Exec SQL File"), NULL);

	imgToolbar = gtk_image_new_from_stock(GTK_STOCK_OPEN, GTK_ICON_SIZE_LARGE_TOOLBAR);
	gtk_widget_show(imgToolbar);
	btnTlbrSqlServerList = gtk_tool_button_new (imgToolbar, _("Servers list"));
	gtk_tool_item_set_is_important (GTK_TOOL_ITEM(btnTlbrSqlServerList), TRUE);
	g_signal_connect (G_OBJECT (btnTlbrSqlServerList), "clicked", 
										G_CALLBACK (gmysqlcc_gui_server_evt_btnTlbrSqlServerList_clicked), (gpointer)gui_server);
	gtk_widget_show(GTK_WIDGET(btnTlbrSqlServerList));
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar), GTK_TOOL_ITEM(btnTlbrSqlServerList), -1);
	gtk_tool_item_set_tooltip (GTK_TOOL_ITEM(btnTlbrSqlServerList), tooltips, _("Open server list window"), NULL);

	imgToolbar = gtk_image_new_from_stock(GTK_STOCK_CLOSE, GTK_ICON_SIZE_LARGE_TOOLBAR);
	gtk_widget_show(imgToolbar);
	btnTlbrClose = gtk_tool_button_new (imgToolbar, _("Close"));
	gtk_tool_item_set_is_important (GTK_TOOL_ITEM(btnTlbrClose), TRUE);
	g_signal_connect (G_OBJECT (btnTlbrClose), "clicked", 
										G_CALLBACK (gmysqlcc_gui_server_evt_btnTlbrClose_clicked), (gpointer)gui_server);
	gtk_widget_show(GTK_WIDGET(btnTlbrClose));
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar), GTK_TOOL_ITEM(btnTlbrClose), -1);
	gtk_tool_item_set_tooltip (GTK_TOOL_ITEM(btnTlbrClose), tooltips, _("Close window"), NULL);
	
	notebook = gtk_notebook_new ();
	gtk_widget_show (notebook);
	gtk_box_pack_start (GTK_BOX (vbox), notebook, TRUE, TRUE, 0);

	iServerVersion = mysql_server_get_version(gui_server->mysql_srv, FALSE);
	
	gmysqlcc_gui_server_create_widget_tab_database (gui_server, notebook);
	if (gui_server->mysql_srv->version >= 50000) {
		gmysqlcc_gui_server_create_widget_tab_procedure (gui_server, notebook);
	}
	gmysqlcc_gui_server_create_widget_tab_user (gui_server, notebook);
	gmysqlcc_gui_server_create_widget_tab_server (gui_server, notebook);
	if (gui_server->mysql_srv->version >= 40100) {
		gmysqlcc_gui_server_create_widget_tab_help (gui_server, notebook);
	}
	
	statusbar = gtk_statusbar_new ();
	gtk_widget_show (statusbar);
	gtk_box_pack_start (GTK_BOX (vbox), statusbar, FALSE, FALSE, 0);
	
}

void gmysqlcc_gui_server_init_widget (p_gmysqlcc_gui_server gui_server) {
	
	gmysqlcc_gui_server_init_widget_tab_database(gui_server);
	if (gui_server->mysql_srv->version >= 50000) {
		gmysqlcc_gui_server_init_widget_tab_procedure (gui_server);
	}
	gmysqlcc_gui_server_init_widget_tab_user(gui_server);
	gmysqlcc_gui_server_init_widget_tab_server(gui_server);
	if (gui_server->mysql_srv->version >= 40100) {
		gmysqlcc_gui_server_init_widget_tab_help (gui_server);
	}
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

void gmysqlcc_gui_server_evt_btnTlbrSql_clicked (GtkWidget *widget, gpointer user_data) {
	p_gmysqlcc_gui_server gui_server = (p_gmysqlcc_gui_server)user_data;
	
	gmysqlcc_gui_server_open_query_window (gui_server, TRUE);
}

void gmysqlcc_gui_server_evt_btnTlbrSqlFile_clicked (GtkWidget *widget, gpointer user_data) {
	/*p_gmysqlcc_gui_server gui_server = (p_gmysqlcc_gui_server)user_data;*/
	p_gmysqlcc_gui_text gui_text;
	
	gui_text = gmysqlcc_gui_text_new();
	gmysqlcc_gui_text_display(gui_text, TRUE);
}

void gmysqlcc_gui_server_evt_btnTlbrSqlServerList_clicked (GtkWidget *widget, gpointer user_data) {
	/*p_gmysqlcc_gui_server gui_server = (p_gmysqlcc_gui_server)user_data;*/
	
	gmysqlcc_gui_list_server_display(gmysqlcc_gui_list_server, TRUE);
}

void gmysqlcc_gui_server_evt_btnTlbrClose_clicked (GtkWidget *widget, gpointer user_data) {
	p_gmysqlcc_gui_server gui_server = (p_gmysqlcc_gui_server)user_data;
	
	gtk_widget_destroy(GTK_WIDGET(gui_server->window));
}
