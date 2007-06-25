/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License.
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

#include "gmlc_gui_server_tab.h"
#include "gmlc_gui_server_tab_query.h"
#include "gmlc_gui_helpers.h"
#include "gmlc_mysql_query.h"

#include <gdk/gdkkeysyms.h>

static void gmlc_gui_server_tab_query_finalize (GmlcGuiServerTabQuery * pGmlcGuiSrvTabQuery);
static void gmlc_gui_server_tab_query_get_property (GObject * object, guint prop_id, GValue * value, GParamSpec * pspec);
static void gmlc_gui_server_tab_query_set_property (GObject * object, guint prop_id, const GValue * value, GParamSpec * pspec);

static void gmlc_gui_server_tab_query_interface_init (gpointer g_iface, gpointer iface_data);
static void gmlc_gui_server_tab_query_update_ui (GmlcGuiServerTabQuery * pGmlcGuiSrvTabQuery, gboolean bShow);

void gmlc_gui_server_tab_query_create_toolbar_items (GmlcGuiServerTabQuery * pGmlcGuiSrvTabQuery);
void gmlc_gui_server_tab_query_create_widgets (GmlcGuiServerTabQuery * pGmlcGuiSrvTabQuery);
void gmlc_gui_server_tab_query_init_widgets (GmlcGuiServerTabQuery * pGmlcGuiSrvTabQuery);

void gmlc_gui_server_tab_query_execute_query (GmlcGuiServerTabQuery * pGmlcGuiSrvTabQuery, const gchar * query);
void gmlc_gui_server_tab_query_clean_all_result(GmlcGuiServerTabQuery * pGmlcGuiSrvTabQuery);
void gmlc_gui_server_tab_query_display_all_result(GmlcGuiServerTabQuery * pGmlcGuiSrvTabQuery);
void gmlc_gui_server_tab_query_display_one_result(GmlcGuiServerTabQuery * pGmlcGuiSrvTabQuery);
void gmlc_gui_server_tab_query_display_one_info(GmlcGuiServerTabQuery * pGmlcGuiSrvTabQuery);

gboolean gmlc_gui_server_tab_query_evt_window_keyrelease (GtkWidget *widget, GdkEventKey *event, gpointer user_data);

void gmlc_gui_server_tab_query_load_from_file (GmlcGuiServerTabQuery * pGmlcGuiSrvTabQuery, const gchar * pcFileName);
void gmlc_gui_server_tab_query_save_to_file (GmlcGuiServerTabQuery * pGmlcGuiSrvTabQuery, const gchar * pcFileName);
void gmlc_gui_server_tab_query_evt_btn_save_clicked(GtkWidget *widget, gpointer user_data);

void gmlc_gui_server_tab_query_evt_btnExecSql_clicked(GtkWidget *widget, gpointer user_data);
void gmlc_gui_server_tab_query_evt_btnDuplicateSql_clicked(GtkWidget *widget, gpointer user_data);
void gmlc_gui_server_tab_query_evt_btnDumpSql_clicked(GtkWidget *widget, gpointer user_data);
void gmlc_gui_server_tab_query_evt_btnClose_clicked(GtkWidget *widget, gpointer user_data);

enum {
	PROP_0,
	PROP_SERVER,
	PROP_SERVER_WINDOW,
	PROP_DATABASE_NAME,
	PROP_FILENAME,
};

G_DEFINE_TYPE_WITH_CODE (GmlcGuiServerTabQuery, gmlc_gui_server_tab_query, GTK_TYPE_VBOX, 
	G_IMPLEMENT_INTERFACE (GMLC_GUI_TYPE_SERVER_TAB, gmlc_gui_server_tab_query_interface_init));

static void gmlc_gui_server_tab_query_interface_init (gpointer g_iface, gpointer iface_data) {
  GmlcGuiServerTabInterface *pIface = (GmlcGuiServerTabInterface *)g_iface;
  pIface->update_ui = (void (*) (GmlcGuiServerTab * self, gboolean bShow))gmlc_gui_server_tab_query_update_ui;
}

static void gmlc_gui_server_tab_query_class_init (GmlcGuiServerTabQueryClass * pClass) {
	GObjectClass *pObjClass = G_OBJECT_CLASS(pClass);
	
	pObjClass->finalize = (GObjectFinalizeFunc) gmlc_gui_server_tab_query_finalize;
	
	pObjClass->get_property = gmlc_gui_server_tab_query_get_property;
	pObjClass->set_property = gmlc_gui_server_tab_query_set_property;
	
	g_object_class_install_property(pObjClass, PROP_SERVER, 
		g_param_spec_object("server", "Server object", "Server object", GMLC_MYSQL_TYPE_SERVER, G_PARAM_READABLE));
	g_object_class_install_property(pObjClass, PROP_SERVER_WINDOW, 
		g_param_spec_object("server_window", "Server window object", "Server window object", GMLC_GUI_TYPE_SERVER, G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY));
	g_object_class_install_property(pObjClass, PROP_DATABASE_NAME, 
		g_param_spec_string("db_name", "Database name", "Name of the database", "", G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY));
	g_object_class_install_property(pObjClass, PROP_FILENAME, 
		g_param_spec_string("filename", "File name", "File name of the sql script", "", G_PARAM_READWRITE));
}

static void gmlc_gui_server_tab_query_get_property (GObject * object, guint prop_id, GValue * value, GParamSpec * pspec) {
	GmlcGuiServerTabQuery * pGmlcGuiSrvTabQuery = GMLC_GUI_SERVER_TAB_QUERY(object);
	
	switch (prop_id) {
		case PROP_SERVER :
			g_value_set_object(value, pGmlcGuiSrvTabQuery->pGmlcMysqlSrv);
			break;
		case PROP_SERVER_WINDOW :
			g_value_set_object(value, pGmlcGuiSrvTabQuery->pGmlcGuiSrv);
			break;
		case PROP_DATABASE_NAME :
			g_value_set_string(value, pGmlcGuiSrvTabQuery->pcDbName);
			break;
		case PROP_FILENAME :
			g_value_set_string(value, pGmlcGuiSrvTabQuery->pcFileName);
			break;
		default: {
			G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
			break;
		}
	}
}

static void gmlc_gui_server_tab_query_set_property (GObject * object, guint prop_id, const GValue * value, GParamSpec * pspec) {
	GmlcGuiServerTabQuery * pGmlcGuiSrvTabQuery = GMLC_GUI_SERVER_TAB_QUERY(object);
	
	switch (prop_id) {
		case PROP_SERVER_WINDOW :
			pGmlcGuiSrvTabQuery->pGmlcGuiSrv = g_value_get_object(value);
			g_object_get(pGmlcGuiSrvTabQuery->pGmlcGuiSrv, "server", &pGmlcGuiSrvTabQuery->pGmlcMysqlSrv, NULL);
			
			gmlc_gui_server_tab_query_create_toolbar_items(pGmlcGuiSrvTabQuery);
			gmlc_gui_server_tab_query_create_widgets(pGmlcGuiSrvTabQuery);
			gmlc_gui_server_tab_query_init_widgets(pGmlcGuiSrvTabQuery);
			
			pGmlcGuiSrvTabQuery->pGmlcMysqlQry = gmlc_mysql_query_new(pGmlcGuiSrvTabQuery->pGmlcMysqlSrv, pGmlcGuiSrvTabQuery->pcDbName);
			break;
		case PROP_DATABASE_NAME :
			g_free(pGmlcGuiSrvTabQuery->pcDbName);
			
			pGmlcGuiSrvTabQuery->pcDbName = g_value_dup_string(value);
			break;
		case PROP_FILENAME :
			g_free(pGmlcGuiSrvTabQuery->pcFileName);
			
			pGmlcGuiSrvTabQuery->pcFileName = g_value_dup_string(value);
			break;
		default: {
			G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
			break;
		}
	}
}

static void gmlc_gui_server_tab_query_init (GmlcGuiServerTabQuery * pGmlcGuiSrvTabQuery) {
	
	pGmlcGuiSrvTabQuery->pcDbName = NULL;
	pGmlcGuiSrvTabQuery->pGmlcMysqlSrv = NULL;
	pGmlcGuiSrvTabQuery->pGmlcMysqlQry = NULL;
	pGmlcGuiSrvTabQuery->pcFileName = NULL;
	
	g_signal_connect (G_OBJECT (pGmlcGuiSrvTabQuery), "key-release-event", G_CALLBACK (gmlc_gui_server_tab_query_evt_window_keyrelease), pGmlcGuiSrvTabQuery);
}

static void gmlc_gui_server_tab_query_finalize (GmlcGuiServerTabQuery * pGmlcGuiSrvTabQuery) {
	
	if (GTK_IS_WIDGET(pGmlcGuiSrvTabQuery->poQueryToolbar)) {
		gtk_widget_destroy(pGmlcGuiSrvTabQuery->poQueryToolbar);
	}
}

static void gmlc_gui_server_tab_query_update_ui (GmlcGuiServerTabQuery * pGmlcGuiSrvTabQuery, gboolean bShow) {
	
	if (bShow) {
		gtk_widget_show(pGmlcGuiSrvTabQuery->poQueryToolbar);
	} else {
		gtk_widget_hide(pGmlcGuiSrvTabQuery->poQueryToolbar);
	}
}

GmlcGuiServerTabQuery * gmlc_gui_server_tab_query_new (GmlcGuiServer * pGmlcGuiSrv, const gchar * pcDefDbName) {
	GmlcGuiServerTabQuery * pGmlcGuiSrvTabQuery = NULL;
	
	pGmlcGuiSrvTabQuery = GMLC_GUI_SERVER_TAB_QUERY(g_object_new(GMLC_GUI_TYPE_SERVER_TAB_QUERY, "db_name", pcDefDbName, "server_window", pGmlcGuiSrv, NULL));
	
	return pGmlcGuiSrvTabQuery;
}

void gmlc_gui_server_tab_query_create_toolbar_items (GmlcGuiServerTabQuery * pGmlcGuiSrvTabQuery) {
	GtkWidget * poHBoxToolbar = NULL;
	GtkWidget * poImgBtn = NULL;
	GtkWidget * poBtn = NULL;
	
	g_object_get(pGmlcGuiSrvTabQuery->pGmlcGuiSrv, "toolbar-hbox", &poHBoxToolbar, NULL);
	
	pGmlcGuiSrvTabQuery->poQueryToolbar = gtk_hbox_new (FALSE, 2);
	gtk_widget_show (pGmlcGuiSrvTabQuery->poQueryToolbar);
	gtk_box_pack_start (GTK_BOX (poHBoxToolbar), pGmlcGuiSrvTabQuery->poQueryToolbar, TRUE, TRUE, 0);
	
	
	poBtn = gtk_button_new_from_stock(GTK_STOCK_OPEN);
	gtk_button_set_relief(GTK_BUTTON(poBtn), GTK_RELIEF_NONE);
	g_signal_connect (poBtn, "clicked", G_CALLBACK (gmlc_gui_server_tab_query_evt_btn_load_clicked), pGmlcGuiSrvTabQuery);
	gtk_widget_show(GTK_WIDGET(poBtn));
	gtk_box_pack_start(GTK_BOX (pGmlcGuiSrvTabQuery->poQueryToolbar), poBtn, FALSE, FALSE, 0);
	
	poBtn = gtk_button_new_from_stock(GTK_STOCK_SAVE);
	gtk_button_set_relief(GTK_BUTTON(poBtn), GTK_RELIEF_NONE);
	g_signal_connect (poBtn, "clicked", G_CALLBACK (gmlc_gui_server_tab_query_evt_btn_save_clicked), pGmlcGuiSrvTabQuery);
	gtk_widget_show(GTK_WIDGET(poBtn));
	gtk_box_pack_start(GTK_BOX (pGmlcGuiSrvTabQuery->poQueryToolbar), poBtn, FALSE, FALSE, 0);
	
	poImgBtn = gtk_image_new_from_stock(GTK_STOCK_EXECUTE, GTK_ICON_SIZE_LARGE_TOOLBAR);
	gtk_widget_show(poImgBtn);
	poBtn = gtk_button_new_with_label(_("Execute"));
	gtk_button_set_image(GTK_BUTTON(poBtn), poImgBtn);
	gtk_button_set_relief(GTK_BUTTON(poBtn), GTK_RELIEF_NONE);
	g_signal_connect(poBtn, "clicked", G_CALLBACK (gmlc_gui_server_tab_query_evt_btnExecSql_clicked), pGmlcGuiSrvTabQuery);
	gtk_widget_show(poBtn);
	gtk_box_pack_start(GTK_BOX(pGmlcGuiSrvTabQuery->poQueryToolbar), poBtn, FALSE, FALSE, 0);
	
	poImgBtn = gtk_image_new_from_stock(GTK_STOCK_COPY, GTK_ICON_SIZE_LARGE_TOOLBAR);
	gtk_widget_show(poImgBtn);
	poBtn = gtk_button_new_with_label(_("Duplicate"));
	gtk_button_set_image(GTK_BUTTON(poBtn), poImgBtn);
	gtk_button_set_relief(GTK_BUTTON(poBtn), GTK_RELIEF_NONE);
	g_signal_connect(poBtn, "clicked", G_CALLBACK (gmlc_gui_server_tab_query_evt_btnDuplicateSql_clicked), pGmlcGuiSrvTabQuery);
	gtk_widget_show(poBtn);
	gtk_box_pack_start(GTK_BOX(pGmlcGuiSrvTabQuery->poQueryToolbar), poBtn, FALSE, FALSE, 0);
	
	poImgBtn = gtk_image_new_from_stock(GTK_STOCK_FLOPPY, GTK_ICON_SIZE_LARGE_TOOLBAR);
	gtk_widget_show(poImgBtn);
	poBtn = gtk_button_new_with_label(_("Dump"));
	gtk_button_set_image(GTK_BUTTON(poBtn), poImgBtn);
	gtk_button_set_relief(GTK_BUTTON(poBtn), GTK_RELIEF_NONE);
/*	g_signal_connect(poBtn, "clicked", G_CALLBACK (gmlc_gui_server_tab_query_evt_btnDumpSql_clicked), pGmlcGuiSrvTabQuery);*/
	gtk_widget_show(poBtn);
	gtk_box_pack_start(GTK_BOX(pGmlcGuiSrvTabQuery->poQueryToolbar), poBtn, FALSE, FALSE, 0);
	
	poImgBtn = gtk_image_new_from_stock(GTK_STOCK_CLOSE, GTK_ICON_SIZE_LARGE_TOOLBAR);
	gtk_widget_show(poImgBtn);
	poBtn = gtk_button_new_with_label(_("Close tab"));
	gtk_button_set_image(GTK_BUTTON(poBtn), poImgBtn);
	gtk_button_set_relief(GTK_BUTTON(poBtn), GTK_RELIEF_NONE);
	g_signal_connect(poBtn, "clicked", G_CALLBACK (gmlc_gui_server_tab_query_evt_btnClose_clicked), pGmlcGuiSrvTabQuery);
	gtk_widget_show(poBtn);
	gtk_box_pack_start(GTK_BOX(pGmlcGuiSrvTabQuery->poQueryToolbar), poBtn, FALSE, FALSE, 0);
	
}

void gmlc_gui_server_tab_query_create_widgets (GmlcGuiServerTabQuery * pGmlcGuiSrvTabQuery) {
	GtkWidget *vpanedSQL;
	GtkWidget *scrlwndSQLRequest;
	GtkTooltips * tooltips;
	PangoFontDescription * pCourierFontDesc = NULL;
	glong lServerVersion = 0;
	
	/*GtkTreeSelection *select;*/
#ifdef USE_GTKSOURCEVIEW
	GtkTextBuffer * txtBuffer;
#endif /* USE_GTKSOURCEVIEW */
	
	tooltips = gtk_tooltips_new();
	g_object_get(G_OBJECT(pGmlcGuiSrvTabQuery->pGmlcMysqlSrv), "version", &lServerVersion, NULL);

	vpanedSQL = gtk_vpaned_new ();
	gtk_widget_show (vpanedSQL);
	gtk_box_pack_start (GTK_BOX (pGmlcGuiSrvTabQuery), vpanedSQL, TRUE, TRUE, 0);
	gtk_paned_set_position (GTK_PANED (vpanedSQL), 100);
	
	scrlwndSQLRequest = gtk_scrolled_window_new (NULL, NULL);
	gtk_widget_show (scrlwndSQLRequest);
	gtk_paned_pack1 (GTK_PANED (vpanedSQL), scrlwndSQLRequest, FALSE, TRUE);

#ifdef USE_GTKSOURCEVIEW
	pGmlcGuiSrvTabQuery->txtSQLRequest = gtk_source_view_new ();
	gtk_source_view_set_show_line_numbers (GTK_SOURCE_VIEW(pGmlcGuiSrvTabQuery->txtSQLRequest), TRUE);
	gtk_source_view_set_tabs_width (GTK_SOURCE_VIEW(pGmlcGuiSrvTabQuery->txtSQLRequest), 2);
	txtBuffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(pGmlcGuiSrvTabQuery->txtSQLRequest));
	gtk_source_buffer_set_highlight (GTK_SOURCE_BUFFER(txtBuffer), TRUE);
	gtk_source_buffer_set_language(GTK_SOURCE_BUFFER(txtBuffer), 
			gtk_source_languages_manager_get_language_from_mime_type(LangManager, "text/x-sql"));
#else /* USE_GTKSOURCEVIEW */
	pGmlcGuiSrvTabQuery->txtSQLRequest = gtk_text_view_new ();
#endif /* USE_GTKSOURCEVIEW */
	gtk_widget_show (pGmlcGuiSrvTabQuery->txtSQLRequest);
	gtk_container_add (GTK_CONTAINER (scrlwndSQLRequest), pGmlcGuiSrvTabQuery->txtSQLRequest);
	
	pCourierFontDesc = pango_font_description_from_string(GpGmlcMscCfg->pcQueryFontName);
	gtk_widget_modify_font(pGmlcGuiSrvTabQuery->txtSQLRequest, pCourierFontDesc);
	pango_font_description_free(pCourierFontDesc);
	
	pGmlcGuiSrvTabQuery->tabSQLResult = gtk_notebook_new ();
	gtk_notebook_set_scrollable(GTK_NOTEBOOK(pGmlcGuiSrvTabQuery->tabSQLResult), TRUE);
	gtk_widget_show (pGmlcGuiSrvTabQuery->tabSQLResult);
	gtk_paned_pack2 (GTK_PANED (vpanedSQL), pGmlcGuiSrvTabQuery->tabSQLResult, TRUE, TRUE);

	pGmlcGuiSrvTabQuery->lstSQLResult = NULL;

	pGmlcGuiSrvTabQuery->statusbarSQL = NULL;
	/*
	pGmlcGuiSrvTabQuery->statusbarSQL = gtk_statusbar_new ();
	gtk_widget_show (pGmlcGuiSrvTabQuery->statusbarSQL);
	gtk_box_pack_start (GTK_BOX (pGmlcGuiSrvTabQuery), pGmlcGuiSrvTabQuery->statusbarSQL, FALSE, FALSE, 0);
	*/
}

void gmlc_gui_server_tab_query_init_widgets (GmlcGuiServerTabQuery * pGmlcGuiSrvTabQuery) {
	UNUSED_VAR(pGmlcGuiSrvTabQuery);
}

gboolean gmlc_gui_server_tab_query_set_query (GmlcGuiServerTabQuery * pGmlcGuiSrvTabQuery, const gchar * pcQuery, gboolean bExecNow) {
	GtkTextBuffer * txtBuffer;

	if (pcQuery != NULL) {
		txtBuffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(pGmlcGuiSrvTabQuery->txtSQLRequest));
		gtk_text_buffer_set_text(GTK_TEXT_BUFFER(txtBuffer), pcQuery, -1);
		if (bExecNow) {
			gmlc_gui_server_tab_query_execute_query (pGmlcGuiSrvTabQuery, pcQuery);
		}
		return TRUE;
	}
	
	return FALSE;
}

gchar * gmlc_gui_server_tab_query_get_query (GmlcGuiServerTabQuery * pGmlcGuiSrvTabQuery) {
	GtkTextBuffer * txtBuffer;
	GtkTextIter begin, end;
	gchar * pcSqlQuery;
	
	txtBuffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(pGmlcGuiSrvTabQuery->txtSQLRequest));
	gtk_text_buffer_get_start_iter (GTK_TEXT_BUFFER(txtBuffer), &begin);
	gtk_text_buffer_get_end_iter (GTK_TEXT_BUFFER(txtBuffer), &end);
	pcSqlQuery = gtk_text_buffer_get_text(GTK_TEXT_BUFFER(txtBuffer), &begin, &end, FALSE);
	
	return pcSqlQuery;
}

void gmlc_gui_server_tab_query_execute_query (GmlcGuiServerTabQuery * pGmlcGuiSrvTabQuery, const gchar * query) {
	GtkWidget * msgdlg;
	
	gmlc_mysql_query_free_result(pGmlcGuiSrvTabQuery->pGmlcMysqlQry);
	if (!gmlc_mysql_query_execute(pGmlcGuiSrvTabQuery->pGmlcMysqlQry, query, strlen(query), FALSE)) {
		/* if is it a write warning */
		if (pGmlcGuiSrvTabQuery->pGmlcMysqlQry->pGmlcMysqlSrv->bWriteWarning && pGmlcGuiSrvTabQuery->pGmlcMysqlQry->iErrCode == -1001) {
			if (askYesno(_("Write Warning !!!"), _("Warning !!! This server has been marked with Write Warning flags !!!\nIt is dangerous for the data ... :)\n\nDo you want force the execution of the query ?"))) {
				if (!gmlc_mysql_query_execute(pGmlcGuiSrvTabQuery->pGmlcMysqlQry, query, strlen(query), TRUE)) {
					/* Query Not Ok */
					msgdlg = gtk_message_dialog_new(GTK_WINDOW(pGmlcGuiSrvTabQuery->pGmlcGuiSrv), GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, _("Error during the query : (%d) %s"), pGmlcGuiSrvTabQuery->pGmlcMysqlQry->iErrCode, pGmlcGuiSrvTabQuery->pGmlcMysqlQry->pcErrMsg);
					gtk_dialog_run (GTK_DIALOG (msgdlg));
					gtk_widget_destroy (msgdlg);
					gmlc_mysql_query_free_result(pGmlcGuiSrvTabQuery->pGmlcMysqlQry);
					return;
				}
			} else {
				gmlc_mysql_query_free_result(pGmlcGuiSrvTabQuery->pGmlcMysqlQry);
				return;
			}
		} else { /* It is really an error */
			/* Query Not Ok */
			msgdlg = gtk_message_dialog_new(GTK_WINDOW(pGmlcGuiSrvTabQuery->pGmlcGuiSrv), GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, _("Error during the query : (%d) %s"), pGmlcGuiSrvTabQuery->pGmlcMysqlQry->iErrCode, pGmlcGuiSrvTabQuery->pGmlcMysqlQry->pcErrMsg);
			gtk_dialog_run (GTK_DIALOG (msgdlg));
			gtk_widget_destroy (msgdlg);
			gmlc_mysql_query_free_result(pGmlcGuiSrvTabQuery->pGmlcMysqlQry);
			return;
		}
	}
	
	/* Query Ok ... Continue */
	
	/* Select query - Display datas */
	
	pGmlcGuiSrvTabQuery->iNumResult = 0;
	
	gmlc_gui_server_tab_query_clean_all_result(pGmlcGuiSrvTabQuery);
	
	gmlc_gui_server_tab_query_display_all_result(pGmlcGuiSrvTabQuery);
}

void gmlc_gui_server_tab_query_clean_all_result(GmlcGuiServerTabQuery * pGmlcGuiSrvTabQuery) {
	
	while (gtk_notebook_get_n_pages(GTK_NOTEBOOK(pGmlcGuiSrvTabQuery->tabSQLResult)) > 0) {
		
		gtk_notebook_remove_page(GTK_NOTEBOOK(pGmlcGuiSrvTabQuery->tabSQLResult), 0);
	}
}

void gmlc_gui_server_tab_query_display_all_result(GmlcGuiServerTabQuery * pGmlcGuiSrvTabQuery) {
	GtkWidget * msgdlg;
	
	do {
		/* Modification query - Display affected rows */
		if (pGmlcGuiSrvTabQuery->pGmlcMysqlQry->iErrCode == 0 && pGmlcGuiSrvTabQuery->pGmlcMysqlQry->iNbField == 0) {
			gmlc_gui_server_tab_query_display_one_info(pGmlcGuiSrvTabQuery);
		} else {
			gmlc_gui_server_tab_query_display_one_result(pGmlcGuiSrvTabQuery);
		}
		
	} while (gmlc_mysql_query_goto_next_result(pGmlcGuiSrvTabQuery->pGmlcMysqlQry));
	
	if (pGmlcGuiSrvTabQuery->pGmlcMysqlQry->iErrCode != 0) {
		/* Query Not Ok */
		msgdlg = gtk_message_dialog_new(GTK_WINDOW(pGmlcGuiSrvTabQuery->pGmlcGuiSrv), GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, _("Error during the query : (%d) %s"), pGmlcGuiSrvTabQuery->pGmlcMysqlQry->iErrCode, pGmlcGuiSrvTabQuery->pGmlcMysqlQry->pcErrMsg);
		gtk_dialog_run (GTK_DIALOG (msgdlg));
		gtk_widget_destroy (msgdlg);
		gmlc_mysql_query_free_result(pGmlcGuiSrvTabQuery->pGmlcMysqlQry);
	}
}

void gmlc_gui_server_tab_query_display_one_result(GmlcGuiServerTabQuery * pGmlcGuiSrvTabQuery) {
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
	
	pGmlcGuiSrvTabQuery->iNumResult ++;
	
	/* Create widgets */
	
	strTitle = g_string_new("");
	g_string_append_printf(strTitle, _("Result %d"), pGmlcGuiSrvTabQuery->iNumResult);
	
	lblTabTitle = gtk_label_new(strTitle->str);
	gtk_widget_show (lblTabTitle);
	
	g_string_free(strTitle, TRUE);
	
	sclSQLResult = gtk_scrolled_window_new (NULL, NULL);
	gtk_widget_show (sclSQLResult);
	
	gtk_notebook_append_page(GTK_NOTEBOOK(pGmlcGuiSrvTabQuery->tabSQLResult), sclSQLResult, lblTabTitle);
	
	lstSQLResult = gtk_tree_view_new ();
	gtk_widget_show (lstSQLResult);
	gtk_tree_view_set_rules_hint(GTK_TREE_VIEW (lstSQLResult), TRUE);
	gtk_container_add (GTK_CONTAINER (sclSQLResult), lstSQLResult);

	/*
	select = gtk_tree_view_get_selection (GTK_TREE_VIEW (lstSQLResult));
	gtk_tree_selection_set_mode (select, GTK_SELECTION_SINGLE);
	g_signal_connect (G_OBJECT (select), "changed", G_CALLBACK (gmysqlcc_gui_server_tab_query_evt_resultRow_selected), gui_pGmlcGuiSrvTabQuery);
	*/
	
	/* Create columns and display values */
	
	arRow = gmlc_mysql_query_next_record(pGmlcGuiSrvTabQuery->pGmlcMysqlQry); /* To get back fields headers */
	arHeaders = gmlc_mysql_query_get_headers(pGmlcGuiSrvTabQuery->pGmlcMysqlQry);
	
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
				g_signal_connect (G_OBJECT (ppoRenderer[i]), "edited", G_CALLBACK (gmysqlcc_gui_server_tab_query_evt_resultRow_edited), (gpointer)&psColsInfo[i]);
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
			} while ((arRow = gmlc_mysql_query_next_record(pGmlcGuiSrvTabQuery->pGmlcMysqlQry)) != NULL);
		}
		
		gtk_tree_view_set_model(GTK_TREE_VIEW(lstSQLResult), GTK_TREE_MODEL(poLstStore));
		g_object_unref (G_OBJECT (poLstStore));
	}
	
}

void gmlc_gui_server_tab_query_display_one_info(GmlcGuiServerTabQuery * pGmlcGuiSrvTabQuery) {
	GtkTextBuffer * txtBuffer;
	GtkWidget * sclSQLResult = NULL;
	GtkWidget * txtSQLInfo = NULL;
	GtkWidget * lblTabTitle = NULL;
	PangoFontDescription * pCourierFontDesc = NULL;
	GString * strTitle = NULL;
	GString * strInfos = NULL;
	
	pGmlcGuiSrvTabQuery->iNumResult ++;
	
	/* Create widgets */
	
	strTitle = g_string_new("");
	g_string_append_printf(strTitle, _("Result %d"), pGmlcGuiSrvTabQuery->iNumResult);
	
	lblTabTitle = gtk_label_new(strTitle->str);
	gtk_widget_show (lblTabTitle);
	
	g_string_free(strTitle, TRUE);
	
	sclSQLResult = gtk_scrolled_window_new (NULL, NULL);
	gtk_widget_show (sclSQLResult);
	
	gtk_notebook_append_page(GTK_NOTEBOOK(pGmlcGuiSrvTabQuery->tabSQLResult), sclSQLResult, lblTabTitle);
	
	txtSQLInfo = gtk_text_view_new ();
	pCourierFontDesc = pango_font_description_from_string(GpGmlcMscCfg->pcQueryFontName);
	gtk_widget_modify_font(txtSQLInfo, pCourierFontDesc);
	pango_font_description_free(pCourierFontDesc);
	gtk_widget_show (txtSQLInfo);
	gtk_container_add (GTK_CONTAINER (sclSQLResult), txtSQLInfo);
	
	/* Display information */
	strInfos = g_string_new("");
	g_string_append_printf(strInfos, _("Query ok.\nAffected row : %d"), pGmlcGuiSrvTabQuery->pGmlcMysqlQry->iEditResult);
	
	txtBuffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(txtSQLInfo));
	gtk_text_buffer_set_text(GTK_TEXT_BUFFER(txtBuffer), strInfos->str, -1);
	
	g_string_free(strInfos, TRUE);
}

gboolean gmlc_gui_server_tab_query_evt_window_keyrelease (GtkWidget *widget, GdkEventKey *event, gpointer user_data) {
	
	if ((event->state & GDK_CONTROL_MASK && (event->keyval == GDK_e || event->keyval == GDK_E)) 
		|| (event->keyval == GDK_F5)) {
		gmlc_gui_server_tab_query_evt_btnExecSql_clicked (widget, user_data);
		return TRUE;
	}
	
	return FALSE;
}

void gmlc_gui_server_tab_query_load_from_file (GmlcGuiServerTabQuery * pGmlcGuiSrvTabQuery, const gchar * pcFileName) {
	gchar * content = NULL;
	GtkTextBuffer * txtBuffer;
	GIOChannel * loadFile;
	GError * err = NULL;
	gsize nbBytes;
	
	g_return_if_fail (pcFileName != NULL);
	
	g_object_set(pGmlcGuiSrvTabQuery, "filename", pcFileName, NULL);
	
	loadFile = g_io_channel_new_file(pcFileName, "r", &err);
	g_io_channel_set_encoding(loadFile, "ISO-8859-15", &err);
	if (g_io_channel_read_to_end(loadFile, &content, &nbBytes, &err) != G_IO_STATUS_NORMAL) {
		content = NULL;
	}
	g_io_channel_unref(loadFile);
	
	txtBuffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(pGmlcGuiSrvTabQuery->txtSQLRequest));
	gtk_text_buffer_set_text(GTK_TEXT_BUFFER(txtBuffer), content, -1);
}

void gmlc_gui_server_tab_query_evt_btn_load_clicked(GtkWidget *widget, gpointer user_data) {
	GmlcGuiServerTabQuery * pGmlcGuiSrvTabQuery = GMLC_GUI_SERVER_TAB_QUERY(user_data);
	GtkWidget * poChooser = NULL;
	gint response;
	
	poChooser = gtk_file_chooser_dialog_new (_("Load sql file"), GTK_WINDOW(pGmlcGuiSrvTabQuery->pGmlcGuiSrv), GTK_FILE_CHOOSER_ACTION_OPEN,
		GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, GTK_STOCK_OPEN, GTK_RESPONSE_OK, NULL);
	
	if (pGmlcGuiSrvTabQuery->pcFileName != NULL) {
		gtk_file_chooser_set_filename (GTK_FILE_CHOOSER (poChooser), pGmlcGuiSrvTabQuery->pcFileName);
	}
	
	response = gtk_dialog_run (GTK_DIALOG (poChooser));
	if (response == GTK_RESPONSE_OK) {
		gchar * pcFileName = NULL;
		
		pcFileName = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (poChooser));
		if (pcFileName != NULL) {
			gmlc_gui_server_tab_query_load_from_file (pGmlcGuiSrvTabQuery, pcFileName);
			g_free (pcFileName);
		}
	}
	
	gtk_widget_destroy (poChooser);
}

void gmlc_gui_server_tab_query_save_to_file (GmlcGuiServerTabQuery * pGmlcGuiSrvTabQuery, const gchar * pcFileName) {
	gchar * content = NULL;
	GtkTextBuffer * txtBuffer;
	GtkTextIter begin, end;
	GIOChannel * saveFile;
	GError * err = NULL;
	gsize nbBytes;
	
	g_return_if_fail (pcFileName != NULL);
	
	g_object_set(pGmlcGuiSrvTabQuery, "filename", pcFileName, NULL);
	
	txtBuffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(pGmlcGuiSrvTabQuery->txtSQLRequest));
	gtk_text_buffer_get_start_iter (GTK_TEXT_BUFFER(txtBuffer), &begin);
	gtk_text_buffer_get_end_iter (GTK_TEXT_BUFFER(txtBuffer), &end);
	content = (gchar *)gtk_text_buffer_get_text (GTK_TEXT_BUFFER(txtBuffer), &begin, &end, FALSE);

	saveFile = g_io_channel_new_file(pcFileName, "w", &err);
	g_io_channel_set_encoding(saveFile, "ISO-8859-15", &err);
	g_io_channel_write_chars(saveFile, content, -1, &nbBytes, &err);
	g_io_channel_unref(saveFile);
}

void gmlc_gui_server_tab_query_evt_btn_save_clicked(GtkWidget *widget, gpointer user_data) {
	GmlcGuiServerTabQuery * pGmlcGuiSrvTabQuery = GMLC_GUI_SERVER_TAB_QUERY(user_data);
	GtkWidget * poChooser = NULL;
	gint response;
	
	poChooser = gtk_file_chooser_dialog_new (_("Save sql file"), GTK_WINDOW(pGmlcGuiSrvTabQuery->pGmlcGuiSrv), GTK_FILE_CHOOSER_ACTION_SAVE,
		GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, GTK_STOCK_SAVE, GTK_RESPONSE_OK, NULL);
	
	if (pGmlcGuiSrvTabQuery->pcFileName != NULL) {
		gtk_file_chooser_set_filename (GTK_FILE_CHOOSER (poChooser), pGmlcGuiSrvTabQuery->pcFileName);
	}
	
	response = gtk_dialog_run (GTK_DIALOG (poChooser));
	if (response == GTK_RESPONSE_OK) {
		gchar * pcFileName = NULL;
		
		pcFileName = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (poChooser));
		if (pcFileName != NULL) {
			gmlc_gui_server_tab_query_save_to_file (pGmlcGuiSrvTabQuery, pcFileName);
			g_free (pcFileName);
		}
	}
	
	gtk_widget_destroy (poChooser);
}

void gmlc_gui_server_tab_query_evt_btnExecSql_clicked(GtkWidget *widget, gpointer user_data) {
	GmlcGuiServerTabQuery * pGmlcGuiSrvTabQuery = GMLC_GUI_SERVER_TAB_QUERY(user_data);
	gchar * pcSqlQuery;
	UNUSED_VAR(widget);

	g_print("Execute SQL\n");
	pcSqlQuery = gmlc_gui_server_tab_query_get_query(pGmlcGuiSrvTabQuery);
	g_print("SQL Query : '%s'\n", pcSqlQuery);
	
	gmlc_gui_server_tab_query_execute_query(pGmlcGuiSrvTabQuery, pcSqlQuery);
	
	g_free(pcSqlQuery);
}

void gmlc_gui_server_tab_query_evt_btnDuplicateSql_clicked(GtkWidget *widget, gpointer user_data) {
	GmlcGuiServerTabQuery * pGmlcGuiSrvTabQuery = GMLC_GUI_SERVER_TAB_QUERY(user_data);
	gchar * pcSqlQuery;
	UNUSED_VAR(widget);
	
	pcSqlQuery = gmlc_gui_server_tab_query_get_query(pGmlcGuiSrvTabQuery);
	
	gmlc_gui_server_add_query_tab(pGmlcGuiSrvTabQuery->pGmlcGuiSrv, pGmlcGuiSrvTabQuery->pcDbName, pcSqlQuery, FALSE);

	g_free(pcSqlQuery);
}

void gmlc_gui_server_tab_query_evt_btnDumpSql_clicked(GtkWidget *widget, gpointer user_data) {
	GmlcGuiServerTabQuery * pGmlcGuiSrvTabQuery = GMLC_GUI_SERVER_TAB_QUERY(user_data);
	/*p_gmysqlcc_gui_dump gui_dump;*/
	gchar * pcSqlQuery;
	UNUSED_VAR(widget);
	
	pcSqlQuery = gmlc_gui_server_tab_query_get_query(pGmlcGuiSrvTabQuery);
	
	g_print("Dump query ...\n%s\n", pcSqlQuery);
	
/*
	gui_dump = gmlc_gui_dump_new(NULL, mysql_query_get_database(pGmlcGuiQry->mysql_qry), NULL, pcSqlQuery);
	gmlc_gui_dump_display(gui_dump, TRUE);
*/	
	g_free(pcSqlQuery);
}

void gmlc_gui_server_tab_query_evt_btnClose_clicked(GtkWidget *widget, gpointer user_data) {
	GmlcGuiServerTabQuery * pGmlcGuiSrvTabQuery = GMLC_GUI_SERVER_TAB_QUERY(user_data);
	UNUSED_VAR(widget);
	
	gmlc_gui_server_close_query_tab(pGmlcGuiSrvTabQuery->pGmlcGuiSrv, GTK_WIDGET(pGmlcGuiSrvTabQuery));
}

