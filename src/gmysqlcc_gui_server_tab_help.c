
#include "gmysqlcc_gui_all.h"
#include "gmysqlcc_gui_server.h"
#include <stdlib.h>
#include <string.h>

void gmysqlcc_gui_server_fill_help_category_list(p_gmysqlcc_gui_server gui_server);
void gmysqlcc_gui_server_fill_help_topic_list(p_gmysqlcc_gui_server gui_server);
void gmysqlcc_gui_server_display_help_topic(p_gmysqlcc_gui_server gui_server);

void gmysqlcc_gui_server_evt_trvHelpCategories_selected (GtkTreeSelection *selection, gpointer user_data);
void gmysqlcc_gui_server_evt_trvHelpTopics_selected (GtkTreeSelection *selection, gpointer user_data);
void gmysqlcc_gui_server_evt_btnHelpSearch_clicked (GtkWidget *widget, gpointer user_data);

void gmysqlcc_gui_server_create_widget_tab_help (p_gmysqlcc_gui_server gui_server, GtkWidget * notebook) {
	GtkWidget *label;
	GtkWidget *hpaned;
	GtkWidget *vpaned;
	GtkWidget *table;
	GtkWidget *scrolledwindow;
	GtkWidget *btnHelpSearch;
	
	GtkTreeSelection *select;
	
	label = gtk_label_new (_("Help"));
	gtk_widget_show (label);
	gtk_label_set_justify (GTK_LABEL (label), GTK_JUSTIFY_LEFT);
	
	hpaned = gtk_hpaned_new ();
	gtk_widget_show (hpaned);
	gtk_paned_set_position (GTK_PANED (hpaned), 200);
	
	gtk_notebook_append_page(GTK_NOTEBOOK(notebook), hpaned, label);
	
	scrolledwindow = gtk_scrolled_window_new (NULL, NULL);
	gtk_widget_show (scrolledwindow);
	gtk_paned_pack1 (GTK_PANED (hpaned), scrolledwindow, FALSE, TRUE);
	gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolledwindow), GTK_SHADOW_IN);
	
	gui_server->trvHelpCategories = gtk_tree_view_new ();
	gtk_widget_show (gui_server->trvHelpCategories);
	gtk_container_add (GTK_CONTAINER (scrolledwindow), gui_server->trvHelpCategories);
	select = gtk_tree_view_get_selection (GTK_TREE_VIEW (gui_server->trvHelpCategories));
	gtk_tree_selection_set_mode (select, GTK_SELECTION_SINGLE);
	g_signal_connect (G_OBJECT (select), "changed", 
						G_CALLBACK (gmysqlcc_gui_server_evt_trvHelpCategories_selected), (gpointer)gui_server);

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
	
	gui_server->txtHelpSearchKeyword = gtk_entry_new ();
	gtk_widget_show (gui_server->txtHelpSearchKeyword);
	gtk_table_attach (GTK_TABLE (table), gui_server->txtHelpSearchKeyword, 1, 2, 0, 1,
					(GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
					(GtkAttachOptions) (0), 0, 0);
	
	btnHelpSearch = gtk_button_new_with_mnemonic (_("Search"));
	gtk_widget_show (btnHelpSearch);
	gtk_table_attach (GTK_TABLE (table), btnHelpSearch, 2, 3, 0, 1,
					(GtkAttachOptions) (GTK_FILL),
					(GtkAttachOptions) (0), 0, 0);
	g_signal_connect (G_OBJECT (btnHelpSearch), "clicked", 
						G_CALLBACK (gmysqlcc_gui_server_evt_btnHelpSearch_clicked), (gpointer)gui_server);
	
	scrolledwindow = gtk_scrolled_window_new (NULL, NULL);
	gtk_widget_show (scrolledwindow);
	gtk_table_attach (GTK_TABLE (table), scrolledwindow, 0, 3, 1, 2,
					(GtkAttachOptions) (GTK_FILL),
					(GtkAttachOptions) (GTK_EXPAND | GTK_FILL), 0, 0);
	gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolledwindow), GTK_SHADOW_IN);
	
	gui_server->trvHelpTopics = gtk_tree_view_new ();
	gtk_widget_show (gui_server->trvHelpTopics);
	gtk_container_add (GTK_CONTAINER (scrolledwindow), gui_server->trvHelpTopics);
	select = gtk_tree_view_get_selection (GTK_TREE_VIEW (gui_server->trvHelpTopics));
	gtk_tree_selection_set_mode (select, GTK_SELECTION_SINGLE);
	g_signal_connect (G_OBJECT (select), "changed", 
						G_CALLBACK (gmysqlcc_gui_server_evt_trvHelpTopics_selected), (gpointer)gui_server);
	
	scrolledwindow = gtk_scrolled_window_new (NULL, NULL);
	gtk_widget_show (scrolledwindow);
	gtk_paned_pack2 (GTK_PANED (vpaned), scrolledwindow, TRUE, TRUE);
	gtk_container_set_border_width (GTK_CONTAINER (scrolledwindow), 3);
	gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolledwindow), GTK_SHADOW_IN);
	
	gui_server->txvHelpTopic = gtk_text_view_new ();
	gtk_widget_show (gui_server->txvHelpTopic);
	gtk_container_add (GTK_CONTAINER (scrolledwindow), gui_server->txvHelpTopic);
	gtk_text_view_set_editable (GTK_TEXT_VIEW (gui_server->txvHelpTopic), FALSE);
	
}

void gmysqlcc_gui_server_init_widget_tab_help (p_gmysqlcc_gui_server gui_server) {
	GtkTreeViewColumn * currCol;
	GtkCellRenderer * renderer;
	GtkListStore * lstEmpty;
	GtkTreeStore * treEmpty;
	
	/* Generate columns for all tree views */
	renderer = gtk_cell_renderer_text_new ();
	
	/* Databases columns */
	currCol = gtk_tree_view_column_new_with_attributes (_("Categories"), renderer, "text", 0, NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW (gui_server->trvHelpCategories), currCol);
	
	treEmpty = gtk_tree_store_new(2, G_TYPE_STRING, G_TYPE_INT);
	gtk_tree_view_set_model(GTK_TREE_VIEW(gui_server->trvHelpCategories), GTK_TREE_MODEL(treEmpty));
	g_object_unref (G_OBJECT (treEmpty));
	
	/* Table columns */
	currCol = gtk_tree_view_column_new_with_attributes (_("Topic name"), renderer, "text", 0, NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW (gui_server->trvHelpTopics), currCol);
	
	lstEmpty = gtk_list_store_new(2, G_TYPE_STRING, G_TYPE_INT);
	gtk_tree_view_set_model(GTK_TREE_VIEW(gui_server->trvHelpTopics), GTK_TREE_MODEL(lstEmpty));
	g_object_unref (G_OBJECT (lstEmpty));

	gmysqlcc_gui_server_fill_help_category_list(gui_server);

}

void gmysqlcc_gui_server_fill_help_category_list(p_gmysqlcc_gui_server gui_server) {
	p_mysql_query mysql_qry;
	GtkTreeStore * treCategories;
	GtkTreeIter * pIter, * pIterParent;
	GtkTreePath * pPath = NULL;
	GHashTable * htCategories;
	GArray * arRow;
	gchar * pcName;
	gint * piKey, iParentId;
	
	treCategories = gtk_tree_store_new(2, G_TYPE_STRING, G_TYPE_INT);
	
	htCategories = g_hash_table_new_full(g_int_hash, g_int_equal, g_free, g_free); 
	
	mysql_qry = mysql_server_query(gui_server->mysql_srv, "mysql");
	
	if (mysql_query_execute_query(mysql_qry, "SELECT * FROM `mysql`.`help_category` ORDER BY parent_category_id, name", FALSE)) {
	
		arRow = mysql_query_get_next_record(mysql_qry);
		
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
			arRow = mysql_query_get_next_record(mysql_qry);
		}
	}
	
	gtk_tree_view_set_model(GTK_TREE_VIEW(gui_server->trvHelpCategories), GTK_TREE_MODEL(treCategories));
	g_object_unref (G_OBJECT (treCategories));
	
	if (pPath != NULL) {
		gtk_tree_view_expand_to_path(GTK_TREE_VIEW(gui_server->trvHelpCategories), pPath);
		gtk_tree_path_free(pPath);
	}
	
	g_hash_table_destroy(htCategories);
	
	mysql_query_delete(mysql_qry);
}

void gmysqlcc_gui_server_fill_help_topic_list(p_gmysqlcc_gui_server gui_server) {
	p_mysql_query mysql_qry;
	GtkListStore * lstTopics;
	GtkTreeIter iter;
	GString * strSql;
	GArray * arRow;
	gchar * pcName;
	gint iKey;
	
	lstTopics = gtk_list_store_new(2, G_TYPE_STRING, G_TYPE_INT);
	
	mysql_qry = mysql_server_query(gui_server->mysql_srv, "mysql");
	
	strSql = g_string_new("");
	g_string_printf(strSql, "SELECT * FROM `mysql`.`help_topic` WHERE help_category_id = %d ORDER BY name", gui_server->iCurrentCategory);
	
	if (mysql_query_execute_query(mysql_qry, strSql->str, FALSE)) {
	
		arRow = mysql_query_get_next_record(mysql_qry);
		
		while (arRow != (GArray *)NULL) {
			iKey = atoi(g_array_index(arRow, gchar *, 0));
			pcName = g_array_index(arRow, gchar *, 1);
			
			gtk_list_store_append (lstTopics, &iter);
			gtk_list_store_set (lstTopics, &iter, 0, pcName, 1, iKey, -1);

			g_array_free(arRow, TRUE);
			arRow = mysql_query_get_next_record(mysql_qry);
		}
	}
	
	gtk_tree_view_set_model(GTK_TREE_VIEW(gui_server->trvHelpTopics), GTK_TREE_MODEL(lstTopics));
	g_object_unref (G_OBJECT (lstTopics));
	
	
	g_string_free(strSql, TRUE);
	mysql_query_delete(mysql_qry);
}

void gmysqlcc_gui_server_fill_help_topic_list_from_search(p_gmysqlcc_gui_server gui_server) {
	p_mysql_query mysql_qry;
	GtkListStore * lstTopics;
	GtkTreeIter iter;
	GString * strSql;
	GArray * arRow;
	gchar * pcName = NULL, * pcSearchedText = NULL, ** ppcSplitedSearchedText = NULL;
	gint iKey = 0, iIdx = 0;
	
	pcSearchedText = gtk_editable_get_chars(GTK_EDITABLE(gui_server->txtHelpSearchKeyword), 0, -1);
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
	
g_print("SQL Search : '%s'\n", strSql->str);
	
	g_strfreev(ppcSplitedSearchedText);
	g_free(pcSearchedText);
	
	lstTopics = gtk_list_store_new(2, G_TYPE_STRING, G_TYPE_INT);
	
	mysql_qry = mysql_server_query(gui_server->mysql_srv, "mysql");
	
	if (mysql_query_execute_query(mysql_qry, strSql->str, FALSE)) {
	
		arRow = mysql_query_get_next_record(mysql_qry);
		
		while (arRow != (GArray *)NULL) {
			iKey = atoi(g_array_index(arRow, gchar *, 0));
			pcName = g_array_index(arRow, gchar *, 1);
			
			gtk_list_store_append (lstTopics, &iter);
			gtk_list_store_set (lstTopics, &iter, 0, pcName, 1, iKey, -1);

			g_array_free(arRow, TRUE);
			arRow = mysql_query_get_next_record(mysql_qry);
		}
	}
	
	gtk_tree_view_set_model(GTK_TREE_VIEW(gui_server->trvHelpTopics), GTK_TREE_MODEL(lstTopics));
	g_object_unref (G_OBJECT (lstTopics));
	
	g_string_free(strSql, TRUE);
	mysql_query_delete(mysql_qry);
}

void gmysqlcc_gui_server_display_help_topic(p_gmysqlcc_gui_server gui_server) {
	p_mysql_query mysql_qry;
	GtkTextBuffer * txtBuffer;
	GString * strSql, * strTopic;
	GArray * arRow;
	gchar * pcName, * pcDescription, * pcSample, * pcUrl;
	
	mysql_qry = mysql_server_query(gui_server->mysql_srv, "mysql");
	
	strSql = g_string_new("");
	g_string_printf(strSql, "SELECT * FROM `mysql`.`help_topic` WHERE help_topic_id = %d", gui_server->iCurrentTopic);
	
	if (mysql_query_execute_query(mysql_qry, strSql->str, FALSE)) {
		strTopic = g_string_new("");
		arRow = mysql_query_get_next_record(mysql_qry);
		
		pcName = g_array_index(arRow, gchar *, 1);
		pcDescription = g_array_index(arRow, gchar *, 3);
		pcSample = g_array_index(arRow, gchar *, 4);
		pcUrl = g_array_index(arRow, gchar *, 5);
		
		g_string_printf(strTopic, _("Name : %s\n\nDescription :\n\n%s\n\nSample :\n\n%s\n\nUrl : %s\n"), pcName, pcDescription, pcSample, pcUrl);
		
		txtBuffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(gui_server->txvHelpTopic));
		gtk_text_buffer_set_text(txtBuffer, strTopic->str, -1);
		
		g_array_free(arRow, TRUE);
		g_string_free(strTopic, TRUE);
	}
	
	g_string_free(strSql, TRUE);
	mysql_query_delete(mysql_qry);
}

void gmysqlcc_gui_server_evt_trvHelpCategories_selected (GtkTreeSelection *selection, gpointer user_data) {
	p_gmysqlcc_gui_server gui_server = (p_gmysqlcc_gui_server)user_data;
	GtkTreeIter iter;
	GtkTreeModel *model;
	
	if (gtk_tree_selection_get_selected (selection, &model, &iter)) {
		gtk_tree_model_get (model, &iter, 1, &gui_server->iCurrentCategory, -1);
		
		gmysqlcc_gui_server_fill_help_topic_list(gui_server);
	}
}

void gmysqlcc_gui_server_evt_trvHelpTopics_selected (GtkTreeSelection *selection, gpointer user_data) {
	p_gmysqlcc_gui_server gui_server = (p_gmysqlcc_gui_server)user_data;
	GtkTreeIter iter;
	GtkTreeModel *model;
	
	if (gtk_tree_selection_get_selected (selection, &model, &iter)) {
		gtk_tree_model_get (model, &iter, 1, &gui_server->iCurrentTopic, -1);
		
		gmysqlcc_gui_server_display_help_topic(gui_server);
	}
}

void gmysqlcc_gui_server_evt_btnHelpSearch_clicked (GtkWidget *widget, gpointer user_data) {
	p_gmysqlcc_gui_server gui_server = (p_gmysqlcc_gui_server)user_data;
	
	gmysqlcc_gui_server_fill_help_topic_list_from_search(gui_server);
}
