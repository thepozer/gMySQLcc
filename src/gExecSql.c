
#include "gExecSql.h"

extern int NbrWnd;

/* Definitions of all locals functions */
GString * protectUnderscore (const gchar * str);
gboolean ExecSqlEvent (GtkWidget *widget, GdkEventKey *event, gpointer user_data);
void displayLstQueries (p_execSqlWnd pExecWnd);
static void loadSqlFile (GtkWidget *widget, gpointer user_data);

static void destroy(GtkWidget *widget, gpointer user_data);
static void btnexecsql_clicked (GtkWidget *widget, gpointer user_data);
static void btnduplicatesql_clicked (GtkWidget *widget, gpointer user_data);
static void btnmultisql_clicked (GtkWidget *widget, gpointer user_data);
static void btnclose_clicked (GtkWidget *widget, gpointer user_data);
void resultRow_edited (GtkCellRendererText *cellrenderertext, gchar *arg1, gchar *arg2, gpointer user_data);

typedef struct _s_cols_nfo {
	p_execSqlWnd pExecWnd;
	int numCol;
} s_cols_nfo;


/* Functions */

GString * protectUnderscore (const gchar * str) {
	GString * pgstr;
	int i;
	
	pgstr = g_string_new(str);
	
	for (i = 0; i < pgstr->len; i++) {
		if (pgstr->str[i] == '_') {
			g_string_insert_c(pgstr, i, '_');
			i++;
		}
	}
	
	return pgstr;
}

void ExecSql (p_execSqlWnd pExecWnd, const char * sqlQuery) {
	p_mysql_query msql_qry;
	p_mysql_row mysql_rw;
	s_cols_nfo * ar_cols_nfo;
	
	gchar * strtmp;
	int row, i, nbrcol;

	GType * arTypes;
	GArray * arRow;
	GtkListStore * ls;
	GtkTreeIter iter;
	GtkTreeViewColumn * currCol;
	GtkCellRenderer * * ar_renderer;
	GValue gval = {0, };
	GValue gvalpoint = {0, };
	GValue gvalbool = {0, };
	GtkWidget * msgdlg;
	GString * colTitle;
	
	
	msql_qry = pExecWnd->msql_qry;
	
	/* Delete old Columns */
	while ((currCol = gtk_tree_view_get_column(GTK_TREE_VIEW(pExecWnd->lstSQLResult), 0)) != (GtkTreeViewColumn *)NULL) {
		gtk_tree_view_remove_column(GTK_TREE_VIEW(pExecWnd->lstSQLResult), currCol);
	}
	
	mysql_query_free_query(msql_qry);
	if (!mysql_query_execute_query(msql_qry, sqlQuery)) {
		/* Query Not Ok */
		msgdlg = gtk_message_dialog_new(GTK_WINDOW(pExecWnd->wndMain), GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "Error during the query : (%d) %s", msql_qry->errCode, msql_qry->errMsg);
		gtk_dialog_run (GTK_DIALOG (msgdlg));
		gtk_widget_destroy (msgdlg);
		mysql_query_free_query(msql_qry);
		return;
	}
	
	/* Query Ok ... Continue */
	
	/* Modification query - Display affected rows */
	if (msql_qry->errCode == 0 && msql_qry->nbrField == 0) {
		g_printerr("Affected row : %d\n", msql_qry->editResult);
		msgdlg = gtk_message_dialog_new(GTK_WINDOW(pExecWnd->wndMain), GTK_DIALOG_MODAL, GTK_MESSAGE_INFO, GTK_BUTTONS_OK, "Affected row : %d\n", msql_qry->editResult);
		gtk_dialog_run (GTK_DIALOG (msgdlg));
		gtk_widget_destroy (msgdlg);
		mysql_query_free_query(msql_qry);
		return;
	}
	
	/* Select query - Display datas */
	
	nbrcol = msql_qry->nbrField + 1;
	arTypes = (GType *)g_try_malloc(nbrcol * sizeof(GType));
	for(i = 0; i < msql_qry->nbrField; i++) {
		arTypes[i] = G_TYPE_STRING;
	}
	arTypes[msql_qry->nbrField] = G_TYPE_POINTER;
	
	ls = gtk_list_store_newv(nbrcol, arTypes);
	
	g_value_init(&gval, G_TYPE_STRING);
	g_value_init(&gvalpoint, G_TYPE_POINTER);
	while (mysql_rw = mysql_row_new_next_record(msql_qry)) {
		
		gtk_list_store_append(ls, &iter);
		
		for(i = 0; i < msql_qry->nbrField; i++) {
			g_value_set_string(&gval, mysql_row_get_field_value(mysql_rw, i));
			gtk_list_store_set_value(ls, &iter, i, &gval);
			
		}
		g_value_set_pointer(&gvalpoint, mysql_rw);
		gtk_list_store_set_value(ls, &iter, msql_qry->nbrField, &gvalpoint);
		
	}
	
	gtk_tree_view_set_model(GTK_TREE_VIEW(pExecWnd->lstSQLResult), GTK_TREE_MODEL(ls));
	g_object_unref (G_OBJECT (ls));
		
	g_free(arTypes);
	
	if ((arRow = mysql_query_get_headers(msql_qry)) != (GArray * )NULL) {

		ar_cols_nfo = (s_cols_nfo *)g_try_malloc(msql_qry->nbrField * sizeof(s_cols_nfo));
		ar_renderer = (GtkCellRenderer * *)g_try_malloc(msql_qry->nbrField * sizeof(GtkCellRenderer *));
		if (mysql_query_is_editable(msql_qry)) {
			g_value_init(&gvalbool, G_TYPE_BOOLEAN);
			g_value_set_boolean(&gvalbool, TRUE);

			for (i = 0; i < msql_qry->nbrField; i++) {
				ar_renderer[i] = gtk_cell_renderer_text_new ();
				
				ar_cols_nfo[i].pExecWnd = pExecWnd;
				ar_cols_nfo[i].numCol = i;
				
				g_object_set_property(G_OBJECT(ar_renderer[i]), "editable", &gvalbool);
				g_signal_connect (G_OBJECT (ar_renderer[i]), "edited", G_CALLBACK (resultRow_edited), (gpointer)&ar_cols_nfo[i]);
			}
		} else {
			for (i = 0; i < msql_qry->nbrField; i++) {
				ar_renderer[i] = gtk_cell_renderer_text_new ();
			}
			g_print("Can't Edit :( !!!\n");
		}
	
		for(i = 0; i < msql_qry->nbrField; i++) {
			colTitle = protectUnderscore(g_array_index(arRow, gchar *, i));
			currCol = gtk_tree_view_column_new_with_attributes (colTitle->str, ar_renderer[i], "text", i, NULL);
			gtk_tree_view_column_set_resizable(GTK_TREE_VIEW_COLUMN(currCol), TRUE);
			gtk_tree_view_column_set_sizing(GTK_TREE_VIEW_COLUMN(currCol), GTK_TREE_VIEW_COLUMN_AUTOSIZE);
			
			gtk_tree_view_append_column (GTK_TREE_VIEW (pExecWnd->lstSQLResult), currCol);
			
			g_string_free(colTitle, TRUE);
		}
		g_array_free(arRow, FALSE);
	}
	
	//mysql_query_free_query(msql_qry);
	
}

void displayLstQueries (p_execSqlWnd pExecWnd) {
	GtkListStore * lstStorQueries;
	GtkTreeIter iter;
	GtkTreeViewColumn * currCol;
	GtkCellRenderer * renderer, * checkRenderer;
	GtkTreeSelection* selection;
	GString * lineSql;
	int pos;
	
	/*lstStorQueries = gtk_list_store_new(3, G_TYPE_UINT, G_TYPE_BOOLEAN, G_TYPE_STRING);*/
	lstStorQueries = gtk_list_store_new(2, G_TYPE_UINT, G_TYPE_STRING);
	for (pos = 0; pos < pExecWnd->lstQueries->len; pos ++) {
		lineSql = (GString *)g_ptr_array_index(pExecWnd->lstQueries, pos);
		if (lineSql != (GString *)NULL) {
			g_print("Sql query : \n%s\n", lineSql->str);
			gtk_list_store_append (lstStorQueries, &iter);
			gtk_list_store_set (lstStorQueries, &iter, 0, pos, 1, lineSql->str, -1);
		} else {
			g_print("Sql query : NULL\n");
		}
	}

	gtk_tree_view_set_model(GTK_TREE_VIEW(pExecWnd->lstSQLQueries), GTK_TREE_MODEL(lstStorQueries));
	g_object_unref (G_OBJECT (lstStorQueries));
		
	currCol = gtk_tree_view_get_column(GTK_TREE_VIEW(pExecWnd->lstSQLQueries), 0);
	while (currCol != (GtkTreeViewColumn *)NULL) {
		gtk_tree_view_remove_column(GTK_TREE_VIEW(pExecWnd->lstSQLQueries), currCol);
	}

	renderer = gtk_cell_renderer_text_new ();
	checkRenderer = gtk_cell_renderer_toggle_new();

	currCol = gtk_tree_view_column_new_with_attributes ("Num", renderer, "text", 0, NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW (pExecWnd->lstSQLQueries), currCol);
	/*currCol = gtk_tree_view_column_new_with_attributes ("Execute", checkRenderer, "text", 1, NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW (pExecWnd->lstSQLQueries), currCol);*/
	currCol = gtk_tree_view_column_new_with_attributes ("Query", renderer, "text", 1, NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW (pExecWnd->lstSQLQueries), currCol);

}

gboolean ExecSqlEvent (GtkWidget *widget, GdkEventKey *event, gpointer user_data) {
	if (event->state & GDK_CONTROL_MASK && (event->keyval == GDK_e || event->keyval == GDK_E)) {
		btnexecsql_clicked (widget, user_data);
		return TRUE;
	}
	return FALSE;
}


static void btnduplicatesql_clicked (GtkWidget *widget, gpointer user_data) {
	p_execSqlWnd pExecWnd = (p_execSqlWnd)user_data;
	p_execSqlWnd psqlWnd;
	GtkTextBuffer * txtBuffer;
	GtkTextIter begin, end;
	gchar * sqlQuery;
	
	psqlWnd = create_wndSQL(TRUE, mysql_query_duplicate(pExecWnd->msql_qry), FALSE);
	txtBuffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(pExecWnd->txtSQLRequest));
	gtk_text_buffer_get_start_iter (GTK_TEXT_BUFFER(txtBuffer), &begin);
	gtk_text_buffer_get_end_iter (GTK_TEXT_BUFFER(txtBuffer), &end);
	sqlQuery = gtk_text_buffer_get_text(GTK_TEXT_BUFFER(txtBuffer), &begin, &end, FALSE);
	
	txtBuffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(psqlWnd->txtSQLRequest));
	gtk_text_buffer_set_text(GTK_TEXT_BUFFER(txtBuffer), sqlQuery, -1);
	
	g_free(sqlQuery);
}

static void btndumpsql_clicked (GtkWidget *widget, gpointer user_data) {
	p_execSqlWnd pExecWnd = (p_execSqlWnd)user_data;
	p_dumpWnd pDmpWnd;
	p_mysql_database mysql_db;
	GtkTextBuffer * txtBuffer;
	GtkTextIter begin, end;
	gchar * sqlQuery;
	
	txtBuffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(pExecWnd->txtSQLRequest));
	gtk_text_buffer_get_start_iter (GTK_TEXT_BUFFER(txtBuffer), &begin);
	gtk_text_buffer_get_end_iter (GTK_TEXT_BUFFER(txtBuffer), &end);
	sqlQuery = gtk_text_buffer_get_text(GTK_TEXT_BUFFER(txtBuffer), &begin, &end, FALSE);
	
	g_print("Dump query ...\n");
	
	pDmpWnd = create_wndDump(TRUE, (p_mysql_server)NULL, mysql_query_get_database(pExecWnd->msql_qry), (p_mysql_table)NULL, sqlQuery);

	g_free(sqlQuery);
}

static void btnexecsql_clicked (GtkWidget *widget, gpointer user_data) {
	p_execSqlWnd pExecWnd = (p_execSqlWnd)user_data;
	GtkTextBuffer * txtBuffer;
	GtkTextIter begin, end;
	gchar * csql;

	/*g_print("Execute SQL\n");*/
	txtBuffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(pExecWnd->txtSQLRequest));
	gtk_text_buffer_get_start_iter (GTK_TEXT_BUFFER(txtBuffer), &begin);
	gtk_text_buffer_get_end_iter (GTK_TEXT_BUFFER(txtBuffer), &end);
	csql = (gchar *)gtk_text_buffer_get_text (GTK_TEXT_BUFFER(txtBuffer), &begin, &end, FALSE);
	/*g_print("SQL Query : '%s'\n", csql);*/
	
	ExecSql(pExecWnd, csql);
}

static void btnexecallsql_clicked (GtkWidget *widget, gpointer user_data) {
	p_execSqlWnd pExecWnd = (p_execSqlWnd)user_data;
	GtkWidget * msgdlg;
	p_mysql_query msql_qry = pExecWnd->msql_qry;
	GString * lineSql, * result;
	int pos;
	
	mysql_query_free_query(msql_qry);
	result = g_string_new("Results : \n");
	for (pos = 0; pos < pExecWnd->lstQueries->len; pos ++) {
		lineSql = (GString *)g_ptr_array_index(pExecWnd->lstQueries, pos);

		if (lineSql != (GString *)NULL) {
			g_print("Sql query : \n%s\n", lineSql->str);
			
			if (!mysql_query_execute_query(msql_qry, lineSql->str)) {
				g_string_append_printf(result, "Query : %d - Error during the query : (%d) %s\n", pos, msql_qry->errCode, msql_qry->errMsg);
			} else {
				g_string_append_printf(result, "Query : %d - Affected row : %d\n", pos, msql_qry->editResult);
			}
			
			mysql_query_free_query(msql_qry);
		}
	}
	
	msgdlg = gtk_message_dialog_new(GTK_WINDOW(pExecWnd->wndMain), 0, GTK_MESSAGE_INFO, GTK_BUTTONS_OK, result->str);
	gtk_dialog_run (GTK_DIALOG (msgdlg));
	gtk_widget_destroy (msgdlg);
	
	g_string_free(result, TRUE);
}

static void btnmultisql_clicked (GtkWidget *widget, gpointer user_data) {
	p_execSqlWnd pExecWnd = (p_execSqlWnd)user_data;
	
	pExecWnd->multiSqlMode = (pExecWnd->multiSqlMode) ? FALSE : TRUE ; /* Toggle state */
	
	if (pExecWnd->multiSqlMode) {
	  gtk_widget_hide (pExecWnd->tlbSingleSql);
	  gtk_widget_hide (pExecWnd->sclSQLResult);
	  gtk_widget_show (pExecWnd->tlbMultiSql);
	  gtk_widget_show (pExecWnd->vbxSQLQueries);
	} else {
	  gtk_widget_show (pExecWnd->tlbSingleSql);
	  gtk_widget_show (pExecWnd->sclSQLResult);
	  gtk_widget_hide (pExecWnd->tlbMultiSql);
	  gtk_widget_hide (pExecWnd->vbxSQLQueries);
	}
}

static void btnclose_clicked (GtkWidget *widget, gpointer user_data) {
	p_execSqlWnd pExecWnd = (p_execSqlWnd)user_data;
	
	gtk_widget_destroy(GTK_WIDGET(pExecWnd->wndMain));
}

static void btnqueryopen_clicked (GtkWidget *widget, gpointer user_data) {
	p_execSqlWnd pExecWnd = (p_execSqlWnd)user_data;
	
	askFilename("Load sql file", (gchar *) NULL, loadSqlFile, user_data);
}

static void loadSqlFile (GtkWidget *widget, gpointer user_data) {
	p_askFilenameInfos paskFnInf = (p_askFilenameInfos)user_data;
	p_execSqlWnd pExecWnd = (p_execSqlWnd)paskFnInf->userData;
	GString * dataSql, * lineSql;
	GIOChannel * sqlFile;
	GError * err = (GError *)NULL;
	gint32 pos = 0, step = 0;
	gchar car, prevcar;
	
	pExecWnd->sqlFilename = g_string_new((gchar *)gtk_file_selection_get_filename(GTK_FILE_SELECTION (paskFnInf->dialog)));
	
	/* Read SQL file */
	dataSql = g_string_new("");
	lineSql = g_string_new("");
	sqlFile = g_io_channel_new_file(pExecWnd->sqlFilename->str, "r", &err);
	if (err != (GError *)NULL) {
		g_print("Error : '%s'\n", err->message);
	}
	if (sqlFile != (GIOChannel *)NULL) {
		while (g_io_channel_read_line_string(sqlFile, lineSql, (gsize *)NULL, &err) != G_IO_STATUS_EOF) {
			if (err != (GError *)NULL) {
				g_print("Error : '%s'\n", err->message);
			}
			g_string_append(dataSql, lineSql->str);
		}
		g_io_channel_unref(sqlFile);
	}
	g_string_free(lineSql, TRUE);
	
	/* Parse SQL File to find queries */
	pExecWnd->lstQueries = g_ptr_array_new();
	
	lineSql = g_string_new("");
	car = 0;
	while (pos < dataSql->len) {
		prevcar = car;
		car = dataSql->str[pos];
		switch (step) {
			case 0:
				switch (car) {
					case '\'' :
						g_string_append_c(lineSql, car);
						step = 1;
						break;
					case '\"' :
						g_string_append_c(lineSql, car);
						step = 2;
						break;
					case ';' :
						step = 3;
						break;
					case '-' :
						step = 4;
						break;
					case '#' :
						step = 5;
						break;
					default :
						if (!(lineSql->len == 0 && (car == ' ' || car == '\t' || car == '\n'))) {
							g_string_append_c(lineSql, car);
						}
						break;
				}
				break;
			case 1:
				g_string_append_c(lineSql, car);
				if (car == '\'' && prevcar != '\\') {
					step = 0;
				}
				break;
			case 2:
				g_string_append_c(lineSql, car);
				if (car == '\"' && prevcar != '\\') {
					step = 0;
				}
				break;
			case 3:
				g_ptr_array_add (pExecWnd->lstQueries, lineSql);
				lineSql = g_string_new("");
				car = prevcar;
				pos --;
				step = 0;
				break;
			case 4:
				if (car != '-') {
					g_string_append_c(lineSql, prevcar);
					g_string_append_c(lineSql, car);
					step = 0;
				} else {
					step = 5;
				}
				break;
			case 5:
				if (car == '\n') {
					step = 0;
				}
				break;
		}
		pos ++;
	}
	
	g_string_free(lineSql, TRUE);
	g_string_free(dataSql, TRUE);
	
	/* Fill the query list */
	displayLstQueries(pExecWnd);
		
}


static void lstsqlqueries_selected (GtkTreeSelection *selection, gpointer user_data) {
	GtkTreeIter iter;
	GtkTreeModel *model;
	p_execSqlWnd pExecWnd = (p_execSqlWnd)user_data;
	gchar * sqlQuery;
	GtkTextBuffer * txtBuffer;
	
	if (gtk_tree_selection_get_selected (selection, &model, &iter)) {
		gtk_tree_model_get (model, &iter, 1, &sqlQuery, -1);
		
		/*g_print ("Selected query : '%s'\n", sqlQuery);*/
		txtBuffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(pExecWnd->txtSQLRequest));
		gtk_text_buffer_set_text(GTK_TEXT_BUFFER(txtBuffer), sqlQuery, -1);
	}
}

static void btnquerysave_clicked (GtkWidget *widget, gpointer user_data) {
	p_execSqlWnd pExecWnd = (p_execSqlWnd)user_data;
	
}

static void btnquerynew_clicked (GtkWidget *widget, gpointer user_data) {
	p_execSqlWnd pExecWnd = (p_execSqlWnd)user_data;
	GtkTextBuffer * txtBuffer;
	
	txtBuffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(pExecWnd->txtSQLRequest));
	gtk_text_buffer_set_text(GTK_TEXT_BUFFER(txtBuffer), "", -1);
}

static void btnqueryup_clicked (GtkWidget *widget, gpointer user_data) {
	p_execSqlWnd pExecWnd = (p_execSqlWnd)user_data;
	
}

static void btnquerydown_clicked (GtkWidget *widget, gpointer user_data) {
	p_execSqlWnd pExecWnd = (p_execSqlWnd)user_data;
	
}

static void btnqueryadd_clicked (GtkWidget *widget, gpointer user_data) {
	p_execSqlWnd pExecWnd = (p_execSqlWnd)user_data;
	
}

static void btnqueryedit_clicked (GtkWidget *widget, gpointer user_data) {
	p_execSqlWnd pExecWnd = (p_execSqlWnd)user_data;
	
}

static void btnquerydel_clicked (GtkWidget *widget, gpointer user_data) {
	p_execSqlWnd pExecWnd = (p_execSqlWnd)user_data;
	
}

static void destroy(GtkWidget *widget, gpointer user_data) {
	p_execSqlWnd pExecWnd = (p_execSqlWnd)user_data;
	
	/* Destroy GUI */
	gtk_widget_hide (GTK_WIDGET(pExecWnd->wndMain));
	gtk_widget_destroy (GTK_WIDGET(pExecWnd->wndMain));
	
	/* Destroy MySql Connection */
	mysql_query_delete(pExecWnd->msql_qry);
	
	/* Destroy Datas */
	g_free(pExecWnd);
	
	/* Destroy Application if needed */
	NbrWnd--;
	g_printerr("Destruction main window - nbrWnd : %d\n", NbrWnd);
	if (NbrWnd <= 0) {
		g_printerr("Destroy App\n");
		gtk_main_quit();
	}
}

static void resultRow_selected (GtkTreeSelection *selection, gpointer data) {
	p_execSqlWnd pExecWnd = (p_execSqlWnd)data;
	GtkTreeIter iter;
	GtkTreeModel *model;
	int numCol;
	
	numCol = pExecWnd->msql_qry->nbrField;
/*	g_print("Changed !\n");*/
	if (numCol > 0) {
		if (gtk_tree_selection_get_selected (selection, &model, &iter)) {
			gtk_tree_model_get (model, &iter, numCol, &pExecWnd->mysql_rw, -1);
			
/*			g_print("Primary where for update/delete : 'FROM %s WHERE %s'\n", pExecWnd->mysql_rw->abs_tbl_name, pExecWnd->mysql_rw->primary_where_part);*/
		}
	}
}

void resultRow_edited (GtkCellRendererText *cellrenderertext, gchar *path_string, gchar *new_value, gpointer data) {
	GtkTreeIter iter;
	GtkTreeModel * tree_model;
	s_cols_nfo * ar_cols_nfo = (s_cols_nfo *)data;
	gchar * new_str;
	
/*	g_print("col : '%d' - string_path : '%s' - new_value : '%s'\n", ar_cols_nfo->numCol, path_string, new_value);*/

	new_str = mysql_row_set_field_value(ar_cols_nfo->pExecWnd->mysql_rw, ar_cols_nfo->numCol, new_value);
	
	if (new_str != (gchar *)NULL) {
		tree_model = gtk_tree_view_get_model(GTK_TREE_VIEW(ar_cols_nfo->pExecWnd->lstSQLResult));
		if (tree_model != (GtkTreeModel *)NULL) {
			if (gtk_tree_model_get_iter_from_string (tree_model, &iter, path_string)) {
				gtk_list_store_set(GTK_LIST_STORE(tree_model), &iter, ar_cols_nfo->numCol, new_str, -1);
			}
		}			
	}
}

/* Main algorithm to colorize SQL code */
gboolean colorizeSqlText(GtkTextView *widget, GdkEventKey *event, gpointer user_data) {
	GtkTextBuffer *txtBuffer;
	char *csql;
	char c, startChar;
	GtkTextIter begin, end, start, finish;
	unsigned int i, length;
	
	txtBuffer = gtk_text_view_get_buffer(widget);
	gtk_text_buffer_get_start_iter (txtBuffer, &begin);
	gtk_text_buffer_get_end_iter (txtBuffer, &end);
	csql = gtk_text_buffer_get_text (txtBuffer, &begin, &end, FALSE);
	
	gtk_text_buffer_remove_all_tags(txtBuffer, &begin, &end);
	length = gtk_text_iter_get_offset(&end);
	
	for (i = 0; i < length; i++) {
		c = csql[i];
		switch (c) {
			case '#':
				gtk_text_buffer_get_iter_at_offset(txtBuffer, &start, i);
				while (++i < length) {
					if (csql[i] == '\n') {
						break;
					}
				}
				gtk_text_buffer_get_iter_at_offset(txtBuffer, &finish, i);
				gtk_text_buffer_apply_tag_by_name(txtBuffer, "comment", &start, &finish);
				
				break;
			case '-':
				if (i + 1 == length) break; /* End of text */
			
				if (csql[i + 1] == '-') { /* Line comment */
					gtk_text_buffer_get_iter_at_offset(txtBuffer, &start, i);
					while (++i < length) {
						if (csql[i] == '\n') {
							break;
						}
					}
					gtk_text_buffer_get_iter_at_offset(txtBuffer, &finish, i);
					gtk_text_buffer_apply_tag_by_name(txtBuffer, "comment", &start, &finish);
				}
				break;
			case '/': /* Comment ? */
				if (i + 1 == length) break; /* End of text */
			
				if (csql[i + 1] == '*') { /* Comment :) */
					gtk_text_buffer_get_iter_at_offset(txtBuffer, &start, i);
					while (++i < length) {
						if (csql[i] == '*') {
							i++;
							if (i == length) break;
							if (csql[i] == '/') {
								i++;
								break;
							}
						}
					}
				}
				else {
					break;
				}
				
				gtk_text_buffer_get_iter_at_offset(txtBuffer, &finish, i);
				gtk_text_buffer_apply_tag_by_name(txtBuffer, "comment", &start, &finish);
				break;
			case '"':
			case '\'':
				gtk_text_buffer_get_iter_at_offset(txtBuffer, &start, i);
				startChar = c;
			
				while (++i < length) {
					c = csql[i];
					if ( c == '\\' ) {
						i++;
					} else if (c == startChar) {
						i++;
						break;
					}
				}
				
				gtk_text_buffer_get_iter_at_offset(txtBuffer, &finish, i);
				gtk_text_buffer_apply_tag_by_name(txtBuffer, "string", &start, &finish);
				break;
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
				/* Numeric coloration */
				/* Check if we are not at begining */
				if ( i > 0 ) {
					c = csql[i - 1];
					// Check if previous character is not a letter
					if (c >= 'a' && c <= 'z' || c >= 'A' && c <= 'Z' ) {
						break;
					} else if (c == '.') { 
						i--;
					}
				}
				
				gtk_text_buffer_get_iter_at_offset(txtBuffer, &start, i);
				while (++i < length) {
					if (i == length) break;
					
					c = csql[i];
					
					if ( !(c >= '0' && c <= '9' || c == '.') ) {
						break;
					}
				}
				
				gtk_text_buffer_get_iter_at_offset(txtBuffer, &finish, i);
				gtk_text_buffer_apply_tag_by_name(txtBuffer, "number", &start, &finish);
				
				break;
		}
	}
	
	free(csql);
	return FALSE;
}

/* Init SQL tags for coloration */
void init_sqlTags(GtkTextBuffer *buffer) {
	gtk_text_buffer_create_tag(buffer, "keyword", "foreground", "#0000CC", "weight", PANGO_WEIGHT_BOLD, NULL);
	gtk_text_buffer_create_tag(buffer, "comment", "foreground", "#337F33", NULL);
	gtk_text_buffer_create_tag(buffer, "string", "foreground", "#B25900", NULL);
	gtk_text_buffer_create_tag(buffer, "number", "foreground", "#007777", NULL);
}

p_execSqlWnd create_wndSQL(gboolean display, p_mysql_query msql_qry, gboolean multiple) {
	p_execSqlWnd pExecWnd;
	GString * sTitle;
	
	GtkWidget *vbox2;
	GtkWidget *hbox1;
	GtkWidget *vpanedSQL;
	GtkWidget *vpanedQueries;
	GtkWidget *scrlwndSQLQueries;
  GtkWidget *scrlwndSQLRequest;
  GtkWidget *scrlwndSQLResult;
  GtkWidget *statusbarSQL;
	GtkWidget *toolbar1;
	GtkWidget *btnExecSql;
	GtkWidget *btnExecAllSql;
	GtkWidget *btnMultiSql;
	GtkWidget *btnDumpSql;
	GtkWidget *btnDuplicateSql;
	GtkWidget *btnClose;
	GtkWidget *alignment2;
  GtkWidget *hbox10;
  GtkWidget *image2;
  GtkWidget *label18;
  GtkWidget *btnQueryNew;
  GtkWidget *btnQueryAdd;
  GtkWidget *btnQueryEdit;
  GtkWidget *btnQueryDel;
	GtkWidget *btnQueryOpen;
	GtkWidget *btnQuerySave;
	
	GtkListStore * ls;
	GtkTreeIter iter;
	GtkCellRenderer * renderer;
	GtkTreeViewColumn * currCol;
	GtkTreeSelection *select;

	/* Init structure */
	pExecWnd = (p_execSqlWnd) g_try_malloc(sizeof(execSqlWnd));
	
	if (pExecWnd == (p_execSqlWnd)NULL) {
		return pExecWnd; /* return NULL pointer */
	}
	
	pExecWnd->msql_qry = msql_qry;
	pExecWnd->multiSqlMode = FALSE;
	
	g_print("Using query(%d) - serveur name(%d) : '%s' - database name : '%s'\n", (int)msql_qry, (int)msql_qry->mysql_srv, msql_qry->mysql_srv->name, msql_qry->db_name);
	
  pExecWnd->wndMain = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	sTitle = g_string_new("");
	g_string_printf(sTitle, "SQL Request - %s - %s", msql_qry->mysql_srv->name, msql_qry->db_name);
  gtk_window_set_title (GTK_WINDOW (pExecWnd->wndMain), sTitle->str);
	g_string_free(sTitle, TRUE);
	gtk_window_set_default_size (GTK_WINDOW (pExecWnd->wndMain), 400, 300);
	
	g_signal_connect (G_OBJECT (pExecWnd->wndMain), "key-release-event", G_CALLBACK (ExecSqlEvent), pExecWnd);
	g_signal_connect (G_OBJECT (pExecWnd->wndMain), "destroy", G_CALLBACK (destroy), pExecWnd);
	
  vbox2 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox2);
  gtk_container_add (GTK_CONTAINER (pExecWnd->wndMain), vbox2);

  pExecWnd->tlbSingleSql = gtk_toolbar_new ();
  gtk_widget_show (pExecWnd->tlbSingleSql);
  gtk_box_pack_start (GTK_BOX (vbox2), pExecWnd->tlbSingleSql, FALSE, FALSE, 0);
  gtk_toolbar_set_style (GTK_TOOLBAR (pExecWnd->tlbSingleSql), GTK_TOOLBAR_BOTH_HORIZ);

	btnExecSql = gtk_toolbar_append_item (GTK_TOOLBAR(pExecWnd->tlbSingleSql), 
		"Execute", "Execute SQL query", "", 
		gtk_image_new_from_stock(GTK_STOCK_EXECUTE, GTK_ICON_SIZE_LARGE_TOOLBAR),
		G_CALLBACK (btnexecsql_clicked), pExecWnd);

	btnDuplicateSql = gtk_toolbar_append_item (GTK_TOOLBAR(pExecWnd->tlbSingleSql), 
		"Duplicate", "Duplicate SQL Windows", "", 
		gtk_image_new_from_stock(GTK_STOCK_COPY, GTK_ICON_SIZE_LARGE_TOOLBAR),
		G_CALLBACK (btnduplicatesql_clicked), pExecWnd);
		
	btnDumpSql = gtk_toolbar_append_item (GTK_TOOLBAR(pExecWnd->tlbSingleSql), 
		"Dump", "Dumpwith current query", "", 
		gtk_image_new_from_stock(GTK_STOCK_FLOPPY, GTK_ICON_SIZE_LARGE_TOOLBAR),
		G_CALLBACK (btndumpsql_clicked), pExecWnd);
		
	btnMultiSql = gtk_toolbar_append_item (GTK_TOOLBAR(pExecWnd->tlbSingleSql), 
		"Multiple", "Toggle Multiple/Single SQL query", "", 
		gtk_image_new_from_stock(GTK_STOCK_CONVERT, GTK_ICON_SIZE_LARGE_TOOLBAR),
		G_CALLBACK (btnmultisql_clicked), pExecWnd);

	btnClose = gtk_toolbar_append_item (GTK_TOOLBAR(pExecWnd->tlbSingleSql), 
		"Close", "Close window", "", 
		gtk_image_new_from_stock(GTK_STOCK_CLOSE, GTK_ICON_SIZE_LARGE_TOOLBAR),
		G_CALLBACK (btnclose_clicked), pExecWnd);

  pExecWnd->tlbMultiSql = gtk_toolbar_new ();
  gtk_box_pack_start (GTK_BOX (vbox2), pExecWnd->tlbMultiSql, FALSE, FALSE, 0);
  gtk_toolbar_set_style (GTK_TOOLBAR (pExecWnd->tlbMultiSql), GTK_TOOLBAR_BOTH_HORIZ);

	btnExecSql = gtk_toolbar_append_item (GTK_TOOLBAR(pExecWnd->tlbMultiSql), 
		"Execute", "Execute SQL query", "", 
		gtk_image_new_from_stock(GTK_STOCK_EXECUTE, GTK_ICON_SIZE_LARGE_TOOLBAR),
		G_CALLBACK (btnexecsql_clicked), pExecWnd);

	btnExecAllSql = gtk_toolbar_append_item (GTK_TOOLBAR(pExecWnd->tlbMultiSql), 
		"Execute all", "Execute all SQL queries", "", 
		gtk_image_new_from_stock(GTK_STOCK_EXECUTE, GTK_ICON_SIZE_LARGE_TOOLBAR),
		G_CALLBACK (btnexecallsql_clicked), pExecWnd);

	btnDuplicateSql = gtk_toolbar_append_item (GTK_TOOLBAR(pExecWnd->tlbMultiSql), 
		"Duplicate", "Duplicate SQL Windows", "", 
		gtk_image_new_from_stock(GTK_STOCK_COPY, GTK_ICON_SIZE_LARGE_TOOLBAR),
		G_CALLBACK (btnduplicatesql_clicked), pExecWnd);

	btnMultiSql = gtk_toolbar_append_item (GTK_TOOLBAR(pExecWnd->tlbMultiSql), 
		"Single", "Toggle Multiple/Single SQL query", "", 
		gtk_image_new_from_stock(GTK_STOCK_CONVERT, GTK_ICON_SIZE_LARGE_TOOLBAR),
		G_CALLBACK (btnmultisql_clicked), pExecWnd);

	btnClose = gtk_toolbar_append_item (GTK_TOOLBAR(pExecWnd->tlbMultiSql), 
		"Close", "Close window", "", 
		gtk_image_new_from_stock(GTK_STOCK_CLOSE, GTK_ICON_SIZE_LARGE_TOOLBAR),
		G_CALLBACK (btnclose_clicked), pExecWnd);

/* SQL Queries Part - Begin */

  vpanedQueries = gtk_vpaned_new ();
  gtk_widget_show (vpanedQueries);
  gtk_box_pack_start (GTK_BOX (vbox2), vpanedQueries, TRUE, TRUE, 0);
  gtk_paned_set_position (GTK_PANED (vpanedQueries), 100);

  pExecWnd->vbxSQLQueries = gtk_vbox_new (FALSE, 0);
  /*gtk_widget_show (pExecWnd->vbxSQLQueries);*/ /* Hide this part of the window */
  /*gtk_box_pack_start (GTK_BOX (vpanedQueries), pExecWnd->vbxSQLQueries, FALSE, FALSE, 0);*/
  gtk_paned_pack1 (GTK_PANED (vpanedQueries), pExecWnd->vbxSQLQueries, TRUE, TRUE);

  scrlwndSQLQueries = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (scrlwndSQLQueries);
  gtk_box_pack_start (GTK_BOX (pExecWnd->vbxSQLQueries), scrlwndSQLQueries, TRUE, TRUE, 0);

  pExecWnd->lstSQLQueries = gtk_tree_view_new ();
  gtk_widget_show (pExecWnd->lstSQLQueries);
  gtk_container_add (GTK_CONTAINER (scrlwndSQLQueries), pExecWnd->lstSQLQueries);
	select = gtk_tree_view_get_selection (GTK_TREE_VIEW (pExecWnd->lstSQLQueries));
	gtk_tree_selection_set_mode (select, GTK_SELECTION_SINGLE);
	g_signal_connect (G_OBJECT (select), "changed", G_CALLBACK (lstsqlqueries_selected), (gpointer)pExecWnd);
	
	hbox1 = gtk_hbox_new (FALSE, 2);
  gtk_widget_show (hbox1);
  gtk_box_pack_start (GTK_BOX (pExecWnd->vbxSQLQueries), hbox1, FALSE, TRUE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (hbox1), 2);

  btnQueryOpen = createIconButton("gtk-open", "Open");
  gtk_widget_show (btnQueryOpen);
  gtk_box_pack_start (GTK_BOX (hbox1), btnQueryOpen, TRUE, TRUE, 0);
	g_signal_connect (G_OBJECT (btnQueryOpen), "clicked", G_CALLBACK (btnqueryopen_clicked), (gpointer)pExecWnd);

  btnQuerySave = createIconButton("gtk-save", "Save");
  gtk_widget_show (btnQuerySave);
  gtk_box_pack_start (GTK_BOX (hbox1), btnQuerySave, TRUE, TRUE, 0);
	g_signal_connect (G_OBJECT (btnQuerySave), "clicked", G_CALLBACK (btnquerysave_clicked), (gpointer)pExecWnd);

	btnQueryNew = createIconButton("gtk-go-up", "Up");
	gtk_widget_show (btnQueryNew);
	gtk_box_pack_start (GTK_BOX (hbox1), btnQueryNew, TRUE, TRUE, 0);
	g_signal_connect (G_OBJECT (btnQueryNew), "clicked", G_CALLBACK (btnqueryup_clicked), (gpointer)pExecWnd);

	btnQueryNew = createIconButton("gtk-go-down", "Down");
	gtk_widget_show (btnQueryNew);
	gtk_box_pack_start (GTK_BOX (hbox1), btnQueryNew, TRUE, TRUE, 0);
	g_signal_connect (G_OBJECT (btnQueryNew), "clicked", G_CALLBACK (btnquerydown_clicked), (gpointer)pExecWnd);

	btnQueryNew = createIconButton("gtk-new", "New");
	gtk_widget_show (btnQueryNew);
	gtk_box_pack_start (GTK_BOX (hbox1), btnQueryNew, TRUE, TRUE, 0);
	g_signal_connect (G_OBJECT (btnQueryNew), "clicked", G_CALLBACK (btnquerynew_clicked), (gpointer)pExecWnd);

  btnQueryAdd = createIconButton ("gtk-add", "Add");
  gtk_widget_show (btnQueryAdd);
  gtk_box_pack_start (GTK_BOX (hbox1), btnQueryAdd, TRUE, TRUE, 0);
	g_signal_connect (G_OBJECT (btnQueryAdd), "clicked", G_CALLBACK (btnqueryadd_clicked), (gpointer)pExecWnd);

  btnQueryEdit = createIconButton ("gtk-apply", "Modify");
  gtk_widget_show (btnQueryEdit);
  gtk_box_pack_start (GTK_BOX (hbox1), btnQueryEdit, TRUE, TRUE, 0);
	g_signal_connect (G_OBJECT (btnQueryEdit), "clicked", G_CALLBACK (btnqueryedit_clicked), (gpointer)pExecWnd);

  btnQueryDel = createIconButton ("gtk-delete", "Delete");
  gtk_widget_show (btnQueryDel);
  gtk_box_pack_start (GTK_BOX (hbox1), btnQueryDel, TRUE, TRUE, 0);
	g_signal_connect (G_OBJECT (btnQueryDel), "clicked", G_CALLBACK (btnquerydel_clicked), (gpointer)pExecWnd);

/* SQL Queries Part - End */

  vpanedSQL = gtk_vpaned_new ();
  gtk_widget_show (vpanedSQL);
  /*gtk_box_pack_start (GTK_BOX (vbox2), vpanedSQL, TRUE, TRUE, 0);*/
  gtk_paned_pack2 (GTK_PANED (vpanedQueries), vpanedSQL, TRUE, TRUE);
  gtk_paned_set_position (GTK_PANED (vpanedSQL), 100);

  scrlwndSQLRequest = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (scrlwndSQLRequest);
  gtk_paned_pack1 (GTK_PANED (vpanedSQL), scrlwndSQLRequest, FALSE, TRUE);

  pExecWnd->txtSQLRequest = gtk_text_view_new ();
  gtk_widget_show (pExecWnd->txtSQLRequest);
  gtk_container_add (GTK_CONTAINER (scrlwndSQLRequest), pExecWnd->txtSQLRequest);
  g_signal_connect (G_OBJECT (pExecWnd->txtSQLRequest), "key-release-event", G_CALLBACK (colorizeSqlText), pExecWnd);
  
  init_sqlTags(gtk_text_view_get_buffer(GTK_TEXT_VIEW(pExecWnd->txtSQLRequest)));
  
  pExecWnd->sclSQLResult = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (pExecWnd->sclSQLResult);
  gtk_paned_pack2 (GTK_PANED (vpanedSQL), pExecWnd->sclSQLResult, TRUE, TRUE);

  pExecWnd->lstSQLResult = gtk_tree_view_new ();
  gtk_widget_show (pExecWnd->lstSQLResult);
  gtk_container_add (GTK_CONTAINER (pExecWnd->sclSQLResult), pExecWnd->lstSQLResult);

	select = gtk_tree_view_get_selection (GTK_TREE_VIEW (pExecWnd->lstSQLResult));
	gtk_tree_selection_set_mode (select, GTK_SELECTION_SINGLE);
	g_signal_connect (G_OBJECT (select), "changed", G_CALLBACK (resultRow_selected), (gpointer)pExecWnd);


  statusbarSQL = gtk_statusbar_new ();
  gtk_widget_show (statusbarSQL);
  gtk_box_pack_start (GTK_BOX (vbox2), statusbarSQL, FALSE, FALSE, 0);
	
	if (display) {
		gtk_widget_show (pExecWnd->wndMain);
		NbrWnd ++;
		if (multiple) {
			btnmultisql_clicked (pExecWnd->wndMain, (gpointer *)pExecWnd);
			btnqueryopen_clicked (pExecWnd->wndMain, (gpointer *)pExecWnd);
		}
	}		

  return pExecWnd;
}
