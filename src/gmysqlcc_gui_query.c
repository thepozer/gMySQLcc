
#include "gmysqlcc_gui_all.h"


void gmysqlcc_gui_query_init_widget (p_gmysqlcc_gui_query gui_query);
void gmysqlcc_gui_query_create_widget (p_gmysqlcc_gui_query gui_query);

void gmysqlcc_gui_query_execute_query (p_gmysqlcc_gui_query gui_query, const gchar * query);
void gmysqlcc_gui_query_display_result (p_gmysqlcc_gui_query gui_query);
void gmysqlcc_gui_query_clean_result (p_gmysqlcc_gui_query gui_query);

void gmysqlcc_gui_query_evt_resultRow_selected (GtkTreeSelection *selection, gpointer data);
void gmysqlcc_gui_query_evt_resultRow_edited (GtkCellRendererText *cellrenderertext, gchar *path_string, gchar *new_value, gpointer data);
void gmysqlcc_gui_query_evt_cmbCharset_changed (GtkComboBox *combobox, gpointer user_data);
gboolean gmysqlcc_gui_query_evt_window_keyrelease (GtkWidget *widget, GdkEventKey *event, gpointer user_data);

void gmysqlcc_gui_query_evt_destroy(GtkWidget *widget, gpointer user_data);
void gmysqlcc_gui_query_evt_btnExecSql_clicked(GtkWidget *widget, gpointer user_data);
void gmysqlcc_gui_query_evt_btnDuplicateSql_clicked(GtkWidget *widget, gpointer user_data);
void gmysqlcc_gui_query_evt_btnDumpSql_clicked(GtkWidget *widget, gpointer user_data);
void gmysqlcc_gui_query_evt_btnClose_clicked(GtkWidget *widget, gpointer user_data);


typedef struct _s_cols_nfo {
	p_gmysqlcc_gui_query gui_query;
	int numCol;
} s_cols_nfo;



p_gmysqlcc_gui_query gmysqlcc_gui_query_new (p_mysql_query mysql_qry) {
	p_gmysqlcc_gui_query gui_query;
	
	/* Init structure */
	gui_query = (p_gmysqlcc_gui_query) g_try_malloc(sizeof(s_gmysqlcc_gui_query));
	
	if (gui_query == NULL) {
		return NULL; /* return NULL pointer */
	}
	
	gui_query->mysql_qry = mysql_qry;
	gui_query->mysql_db = NULL;
	gui_query->mysql_rw = NULL;
	
	gui_query->lstRows = NULL;
	gui_query->currCharset = -1;
	
	gmysqlcc_gui_query_create_widget(gui_query);
	gmysqlcc_gui_query_init_widget(gui_query);
	
	NbrWnd ++;
	
	return gui_query;
}

gboolean gmysqlcc_gui_query_set_query (p_gmysqlcc_gui_query gui_query, const gchar * query, gboolean execNow) {
	GtkTextBuffer * txtBuffer;

	if (query != (gchar *) NULL) {
		txtBuffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(gui_query->txtSQLRequest));
		gtk_text_buffer_set_text(GTK_TEXT_BUFFER(txtBuffer), query, -1);
		if (execNow) {
			gmysqlcc_gui_query_execute_query (gui_query, query);
		}
		return TRUE;
	}
	
	return FALSE;
}

gboolean gmysqlcc_gui_query_display (p_gmysqlcc_gui_query gui_query, gboolean display) {
	if (display) {
		gtk_widget_show (gui_query->window);
	} else {
		gtk_widget_hide (gui_query->window);
	}
	
	return TRUE;
}

gboolean gmysqlcc_gui_query_delete (p_gmysqlcc_gui_query gui_query) {
	
	/* Clean table */
	gmysqlcc_gui_query_clean_result (gui_query);
	
	/* Destroy MySql Connection */
	mysql_query_delete(gui_query->mysql_qry);
	
	/* Destroy Datas */
	g_free(gui_query);
	
	/* Destroy Application if needed */
	NbrWnd--;
	g_printerr("Destruction Exec SQL window - nbrWnd : %d\n", NbrWnd);
	if (NbrWnd <= 0) {
		g_printerr("Destroy App\n");
		gtk_main_quit();
	}
	
	return TRUE;
}

void gmysqlcc_gui_query_init_widget (p_gmysqlcc_gui_query gui_query) {
	gint idx;
	gchar * charset;
		
	idx = 0;
	while ((charset = gmysqlcc_helpers_charset_list_get_by_index(idx)) != NULL) {
		gtk_combo_box_append_text(GTK_COMBO_BOX(gui_query->cmbCharset), charset);
		idx++;
	}
}

void gmysqlcc_gui_query_create_widget (p_gmysqlcc_gui_query gui_query) {
	GString * sTitle;
	
	GtkWidget *vbox2;
	GtkWidget *vpanedSQL;
  GtkWidget *scrlwndSQLRequest;
	GtkToolItem *btnExecSql;
	GtkToolItem *ticmbCharset;
	GtkToolItem *btnDumpSql;
	GtkToolItem *btnDuplicateSql;
	GtkToolItem *btnClose;
	GtkTooltips * tooltips;
	GtkWidget * imgToolbar;
	GtkWidget * tlbSql;
	
	GtkTreeSelection *select;
	GtkTextBuffer * txtBuffer;
	
	tooltips = gtk_tooltips_new();
	
  gui_query->window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	sTitle = g_string_new("");
	g_string_printf(sTitle, _("SQL Request - %s - %s"), gui_query->mysql_qry->mysql_srv->name, gui_query->mysql_qry->db_name);
  gtk_window_set_title (GTK_WINDOW (gui_query->window), sTitle->str);
	g_string_free(sTitle, TRUE);
	gtk_window_set_default_size (GTK_WINDOW (gui_query->window), 400, 300);
	
	g_signal_connect (G_OBJECT (gui_query->window), "key-release-event", G_CALLBACK (gmysqlcc_gui_query_evt_window_keyrelease), gui_query);
	g_signal_connect (G_OBJECT (gui_query->window), "destroy", G_CALLBACK (gmysqlcc_gui_query_evt_destroy), gui_query);
	
  vbox2 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox2);
  gtk_container_add (GTK_CONTAINER (gui_query->window), vbox2);

  tlbSql = gtk_toolbar_new ();
  gtk_widget_show (tlbSql);
  gtk_box_pack_start (GTK_BOX (vbox2), tlbSql, FALSE, FALSE, 0);
  gtk_toolbar_set_style (GTK_TOOLBAR (tlbSql), GTK_TOOLBAR_BOTH_HORIZ);
	
	imgToolbar = gtk_image_new_from_stock(GTK_STOCK_EXECUTE, GTK_ICON_SIZE_LARGE_TOOLBAR);
	gtk_widget_show(imgToolbar);
	btnExecSql = gtk_tool_button_new (imgToolbar, _("Execute"));
	gtk_tool_item_set_is_important (GTK_TOOL_ITEM(btnExecSql), TRUE);
	g_signal_connect (G_OBJECT (btnExecSql), "clicked", G_CALLBACK (gmysqlcc_gui_query_evt_btnExecSql_clicked), gui_query);
	gtk_widget_show(GTK_WIDGET(btnExecSql));
	gtk_toolbar_insert(GTK_TOOLBAR(tlbSql), GTK_TOOL_ITEM(btnExecSql), -1);
	gtk_tool_item_set_tooltip (GTK_TOOL_ITEM(btnExecSql), tooltips, _("Execute SQL query"), NULL);
	
	gui_query->cmbCharset = gtk_combo_box_new_text ();
	gtk_widget_show (gui_query->cmbCharset);
	g_signal_connect (G_OBJECT (gui_query->cmbCharset), "changed", G_CALLBACK (gmysqlcc_gui_query_evt_cmbCharset_changed), gui_query);
	
	ticmbCharset = gtk_tool_item_new();
	gtk_widget_show(GTK_WIDGET(ticmbCharset));
	gtk_container_add (GTK_CONTAINER(ticmbCharset), gui_query->cmbCharset);
	gtk_toolbar_insert(GTK_TOOLBAR(tlbSql), GTK_TOOL_ITEM(ticmbCharset), -1);
	gtk_tool_item_set_tooltip (GTK_TOOL_ITEM(ticmbCharset), tooltips, _("Select output charset"), NULL);
	
	imgToolbar = gtk_image_new_from_stock(GTK_STOCK_COPY, GTK_ICON_SIZE_LARGE_TOOLBAR);
	gtk_widget_show(imgToolbar);
	btnDuplicateSql = gtk_tool_button_new (imgToolbar, _("Duplicate"));
	gtk_tool_item_set_is_important (GTK_TOOL_ITEM(btnDuplicateSql), TRUE);
	g_signal_connect (G_OBJECT (btnDuplicateSql), "clicked", G_CALLBACK (gmysqlcc_gui_query_evt_btnDuplicateSql_clicked), gui_query);
	gtk_widget_show(GTK_WIDGET(btnDuplicateSql));
	gtk_toolbar_insert(GTK_TOOLBAR(tlbSql), GTK_TOOL_ITEM(btnDuplicateSql), -1);
	gtk_tool_item_set_tooltip (GTK_TOOL_ITEM(btnDuplicateSql), tooltips, _("Duplicate SQL Windows"), NULL);
	
	imgToolbar = gtk_image_new_from_stock(GTK_STOCK_FLOPPY, GTK_ICON_SIZE_LARGE_TOOLBAR);
	gtk_widget_show(imgToolbar);
	btnDumpSql = gtk_tool_button_new (imgToolbar, _("Dump"));
	gtk_tool_item_set_is_important (GTK_TOOL_ITEM(btnDumpSql), TRUE);
	g_signal_connect (G_OBJECT (btnDumpSql), "clicked", G_CALLBACK (gmysqlcc_gui_query_evt_btnDumpSql_clicked), gui_query);
	gtk_widget_show(GTK_WIDGET(btnDumpSql));
	gtk_toolbar_insert(GTK_TOOLBAR(tlbSql), GTK_TOOL_ITEM(btnDumpSql), -1);
	gtk_tool_item_set_tooltip (GTK_TOOL_ITEM(btnDumpSql), tooltips, _("Dump with current query"), NULL);
	
	imgToolbar = gtk_image_new_from_stock(GTK_STOCK_CLOSE, GTK_ICON_SIZE_LARGE_TOOLBAR);
	gtk_widget_show(imgToolbar);
	btnClose = gtk_tool_button_new (imgToolbar, _("Close"));
	gtk_tool_item_set_is_important (GTK_TOOL_ITEM(btnClose), TRUE);
	g_signal_connect (G_OBJECT (btnClose), "clicked", G_CALLBACK (gmysqlcc_gui_query_evt_btnClose_clicked), gui_query);
	gtk_widget_show(GTK_WIDGET(btnClose));
	gtk_toolbar_insert(GTK_TOOLBAR(tlbSql), GTK_TOOL_ITEM(btnClose), -1);
	gtk_tool_item_set_tooltip (GTK_TOOL_ITEM(btnClose), tooltips, _("Close window"), NULL);

  vpanedSQL = gtk_vpaned_new ();
  gtk_widget_show (vpanedSQL);
  gtk_box_pack_start (GTK_BOX (vbox2), vpanedSQL, TRUE, TRUE, 0);
  gtk_paned_set_position (GTK_PANED (vpanedSQL), 100);

  scrlwndSQLRequest = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (scrlwndSQLRequest);
  gtk_paned_pack1 (GTK_PANED (vpanedSQL), scrlwndSQLRequest, FALSE, TRUE);

#ifdef USE_GTKSOURCEVIEW
  gui_query->txtSQLRequest = gtk_source_view_new ();
	gtk_source_view_set_show_line_numbers (GTK_SOURCE_VIEW(gui_query->txtSQLRequest), TRUE);
	gtk_source_view_set_tabs_width (GTK_SOURCE_VIEW(gui_query->txtSQLRequest), 2);
	txtBuffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(gui_query->txtSQLRequest));
	gtk_source_buffer_set_highlight (GTK_SOURCE_BUFFER(txtBuffer), TRUE);
	gtk_source_buffer_set_language(GTK_SOURCE_BUFFER(txtBuffer), 
			gtk_source_languages_manager_get_language_from_mime_type(LangManager, "text/x-sql"));
#else /* USE_GTKSOURCEVIEW */
  gui_query->txtSQLRequest = gtk_text_view_new ();
#endif /* USE_GTKSOURCEVIEW */
  gtk_widget_show (gui_query->txtSQLRequest);
  gtk_container_add (GTK_CONTAINER (scrlwndSQLRequest), gui_query->txtSQLRequest);
  
  gui_query->sclSQLResult = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (gui_query->sclSQLResult);
  gtk_paned_pack2 (GTK_PANED (vpanedSQL), gui_query->sclSQLResult, TRUE, TRUE);

  gui_query->lstSQLResult = gtk_tree_view_new ();
  gtk_widget_show (gui_query->lstSQLResult);
  gtk_container_add (GTK_CONTAINER (gui_query->sclSQLResult), gui_query->lstSQLResult);

	select = gtk_tree_view_get_selection (GTK_TREE_VIEW (gui_query->lstSQLResult));
	gtk_tree_selection_set_mode (select, GTK_SELECTION_SINGLE);
	g_signal_connect (G_OBJECT (select), "changed", G_CALLBACK (gmysqlcc_gui_query_evt_resultRow_selected), gui_query);

  gui_query->statusbarSQL = gtk_statusbar_new ();
  gtk_widget_show (gui_query->statusbarSQL);
  gtk_box_pack_start (GTK_BOX (vbox2), gui_query->statusbarSQL, FALSE, FALSE, 0);
	
}

void gmysqlcc_gui_query_execute_query (p_gmysqlcc_gui_query gui_query, const gchar * query) {
	GtkWidget * msgdlg;
	
	/*gmysql_history_add(gmysql_conf->queryHistory, gui_query->mysql_qry->mysql_srv->name, query, TRUE);*/
	
	mysql_query_free_query(gui_query->mysql_qry);
	if (!mysql_query_execute_query(gui_query->mysql_qry, query, TRUE)) {
		/* Query Not Ok */
		msgdlg = gtk_message_dialog_new(GTK_WINDOW(gui_query->window), GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, _("Error during the query : (%d) %s"), gui_query->mysql_qry->errCode, gui_query->mysql_qry->errMsg);
		gtk_dialog_run (GTK_DIALOG (msgdlg));
		gtk_widget_destroy (msgdlg);
		mysql_query_free_query(gui_query->mysql_qry);
		return;
	}
	
	/* Query Ok ... Continue */
	
	/* Modification query - Display affected rows */
	if (gui_query->mysql_qry->errCode == 0 && gui_query->mysql_qry->nbrField == 0) {
		g_printerr("Affected row : %d\n", gui_query->mysql_qry->editResult);
		msgdlg = gtk_message_dialog_new(GTK_WINDOW(gui_query->window), GTK_DIALOG_MODAL, GTK_MESSAGE_INFO, GTK_BUTTONS_OK, _("Affected row : %d\n"), gui_query->mysql_qry->editResult);
		gtk_dialog_run (GTK_DIALOG (msgdlg));
		gtk_widget_destroy (msgdlg);
		mysql_query_free_query(gui_query->mysql_qry);
		return;
	}
	
	/* Select query - Display datas */
	
	gmysqlcc_gui_query_clean_result (gui_query);
	
	gmysqlcc_gui_query_display_result (gui_query);

}

void gmysqlcc_gui_query_display_result (p_gmysqlcc_gui_query gui_query) {
	p_mysql_row mysql_rw;
	s_cols_nfo * ar_cols_nfo;
	
	int i, nbrcol, count;
	guint sbContext, sbItemId;
	
	GType * arTypes;
	GArray * arRow;
	GtkListStore * ls;
	GtkTreeIter iter;
	GtkTreeViewColumn * currCol;
	GtkCellRenderer * * ar_renderer;
	GValue gval = {0, };
	GValue gvalpoint = {0, };
	GValue gvalbool = {0, };
	GString * colTitle, *strSttsBar;
	
	nbrcol = gui_query->mysql_qry->nbrField + 1;
	arTypes = (GType *)g_try_malloc(nbrcol * sizeof(GType));
	for(i = 0; i < gui_query->mysql_qry->nbrField; i++) {
		arTypes[i] = G_TYPE_STRING;
	}
	arTypes[gui_query->mysql_qry->nbrField] = G_TYPE_POINTER;
	
	ls = gtk_list_store_newv(nbrcol, arTypes);
	
	g_free(arTypes);

	count = 0;
	g_value_init(&gval, G_TYPE_STRING);
	g_value_init(&gvalpoint, G_TYPE_POINTER);
	while ((mysql_rw = mysql_row_new_next_record(gui_query->mysql_qry))) {
		count ++;
		
		gtk_list_store_append(ls, &iter);
		
		for(i = 0; i < gui_query->mysql_qry->nbrField; i++) {
			g_value_set_string(&gval, mysql_row_get_field_value(mysql_rw, i));
			gtk_list_store_set_value(ls, &iter, i, &gval);
		}
		g_value_set_pointer(&gvalpoint, mysql_rw);
		gtk_list_store_set_value(ls, &iter, gui_query->mysql_qry->nbrField, &gvalpoint);
		gui_query->lstRows = g_slist_append(gui_query->lstRows, mysql_rw);
		
	}
	
	gtk_tree_view_set_model(GTK_TREE_VIEW(gui_query->lstSQLResult), GTK_TREE_MODEL(ls));
	g_object_unref (G_OBJECT (ls));
	
	if ((arRow = mysql_query_get_headers(gui_query->mysql_qry)) != (GArray * )NULL) {
		
		ar_cols_nfo = (s_cols_nfo *)g_try_malloc(gui_query->mysql_qry->nbrField * sizeof(s_cols_nfo));
		ar_renderer = (GtkCellRenderer * *)g_try_malloc(gui_query->mysql_qry->nbrField * sizeof(GtkCellRenderer *));
		if (mysql_query_is_editable(gui_query->mysql_qry)) {
			g_value_init(&gvalbool, G_TYPE_BOOLEAN);
			g_value_set_boolean(&gvalbool, TRUE);

			for (i = 0; i < gui_query->mysql_qry->nbrField; i++) {
				ar_renderer[i] = gtk_cell_renderer_text_new ();
				
				ar_cols_nfo[i].gui_query = gui_query;
				ar_cols_nfo[i].numCol = i;
				
				g_object_set_property(G_OBJECT(ar_renderer[i]), "editable", &gvalbool);
				g_signal_connect (G_OBJECT (ar_renderer[i]), "edited", G_CALLBACK (gmysqlcc_gui_query_evt_resultRow_edited), (gpointer)&ar_cols_nfo[i]);
			}
		} else {
			for (i = 0; i < gui_query->mysql_qry->nbrField; i++) {
				ar_renderer[i] = gtk_cell_renderer_text_new ();
			}
			g_print("Can't Edit :( !!!\n");
		}
	
		for(i = 0; i < gui_query->mysql_qry->nbrField; i++) {
			colTitle = gmysqlcc_helpers_protect_underscore(g_array_index(arRow, gchar *, i));
			currCol = gtk_tree_view_column_new_with_attributes (colTitle->str, ar_renderer[i], "text", i, NULL);
			gtk_tree_view_column_set_resizable(GTK_TREE_VIEW_COLUMN(currCol), TRUE);
			gtk_tree_view_column_set_sizing(GTK_TREE_VIEW_COLUMN(currCol), GTK_TREE_VIEW_COLUMN_AUTOSIZE);
			
			gtk_tree_view_append_column (GTK_TREE_VIEW (gui_query->lstSQLResult), currCol);
			
			g_string_free(colTitle, TRUE);
		}
		g_array_free(arRow, FALSE);
	}
	
	strSttsBar = g_string_new("");
	g_string_printf(strSttsBar, _("Records : %d"), count);
	
	g_print("%s\n", strSttsBar->str);
	sbContext = gtk_statusbar_get_context_id(GTK_STATUSBAR(gui_query->statusbarSQL), "SQL_Nb_Record");
	sbItemId = gtk_statusbar_push(GTK_STATUSBAR(gui_query->statusbarSQL), sbContext, strSttsBar->str);
	
	g_string_free(strSttsBar, TRUE);
	
}

void gmysqlcc_gui_query_clean_result (p_gmysqlcc_gui_query gui_query) {
	int count;
	GtkTreeViewColumn * currCol;
	GtkListStore * ls;
	
	/* Delete old Columns */
	while ((currCol = gtk_tree_view_get_column(GTK_TREE_VIEW(gui_query->lstSQLResult), 0)) != NULL) {
		gtk_tree_view_remove_column(GTK_TREE_VIEW(gui_query->lstSQLResult), currCol);
	}
	
	/* Clean List store */
	ls = (GtkListStore *)gtk_tree_view_get_model(GTK_TREE_VIEW(gui_query->lstSQLResult));
	if (ls != NULL) {
		
		g_print("Clean current list store ...\n");
		
		count = 0;
		while (gui_query->lstRows != NULL) {
			mysql_row_free((p_mysql_row)gui_query->lstRows->data);
			gui_query->lstRows = g_slist_delete_link (gui_query->lstRows, gui_query->lstRows);
			count ++;
		}
		g_print("Remove %d line(s)\n", count);
		
		/* Clean old List store */
		gtk_list_store_clear(ls);
		/*g_object_unref (G_OBJECT (ls));*/
		
	}	
}

void gmysqlcc_gui_query_evt_resultRow_edited (GtkCellRendererText *cellrenderertext, gchar *path_string, gchar *new_value, gpointer data) {
	GtkTreeIter iter;
	GtkTreeModel * tree_model;
	s_cols_nfo * ar_cols_nfo = (s_cols_nfo *)data;
	gchar * new_str;
	
/*	g_print("col : '%d' - string_path : '%s' - new_value : '%s'\n", ar_cols_nfo->numCol, path_string, new_value);*/

	new_str = mysql_row_set_field_value(ar_cols_nfo->gui_query->mysql_rw, ar_cols_nfo->numCol, new_value);
	
	if (new_str != (gchar *)NULL) {
		tree_model = gtk_tree_view_get_model(GTK_TREE_VIEW(ar_cols_nfo->gui_query->lstSQLResult));
		if (tree_model != (GtkTreeModel *)NULL) {
			if (gtk_tree_model_get_iter_from_string (tree_model, &iter, path_string)) {
				gtk_list_store_set(GTK_LIST_STORE(tree_model), &iter, ar_cols_nfo->numCol, new_str, -1);
			}
		}			
	}
}

void gmysqlcc_gui_query_evt_resultRow_selected (GtkTreeSelection *selection, gpointer data) {
	p_gmysqlcc_gui_query gui_query = (p_gmysqlcc_gui_query)data;
	GtkTreeIter iter;
	GtkTreeModel *model;
	int numCol;
	
	numCol = gui_query->mysql_qry->nbrField;
/*	g_print("Changed !\n");*/
	if (numCol > 0) {
		if (gtk_tree_selection_get_selected (selection, &model, &iter)) {
			gtk_tree_model_get (model, &iter, numCol, &gui_query->mysql_rw, -1);
			
/*			g_print("Primary where for update/delete : 'FROM %s WHERE %s'\n", pExecWnd->mysql_rw->abs_tbl_name, pExecWnd->mysql_rw->primary_where_part);*/
		}
	}	
}

void gmysqlcc_gui_query_evt_cmbCharset_changed (GtkComboBox *combobox, gpointer user_data) {
	p_gmysqlcc_gui_query gui_query = (p_gmysqlcc_gui_query)user_data;
	gint idx;
	gchar * charset;
	GtkWidget * msgdlg;
	
	idx = gtk_combo_box_get_active(GTK_COMBO_BOX(combobox));
	charset = gmysqlcc_helpers_charset_list_get_by_index(idx);
	if (!mysql_query_change_charset(gui_query->mysql_qry, charset)) {
		msgdlg = gtk_message_dialog_new(GTK_WINDOW(gui_query->window), GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, _("Can't use the charset : '%s'"), charset);
		gtk_dialog_run (GTK_DIALOG (msgdlg));
		gtk_widget_destroy (msgdlg);
	} else {
		gmysqlcc_gui_query_evt_btnExecSql_clicked (GTK_WIDGET(combobox), user_data);
	}
}

gboolean gmysqlcc_gui_query_evt_window_keyrelease (GtkWidget *widget, GdkEventKey *event, gpointer user_data) {
	if (event->state & GDK_CONTROL_MASK && (event->keyval == GDK_e || event->keyval == GDK_E)) {
		gmysqlcc_gui_query_evt_btnExecSql_clicked (widget, user_data);
		return TRUE;
	}
	return FALSE;
}

void gmysqlcc_gui_query_evt_destroy(GtkWidget *widget, gpointer user_data) {
	p_gmysqlcc_gui_query gui_query = (p_gmysqlcc_gui_query)user_data;
	
	gmysqlcc_gui_query_display(gui_query, FALSE);
	gmysqlcc_gui_query_delete(gui_query);
}

void gmysqlcc_gui_query_evt_btnExecSql_clicked(GtkWidget *widget, gpointer user_data) {
	p_gmysqlcc_gui_query gui_query = (p_gmysqlcc_gui_query)user_data;
	GtkTextBuffer * txtBuffer;
	GtkTextIter begin, end;
	gchar * csql;

	/*g_print("Execute SQL\n");*/
	txtBuffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(gui_query->txtSQLRequest));
	gtk_text_buffer_get_start_iter (GTK_TEXT_BUFFER(txtBuffer), &begin);
	gtk_text_buffer_get_end_iter (GTK_TEXT_BUFFER(txtBuffer), &end);
	csql = (gchar *)gtk_text_buffer_get_text (GTK_TEXT_BUFFER(txtBuffer), &begin, &end, FALSE);
	/*g_print("SQL Query : '%s'\n", csql);*/
	
	gmysqlcc_gui_query_execute_query(gui_query, csql);
}

void gmysqlcc_gui_query_evt_btnDuplicateSql_clicked(GtkWidget *widget, gpointer user_data) {
	p_gmysqlcc_gui_query gui_query = (p_gmysqlcc_gui_query)user_data;
	p_gmysqlcc_gui_query gui_query_dup;
	GtkTextBuffer * txtBuffer;
	GtkTextIter begin, end;
	gchar * sqlQuery;
	
	txtBuffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(gui_query->txtSQLRequest));
	gtk_text_buffer_get_start_iter (GTK_TEXT_BUFFER(txtBuffer), &begin);
	gtk_text_buffer_get_end_iter (GTK_TEXT_BUFFER(txtBuffer), &end);
	sqlQuery = gtk_text_buffer_get_text(GTK_TEXT_BUFFER(txtBuffer), &begin, &end, FALSE);
	
	gui_query_dup = gmysqlcc_gui_query_new(mysql_query_duplicate(gui_query->mysql_qry));
	gmysqlcc_gui_query_set_query(gui_query_dup, sqlQuery, FALSE);
	gmysqlcc_gui_query_display(gui_query_dup, TRUE);
	
	g_free(sqlQuery);
}

void gmysqlcc_gui_query_evt_btnDumpSql_clicked(GtkWidget *widget, gpointer user_data) {
	p_gmysqlcc_gui_query gui_query = (p_gmysqlcc_gui_query)user_data;
	p_gmysqlcc_gui_dump gui_dump;
	GtkTextBuffer * txtBuffer;
	GtkTextIter begin, end;
	gchar * sqlQuery;
	
	txtBuffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(gui_query->txtSQLRequest));
	gtk_text_buffer_get_start_iter (GTK_TEXT_BUFFER(txtBuffer), &begin);
	gtk_text_buffer_get_end_iter (GTK_TEXT_BUFFER(txtBuffer), &end);
	sqlQuery = gtk_text_buffer_get_text(GTK_TEXT_BUFFER(txtBuffer), &begin, &end, FALSE);
	
	g_print("Dump query ...\n");
	
	gui_dump = gmysqlcc_gui_dump_new(NULL, mysql_query_get_database(gui_query->mysql_qry), NULL, sqlQuery);
	gmysqlcc_gui_dump_display(gui_dump, TRUE);
	
	g_free(sqlQuery);
}

void gmysqlcc_gui_query_evt_btnClose_clicked(GtkWidget *widget, gpointer user_data) {
	p_gmysqlcc_gui_query gui_query = (p_gmysqlcc_gui_query)user_data;
	
	gtk_widget_destroy(GTK_WIDGET(gui_query->window));
}

