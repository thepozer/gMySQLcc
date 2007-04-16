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
#include "gmlc_gui_helpers.h"
#include "gmlc_mysql_query.h"

#include <gdk/gdkkeysyms.h>

static void gmlc_gui_query_finalize (GmlcGuiQuery * pGmlcGuiQry);
static void gmlc_gui_query_create_widgets (GmlcGuiQuery * pGmlcGuiQry);
static void gmlc_gui_query_init_widgets (GmlcGuiQuery * pGmlcGuiQry);

void gmlc_gui_query_execute_query (GmlcGuiQuery * pGmlcGuiQry, const gchar * query);
void gmlc_gui_query_clean_all_result(GmlcGuiQuery * pGmlcGuiQry);
void gmlc_gui_query_display_all_result(GmlcGuiQuery * pGmlcGuiQry);
void gmlc_gui_query_display_one_result(GmlcGuiQuery * pGmlcGuiQry);
void gmlc_gui_query_display_one_info(GmlcGuiQuery * pGmlcGuiQry);

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
	pGmlcGuiQry->pGmlcMysqlQry = gmlc_mysql_query_new(pGmlcGuiQry->pGmlcMysqlSrv, pGmlcGuiQry->pcDefDbName);
	
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
	
	/*GtkTreeSelection *select;*/
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
	
	
	
	
	
	pGmlcGuiQry->tabSQLResult = gtk_notebook_new ();
	gtk_widget_show (pGmlcGuiQry->tabSQLResult);
	gtk_paned_pack2 (GTK_PANED (vpanedSQL), pGmlcGuiQry->tabSQLResult, TRUE, TRUE);

	pGmlcGuiQry->lstSQLResult = NULL;

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
			gmlc_gui_query_execute_query (pGmlcGuiQry, pcQuery);
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

void gmlc_gui_query_execute_query (GmlcGuiQuery * pGmlcGuiQry, const gchar * query) {
	GtkWidget * msgdlg;
	
	gmlc_mysql_query_free_result(pGmlcGuiQry->pGmlcMysqlQry);
	if (!gmlc_mysql_query_execute(pGmlcGuiQry->pGmlcMysqlQry, query, strlen(query), FALSE)) {
		/* if is it a write warning */
		if (pGmlcGuiQry->pGmlcMysqlQry->pGmlcMysqlSrv->bWriteWarning && pGmlcGuiQry->pGmlcMysqlQry->iErrCode == -1001) {
			if (askYesno(_("Write Warning !!!"), _("Warning !!! This server has been marked with Write Warning flags !!!\nIt is dangerous for the data ... :)\n\nDo you want force the execution of the query ?"))) {
				if (!gmlc_mysql_query_execute(pGmlcGuiQry->pGmlcMysqlQry, query, strlen(query), TRUE)) {
					/* Query Not Ok */
					msgdlg = gtk_message_dialog_new(GTK_WINDOW(pGmlcGuiQry), GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, _("Error during the query : (%d) %s"), pGmlcGuiQry->pGmlcMysqlQry->iErrCode, pGmlcGuiQry->pGmlcMysqlQry->pcErrMsg);
					gtk_dialog_run (GTK_DIALOG (msgdlg));
					gtk_widget_destroy (msgdlg);
					gmlc_mysql_query_free_result(pGmlcGuiQry->pGmlcMysqlQry);
					return;
				}
			} else {
				gmlc_mysql_query_free_result(pGmlcGuiQry->pGmlcMysqlQry);
				return;
			}
		} else { /* It is really an error */
			/* Query Not Ok */
			msgdlg = gtk_message_dialog_new(GTK_WINDOW(pGmlcGuiQry), GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, _("Error during the query : (%d) %s"), pGmlcGuiQry->pGmlcMysqlQry->iErrCode, pGmlcGuiQry->pGmlcMysqlQry->pcErrMsg);
			gtk_dialog_run (GTK_DIALOG (msgdlg));
			gtk_widget_destroy (msgdlg);
			gmlc_mysql_query_free_result(pGmlcGuiQry->pGmlcMysqlQry);
			return;
		}
	}
	
	/* Query Ok ... Continue */
	
	/* Select query - Display datas */
	
	pGmlcGuiQry->iNumResult = 0;
	
	gmlc_gui_query_clean_all_result(pGmlcGuiQry);
	
	gmlc_gui_query_display_all_result(pGmlcGuiQry);
}

void gmlc_gui_query_clean_all_result(GmlcGuiQuery * pGmlcGuiQry) {
	
	while (gtk_notebook_get_n_pages(GTK_NOTEBOOK(pGmlcGuiQry->tabSQLResult)) > 0) {
		
		gtk_notebook_remove_page(GTK_NOTEBOOK(pGmlcGuiQry->tabSQLResult), 0);
	}
}

void gmlc_gui_query_display_all_result(GmlcGuiQuery * pGmlcGuiQry) {
	
	do {
		/* Modification query - Display affected rows */
		if (pGmlcGuiQry->pGmlcMysqlQry->iErrCode == 0 && pGmlcGuiQry->pGmlcMysqlQry->iNbField == 0) {
			gmlc_gui_query_display_one_info(pGmlcGuiQry);
		} else {
			gmlc_gui_query_display_one_result(pGmlcGuiQry);
		}
	} while (gmlc_mysql_query_goto_next_result(pGmlcGuiQry->pGmlcMysqlQry));
}

void gmlc_gui_query_display_one_result(GmlcGuiQuery * pGmlcGuiQry) {
	GtkWidget * sclSQLResult = NULL;
	GtkWidget * lstSQLResult = NULL;
	/*GtkWidget * select = NULL;*/
	GtkWidget * lblTabTitle = NULL;
	GString * strTitle = NULL;
	GArray * arRow = NULL;
	GArray * arHeaders = NULL;
	gint i = 0;

	GtkTreeViewColumn * poCurrCol = NULL;
	GtkListStore * poLstStore = NULL;
	GtkCellRenderer * * ppoRenderer = NULL;
	GType * poTypes = NULL;
	gchar * pcColTitle = NULL;
	gint iNbrCol = 0;
	guint iCount = 0;
	GValue poGVal = {0, };
	GValue poGValPtr = {0, };
	GtkTreeIter iter;
	
/*
	sColsInfo * psColsInfo = NULL;
	guint sbContext, sbItemId;
	GValue gvalbool = {0, };
	GString *strSttsBar;
*/
	
	pGmlcGuiQry->iNumResult ++;
	
	/* Create widgets */
	
	strTitle = g_string_new("");
	g_string_append_printf(strTitle, _("Result %d"), pGmlcGuiQry->iNumResult);
	
	lblTabTitle = gtk_label_new(strTitle->str);
	gtk_widget_show (lblTabTitle);
	
	g_string_free(strTitle, TRUE);
	
	sclSQLResult = gtk_scrolled_window_new (NULL, NULL);
	gtk_widget_show (sclSQLResult);
	
	gtk_notebook_append_page(GTK_NOTEBOOK(pGmlcGuiQry->tabSQLResult), sclSQLResult, lblTabTitle);
	
	lstSQLResult = gtk_tree_view_new ();
	gtk_widget_show (lstSQLResult);
	gtk_tree_view_set_rules_hint(GTK_TREE_VIEW (lstSQLResult), TRUE);
	gtk_container_add (GTK_CONTAINER (sclSQLResult), lstSQLResult);

	/*select = gtk_tree_view_get_selection (GTK_TREE_VIEW (lstSQLResult));
	gtk_tree_selection_set_mode (select, GTK_SELECTION_SINGLE);
	g_signal_connect (G_OBJECT (select), "changed", G_CALLBACK (gmysqlcc_gui_query_evt_resultRow_selected), gui_pGmlcGuiQry);*/
	
	/* Create columns and display values */
	
	arRow = gmlc_mysql_query_next_record(pGmlcGuiQry->pGmlcMysqlQry); /* To get back fields headers */
	arHeaders = gmlc_mysql_query_get_headers(pGmlcGuiQry->pGmlcMysqlQry);
	
	if (arHeaders != NULL) {
		
		ppoRenderer = (GtkCellRenderer * *)g_try_malloc(arHeaders->len * sizeof(GtkCellRenderer *));
/*
		if (mysql_query_is_editable(gui_query->mysql_qry)) {
			psColsInfo = (sColsInfo *)g_try_malloc(arHeaders->len * sizeof(sColsInfo));
			
			g_value_init(&gvalbool, G_TYPE_BOOLEAN);
			g_value_set_boolean(&gvalbool, TRUE);
			
			for (i = 0; i < gui_query->mysql_qry->nbrField; i++) {
				ppoRenderer[i] = gtk_cell_renderer_text_new ();
				
				psColsInfo[i].gui_query = gui_query;
				psColsInfo[i].numCol = i;
				
				g_object_set_property(G_OBJECT(ppoRenderer[i]), "editable", &gvalbool);
				g_signal_connect (G_OBJECT (ppoRenderer[i]), "edited", G_CALLBACK (gmysqlcc_gui_query_evt_resultRow_edited), (gpointer)&psColsInfo[i]);
			}
		} else {
*/
			for (i = 0; i < arHeaders->len; i++) {
				ppoRenderer[i] = gtk_cell_renderer_text_new ();
			}
			g_print("Can't Edit :( !!!\n");
/*
		}
*/
		
		iNbrCol = arHeaders->len + 1;
		poTypes = (GType *)g_try_malloc(iNbrCol * sizeof(GType));
		
		for(i = 0; i < arHeaders->len; i++) {
			pcColTitle = gmlc_helpers_protect_underscore(g_array_index(arHeaders, gchar *, i));
			poCurrCol = gtk_tree_view_column_new_with_attributes (pcColTitle, ppoRenderer[i], "text", i, NULL);
			gtk_tree_view_column_set_resizable(GTK_TREE_VIEW_COLUMN(poCurrCol), TRUE);
			gtk_tree_view_column_set_sizing(GTK_TREE_VIEW_COLUMN(poCurrCol), GTK_TREE_VIEW_COLUMN_AUTOSIZE);
			
			gtk_tree_view_append_column (GTK_TREE_VIEW (lstSQLResult), poCurrCol);
			
			g_free(pcColTitle);
			
			poTypes[i] = G_TYPE_STRING;
		}
			
		poTypes[arHeaders->len] = G_TYPE_POINTER;
		
		poLstStore = gtk_list_store_newv(iNbrCol, poTypes);
		
		
		iCount = 0;
		g_value_init(&poGVal, G_TYPE_STRING);
		g_value_init(&poGValPtr, G_TYPE_POINTER);
		g_print ("*** Read data of new query \n");
		
		if (arRow != NULL) {
			do {
				iCount ++;
				
				gtk_list_store_append(poLstStore, &iter);
			
				for(i = 0; i < arRow->len; i++) {
					g_value_set_string(&poGVal, g_array_index(arRow, gchar *, i));
					gtk_list_store_set_value(poLstStore, &iter, i, &poGVal);
					
					g_free(g_array_index(arRow, gchar *, i));
				}
				g_value_set_pointer(&poGValPtr, NULL);
				gtk_list_store_set_value(poLstStore, &iter, arRow->len, &poGValPtr);
				
				g_array_free(arRow, TRUE);
			} while ((arRow = gmlc_mysql_query_next_record(pGmlcGuiQry->pGmlcMysqlQry)) != NULL);
		}
		
		gtk_tree_view_set_model(GTK_TREE_VIEW(lstSQLResult), GTK_TREE_MODEL(poLstStore));
		g_object_unref (G_OBJECT (poLstStore));
	}
	
}

void gmlc_gui_query_display_one_info(GmlcGuiQuery * pGmlcGuiQry) {
	GtkTextBuffer * txtBuffer;
	GtkWidget * sclSQLResult = NULL;
	GtkWidget * txtSQLInfo = NULL;
	GtkWidget * lblTabTitle = NULL;
	PangoFontDescription * pCourierFontDesc = NULL;
	GString * strTitle = NULL;
	GString * strInfos = NULL;
	
	pGmlcGuiQry->iNumResult ++;
	
	/* Create widgets */
	
	strTitle = g_string_new("");
	g_string_append_printf(strTitle, _("Query ok.\nResult %d"), pGmlcGuiQry->iNumResult);
	
	lblTabTitle = gtk_label_new(strTitle->str);
	gtk_widget_show (lblTabTitle);
	
	g_string_free(strTitle, TRUE);
	
	sclSQLResult = gtk_scrolled_window_new (NULL, NULL);
	gtk_widget_show (sclSQLResult);
	
	gtk_notebook_append_page(GTK_NOTEBOOK(pGmlcGuiQry->tabSQLResult), sclSQLResult, lblTabTitle);
	
	txtSQLInfo = gtk_text_view_new ();
	pCourierFontDesc = pango_font_description_from_string(GpGmlcMscCfg->pcQueryFontName);
	gtk_widget_modify_font(txtSQLInfo, pCourierFontDesc);
	pango_font_description_free(pCourierFontDesc);
	gtk_widget_show (txtSQLInfo);
	gtk_container_add (GTK_CONTAINER (sclSQLResult), txtSQLInfo);
	
	/* Display information */
	strInfos = g_string_new("");
	g_string_append_printf(strInfos, _("Affected row : %d"), pGmlcGuiQry->pGmlcMysqlQry->iEditResult);
	
	txtBuffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(txtSQLInfo));
	gtk_text_buffer_set_text(GTK_TEXT_BUFFER(txtBuffer), strInfos->str, -1);
	
	g_string_free(strInfos, TRUE);
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

	g_print("Execute SQL\n");
	pcSqlQuery = gmlc_gui_query_get_query(pGmlcGuiQry);
	g_print("SQL Query : '%s'\n", pcSqlQuery);
	

	gmlc_gui_query_execute_query(pGmlcGuiQry, pcSqlQuery);

	
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

