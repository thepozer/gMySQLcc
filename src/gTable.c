 
#include "gTable.h"

extern int NbrWnd;

char * arTableType[] = {"MYISAM", "MRG_MYISAM", "MERGE", "InnoDB", "ISAM", "HEAP", "BDB", (char *)NULL};
int szarTableType = 7;

void initDataTable (p_tableWnd pTblWnd);

void fillFieldList (p_tableWnd pTblWnd);
void fillIndexList (p_tableWnd pTblWnd);
void fillMiscInfos (p_tableWnd pTblWnd);

void displayFieldList (p_tableWnd pTblWnd);
void displayIndexList (p_tableWnd pTblWnd);


void initDataTable (p_tableWnd pTblWnd) {
	p_fieldTypeCapability * ptr_arFields;
	char ** ptr_arItems;
  GList * items;

	fillFieldList (pTblWnd); /* Fill field list */
	fillIndexList (pTblWnd); /* Fill index list */
	fillMiscInfos (pTblWnd); /* Fill misc infos */

	items = NULL;
	ptr_arFields = arFieldTypes;
	while (*ptr_arFields != (p_fieldTypeCapability) NULL) {
		items = g_list_append (items, (*ptr_arFields)->name);
		ptr_arFields ++;
	}
	gtk_combo_set_popdown_strings (GTK_COMBO (pTblWnd->cmbFieldType), items);
	g_list_free(items);

	items = NULL;
	ptr_arItems = arTableType;
	while (*ptr_arItems != (char *) NULL) {
		items = g_list_append (items, (*ptr_arItems));
		ptr_arItems ++;
	}
	gtk_combo_set_popdown_strings (GTK_COMBO (pTblWnd->cmbTableFormat), items);
	g_list_free(items);
	
	gtk_entry_set_text(GTK_ENTRY(pTblWnd->txtTableName), pTblWnd->tableName->str);
	
	displayFieldList (pTblWnd); /* Display field list */
	displayIndexList (pTblWnd); /* Display index list */
	
}

void fillFieldList (p_tableWnd pTblWnd) {
	GArray * arRow;
	GString * query, * type;
	char * tmp, *tmp2, *tmp3;
	int row, idx, lenType, lenSize;
	mysql_query_infos * p_mqi;
	p_fieldInfo p_fi;
	char ** ptr_arItems, ** arParams;
	
	
	pTblWnd->p_lstField = (GList *)NULL;
	
	query = g_string_new("");
	g_string_printf(query, "SHOW FIELDS FROM `%s`", pTblWnd->tableName->str);
	p_mqi = db_query_all(pTblWnd->p_mci, query->str);
	if (p_mqi != (mysql_query_infos *)NULL && p_mqi->errCode <= 0) { 
		/* Query Ok -> Display databases */
		for (row = 0; row < p_mqi->selectResult->len; row++) {
			p_fi = createFieldInfo(FI_ORIGIN);

			if (p_fi != (p_fieldInfo)NULL) {
				
				arRow = g_array_index(p_mqi->selectResult, GArray *, row);
				
				p_fi->name = g_string_new(g_array_index(arRow, gchar *, 0));
				
				tmp = g_array_index(arRow, gchar *, 1);
				tmp2 = (char *)strchr(tmp, '(');
				if (tmp2 != (char *)NULL) {
					tmp3 = (char *)strchr(tmp, ')');
					lenType = (int)(tmp2 - (long)tmp);
					lenSize = (int)(tmp3 - (tmp2 + 1));
					type = g_string_new_len(tmp, lenType);
					p_fi->set = g_string_new_len((const char *)(tmp2 + 1), lenSize);
					if (strncmp(tmp, "enum", lenType) != 0 && strncmp(tmp, "set", lenType) != 0) {
						p_fi->length = (int)strtoul(p_fi->set->str, (char *)NULL, 10);
					}
					tmp3++;
					ptr_arItems = arParams = g_strsplit(tmp3, " ", 0);
					while (*ptr_arItems != (char *) NULL) {
						if (g_ascii_strcasecmp(*ptr_arItems, "unsigned") == 0) {
							p_fi->unsign = TRUE;
						} else if (g_ascii_strcasecmp(*ptr_arItems, "zerofill") == 0) {
							p_fi->zerofill = TRUE;
						} else if (g_ascii_strcasecmp(*ptr_arItems, "binary") == 0) {
							p_fi->binary = TRUE;
						}
						ptr_arItems ++;
					}
					g_strfreev(arParams);
				} else {
					type = g_string_new(tmp);
					p_fi->def = g_string_new("");
					p_fi->set = g_string_new("");
				}
				
				p_fi->type_idx = -1;
				for (idx = 0; idx < szarFieldTypes; idx++) {
					if (strcasecmp(type->str, arFieldTypes[idx]->name) == 0) {
							p_fi->type_idx = idx;
						break;
					}
				}
				
				tmp = g_array_index(arRow, gchar *, 2);
				p_fi->nullOk = (g_ascii_strcasecmp(tmp, "yes") == 0);
				
				p_fi->def = g_string_new(g_array_index(arRow, gchar *, 4));
				
				tmp = g_array_index(arRow, gchar *, 5);
				p_fi->autoIncr = (g_ascii_strcasecmp(tmp, "auto_increment") == 0);
				
				pTblWnd->p_lstField = g_list_append (pTblWnd->p_lstField, p_fi);
			}
		}
		db_free_query(p_mqi);
		g_string_free(query, TRUE);
	}
}

void fillIndexList (p_tableWnd pTblWnd) {
	GArray * arRow;
	GString * query, * idxName, * subpart;
	int row, value;
	mysql_query_infos * p_mqi;
	p_indexFieldInfo p_ifi;
	p_indexInfo p_ii;
	
	pTblWnd->p_lstIndex = (GList *)NULL;
	
	query = g_string_new("");
	g_string_printf(query, "SHOW INDEX FROM `%s`", pTblWnd->tableName->str);
	p_mqi = db_query_all(pTblWnd->p_mci, query->str);
	if (p_mqi != (mysql_query_infos *)NULL && p_mqi->errCode <= 0) { 
		/* Query Ok -> Display databases */
		p_ii = createIndexInfo(STTS_ORIGIN);
		for (row = 0; row < p_mqi->selectResult->len; row++) {
			arRow = g_array_index(p_mqi->selectResult, GArray *, row);
			
			idxName = g_string_new(g_array_index(arRow, gchar *, 2));
			if (p_ii->name == (GString *)NULL) {
				p_ii->name = idxName;
			} else if (!g_string_equal(p_ii->name, idxName)) {
				pTblWnd->p_lstIndex = g_list_append (pTblWnd->p_lstIndex, p_ii);
				p_ii = createIndexInfo(STTS_ORIGIN);
				p_ii->name = idxName;
			} else {
				g_string_free(idxName, TRUE);
			}
			p_ii->primary = (g_ascii_strcasecmp(p_ii->name->str, "PRIMARY") == 0);
			value = (int)strtoul(g_array_index(arRow, gchar *, 1));
			p_ii->unique = (value == 1);

			p_ifi = createIndexFieldInfo(STTS_ORIGIN);
			
			p_ifi->name = g_string_new(g_array_index(arRow, gchar *, 4));
			p_ifi->field_ref = getFieldByName(pTblWnd->p_lstField, p_ifi->name);

			subpart = g_string_new(g_array_index(arRow, gchar *, 7));
			if (g_ascii_strcasecmp(subpart->str, "NULL") != 0) {
				p_ifi->subpart = (int)strtoul(subpart->str, (char *)NULL, 10);
			}
			g_string_free(subpart, TRUE);
			
			p_ii->fieldsIdx = g_list_append (p_ii->fieldsIdx, p_ifi);
		}
		if (p_ii->name != (GString *)NULL) {
			pTblWnd->p_lstIndex = g_list_append (pTblWnd->p_lstIndex, p_ii);
		}
		db_free_query(p_mqi);
		g_string_free(query, TRUE);
	}
}

void fillMiscInfos (p_tableWnd pTblWnd) {
	GArray * arRow;
	GString * query, * idxName, * subpart;
	int row, value;
	mysql_query_infos * p_mqi;
	p_indexFieldInfo p_ifi;
	p_indexInfo p_ii;
	
	pTblWnd->p_dctStatus = (GData *)NULL;
	
	query = g_string_new("");
	g_string_printf(query, "SHOW TABLE STATUS LIKE '%s'", pTblWnd->tableName->str);
	p_mqi = db_query_all(pTblWnd->p_mci, query->str);
	if (p_mqi != (mysql_query_infos *)NULL && p_mqi->errCode <= 0) { 
		arRow = g_array_index(p_mqi->selectResult, GArray *, 0);
		
	}
}

void displayFieldList(p_tableWnd pTblWnd) {
	mysql_connect_infos * p_mci;
	mysql_query_infos * p_mqi;
	p_fieldInfo p_fi;

  GList * items, * currField;
	GtkListStore * lstStrBase;
	GtkTreeIter iter;
	GtkTreeViewColumn * currCol;
	GtkCellRenderer * renderer;

	lstStrBase = gtk_list_store_new(3, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_POINTER);
	currField = NULL;
	items = g_list_first(pTblWnd->p_lstField);
	while (items != (GList *)NULL) {
		/* Check if field can be displayed */
		p_fi = (p_fieldInfo)items->data;
		if (p_fi->status == FI_MODIFY && p_fi->update != (p_fieldInfo)NULL) {
			p_fi = p_fi->update;
		} else if (p_fi->status == FI_DEL) {
			p_fi = (p_fieldInfo)NULL;
		}
		
		/* Display if needed the field */
		if (p_fi != (p_fieldInfo)NULL) {
			gtk_list_store_append (lstStrBase, &iter);
			gtk_list_store_set (lstStrBase, &iter, 0, p_fi->name->str, -1);
			gtk_list_store_set (lstStrBase, &iter, 1, arFieldTypes[p_fi->type_idx]->name, -1);
			gtk_list_store_set (lstStrBase, &iter, 2, p_fi, -1);

			currField = g_list_append (currField, p_fi->name->str);
		}
		
		/* Go to next item */
		items = g_list_next(items);
	}
	
	gtk_tree_view_set_model(GTK_TREE_VIEW(pTblWnd->LstField), GTK_TREE_MODEL(lstStrBase));
	g_object_unref (G_OBJECT (lstStrBase));
		
	currCol = gtk_tree_view_get_column(GTK_TREE_VIEW(pTblWnd->LstField), 0);
	while (currCol != (GtkTreeViewColumn *)NULL) {
		gtk_tree_view_remove_column(GTK_TREE_VIEW(pTblWnd->LstField), currCol);
		currCol = gtk_tree_view_get_column(GTK_TREE_VIEW(pTblWnd->LstField), 0);
	}

	renderer = gtk_cell_renderer_text_new ();
	currCol = gtk_tree_view_column_new_with_attributes ("Field", renderer, "text", 0, NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW (pTblWnd->LstField), currCol);
	currCol = gtk_tree_view_column_new_with_attributes ("Type", renderer, "text", 1, NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW (pTblWnd->LstField), currCol);

	/* Update list of fields for indexes */
	gtk_list_clear_items(GTK_LIST(GTK_COMBO (pTblWnd->cmbIndexLstFields)->list), 0, -1);
	gtk_combo_set_popdown_strings (GTK_COMBO (pTblWnd->cmbIndexLstFields), currField);
	g_list_free(currField);
}

void displayIndexList(p_tableWnd pTblWnd) {
	mysql_connect_infos * p_mci;
	mysql_query_infos * p_mqi;
	p_indexInfo p_ii;

  GList * items = NULL;
	GtkListStore * lstStrBase;
	GtkTreeIter iter;
	GtkTreeViewColumn * currCol;
	GtkCellRenderer * renderer;

	lstStrBase = gtk_list_store_new(2, G_TYPE_STRING, G_TYPE_POINTER);
	items = g_list_first(pTblWnd->p_lstIndex);
	while (items != (GList *)NULL) {
		/* Check if field can be displayed */
		p_ii = (p_indexInfo)items->data;
		/*if (p_ii->status == FI_MODIFY && p_ii->update != (p_indexInfo)NULL) {
			p_ii = p_ii->update;
		} else*/ if (p_ii->status == FI_DEL) {
			p_ii = (p_indexInfo)NULL;
		}
		
		/* Display if needed the field */
		if (p_ii != (p_indexInfo)NULL) {
			gtk_list_store_append (lstStrBase, &iter);
			gtk_list_store_set (lstStrBase, &iter, 0, p_ii->name->str, -1);
			gtk_list_store_set (lstStrBase, &iter, 1, p_ii, -1);
		}
		
		/* Go to next item */
		items = g_list_next(items);
	}
	
	gtk_tree_view_set_model(GTK_TREE_VIEW(pTblWnd->LstIndex), GTK_TREE_MODEL(lstStrBase));
	g_object_unref (G_OBJECT (lstStrBase));
		
	currCol = gtk_tree_view_get_column(GTK_TREE_VIEW(pTblWnd->LstIndex), 0);
	if (currCol != (GtkTreeViewColumn *)NULL) {
		gtk_tree_view_remove_column(GTK_TREE_VIEW(pTblWnd->LstIndex), currCol);
	}

	renderer = gtk_cell_renderer_text_new ();
	currCol = gtk_tree_view_column_new_with_attributes ("Index", renderer, "text", 0, NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW (pTblWnd->LstIndex), currCol);

}

p_tableWnd create_wndTable (gboolean display, mysql_connect_infos * p_mci, const char * table) {
  GtkWidget *vbox1;
  GtkWidget *toolbar1;
  GtkWidget *notebook1;
	GtkWidget *statusbar1;
	
  GtkWidget *hpaned1, *hpaned2;
  GtkWidget *frame1, *frame2, *frame3, *frame4, *frame5;
  GtkWidget *hbox1, *hbox2, *hbox3, *hbox4, *hbox5, *hbox6, *hbox7, *hbox8, *hbox9;
  GtkWidget *hbox10, *hbox11, *hbox12, *hbox13, *hbox14, *hbox15, *hbox16;
  GtkWidget *hbox24, *hbox25, *hbox26, *hbox27, *hbox28, *hbox29;
  GtkWidget *vbox2, *vbox3, *vbox4, *vbox5, *vbox6, *vbox7, *vbox8, *vbox9, *vbox10, *vbox11;
  GtkWidget *scrolledwindow1, *scrolledwindow2, *scrolledwindow3, *scrolledwindow4, *scrolledwindow5;
  GtkWidget *label1, *label2, *label3, *label4, *label5, *label8, *label9;
  GtkWidget *label10, *label11, *label12, *label13, *label14, *label15, *label16, *label17, *label18, *label19;
  GtkWidget *label20, *label21, *label22, *label24, *label25, *label26, *label27, *label28, *label29;
  GtkWidget *label30;
  GtkWidget *combo_entry1, *combo_entry2, *combo_entry3, *combo_entry4, *combo_entry5;
  GtkWidget *alignment1, *alignment2, *alignment3, *alignment4;
  GtkWidget *image1, *image2, *image3, *image4;

  GtkWidget *treeview2;
	GtkWidget *txtField;
	
  GtkWidget *btnTlbrApply, *btnTlbrClose;
  GtkWidget *btnFieldUp, *btnFieldDown, *btnFieldNew, *btnFieldAdd, *btnFieldEdit, *btnFieldDel;
  GtkWidget *btnIndexSetField, *btnIndexUnsetField, *btnIndexNew, *btnIndexAdd, *btnIndexEdit, *btnIndexDel;
	
	GtkTreeSelection *select;
	p_tableWnd pTblWnd = (p_tableWnd)NULL;
	GString * sTitle;

	/* Init structure */
	pTblWnd = (p_tableWnd) g_try_malloc(sizeof(tableWnd));
	
	if (pTblWnd == (p_tableWnd)NULL) {
		return (p_tableWnd)NULL; /* return NULL pointer */
	}
	
	pTblWnd->p_mci = db_duplicate(p_mci, (char *)NULL);
	pTblWnd->tableName = g_string_new(table);
	pTblWnd->comments = g_string_new(table);
	pTblWnd->tableFormat = g_string_new(table);
	pTblWnd->currField = (p_fieldInfo)NULL;
	pTblWnd->p_lstField = (GList *)NULL;
	pTblWnd->p_lstIndex = (GList *)NULL;
	pTblWnd->tableModified = FALSE;
	
	/* Init GUI */
  pTblWnd->wndTable = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	sTitle = g_string_new("");
	g_string_printf(sTitle, "[%s] - [%s] - Table : %s", p_mci->name, p_mci->db, table);
  gtk_window_set_title (GTK_WINDOW (pTblWnd->wndTable), sTitle->str);
	g_string_free(sTitle, TRUE);
	gtk_window_set_default_size (GTK_WINDOW (pTblWnd->wndTable), 500, 350);
	g_signal_connect (G_OBJECT (pTblWnd->wndTable), "destroy", G_CALLBACK (destroy), pTblWnd);

  vbox1 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox1);
  gtk_container_add (GTK_CONTAINER (pTblWnd->wndTable), vbox1);

  toolbar1 = gtk_toolbar_new ();
  gtk_widget_show (toolbar1);
  gtk_box_pack_start (GTK_BOX (vbox1), toolbar1, FALSE, FALSE, 0);
  gtk_toolbar_set_style (GTK_TOOLBAR (toolbar1), GTK_TOOLBAR_BOTH_HORIZ);

	btnTlbrApply = gtk_toolbar_append_item (GTK_TOOLBAR(toolbar1), 
		"Validate", "Validate modification", "", 
		gtk_image_new_from_stock(GTK_STOCK_OK, GTK_ICON_SIZE_LARGE_TOOLBAR),
		G_CALLBACK (btntlbrvalidate_clicked), (gpointer)pTblWnd);

	btnTlbrClose = gtk_toolbar_append_item (GTK_TOOLBAR(toolbar1), 
		"Close", "Close window", "", 
		gtk_image_new_from_stock(GTK_STOCK_CLOSE, GTK_ICON_SIZE_LARGE_TOOLBAR),
		G_CALLBACK (btntlbrclose_clicked), (gpointer)pTblWnd);

  notebook1 = gtk_notebook_new ();
  gtk_widget_show (notebook1);
  gtk_box_pack_start (GTK_BOX (vbox1), notebook1, TRUE, TRUE, 0);


/* Fields part */

  hpaned1 = gtk_hpaned_new ();
  gtk_widget_show (hpaned1);
  gtk_container_add (GTK_CONTAINER (notebook1), hpaned1);
  gtk_paned_set_position (GTK_PANED (hpaned1), 200);

  vbox9 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox9);
  gtk_paned_pack1 (GTK_PANED (hpaned1), vbox9, FALSE, TRUE);

  scrolledwindow1 = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (scrolledwindow1);
  gtk_box_pack_start (GTK_BOX (vbox9), scrolledwindow1, TRUE, TRUE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (scrolledwindow1), 3);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow1), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

  pTblWnd->LstField = gtk_tree_view_new ();
  gtk_widget_show (pTblWnd->LstField);
  gtk_container_add (GTK_CONTAINER (scrolledwindow1), pTblWnd->LstField);
  gtk_tree_view_set_reorderable (GTK_TREE_VIEW (pTblWnd->LstField), TRUE);
	select = gtk_tree_view_get_selection (GTK_TREE_VIEW (pTblWnd->LstField));
	gtk_tree_selection_set_mode (select, GTK_SELECTION_SINGLE);
	g_signal_connect (G_OBJECT (select), "changed", G_CALLBACK (lstfield_selected), (gpointer)pTblWnd);

  hbox16 = gtk_hbox_new (TRUE, 0);
  gtk_widget_show (hbox16);
  gtk_box_pack_start (GTK_BOX (vbox9), hbox16, FALSE, TRUE, 0);

	btnFieldUp = createIconButton("gtk-go-up", "_Monter");
  gtk_box_pack_start (GTK_BOX (hbox16), btnFieldUp, FALSE, FALSE, 0);

	btnFieldDown = createIconButton("gtk-go-down", "_Descendre");
  gtk_box_pack_start (GTK_BOX (hbox16), btnFieldDown, FALSE, FALSE, 0);

  vbox2 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox2);
  gtk_paned_pack2 (GTK_PANED (hpaned1), vbox2, TRUE, TRUE);
  gtk_container_set_border_width (GTK_CONTAINER (vbox2), 3);

  frame1 = gtk_frame_new (NULL);
  gtk_widget_show (frame1);
  gtk_box_pack_start (GTK_BOX (vbox2), frame1, FALSE, TRUE, 0);

  vbox3 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox3);
  gtk_container_add (GTK_CONTAINER (frame1), vbox3);

  hbox2 = gtk_hbox_new (FALSE, 2);
  gtk_widget_show (hbox2);
  gtk_box_pack_start (GTK_BOX (vbox3), hbox2, FALSE, TRUE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (hbox2), 2);

  label5 = gtk_label_new ("Nom du champ :");
  gtk_widget_show (label5);
  gtk_box_pack_start (GTK_BOX (hbox2), label5, FALSE, FALSE, 0);
  gtk_label_set_justify (GTK_LABEL (label5), GTK_JUSTIFY_LEFT);

  pTblWnd->txtFieldName = gtk_entry_new ();
  gtk_widget_show (pTblWnd->txtFieldName);
  gtk_box_pack_start (GTK_BOX (hbox2), pTblWnd->txtFieldName, TRUE, TRUE, 0);

  hbox3 = gtk_hbox_new (FALSE, 2);
  gtk_widget_show (hbox3);
  gtk_box_pack_start (GTK_BOX (vbox3), hbox3, TRUE, TRUE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (hbox3), 2);

  label8 = gtk_label_new ("Type de champ :");
  gtk_widget_show (label8);
  gtk_box_pack_start (GTK_BOX (hbox3), label8, FALSE, FALSE, 0);
  gtk_label_set_justify (GTK_LABEL (label8), GTK_JUSTIFY_LEFT);

  pTblWnd->cmbFieldType = gtk_combo_new ();
  gtk_widget_show (pTblWnd->cmbFieldType);
  gtk_box_pack_start (GTK_BOX (hbox3), pTblWnd->cmbFieldType, TRUE, TRUE, 0);
	g_signal_connect (G_OBJECT(GTK_COMBO(pTblWnd->cmbFieldType)->list), "select-child", G_CALLBACK (cmbfieldtype_selected), (gpointer)pTblWnd);

  combo_entry3 = GTK_COMBO (pTblWnd->cmbFieldType)->entry;
	gtk_editable_set_editable (GTK_EDITABLE(combo_entry3), FALSE);
  gtk_widget_show (combo_entry3);
	
  hbox4 = gtk_hbox_new (FALSE, 2);
  gtk_widget_show (hbox4);
  gtk_box_pack_start (GTK_BOX (vbox3), hbox4, TRUE, TRUE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (hbox4), 2);

  label9 = gtk_label_new ("Longueur du champ :");
  gtk_widget_show (label9);
  gtk_box_pack_start (GTK_BOX (hbox4), label9, FALSE, FALSE, 0);
  gtk_label_set_justify (GTK_LABEL (label9), GTK_JUSTIFY_LEFT);

  pTblWnd->txtFieldLen = gtk_entry_new ();
  gtk_widget_show (pTblWnd->txtFieldLen);
  gtk_box_pack_start (GTK_BOX (hbox4), pTblWnd->txtFieldLen, TRUE, TRUE, 0);

  hbox25 = gtk_hbox_new (FALSE, 2);
  gtk_widget_show (hbox25);
  gtk_box_pack_start (GTK_BOX (vbox3), hbox25, TRUE, TRUE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (hbox25), 2);

  label25 = gtk_label_new ("Nombre de decimale :");
  gtk_widget_show (label25);
  gtk_box_pack_start (GTK_BOX (hbox25), label25, FALSE, FALSE, 0);
  gtk_label_set_justify (GTK_LABEL (label25), GTK_JUSTIFY_LEFT);

  pTblWnd->txtFieldDecimal = gtk_entry_new ();
  gtk_widget_show (pTblWnd->txtFieldDecimal);
  gtk_box_pack_start (GTK_BOX (hbox25), pTblWnd->txtFieldDecimal, TRUE, TRUE, 0);

  hbox24 = gtk_hbox_new (FALSE, 2);
  gtk_widget_show (hbox24);
  gtk_box_pack_start (GTK_BOX (vbox3), hbox24, TRUE, TRUE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (hbox24), 2);

  label24 = gtk_label_new ("Liste des valeurs :");
  gtk_widget_show (label24);
  gtk_box_pack_start (GTK_BOX (hbox24), label24, FALSE, FALSE, 0);
  gtk_label_set_justify (GTK_LABEL (label24), GTK_JUSTIFY_LEFT);

  pTblWnd->txtFieldValues = gtk_entry_new ();
  gtk_widget_show (pTblWnd->txtFieldValues);
  gtk_box_pack_start (GTK_BOX (hbox24), pTblWnd->txtFieldValues, TRUE, TRUE, 0);

  hbox15 = gtk_hbox_new (FALSE, 2);
  gtk_widget_show (hbox15);
  gtk_box_pack_start (GTK_BOX (vbox3), hbox15, TRUE, TRUE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (hbox15), 2);

  label22 = gtk_label_new ("Valeur par defaut :");
  gtk_widget_show (label22);
  gtk_box_pack_start (GTK_BOX (hbox15), label22, FALSE, FALSE, 0);
  gtk_label_set_justify (GTK_LABEL (label22), GTK_JUSTIFY_LEFT);

  pTblWnd->txtFieldDefault = gtk_entry_new ();
  gtk_widget_show (pTblWnd->txtFieldDefault);
  gtk_box_pack_start (GTK_BOX (hbox15), pTblWnd->txtFieldDefault, TRUE, TRUE, 0);

  hbox5 = gtk_hbox_new (FALSE, 2);
  gtk_widget_show (hbox5);
  gtk_box_pack_start (GTK_BOX (vbox3), hbox5, TRUE, TRUE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (hbox5), 2);

  pTblWnd->chkFieldUnsign = gtk_check_button_new_with_mnemonic ("Non signe");
  gtk_widget_show (pTblWnd->chkFieldUnsign);
  gtk_box_pack_start (GTK_BOX (vbox3), pTblWnd->chkFieldUnsign, FALSE, FALSE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (pTblWnd->chkFieldUnsign), 2);

  pTblWnd->chkFieldZeroFill = gtk_check_button_new_with_mnemonic ("Remplissage de Zero (ZeroFill)");
  gtk_widget_show (pTblWnd->chkFieldZeroFill);
  gtk_box_pack_start (GTK_BOX (vbox3), pTblWnd->chkFieldZeroFill, FALSE, FALSE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (pTblWnd->chkFieldZeroFill), 2);

  pTblWnd->chkFieldBinary = gtk_check_button_new_with_mnemonic ("Binaire");
  gtk_widget_show (pTblWnd->chkFieldBinary);
  gtk_box_pack_start (GTK_BOX (vbox3), pTblWnd->chkFieldBinary, FALSE, FALSE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (pTblWnd->chkFieldBinary), 2);

  pTblWnd->chkFieldAutoIncr = gtk_check_button_new_with_mnemonic ("Compteur automatique (AutoIncrement)");
  gtk_widget_show (pTblWnd->chkFieldAutoIncr);
  gtk_box_pack_start (GTK_BOX (vbox3), pTblWnd->chkFieldAutoIncr, FALSE, FALSE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (pTblWnd->chkFieldAutoIncr), 2);

	pTblWnd->chkFieldNull = gtk_check_button_new_with_mnemonic ("Accepte la valeur nulle");
  gtk_widget_show (pTblWnd->chkFieldNull);
  gtk_box_pack_start (GTK_BOX (vbox3), pTblWnd->chkFieldNull, FALSE, FALSE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (pTblWnd->chkFieldNull), 2);

  label4 = gtk_label_new ("Champ :");
  gtk_widget_show (label4);
  gtk_frame_set_label_widget (GTK_FRAME (frame1), label4);
  gtk_label_set_justify (GTK_LABEL (label4), GTK_JUSTIFY_LEFT);

  hbox1 = gtk_hbox_new (FALSE, 2);
  gtk_widget_show (hbox1);
  gtk_box_pack_start (GTK_BOX (vbox2), hbox1, FALSE, TRUE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (hbox1), 2);

	btnFieldNew = createIconButton("gtk-new", "_Nouveau");
  gtk_box_pack_start (GTK_BOX (hbox1), btnFieldNew, TRUE, TRUE, 0);
	g_signal_connect (G_OBJECT (btnFieldNew), "clicked", G_CALLBACK (btnfieldnew_clicked), (gpointer)pTblWnd);

	btnFieldAdd = createIconButton("gtk-add", "_Ajouter");
  gtk_box_pack_start (GTK_BOX (hbox1), btnFieldAdd, TRUE, TRUE, 0);
	g_signal_connect (G_OBJECT (btnFieldAdd), "clicked", G_CALLBACK (btnfieldadd_clicked), (gpointer)pTblWnd);

	btnFieldEdit = createIconButton("gtk-apply", "_Modifier");
  gtk_box_pack_start (GTK_BOX (hbox1), btnFieldEdit, TRUE, TRUE, 0);
	g_signal_connect (G_OBJECT (btnFieldEdit), "clicked", G_CALLBACK (btnfieldedit_clicked), (gpointer)pTblWnd);

	btnFieldDel = createIconButton("gtk-delete", "_Supprimer");
  gtk_box_pack_start (GTK_BOX (hbox1), btnFieldDel, TRUE, TRUE, 0);
	g_signal_connect (G_OBJECT (btnFieldDel), "clicked", G_CALLBACK (btnfielddel_clicked), (gpointer)pTblWnd);

  label1 = gtk_label_new ("Champs");
  gtk_widget_show (label1);
  gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook1), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook1), 0), label1);
  gtk_label_set_justify (GTK_LABEL (label1), GTK_JUSTIFY_LEFT);


/* Indexes part */

  hpaned2 = gtk_hpaned_new ();
  gtk_widget_show (hpaned2);
  gtk_container_add (GTK_CONTAINER (notebook1), hpaned2);
  gtk_paned_set_position (GTK_PANED (hpaned2), 200);

  scrolledwindow3 = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (scrolledwindow3);
  gtk_paned_pack1 (GTK_PANED (hpaned2), scrolledwindow3, FALSE, TRUE);

  pTblWnd->LstIndex = gtk_tree_view_new ();
  gtk_widget_show (pTblWnd->LstIndex);
  gtk_container_add (GTK_CONTAINER (scrolledwindow3), pTblWnd->LstIndex);
	select = gtk_tree_view_get_selection (GTK_TREE_VIEW (pTblWnd->LstIndex));
	gtk_tree_selection_set_mode (select, GTK_SELECTION_SINGLE);
	g_signal_connect (G_OBJECT (select), "changed", G_CALLBACK (lstindex_selected), (gpointer)pTblWnd);

  vbox7 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox7);
  gtk_paned_pack2 (GTK_PANED (hpaned2), vbox7, TRUE, TRUE);

  frame4 = gtk_frame_new (NULL);
  gtk_widget_show (frame4);
  gtk_box_pack_start (GTK_BOX (vbox7), frame4, TRUE, TRUE, 0);

  vbox8 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox8);
  gtk_container_add (GTK_CONTAINER (frame4), vbox8);
  gtk_container_set_border_width (GTK_CONTAINER (vbox8), 3);

  hbox26 = gtk_hbox_new (FALSE, 2);
  gtk_widget_show (hbox26);
  gtk_box_pack_start (GTK_BOX (vbox8), hbox26, FALSE, FALSE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (hbox26), 2);

  label26 = gtk_label_new ("Nom de l'index :");
  gtk_widget_show (label26);
  gtk_box_pack_start (GTK_BOX (hbox26), label26, FALSE, FALSE, 0);
  gtk_label_set_justify (GTK_LABEL (label26), GTK_JUSTIFY_LEFT);

  pTblWnd->txtIndexName = gtk_entry_new ();
  gtk_widget_show (pTblWnd->txtIndexName);
  gtk_box_pack_start (GTK_BOX (hbox26), pTblWnd->txtIndexName, TRUE, TRUE, 0);

	pTblWnd->chkIndexPrimary = gtk_check_button_new_with_mnemonic ("Index primaire");
  gtk_widget_show (pTblWnd->chkIndexPrimary);
  gtk_box_pack_start (GTK_BOX (vbox8), pTblWnd->chkIndexPrimary, FALSE, FALSE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (pTblWnd->chkIndexPrimary), 2);
	
	pTblWnd->chkIndexUnique = gtk_check_button_new_with_mnemonic ("Valeurs uniques");
  gtk_widget_show (pTblWnd->chkIndexUnique);
  gtk_box_pack_start (GTK_BOX (vbox8), pTblWnd->chkIndexUnique, FALSE, FALSE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (pTblWnd->chkIndexUnique), 2);

  scrolledwindow4 = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (scrolledwindow4);
  gtk_box_pack_start (GTK_BOX (vbox8), scrolledwindow4, TRUE, TRUE, 0);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow4), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);

  pTblWnd->lstIndexUsed = gtk_tree_view_new ();
  gtk_widget_show (pTblWnd->lstIndexUsed);
  gtk_container_add (GTK_CONTAINER (scrolledwindow4), pTblWnd->lstIndexUsed);
  gtk_tree_view_set_headers_visible (GTK_TREE_VIEW (pTblWnd->lstIndexUsed), FALSE);
	select = gtk_tree_view_get_selection (GTK_TREE_VIEW (pTblWnd->lstIndexUsed));
	gtk_tree_selection_set_mode (select, GTK_SELECTION_SINGLE);
	g_signal_connect (G_OBJECT (select), "changed", G_CALLBACK (lstindexused_selected), (gpointer)pTblWnd);

  frame5 = gtk_frame_new (NULL);
  gtk_widget_show (frame5);
  gtk_box_pack_start (GTK_BOX (vbox8), frame5, FALSE, TRUE, 0);

  label30 = gtk_label_new ("Field :");
  gtk_widget_show (label30);
  gtk_frame_set_label_widget (GTK_FRAME (frame5), label30);
  gtk_label_set_justify (GTK_LABEL (label30), GTK_JUSTIFY_LEFT);

  hbox29 = gtk_hbox_new (FALSE, 2);
  gtk_widget_show (hbox29);
  gtk_container_add (GTK_CONTAINER (frame5), hbox29);
  gtk_container_set_border_width (GTK_CONTAINER (hbox29), 2);

  vbox10 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox10);
  gtk_box_pack_start (GTK_BOX (hbox29), vbox10, FALSE, FALSE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (vbox10), 3);

  label29 = gtk_label_new ("Nom du champ");
  gtk_widget_show (label29);
  gtk_box_pack_start (GTK_BOX (vbox10), label29, FALSE, FALSE, 0);
  gtk_label_set_justify (GTK_LABEL (label29), GTK_JUSTIFY_LEFT);

  pTblWnd->cmbIndexLstFields = gtk_combo_new ();
  gtk_widget_show (pTblWnd->cmbIndexLstFields);
  gtk_box_pack_start (GTK_BOX (vbox10), pTblWnd->cmbIndexLstFields, FALSE, TRUE, 0);

  combo_entry1 = GTK_COMBO (pTblWnd->cmbIndexLstFields)->entry;
  gtk_widget_show (combo_entry1);

  label27 = gtk_label_new ("Utilise xx caracteres");
  gtk_widget_show (label27);
  gtk_box_pack_start (GTK_BOX (vbox10), label27, FALSE, FALSE, 0);
  gtk_label_set_justify (GTK_LABEL (label27), GTK_JUSTIFY_LEFT);

  pTblWnd->txtIndexSubPart = gtk_entry_new ();
  gtk_widget_show (pTblWnd->txtIndexSubPart);
  gtk_box_pack_start (GTK_BOX (vbox10), pTblWnd->txtIndexSubPart, FALSE, FALSE, 0);

  vbox11 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox11);
  gtk_box_pack_start (GTK_BOX (hbox29), vbox11, FALSE, FALSE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (vbox11), 3);

	btnIndexSetField = createIconButton("gtk-add", "_Ajouter");
  gtk_box_pack_start (GTK_BOX (vbox11), btnIndexSetField, FALSE, FALSE, 0);

	btnIndexUnsetField = createIconButton("gtk-delete", "_Enlever");
  gtk_box_pack_start (GTK_BOX (vbox11), btnIndexUnsetField, FALSE, FALSE, 0);

  label16 = gtk_label_new ("Index :");
  gtk_widget_show (label16);
  gtk_frame_set_label_widget (GTK_FRAME (frame4), label16);
  gtk_label_set_justify (GTK_LABEL (label16), GTK_JUSTIFY_LEFT);

  hbox8 = gtk_hbox_new (FALSE, 2);
  gtk_widget_show (hbox8);
  gtk_box_pack_start (GTK_BOX (vbox7), hbox8, FALSE, TRUE, 2);
  gtk_container_set_border_width (GTK_CONTAINER (hbox8), 2);

	btnIndexNew = createIconButton("gtk-new", "_Nouveau");
  gtk_box_pack_start (GTK_BOX (hbox8), btnIndexNew, TRUE, TRUE, 0);

	btnIndexAdd = createIconButton("gtk-add", "_Ajouter");
  gtk_box_pack_start (GTK_BOX (hbox8), btnIndexAdd, TRUE, TRUE, 0);

	btnIndexEdit = createIconButton("gtk-apply", "_Edit");
  gtk_box_pack_start (GTK_BOX (hbox8), btnIndexEdit, TRUE, TRUE, 0);

	btnIndexDel = createIconButton("gtk-delete", "_Supprimer");
  gtk_box_pack_start (GTK_BOX (hbox8), btnIndexDel, TRUE, TRUE, 0);

  label2 = gtk_label_new ("Index");
  gtk_widget_show (label2);
  gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook1), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook1), 1), label2);
  gtk_label_set_justify (GTK_LABEL (label2), GTK_JUSTIFY_LEFT);


/* Misc part */

  vbox4 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox4);
  gtk_container_add (GTK_CONTAINER (notebook1), vbox4);

  frame2 = gtk_frame_new (NULL);
  gtk_widget_show (frame2);
  gtk_box_pack_start (GTK_BOX (vbox4), frame2, FALSE, TRUE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (frame2), 3);

  vbox5 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox5);
  gtk_container_add (GTK_CONTAINER (frame2), vbox5);

  hbox6 = gtk_hbox_new (FALSE, 2);
  gtk_widget_show (hbox6);
  gtk_box_pack_start (GTK_BOX (vbox5), hbox6, TRUE, TRUE, 2);

  label13 = gtk_label_new ("Nom de la table :");
  gtk_widget_show (label13);
  gtk_box_pack_start (GTK_BOX (hbox6), label13, FALSE, FALSE, 0);
  gtk_label_set_justify (GTK_LABEL (label13), GTK_JUSTIFY_LEFT);

  pTblWnd->txtTableName = gtk_entry_new ();
  gtk_widget_show (pTblWnd->txtTableName);
  gtk_box_pack_start (GTK_BOX (hbox6), pTblWnd->txtTableName, TRUE, TRUE, 0);

  label14 = gtk_label_new ("Commentaires :");
  gtk_widget_show (label14);
  gtk_box_pack_start (GTK_BOX (vbox5), label14, FALSE, FALSE, 0);
  gtk_label_set_justify (GTK_LABEL (label14), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (label14), 0, 0.5);

  scrolledwindow2 = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (scrolledwindow2);
  gtk_box_pack_start (GTK_BOX (vbox5), scrolledwindow2, TRUE, TRUE, 0);

  pTblWnd->txtXComments = gtk_text_view_new ();
  gtk_widget_show (pTblWnd->txtXComments);
  gtk_container_add (GTK_CONTAINER (scrolledwindow2), pTblWnd->txtXComments);

  label11 = gtk_label_new ("Informations :");
  gtk_widget_show (label11);
  gtk_frame_set_label_widget (GTK_FRAME (frame2), label11);
  gtk_label_set_justify (GTK_LABEL (label11), GTK_JUSTIFY_LEFT);

  frame3 = gtk_frame_new (NULL);
  gtk_widget_show (frame3);
  gtk_box_pack_start (GTK_BOX (vbox4), frame3, FALSE, TRUE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (frame3), 3);

  vbox6 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox6);
  gtk_container_add (GTK_CONTAINER (frame3), vbox6);

  hbox7 = gtk_hbox_new (FALSE, 2);
  gtk_widget_show (hbox7);
  gtk_box_pack_start (GTK_BOX (vbox6), hbox7, TRUE, TRUE, 2);

  label15 = gtk_label_new ("Type de format :");
  gtk_widget_show (label15);
  gtk_box_pack_start (GTK_BOX (hbox7), label15, FALSE, FALSE, 0);
  gtk_label_set_justify (GTK_LABEL (label15), GTK_JUSTIFY_LEFT);

  pTblWnd->cmbTableFormat = gtk_combo_new ();
  gtk_widget_show (pTblWnd->cmbTableFormat);
  gtk_box_pack_start (GTK_BOX (hbox7), pTblWnd->cmbTableFormat, FALSE, TRUE, 0);

  combo_entry4 = GTK_COMBO (pTblWnd->cmbTableFormat)->entry;
  gtk_widget_show (combo_entry4);

  label12 = gtk_label_new ("Format :");
  gtk_widget_show (label12);
  gtk_frame_set_label_widget (GTK_FRAME (frame3), label12);
  gtk_label_set_justify (GTK_LABEL (label12), GTK_JUSTIFY_LEFT);

  label3 = gtk_label_new ("General");
  gtk_widget_show (label3);
  gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook1), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook1), 2), label3);
  gtk_label_set_justify (GTK_LABEL (label3), GTK_JUSTIFY_LEFT);

  statusbar1 = gtk_statusbar_new ();
  gtk_widget_show (statusbar1);
  gtk_box_pack_start (GTK_BOX (vbox1), statusbar1, FALSE, FALSE, 0);
	
	gtk_widget_set_sensitive (btnFieldUp, FALSE);
	gtk_widget_set_sensitive (btnFieldDown, FALSE);

	
	initDataTable(pTblWnd);
	
	if (display) {
		gtk_widget_show (pTblWnd->wndTable);
		NbrWnd ++;	
	}

	return pTblWnd;
}

