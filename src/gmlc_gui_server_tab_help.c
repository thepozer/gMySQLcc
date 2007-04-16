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

#include "gmlc_gui_server_tab_help.h"
#include "gmlc_mysql_query.h"

#include <stdlib.h>
#include <string.h>

void gmlc_gui_server_fill_help_category_list(GmlcGuiServer * pGmlcGuiSrv);
void gmlc_gui_server_fill_help_topic_list(GmlcGuiServer * pGmlcGuiSrv);
void gmlc_gui_server_fill_help_topic_list_from_search(GmlcGuiServer * pGmlcGuiSrv);
void gmlc_gui_server_display_help_topic(GmlcGuiServer * pGmlcGuiSrv);

void gmlc_gui_server_evt_trvHelpCategories_selected (GtkTreeSelection *selection, gpointer user_data);
void gmlc_gui_server_evt_trvHelpTopics_selected (GtkTreeSelection *selection, gpointer user_data);
void gmlc_gui_server_evt_btnHelpSearch_clicked (GtkWidget *widget, gpointer user_data);

void gmlc_gui_server_create_widget_tab_help (GmlcGuiServer * pGmlcGuiSrv) {
	GtkWidget *label;
	GtkWidget *hpaned;
	GtkWidget *vpaned;
	GtkWidget *table;
	GtkWidget *scrolledwindow;
	GtkWidget *btnHelpSearch;
	
	GtkTreeSelection *select;
	
	PangoFontDescription * pCourierFontDesc = NULL;
	
	label = gtk_label_new (_("Help"));
	gtk_widget_show (label);
	gtk_label_set_justify (GTK_LABEL (label), GTK_JUSTIFY_LEFT);
	
	hpaned = gtk_hpaned_new ();
	gtk_widget_show (hpaned);
	gtk_paned_set_position (GTK_PANED (hpaned), 200);
	
	gtk_notebook_append_page(GTK_NOTEBOOK(pGmlcGuiSrv->nbkGeneral), hpaned, label);
	
	scrolledwindow = gtk_scrolled_window_new (NULL, NULL);
	gtk_widget_show (scrolledwindow);
	gtk_paned_pack1 (GTK_PANED (hpaned), scrolledwindow, FALSE, TRUE);
	gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolledwindow), GTK_SHADOW_IN);
	
	pGmlcGuiSrv->trvHelpCategories = gtk_tree_view_new ();
	gtk_widget_show (pGmlcGuiSrv->trvHelpCategories);
	gtk_container_add (GTK_CONTAINER (scrolledwindow), pGmlcGuiSrv->trvHelpCategories);
	select = gtk_tree_view_get_selection (GTK_TREE_VIEW (pGmlcGuiSrv->trvHelpCategories));
	gtk_tree_selection_set_mode (select, GTK_SELECTION_SINGLE);
	g_signal_connect (G_OBJECT (select), "changed", 
						G_CALLBACK (gmlc_gui_server_evt_trvHelpCategories_selected), pGmlcGuiSrv);

	vpaned = gtk_vpaned_new ();
	gtk_widget_show (vpaned);
	gtk_paned_pack2 (GTK_PANED (hpaned), vpaned, TRUE, TRUE);
	gtk_paned_set_position (GTK_PANED (vpaned), 200);
	
	table = gtk_table_new (2, 3, FALSE);
	gtk_widget_show (table);
	gtk_paned_pack1 (GTK_PANED (vpaned), table, FALSE, TRUE);
	gtk_container_set_border_width (GTK_CONTAINER (table), 3);
	gtk_table_set_row_spacings (GTK_TABLE (table), 3);
	gtk_table_set_col_spacings (GTK_TABLE (table), 3);
	
	label = gtk_label_new (_("Search (keyword) :"));
	gtk_widget_show (label);
	gtk_table_attach (GTK_TABLE (table), label, 0, 1, 0, 1,
					(GtkAttachOptions) (GTK_FILL),
					(GtkAttachOptions) (0), 0, 0);
	gtk_misc_set_alignment (GTK_MISC (label), 0, 0.5);
	
	pGmlcGuiSrv->txtHelpSearchKeyword = gtk_entry_new ();
	gtk_widget_show (pGmlcGuiSrv->txtHelpSearchKeyword);
	gtk_table_attach (GTK_TABLE (table), pGmlcGuiSrv->txtHelpSearchKeyword, 1, 2, 0, 1,
					(GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
					(GtkAttachOptions) (0), 0, 0);
	
	btnHelpSearch = gtk_button_new_with_mnemonic (_("Search"));
	gtk_widget_show (btnHelpSearch);
	gtk_table_attach (GTK_TABLE (table), btnHelpSearch, 2, 3, 0, 1,
					(GtkAttachOptions) (GTK_FILL),
					(GtkAttachOptions) (0), 0, 0);
	g_signal_connect (G_OBJECT (btnHelpSearch), "clicked", 
						G_CALLBACK (gmlc_gui_server_evt_btnHelpSearch_clicked), pGmlcGuiSrv);
	
	scrolledwindow = gtk_scrolled_window_new (NULL, NULL);
	gtk_widget_show (scrolledwindow);
	gtk_table_attach (GTK_TABLE (table), scrolledwindow, 0, 3, 1, 2,
					(GtkAttachOptions) (GTK_FILL),
					(GtkAttachOptions) (GTK_EXPAND | GTK_FILL), 0, 0);
	gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolledwindow), GTK_SHADOW_IN);
	
	pGmlcGuiSrv->trvHelpTopics = gtk_tree_view_new ();
	gtk_widget_show (pGmlcGuiSrv->trvHelpTopics);
	gtk_container_add (GTK_CONTAINER (scrolledwindow), pGmlcGuiSrv->trvHelpTopics);
	select = gtk_tree_view_get_selection (GTK_TREE_VIEW (pGmlcGuiSrv->trvHelpTopics));
	gtk_tree_selection_set_mode (select, GTK_SELECTION_SINGLE);
	g_signal_connect (G_OBJECT (select), "changed", 
						G_CALLBACK (gmlc_gui_server_evt_trvHelpTopics_selected), pGmlcGuiSrv);
	
	scrolledwindow = gtk_scrolled_window_new (NULL, NULL);
	gtk_widget_show (scrolledwindow);
	gtk_paned_pack2 (GTK_PANED (vpaned), scrolledwindow, TRUE, TRUE);
	gtk_container_set_border_width (GTK_CONTAINER (scrolledwindow), 3);
	gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolledwindow), GTK_SHADOW_IN);
	
	pGmlcGuiSrv->txvHelpTopic = gtk_text_view_new ();
	pCourierFontDesc = pango_font_description_from_string(GpGmlcMscCfg->pcHelpFontName);
	gtk_widget_modify_font(pGmlcGuiSrv->txvHelpTopic, pCourierFontDesc);
	gtk_widget_show (pGmlcGuiSrv->txvHelpTopic);
	gtk_container_add (GTK_CONTAINER (scrolledwindow), pGmlcGuiSrv->txvHelpTopic);
	gtk_text_view_set_editable (GTK_TEXT_VIEW (pGmlcGuiSrv->txvHelpTopic), FALSE);
	pango_font_description_free(pCourierFontDesc);
}

void gmlc_gui_server_init_widget_tab_help (GmlcGuiServer * pGmlcGuiSrv) {
	GtkTreeViewColumn * currCol;
	GtkCellRenderer * renderer;
	GtkListStore * lstEmpty;
	GtkTreeStore * treEmpty;
	
	/* Generate columns for all tree views */
	renderer = gtk_cell_renderer_text_new ();
	
	/* Databases columns */
	currCol = gtk_tree_view_column_new_with_attributes (_("Categories"), renderer, "text", 0, NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW (pGmlcGuiSrv->trvHelpCategories), currCol);
	
	treEmpty = gtk_tree_store_new(2, G_TYPE_STRING, G_TYPE_INT);
	gtk_tree_view_set_model(GTK_TREE_VIEW(pGmlcGuiSrv->trvHelpCategories), GTK_TREE_MODEL(treEmpty));
	g_object_unref (G_OBJECT (treEmpty));
	
	/* Table columns */
	currCol = gtk_tree_view_column_new_with_attributes (_("Topic name"), renderer, "text", 0, NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW (pGmlcGuiSrv->trvHelpTopics), currCol);
	
	lstEmpty = gtk_list_store_new(2, G_TYPE_STRING, G_TYPE_INT);
	gtk_tree_view_set_model(GTK_TREE_VIEW(pGmlcGuiSrv->trvHelpTopics), GTK_TREE_MODEL(lstEmpty));
	g_object_unref (G_OBJECT (lstEmpty));

	gmlc_gui_server_fill_help_category_list(pGmlcGuiSrv);
}

void gmlc_gui_server_fill_help_category_list(GmlcGuiServer * pGmlcGuiSrv) {
	GmlcMysqlQuery * pGmlcMysqlQry = NULL;
	GtkTreeStore * treCategories = NULL;
	GtkTreeIter * pIter = NULL, * pIterParent = NULL;
	GtkTreePath * pPath = NULL;
	GHashTable * htCategories = NULL;
	GArray * arRow = NULL;
	gchar * pcName = NULL;
	gint * piKey = 0, iParentId = 0;
	
	treCategories = gtk_tree_store_new(2, G_TYPE_STRING, G_TYPE_INT);
	htCategories = g_hash_table_new_full(g_int_hash, g_int_equal, g_free, g_free); 
	
	pGmlcMysqlQry = gmlc_mysql_query_new(pGmlcGuiSrv->pGmlcMysqlSrv, "mysql");
	
	if (gmlc_mysql_query_execute(pGmlcMysqlQry, "SELECT * FROM `mysql`.`help_category` ORDER BY parent_category_id, name;", 72, FALSE)) {
		arRow = gmlc_mysql_query_next_record(pGmlcMysqlQry);
	
		while (arRow != (GArray *)NULL) {
			
			pIter = (GtkTreeIter *) g_try_malloc(sizeof(GtkTreeIter));
			piKey = (gint *) g_try_malloc(sizeof(gint));
			
			*piKey = atoi(g_array_index(arRow, gchar *, 0));
			pcName = g_array_index(arRow, gchar *, 1);
			iParentId = atoi(g_array_index(arRow, gchar *, 2));
			
			if (iParentId == 0) {
				pIterParent = NULL;
			} else {
				pIterParent = g_hash_table_lookup(htCategories, &iParentId);
			}
			
			gtk_tree_store_append (treCategories, pIter, pIterParent);
			gtk_tree_store_set (treCategories, pIter, 0, pcName, 1, *piKey, -1);

			if (iParentId == 0) {
				pPath = gtk_tree_model_get_path(GTK_TREE_MODEL(treCategories), pIter);
			}
			
			g_hash_table_insert(htCategories, piKey, pIter);
			
			g_array_free(arRow, TRUE);
			arRow = gmlc_mysql_query_next_record(pGmlcMysqlQry);
		}
	}
	
	gtk_tree_view_set_model(GTK_TREE_VIEW(pGmlcGuiSrv->trvHelpCategories), GTK_TREE_MODEL(treCategories));
	g_object_unref (G_OBJECT (treCategories));
	
	if (pPath != NULL) {
		gtk_tree_view_expand_to_path(GTK_TREE_VIEW(pGmlcGuiSrv->trvHelpCategories), pPath);
		gtk_tree_path_free(pPath);
	}
	
	g_hash_table_destroy(htCategories);
	
	g_object_unref(G_OBJECT(pGmlcMysqlQry));
}

void gmlc_gui_server_fill_help_topic_list(GmlcGuiServer * pGmlcGuiSrv) {
	GmlcMysqlQuery * pGmlcMysqlQry = NULL;
	GtkListStore * lstTopics = NULL;
	GtkTreeIter iter;
	GArray * arRow = NULL;
	gchar * pcName = NULL, * pcSql = NULL;
	gint iKey = 0;
	
	lstTopics = gtk_list_store_new(2, G_TYPE_STRING, G_TYPE_INT);
	
	pGmlcMysqlQry = gmlc_mysql_query_new(pGmlcGuiSrv->pGmlcMysqlSrv, "mysql");
	
	pcSql = g_strdup_printf("SELECT * FROM `mysql`.`help_topic` WHERE help_category_id = %d ORDER BY name", pGmlcGuiSrv->iCurrentCategory);
	
	if (gmlc_mysql_query_execute(pGmlcMysqlQry, pcSql, strlen(pcSql), FALSE)) {
		arRow = gmlc_mysql_query_next_record(pGmlcMysqlQry);
		
		while (arRow != NULL) {
			iKey = atoi(g_array_index(arRow, gchar *, 0));
			pcName = g_array_index(arRow, gchar *, 1);
			
			gtk_list_store_append (lstTopics, &iter);
			gtk_list_store_set (lstTopics, &iter, 0, pcName, 1, iKey, -1);

			gmlc_mysql_query_free_record_content(arRow);
			arRow = gmlc_mysql_query_next_record(pGmlcMysqlQry);
		}
	}
	
	gtk_tree_view_set_model(GTK_TREE_VIEW(pGmlcGuiSrv->trvHelpTopics), GTK_TREE_MODEL(lstTopics));
	g_object_unref (G_OBJECT (lstTopics));
	
	g_free(pcSql);
	g_object_unref(G_OBJECT(pGmlcMysqlQry));
}

void gmlc_gui_server_fill_help_topic_list_from_search(GmlcGuiServer * pGmlcGuiSrv) {
	GmlcMysqlQuery * pGmlcMysqlQry = NULL;
	GtkListStore * lstTopics = NULL;
	GtkTreeIter iter;
	GString * strSql = NULL;
	GArray * arRow = NULL;
	gchar * pcName = NULL, * pcSearchedText = NULL, ** ppcSplitedSearchedText = NULL;
	gint iKey = 0, iIdx = 0;
	
	pcSearchedText = gtk_editable_get_chars(GTK_EDITABLE(pGmlcGuiSrv->txtHelpSearchKeyword), 0, -1);
	if (strlen(pcSearchedText) == 0) {
		g_free(pcSearchedText);
		return;
	}

	ppcSplitedSearchedText = g_strsplit(pcSearchedText, " ", 0);
	
	strSql = g_string_new("SELECT * FROM `mysql`.`help_topic` WHERE");
	while (ppcSplitedSearchedText[iIdx] != NULL) {
		if (iIdx == 0) {
			g_string_append_printf (strSql, " (description LIKE '%%%s%%' OR example LIKE '%%%s%%')", ppcSplitedSearchedText[iIdx], ppcSplitedSearchedText[iIdx]);
		} else {
			g_string_append_printf (strSql, " AND (description LIKE '%%%s%%' OR example LIKE '%%%s%%')", ppcSplitedSearchedText[iIdx], ppcSplitedSearchedText[iIdx]);
		}
		
		iIdx ++;
	}
	g_string_append (strSql, " ORDER BY name");
	
	g_strfreev(ppcSplitedSearchedText);
	g_free(pcSearchedText);
	
	lstTopics = gtk_list_store_new(2, G_TYPE_STRING, G_TYPE_INT);
	
	pGmlcMysqlQry = gmlc_mysql_query_new(pGmlcGuiSrv->pGmlcMysqlSrv, "mysql");
	
	if (gmlc_mysql_query_execute(pGmlcMysqlQry, strSql->str, strSql->len, FALSE)) {
	
		arRow = gmlc_mysql_query_next_record(pGmlcMysqlQry);
		
		while (arRow != (GArray *)NULL) {
			iKey = atoi(g_array_index(arRow, gchar *, 0));
			pcName = g_array_index(arRow, gchar *, 1);
			
			gtk_list_store_append (lstTopics, &iter);
			gtk_list_store_set (lstTopics, &iter, 0, pcName, 1, iKey, -1);
			
			gmlc_mysql_query_free_record_content(arRow);
			arRow = gmlc_mysql_query_next_record(pGmlcMysqlQry);
		}
	}
	
	gtk_tree_view_set_model(GTK_TREE_VIEW(pGmlcGuiSrv->trvHelpTopics), GTK_TREE_MODEL(lstTopics));
	g_object_unref (G_OBJECT (lstTopics));
	
	g_string_free(strSql, TRUE);
	g_object_unref(G_OBJECT(pGmlcMysqlQry));
}

void gmlc_gui_server_display_help_topic(GmlcGuiServer * pGmlcGuiSrv) {
	GmlcMysqlQuery * pGmlcMysqlQry = NULL;
	GtkTextBuffer * txtBuffer = NULL;
	GArray * arRow = NULL;
	gchar * pcSql = NULL, * pcTopic = NULL;
	gchar * pcName = NULL, * pcDescription = NULL, * pcSample = NULL, * pcUrl = NULL;
	
	pGmlcMysqlQry = gmlc_mysql_query_new(pGmlcGuiSrv->pGmlcMysqlSrv, "mysql");
	
	pcSql = g_strdup_printf("SELECT * FROM `mysql`.`help_topic` WHERE help_topic_id = %d", pGmlcGuiSrv->iCurrentTopic);
	
	if (gmlc_mysql_query_execute(pGmlcMysqlQry, pcSql, strlen(pcSql), FALSE)) {
		arRow = gmlc_mysql_query_next_record(pGmlcMysqlQry);
		
		pcName = g_array_index(arRow, gchar *, 1);
		pcDescription = g_array_index(arRow, gchar *, 3);
		pcSample = g_array_index(arRow, gchar *, 4);
		pcUrl = g_array_index(arRow, gchar *, 5);
		
		pcTopic = g_strdup_printf(_("Name : %s\n\nDescription :\n\n%s\n\nSample :\n\n%s\n\nUrl : %s\n"), pcName, pcDescription, pcSample, pcUrl);
		
		txtBuffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(pGmlcGuiSrv->txvHelpTopic));
		gtk_text_buffer_set_text(txtBuffer, pcTopic, -1);
		
		gmlc_mysql_query_free_record_content(arRow);
		g_free(pcTopic);
	}
	
	g_free(pcSql);
	g_object_unref(G_OBJECT(pGmlcMysqlQry));
}

void gmlc_gui_server_evt_trvHelpCategories_selected (GtkTreeSelection *selection, gpointer user_data) {
	GmlcGuiServer * pGmlcGuiSrv = GMLC_GUI_SERVER(user_data);
	GtkTreeIter iter;
	GtkTreeModel *model;
	
	if (gtk_tree_selection_get_selected (selection, &model, &iter)) {
		gtk_tree_model_get (model, &iter, 1, &pGmlcGuiSrv->iCurrentCategory, -1);
		
		gmlc_gui_server_fill_help_topic_list(pGmlcGuiSrv);
	}
}

void gmlc_gui_server_evt_trvHelpTopics_selected (GtkTreeSelection *selection, gpointer user_data) {
	GmlcGuiServer * pGmlcGuiSrv = GMLC_GUI_SERVER(user_data);
	GtkTreeIter iter;
	GtkTreeModel *model;
	
	if (gtk_tree_selection_get_selected (selection, &model, &iter)) {
		gtk_tree_model_get (model, &iter, 1, &pGmlcGuiSrv->iCurrentTopic, -1);
		
		gmlc_gui_server_display_help_topic(pGmlcGuiSrv);
	}
}

void gmlc_gui_server_evt_btnHelpSearch_clicked (GtkWidget *widget, gpointer user_data) {
	GmlcGuiServer * pGmlcGuiSrv = GMLC_GUI_SERVER(user_data);
	
	gmlc_gui_server_fill_help_topic_list_from_search(pGmlcGuiSrv);
}

