
#include "gTableEvents.h"

extern int NbrWnd;

void lstindexused_selected (GtkTreeSelection *selection, gpointer user_data) {
	GtkTreeIter iter;
	GtkTreeModel *model;
	p_tableWnd pTblWnd = (p_tableWnd)user_data;
	gchar * tmpch;
	p_indexFieldInfo p_ifi;
	p_fieldInfo p_fi;
  GList * items;
	int idx;

	if (gtk_tree_selection_get_selected (selection, &model, &iter)) {
		gtk_tree_model_get (model, &iter, 2, &pTblWnd->currIndexFI, -1);
		
		idx = -1;
		items = g_list_first(pTblWnd->p_lstField);
		while (items != (GList *)NULL) {
			idx ++;
			p_fi = (p_fieldInfo)items->data;
			if (pTblWnd->currIndexFI->field_ref == p_fi) {
				gtk_list_select_item(GTK_LIST(GTK_COMBO(pTblWnd->cmbIndexLstFields)->list), idx);
				break;
			}
			/* Go to next item */
			items = g_list_next(items);
		}
		
		tmpch = g_strdup_printf("%i", pTblWnd->currIndexFI->subpart);
		gtk_entry_set_text(GTK_ENTRY(pTblWnd->txtIndexSubPart), tmpch);
		g_free(tmpch);
	}		
}

void lstindex_selected (GtkTreeSelection *selection, gpointer user_data) {
	GtkTreeIter iter;
	GtkTreeModel *model;
	p_tableWnd pTblWnd = (p_tableWnd)user_data;
	
	p_indexFieldInfo p_ifi;

  GList * items;
	GtkListStore * lstStrIdxDetail;
	GtkTreeViewColumn * currCol;
	GtkCellRenderer * renderer;

	if (gtk_tree_selection_get_selected (selection, &model, &iter)) {
		gtk_tree_model_get (model, &iter, 1, &pTblWnd->currIndex, -1);
		gtk_entry_set_text(GTK_ENTRY(pTblWnd->txtIndexName), pTblWnd->currIndex->name->str);

		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(pTblWnd->chkIndexPrimary), pTblWnd->currIndex->primary);
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(pTblWnd->chkIndexUnique), pTblWnd->currIndex->unique);
		
		lstStrIdxDetail = gtk_list_store_new(3, G_TYPE_STRING, G_TYPE_INT, G_TYPE_POINTER);
		items = g_list_first(pTblWnd->currIndex->fieldsIdx);
		while (items != (GList *)NULL) {
			/* Check if field can be displayed */
			p_ifi = (p_indexFieldInfo)items->data;
			if (p_ifi->status == FI_DEL) {
				p_ifi = (p_indexFieldInfo)NULL;
			} else {
				gtk_list_store_append (lstStrIdxDetail, &iter);
				gtk_list_store_set (lstStrIdxDetail, &iter, 0, p_ifi->name->str, -1);
				gtk_list_store_set (lstStrIdxDetail, &iter, 1, p_ifi->subpart, -1);
				gtk_list_store_set (lstStrIdxDetail, &iter, 2, p_ifi, -1);
			}
		
			/* Go to next item */
			items = g_list_next(items);
		}
		
		gtk_tree_view_set_model(GTK_TREE_VIEW(pTblWnd->lstIndexUsed), GTK_TREE_MODEL(lstStrIdxDetail));
		g_object_unref (G_OBJECT (lstStrIdxDetail));
		
		currCol = gtk_tree_view_get_column(GTK_TREE_VIEW(pTblWnd->lstIndexUsed), 0);
		while (currCol != (GtkTreeViewColumn *)NULL) {
			gtk_tree_view_remove_column(GTK_TREE_VIEW(pTblWnd->lstIndexUsed), currCol);
			currCol = gtk_tree_view_get_column(GTK_TREE_VIEW(pTblWnd->lstIndexUsed), 0);
		}
	
		renderer = gtk_cell_renderer_text_new ();
		currCol = gtk_tree_view_column_new_with_attributes ("Field", renderer, "text", 0, NULL);
		gtk_tree_view_append_column (GTK_TREE_VIEW (pTblWnd->lstIndexUsed), currCol);
		currCol = gtk_tree_view_column_new_with_attributes ("Subpart", renderer, "text", 1, NULL);
		gtk_tree_view_append_column (GTK_TREE_VIEW (pTblWnd->lstIndexUsed), currCol);
	}
}

void lstfield_selected (GtkTreeSelection *selection, gpointer user_data) {
	GtkTreeIter iter;
	GtkTreeModel *model;
	p_tableWnd pTblWnd = (p_tableWnd)user_data;
	gchar * tmpch;
	
	if (gtk_tree_selection_get_selected (selection, &model, &iter)) {
		gtk_tree_model_get (model, &iter, 2, &pTblWnd->currField, -1);
		gtk_entry_set_text(GTK_ENTRY(pTblWnd->txtFieldName), pTblWnd->currField->name->str);
		gtk_list_select_item(GTK_LIST(GTK_COMBO(pTblWnd->cmbFieldType)->list), pTblWnd->currField->type_idx);
		pTblWnd->currType = pTblWnd->currField->type_idx;
		gtk_entry_set_text(GTK_ENTRY(pTblWnd->txtFieldDefault), pTblWnd->currField->def->str);

		switch(arFieldTypes[pTblWnd->currType]->typeValue) {
			case 1 : /* Length */
				tmpch = g_strdup_printf("%i", pTblWnd->currField->length);
				gtk_entry_set_text(GTK_ENTRY(pTblWnd->txtFieldLen), tmpch);
				g_free(tmpch);
				gtk_entry_set_text(GTK_ENTRY(pTblWnd->txtFieldDecimal), "");
				gtk_entry_set_text(GTK_ENTRY(pTblWnd->txtFieldValues), "");
				break;
			case 2 : /* Length and decimals */
				tmpch = g_strdup_printf("%i", pTblWnd->currField->length);
				gtk_entry_set_text(GTK_ENTRY(pTblWnd->txtFieldLen), tmpch);
				g_free(tmpch);
				tmpch = g_strdup_printf("%i", pTblWnd->currField->decimal);
				gtk_entry_set_text(GTK_ENTRY(pTblWnd->txtFieldDecimal), tmpch);
				g_free(tmpch);
				gtk_entry_set_text(GTK_ENTRY(pTblWnd->txtFieldValues), "");
				break;
			case 3 : /* List of value */
				gtk_entry_set_text(GTK_ENTRY(pTblWnd->txtFieldLen), "");
				gtk_entry_set_text(GTK_ENTRY(pTblWnd->txtFieldDecimal), "");
				gtk_entry_set_text(GTK_ENTRY(pTblWnd->txtFieldValues), pTblWnd->currField->set->str);
				break;
			case 0 : /* None */
			default: 
				gtk_entry_set_text(GTK_ENTRY(pTblWnd->txtFieldLen), "");
				gtk_entry_set_text(GTK_ENTRY(pTblWnd->txtFieldDecimal), "");
				gtk_entry_set_text(GTK_ENTRY(pTblWnd->txtFieldValues), "");
				break;
		}

		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(pTblWnd->chkFieldUnsign), 
					((arFieldTypes[pTblWnd->currType]->unsign) ? pTblWnd->currField->unsign : FALSE));
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(pTblWnd->chkFieldZeroFill), 
					((arFieldTypes[pTblWnd->currType]->zerofill) ? pTblWnd->currField->zerofill : FALSE));
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(pTblWnd->chkFieldBinary), 
					((arFieldTypes[pTblWnd->currType]->binary) ? pTblWnd->currField->binary : FALSE));
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(pTblWnd->chkFieldAutoIncr), 
					((arFieldTypes[pTblWnd->currType]->autoincr) ? pTblWnd->currField->autoIncr : FALSE));
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(pTblWnd->chkFieldNull), pTblWnd->currField->nullOk);
	}
}

void cmbfieldtype_selected (GtkWidget *list, GtkWidget *widget, gpointer user_data) {
	p_tableWnd pTblWnd = (p_tableWnd)user_data;
	
	pTblWnd->currType = gtk_list_child_position(GTK_LIST(list), widget);
	if (pTblWnd->currType > -1) {
		gtk_widget_set_sensitive (pTblWnd->txtFieldLen, (arFieldTypes[pTblWnd->currType]->typeValue == FTC_TV_LENGTH || arFieldTypes[pTblWnd->currType]->typeValue == FTC_TV_LENGTH_DECI));
		gtk_widget_set_sensitive (pTblWnd->txtFieldDecimal, (arFieldTypes[pTblWnd->currType]->typeValue == FTC_TV_LENGTH_DECI));
		gtk_widget_set_sensitive (pTblWnd->txtFieldValues, (arFieldTypes[pTblWnd->currType]->typeValue == FTC_TV_LIST_VALUE));
		gtk_widget_set_sensitive (pTblWnd->chkFieldUnsign, arFieldTypes[pTblWnd->currType]->unsign);
		gtk_widget_set_sensitive (pTblWnd->chkFieldZeroFill, arFieldTypes[pTblWnd->currType]->zerofill);
		gtk_widget_set_sensitive (pTblWnd->chkFieldBinary, arFieldTypes[pTblWnd->currType]->binary);
		gtk_widget_set_sensitive (pTblWnd->chkFieldAutoIncr, arFieldTypes[pTblWnd->currType]->autoincr);
	}
}

void btnfieldnew_clicked (GtkWidget *widget, gpointer user_data) {
	p_tableWnd pTblWnd = (p_tableWnd)user_data;
	
	pTblWnd->currField = (p_fieldInfo)NULL;
	pTblWnd->currType = -1;
	gtk_entry_set_text(GTK_ENTRY(pTblWnd->txtFieldName), "");
	gtk_list_select_item(GTK_LIST(GTK_COMBO(pTblWnd->cmbFieldType)->list), -1);
	gtk_entry_set_text(GTK_ENTRY(pTblWnd->txtFieldLen), "");
	gtk_entry_set_text(GTK_ENTRY(pTblWnd->txtFieldDecimal), "");
	gtk_entry_set_text(GTK_ENTRY(pTblWnd->txtFieldValues), "");
	gtk_entry_set_text(GTK_ENTRY(pTblWnd->txtFieldDefault), "");
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(pTblWnd->chkFieldUnsign), FALSE);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(pTblWnd->chkFieldZeroFill), FALSE);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(pTblWnd->chkFieldBinary), FALSE);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(pTblWnd->chkFieldAutoIncr), FALSE);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(pTblWnd->chkFieldNull), FALSE);
}

void btnfieldadd_clicked (GtkWidget *widget, gpointer user_data) {
	p_tableWnd pTblWnd = (p_tableWnd)user_data;
	p_fieldInfo p_fi;
	
	p_fi = createFieldInfo(FI_ADD);
	/* Set informations */
	if (p_fi != (p_fieldInfo)NULL) {
		/* Set data value */
		p_fi->name = g_string_new(gtk_entry_get_text(GTK_ENTRY(pTblWnd->txtFieldName)));
		p_fi->type_idx = pTblWnd->currType;
		switch(arFieldTypes[pTblWnd->currType]->typeValue) {
			case 1 : /* Length */
				p_fi->length = (int)strtol(gtk_entry_get_text(GTK_ENTRY(pTblWnd->txtFieldLen)), (char **)NULL, 10);
				p_fi->decimal = 0;
				p_fi->set = (GString *)NULL;
				break;
			case 2 : /* Length and decimals */
				p_fi->length = (int)strtol(gtk_entry_get_text(GTK_ENTRY(pTblWnd->txtFieldLen)), (char **)NULL, 10);
				p_fi->decimal = (int)strtol(gtk_entry_get_text(GTK_ENTRY(pTblWnd->txtFieldDecimal)), (char **)NULL, 10);
				p_fi->set = (GString *)NULL;
				break;
			case 3 : /* List of value */
				p_fi->length = 0;
				p_fi->decimal = 0;
				p_fi->set = g_string_new(gtk_entry_get_text(GTK_ENTRY(pTblWnd->txtFieldLen)));
				break;
			case 0 : /* None */
			default: 
				p_fi->length = 0;
				p_fi->decimal = 0;
				p_fi->set = (GString *)NULL;
				break;
		}
		p_fi->def = g_string_new(gtk_entry_get_text(GTK_ENTRY(pTblWnd->txtFieldDefault)));
		p_fi->unsign = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(pTblWnd->chkFieldUnsign));
		p_fi->zerofill = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(pTblWnd->chkFieldZeroFill));
		p_fi->binary = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(pTblWnd->chkFieldBinary));
		p_fi->autoIncr = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(pTblWnd->chkFieldAutoIncr));
		p_fi->nullOk = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(pTblWnd->chkFieldNull));
		
		pTblWnd->p_lstField = g_list_append(pTblWnd->p_lstField, p_fi);
	}
	displayFieldList (pTblWnd); /* Display field list */
	pTblWnd->tableModified = TRUE;
}

void btnfieldedit_clicked (GtkWidget *widget, gpointer user_data) {
	p_tableWnd pTblWnd = (p_tableWnd)user_data;
	p_fieldInfo p_fi;
	
	if (pTblWnd->currField != (p_fieldInfo)NULL) {
		/* Select p_fi value */
		if (pTblWnd->currField->status == FI_ADD) {
			p_fi = pTblWnd->currField;
		} else if (pTblWnd->currField->status == FI_ORIGIN && pTblWnd->currField->update != (p_fieldInfo)NULL) {
			p_fi = pTblWnd->currField->update;
		} else {
			pTblWnd->currField->status = FI_MODIFY;
			p_fi = createFieldInfo(FI_MODIFY);
		}
		/* Update informations */
		if (p_fi != (p_fieldInfo)NULL) {
			/* Set data value */
			p_fi->name = g_string_new(gtk_entry_get_text(GTK_ENTRY(pTblWnd->txtFieldName)));
			p_fi->type_idx = pTblWnd->currType;
			switch(arFieldTypes[pTblWnd->currType]->typeValue) {
				case 1 : /* Length */
					p_fi->length = (int)strtol(gtk_entry_get_text(GTK_ENTRY(pTblWnd->txtFieldLen)), (char **)NULL, 10);
					p_fi->decimal = 0;
					p_fi->set = (GString *)NULL;
					break;
				case 2 : /* Length and decimals */
					p_fi->length = (int)strtol(gtk_entry_get_text(GTK_ENTRY(pTblWnd->txtFieldLen)), (char **)NULL, 10);
					p_fi->decimal = (int)strtol(gtk_entry_get_text(GTK_ENTRY(pTblWnd->txtFieldDecimal)), (char **)NULL, 10);
					p_fi->set = (GString *)NULL;
					break;
				case 3 : /* List of value */
					p_fi->length = 0;
					p_fi->decimal = 0;
					p_fi->set = g_string_new(gtk_entry_get_text(GTK_ENTRY(pTblWnd->txtFieldLen)));
					break;
				case 0 : /* None */
				default: 
					p_fi->length = 0;
					p_fi->decimal = 0;
					p_fi->set = (GString *)NULL;
					break;
			}
			p_fi->def = g_string_new(gtk_entry_get_text(GTK_ENTRY(pTblWnd->txtFieldDefault)));
			p_fi->unsign = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(pTblWnd->chkFieldUnsign));
			p_fi->zerofill = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(pTblWnd->chkFieldZeroFill));
			p_fi->binary = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(pTblWnd->chkFieldBinary));
			p_fi->autoIncr = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(pTblWnd->chkFieldAutoIncr));
			p_fi->nullOk = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(pTblWnd->chkFieldNull));
			
			/* Set link */
			if (p_fi != pTblWnd->currField) {
				p_fi->origin = pTblWnd->currField;
				pTblWnd->currField->update = p_fi;
			} else {
				p_fi->origin = (p_fieldInfo)NULL;
				p_fi->update = (p_fieldInfo)NULL;
			}
		}
		displayFieldList (pTblWnd); /* Display field list */
		pTblWnd->tableModified = TRUE;
	}
}

void btnfielddel_clicked (GtkWidget *widget, gpointer user_data) {
	p_tableWnd pTblWnd = (p_tableWnd)user_data;
	p_fieldInfo p_fi;
	
	if (pTblWnd->currField != (p_fieldInfo)NULL) {
		pTblWnd->currField->status = FI_DEL;
		displayFieldList (pTblWnd); /* Display field list */
		pTblWnd->tableModified = TRUE;
	}
}

void btntlbrclose_clicked (GtkWidget *widget, gpointer user_data) {
	p_tableWnd pTblWnd = (p_tableWnd)user_data;
	gboolean ok = FALSE;
	
	if (pTblWnd->tableModified) {
		ok = askYesno("Table modified", "Do you want apply modification ?");
		if (ok) {
			btntlbrvalidate_clicked(widget, user_data);
		}
	}
	
	gtk_widget_destroy(GTK_WIDGET(pTblWnd->wndTable));
}

void btntlbrvalidate_clicked (GtkWidget *widget, gpointer user_data) {
	p_tableWnd pTblWnd = (p_tableWnd)user_data;
	GList * lstSQLQueries,  * lst;
	mysql_query_infos * p_mqi;
	GtkWidget * msgdlg;
	gboolean ok = FALSE;
	char * query;
	
	lstSQLQueries = generateFieldAlterTable(pTblWnd->p_lstField, pTblWnd->tableName);
	
	if (pTblWnd->tableModified) {
		ok = askYesno("Modify table", "Are you sure ?");
		if (ok) {
			g_print("Modify table : '%s'\n", pTblWnd->tableName->str);
			lst = g_list_first(lstSQLQueries);
			while (lst != (GList *)NULL) {
				query = ((GString *)lst->data)->str;
				g_print("SQL : '%s'\n", query);
				p_mqi = db_query_all(pTblWnd->p_mci, query);
				if (p_mqi != (mysql_query_infos *)NULL) {
					db_free_query(p_mqi);
				}
				lst = g_list_next(lst);
			}
		}
		
		fillFieldList (pTblWnd); /* Fill field list */
		displayFieldList (pTblWnd); /* Display field list */
		pTblWnd->tableModified = FALSE;
	}
}

void destroy(GtkWidget *widget, gpointer user_data) {
	p_tableWnd pTblWnd = (p_tableWnd)user_data;
	
	/* Destroy GUI */
	gtk_widget_hide (GTK_WIDGET(pTblWnd->wndTable));
	gtk_widget_destroy (GTK_WIDGET(pTblWnd->wndTable));
	
	/* Destroy Datas */
	g_free(pTblWnd);
	
	/* Destroy Application if needed */
	NbrWnd--;
	g_printerr("Destruction Server window - nbrWnd : %d\n", NbrWnd);
	if (NbrWnd <= 0) {
		g_printerr("Destroy App\n");
		gtk_main_quit();
	}
}

