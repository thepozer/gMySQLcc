/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include "gmlc_gui_query.h"
#include "gmlc_mysql_query.h"

#include <gdk/gdkkeysyms.h>

static void gmlc_gui_query_finalize (GmlcGuiQuery * pGmlcGuiQry);
static void gmlc_gui_query_create_widgets (GmlcGuiQuery * pGmlcGuiQry);
static void gmlc_gui_query_init_widgets (GmlcGuiQuery * pGmlcGuiQry);



static void gmlc_gui_query_evt_destroy(GtkWidget *widget, gpointer user_data);
gboolean gmlc_gui_query_evt_window_keyrelease (GtkWidget *widget, GdkEventKey *event, gpointer user_data);

void gmlc_gui_query_evt_btnExecSql_clicked(GtkWidget *widget, gpointer user_data);
void gmlc_gui_query_evt_btnDuplicateSql_clicked(GtkWidget *widget, gpointer user_data);
void gmlc_gui_query_evt_btnDumpSql_clicked(GtkWidget *widget, gpointer user_data);
void gmlc_gui_query_evt_btnServerList_clicked (GtkWidget *widget, gpointer user_data);
void gmlc_gui_query_evt_btnClose_clicked(GtkWidget *widget, gpointer user_data);

enum {
	PROP_0,
};

G_DEFINE_TYPE (GmlcGuiQuery, gmlc_gui_query, GTK_TYPE_WINDOW);

static void gmlc_gui_query_class_init (GmlcGuiQueryClass * pClass) {
	GObjectClass *pObjClass = G_OBJECT_CLASS(pClass);
	
	pObjClass->finalize = (GObjectFinalizeFunc) gmlc_gui_query_finalize;
}

static void gmlc_gui_query_init (GmlcGuiQuery * pGmlcGuiQry) {

	GiNbrWnd ++;
	
/*
	sTitle = g_string_new("");
	g_string_printf(sTitle, _("SQL Request - %s - %s"), pGmlcGuiQry->mysql_qry->mysql_srv->name, pGmlcGuiQry->mysql_qry->db_name);
	gtk_window_set_title (GTK_WINDOW (pGmlcGuiQry), sTitle->str);
	g_string_free(sTitle, TRUE);
*/	
	gtk_window_set_title (GTK_WINDOW(pGmlcGuiQry), _("GMySQLcc - Query window"));
	gtk_window_set_default_size (GTK_WINDOW(pGmlcGuiQry), 640, 480);
	
	g_signal_connect(G_OBJECT (pGmlcGuiQry), "destroy", G_CALLBACK (gmlc_gui_query_evt_destroy), NULL);
	g_signal_connect (G_OBJECT (pGmlcGuiQry), "key-release-event", G_CALLBACK (gmlc_gui_query_evt_window_keyrelease), pGmlcGuiQry);
	
	pGmlcGuiQry->pGmlcMysqlSrv = NULL;
	pGmlcGuiQry->pcDefDbName = NULL;
	pGmlcGuiQry->pGmlcMysqlQry = NULL;
	
	gmlc_gui_query_create_widgets(pGmlcGuiQry);
	gmlc_gui_query_init_widgets(pGmlcGuiQry);
}

static void gmlc_gui_query_finalize (GmlcGuiQuery * pGmlcGuiQry) {
	UNUSED_VAR(pGmlcGuiQry);
	
}

GmlcGuiQuery * gmlc_gui_query_new (GmlcMysqlServer * pGmlcMysqlSrv, const gchar * pcDefDbName) {
	GmlcGuiQuery * pGmlcGuiQry = NULL;
	
	pGmlcGuiQry = GMLC_GUI_QUERY(g_object_new(GMLC_GUI_TYPE_QUERY, NULL));
	
	pGmlcGuiQry->pGmlcMysqlSrv = pGmlcMysqlSrv;
	pGmlcGuiQry->pcDefDbName = g_strdup(pcDefDbName);
	
	return pGmlcGuiQry;
}

static void gmlc_gui_query_create_widgets (GmlcGuiQuery * pGmlcGuiQry) {
	GtkWidget *vbox;
	GtkWidget *vpanedSQL;
	GtkWidget *scrlwndSQLRequest;
	GtkToolItem *btnExecSql;
	GtkToolItem *btnDumpSql;
	GtkToolItem *btnDuplicateSql;
	GtkToolItem *btnServerList;
	GtkToolItem *btnClose;
	GtkTooltips * tooltips;
	GtkWidget * imgToolbar;
	GtkWidget * tlbSql;
	
	GtkTreeSelection *select;
#ifdef USE_GTKSOURCEVIEW
	GtkTextBuffer * txtBuffer;
#endif /* USE_GTKSOURCEVIEW */
	
	PangoFontDescription * pCourierFontDesc = NULL;
	
	tooltips = gtk_tooltips_new();
	
	vbox = gtk_vbox_new (FALSE, 0);
	gtk_widget_show (vbox);
	gtk_container_add (GTK_CONTAINER (pGmlcGuiQry), vbox);

	tlbSql = gtk_toolbar_new ();
	gtk_widget_show (tlbSql);
	gtk_box_pack_start (GTK_BOX (vbox), tlbSql, FALSE, FALSE, 0);
	gtk_toolbar_set_style (GTK_TOOLBAR (tlbSql), GTK_TOOLBAR_BOTH_HORIZ);
	
	imgToolbar = gtk_image_new_from_stock(GTK_STOCK_EXECUTE, GTK_ICON_SIZE_LARGE_TOOLBAR);
	gtk_widget_show(imgToolbar);
	btnExecSql = gtk_tool_button_new (imgToolbar, _("Execute"));
	gtk_tool_item_set_is_important (GTK_TOOL_ITEM(btnExecSql), TRUE);
	g_signal_connect (G_OBJECT (btnExecSql), "clicked", G_CALLBACK (gmlc_gui_query_evt_btnExecSql_clicked), pGmlcGuiQry);
	gtk_widget_show(GTK_WIDGET(btnExecSql));
	gtk_toolbar_insert(GTK_TOOLBAR(tlbSql), GTK_TOOL_ITEM(btnExecSql), -1);
	gtk_tool_item_set_tooltip (GTK_TOOL_ITEM(btnExecSql), tooltips, _("Execute SQL query"), NULL);
	
	imgToolbar = gtk_image_new_from_stock(GTK_STOCK_COPY, GTK_ICON_SIZE_LARGE_TOOLBAR);
	gtk_widget_show(imgToolbar);
	btnDuplicateSql = gtk_tool_button_new (imgToolbar, _("Duplicate"));
	gtk_tool_item_set_is_important (GTK_TOOL_ITEM(btnDuplicateSql), TRUE);
	g_signal_connect (G_OBJECT (btnDuplicateSql), "clicked", G_CALLBACK (gmlc_gui_query_evt_btnDuplicateSql_clicked), pGmlcGuiQry);
	gtk_widget_show(GTK_WIDGET(btnDuplicateSql));
	gtk_toolbar_insert(GTK_TOOLBAR(tlbSql), GTK_TOOL_ITEM(btnDuplicateSql), -1);
	gtk_tool_item_set_tooltip (GTK_TOOL_ITEM(btnDuplicateSql), tooltips, _("Duplicate SQL Windows"), NULL);
	
	imgToolbar = gtk_image_new_from_stock(GTK_STOCK_FLOPPY, GTK_ICON_SIZE_LARGE_TOOLBAR);
	gtk_widget_show(imgToolbar);
	btnDumpSql = gtk_tool_button_new (imgToolbar, _("Dump"));
	gtk_tool_item_set_is_important (GTK_TOOL_ITEM(btnDumpSql), TRUE);
/*	g_signal_connect (G_OBJECT (btnDumpSql), "clicked", G_CALLBACK (gmlc_gui_query_evt_btnDumpSql_clicked), pGmlcGuiQry);*/
	gtk_widget_set_sensitive(GTK_WIDGET(btnDumpSql), FALSE);
	gtk_widget_show(GTK_WIDGET(btnDumpSql));
	gtk_toolbar_insert(GTK_TOOLBAR(tlbSql), GTK_TOOL_ITEM(btnDumpSql), -1);
	gtk_tool_item_set_tooltip (GTK_TOOL_ITEM(btnDumpSql), tooltips, _("Dump with current query"), NULL);
	
	imgToolbar = gtk_image_new_from_stock(GTK_STOCK_OPEN, GTK_ICON_SIZE_LARGE_TOOLBAR);
	gtk_widget_show(imgToolbar);
	btnServerList = gtk_tool_button_new (imgToolbar, _("Servers list"));
	gtk_tool_item_set_is_important (GTK_TOOL_ITEM(btnServerList), TRUE);
	g_signal_connect (G_OBJECT (btnServerList), "clicked", G_CALLBACK (gmlc_gui_query_evt_btnServerList_clicked), pGmlcGuiQry);
	gtk_widget_show(GTK_WIDGET(btnServerList));
	gtk_toolbar_insert(GTK_TOOLBAR(tlbSql), GTK_TOOL_ITEM(btnServerList), -1);
	gtk_tool_item_set_tooltip (GTK_TOOL_ITEM(btnServerList), tooltips, _("Open server list window"), NULL);

	imgToolbar = gtk_image_new_from_stock(GTK_STOCK_CLOSE, GTK_ICON_SIZE_LARGE_TOOLBAR);
	gtk_widget_show(imgToolbar);
	btnClose = gtk_tool_button_new (imgToolbar, _("Close"));
	gtk_tool_item_set_is_important (GTK_TOOL_ITEM(btnClose), TRUE);
	g_signal_connect (G_OBJECT (btnClose), "clicked", G_CALLBACK (gmlc_gui_query_evt_btnClose_clicked), pGmlcGuiQry);
	gtk_widget_show(GTK_WIDGET(btnClose));
	gtk_toolbar_insert(GTK_TOOLBAR(tlbSql), GTK_TOOL_ITEM(btnClose), -1);
	gtk_tool_item_set_tooltip (GTK_TOOL_ITEM(btnClose), tooltips, _("Close window"), NULL);

	vpanedSQL = gtk_vpaned_new ();
	gtk_widget_show (vpanedSQL);
	gtk_box_pack_start (GTK_BOX (vbox), vpanedSQL, TRUE, TRUE, 0);
	gtk_paned_set_position (GTK_PANED (vpanedSQL), 100);
	
	scrlwndSQLRequest = gtk_scrolled_window_new (NULL, NULL);
	gtk_widget_show (scrlwndSQLRequest);
	gtk_paned_pack1 (GTK_PANED (vpanedSQL), scrlwndSQLRequest, FALSE, TRUE);

#ifdef USE_GTKSOURCEVIEW
	pGmlcGuiQry->txtSQLRequest = gtk_source_view_new ();
	gtk_source_view_set_show_line_numbers (GTK_SOURCE_VIEW(pGmlcGuiQry->txtSQLRequest), TRUE);
	gtk_source_view_set_tabs_width (GTK_SOURCE_VIEW(pGmlcGuiQry->txtSQLRequest), 2);
	txtBuffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(pGmlcGuiQry->txtSQLRequest));
	gtk_source_buffer_set_highlight (GTK_SOURCE_BUFFER(txtBuffer), TRUE);
	gtk_source_buffer_set_language(GTK_SOURCE_BUFFER(txtBuffer), 
			gtk_source_languages_manager_get_language_from_mime_type(LangManager, "text/x-sql"));
#else /* USE_GTKSOURCEVIEW */
	pGmlcGuiQry->txtSQLRequest = gtk_text_view_new ();
#endif /* USE_GTKSOURCEVIEW */
	pCourierFontDesc = pango_font_description_from_string(GpGmlcMscCfg->pcQueryFontName);
	gtk_widget_modify_font(pGmlcGuiQry->txtSQLRequest, pCourierFontDesc);
	pango_font_description_free(pCourierFontDesc);
	gtk_widget_show (pGmlcGuiQry->txtSQLRequest);
	gtk_container_add (GTK_CONTAINER (scrlwndSQLRequest), pGmlcGuiQry->txtSQLRequest);
  
	pGmlcGuiQry->sclSQLResult = gtk_scrolled_window_new (NULL, NULL);
	gtk_widget_show (pGmlcGuiQry->sclSQLResult);
	gtk_paned_pack2 (GTK_PANED (vpanedSQL), pGmlcGuiQry->sclSQLResult, TRUE, TRUE);

	pGmlcGuiQry->lstSQLResult = gtk_tree_view_new ();
	gtk_widget_show (pGmlcGuiQry->lstSQLResult);
	gtk_tree_view_set_rules_hint(GTK_TREE_VIEW (pGmlcGuiQry->lstSQLResult), TRUE);
	gtk_container_add (GTK_CONTAINER (pGmlcGuiQry->sclSQLResult), pGmlcGuiQry->lstSQLResult);

	select = gtk_tree_view_get_selection (GTK_TREE_VIEW (pGmlcGuiQry->lstSQLResult));
	gtk_tree_selection_set_mode (select, GTK_SELECTION_SINGLE);
/*	g_signal_connect (G_OBJECT (select), "changed", G_CALLBACK (gmysqlcc_gui_query_evt_resultRow_selected), pGmlcGuiQry);*/

	pGmlcGuiQry->statusbarSQL = gtk_statusbar_new ();
	gtk_widget_show (pGmlcGuiQry->statusbarSQL);
	gtk_box_pack_start (GTK_BOX (vbox), pGmlcGuiQry->statusbarSQL, FALSE, FALSE, 0);
	
}

static void gmlc_gui_query_init_widgets (GmlcGuiQuery * pGmlcGuiQry) {

}

gboolean gmlc_gui_query_set_query (GmlcGuiQuery * pGmlcGuiQry, const gchar * pcQuery, gboolean bExecNow) {
	GtkTextBuffer * txtBuffer;

	if (pcQuery != NULL) {
		txtBuffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(pGmlcGuiQry->txtSQLRequest));
		gtk_text_buffer_set_text(GTK_TEXT_BUFFER(txtBuffer), pcQuery, -1);
		if (bExecNow) {
/*			gmlc_gui_query_execute_query (pGmlcGuiQry, pcQuery);*/
		}
		return TRUE;
	}
	
	return FALSE;
}

gchar * gmlc_gui_query_get_query (GmlcGuiQuery * pGmlcGuiQry) {
	GtkTextBuffer * txtBuffer;
	GtkTextIter begin, end;
	gchar * pcSqlQuery;
	
	txtBuffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(pGmlcGuiQry->txtSQLRequest));
	gtk_text_buffer_get_start_iter (GTK_TEXT_BUFFER(txtBuffer), &begin);
	gtk_text_buffer_get_end_iter (GTK_TEXT_BUFFER(txtBuffer), &end);
	pcSqlQuery = gtk_text_buffer_get_text(GTK_TEXT_BUFFER(txtBuffer), &begin, &end, FALSE);
	
	return pcSqlQuery;
}

static void gmlc_gui_query_evt_destroy(GtkWidget *widget, gpointer user_data) {
	UNUSED_VAR(user_data);

	gtk_widget_hide(GTK_WIDGET(widget));
	GiNbrWnd --;
	
	if (GiNbrWnd <= 0) {
		gtk_main_quit();
	}
}

gboolean gmlc_gui_query_evt_window_keyrelease (GtkWidget *widget, GdkEventKey *event, gpointer user_data) {
	
	if (event->state & GDK_CONTROL_MASK && (event->keyval == GDK_e || event->keyval == GDK_E)) {
		gmlc_gui_query_evt_btnExecSql_clicked (widget, user_data);
		return TRUE;
	}
	
	return FALSE;
}

void gmlc_gui_query_evt_btnExecSql_clicked(GtkWidget *widget, gpointer user_data) {
	GmlcGuiQuery * pGmlcGuiQry = GMLC_GUI_QUERY(user_data);
	gchar * pcSqlQuery;
	UNUSED_VAR(widget);

	/*g_print("Execute SQL\n");*/
	pcSqlQuery = gmlc_gui_query_get_query(pGmlcGuiQry);
	/*g_print("SQL Query : '%s'\n", pcSqlQuery);*/
	
/*
	gmlc_gui_query_execute_query(pGmlcGuiQry, pcSqlQuery);
*/
	
	g_free(pcSqlQuery);
}

void gmlc_gui_query_evt_btnDuplicateSql_clicked(GtkWidget *widget, gpointer user_data) {
	GmlcGuiQuery * pGmlcGuiQry = GMLC_GUI_QUERY(user_data);
	GmlcGuiQuery * pGmlcGuiQryDup = NULL;
	gchar * pcSqlQuery;
	UNUSED_VAR(widget);
	
	pcSqlQuery = gmlc_gui_query_get_query(pGmlcGuiQry);
	
	pGmlcGuiQryDup = gmlc_gui_query_new(pGmlcGuiQry->pGmlcMysqlSrv, pGmlcGuiQry->pcDefDbName);
	gmlc_gui_query_set_query(pGmlcGuiQryDup, pcSqlQuery, FALSE);
	gtk_widget_show(GTK_WIDGET(pGmlcGuiQryDup));
	
	g_free(pcSqlQuery);
}

void gmlc_gui_query_evt_btnDumpSql_clicked(GtkWidget *widget, gpointer user_data) {
	GmlcGuiQuery * pGmlcGuiQry = GMLC_GUI_QUERY(user_data);
	/*p_gmysqlcc_gui_dump gui_dump;*/
	gchar * pcSqlQuery;
	UNUSED_VAR(widget);
	
	pcSqlQuery = gmlc_gui_query_get_query(pGmlcGuiQry);
	
	g_print("Dump query ...\n%s\n", pcSqlQuery);
	
/*
	gui_dump = gmlc_gui_dump_new(NULL, mysql_query_get_database(pGmlcGuiQry->mysql_qry), NULL, pcSqlQuery);
	gmlc_gui_dump_display(gui_dump, TRUE);
*/	
	g_free(pcSqlQuery);
}

void gmlc_gui_query_evt_btnServerList_clicked (GtkWidget *widget, gpointer user_data) {
/*	GmlcGuiQuery * pGmlcGuiQry = GMLC_GUI_QUERY(user_data);*/
	UNUSED_VAR(widget);
	UNUSED_VAR(user_data);

/*	
	gmysqlcc_gui_list_server_display(gmysqlcc_gui_list_server, TRUE);
*/
}

void gmlc_gui_query_evt_btnClose_clicked(GtkWidget *widget, gpointer user_data) {
	GmlcGuiQuery * pGmlcGuiQry = GMLC_GUI_QUERY(user_data);
	UNUSED_VAR(widget);
	
	gtk_widget_destroy(GTK_WIDGET(pGmlcGuiQry));
}

