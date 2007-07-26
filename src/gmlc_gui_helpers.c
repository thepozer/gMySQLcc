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

#include "gmlc_gui_helpers.h"

void closeAskFilename (GtkWidget *widget, gpointer user_data);

GString * askInfo(const gchar * title, const gchar * message, const gchar * defaultValue) {
	GtkWidget *dialog, *label, *txtValue;
	GString * retStr;
	gint ret ;
	
	dialog = gtk_dialog_new_with_buttons (title, GTK_WINDOW(NULL), GTK_DIALOG_MODAL, GTK_STOCK_OK, GTK_RESPONSE_OK, GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, NULL);

	label = gtk_label_new (message);
	gtk_container_add (GTK_CONTAINER (GTK_DIALOG(dialog)->vbox), label);
	gtk_widget_show(label);
	
	txtValue = gtk_entry_new ();
	if (defaultValue != (gchar *)NULL) {
		gtk_entry_set_text (GTK_ENTRY(txtValue), defaultValue);
	}
	gtk_container_add (GTK_CONTAINER (GTK_DIALOG(dialog)->vbox), txtValue);
	gtk_widget_show(txtValue);

	ret = gtk_dialog_run (GTK_DIALOG(dialog));
	
	if (ret == GTK_RESPONSE_OK) {
		retStr = g_string_new(gtk_entry_get_text(GTK_ENTRY(txtValue)));
	} else {
		retStr = (GString *)NULL;
	}
	gtk_widget_hide (GTK_WIDGET(dialog));
	gtk_widget_destroy (GTK_WIDGET(dialog));
	
	
	return retStr;
}

gboolean askConfirmation(const gchar * title, const gchar * message) {
	GtkWidget *dialog, *label;
	gboolean retVal;
	gint ret ;
	
	dialog = gtk_dialog_new_with_buttons (title, GTK_WINDOW(NULL), GTK_DIALOG_MODAL, GTK_STOCK_OK, GTK_RESPONSE_OK, GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, NULL);

	label = gtk_label_new (message);
	gtk_container_add (GTK_CONTAINER (GTK_DIALOG(dialog)->vbox), label);
	gtk_widget_show(label);
	
	ret = gtk_dialog_run (GTK_DIALOG(dialog));
	
	retVal = (ret == GTK_RESPONSE_OK);

	gtk_widget_hide (GTK_WIDGET(dialog));
	gtk_widget_destroy (GTK_WIDGET(dialog));
	
	
	return retVal;
}

gboolean askYesno(const gchar * title, const gchar * message) {
	GtkWidget *dialog, *label;
	gboolean retVal;
	gint ret ;
	
	dialog = gtk_dialog_new_with_buttons (title, GTK_WINDOW(NULL), GTK_DIALOG_MODAL, GTK_STOCK_YES, GTK_RESPONSE_OK, GTK_STOCK_NO, GTK_RESPONSE_CANCEL, NULL);

	label = gtk_label_new (message);
	gtk_container_add (GTK_CONTAINER (GTK_DIALOG(dialog)->vbox), label);
	gtk_widget_show(label);
	
	ret = gtk_dialog_run (GTK_DIALOG(dialog));
	
	retVal = (ret == GTK_RESPONSE_OK);

	gtk_widget_hide (GTK_WIDGET(dialog));
	gtk_widget_destroy (GTK_WIDGET(dialog));
	
	
	return retVal;
}

gboolean askFilename(const gchar * title, const gchar * filename, void (*okevent) (GtkWidget *widget, gpointer user_data), gpointer user_data) {
	p_askFilenameInfos paskFnInf;
	
	paskFnInf = (p_askFilenameInfos) g_try_malloc(sizeof(askFilenameInfos));
	
	if (paskFnInf == (p_askFilenameInfos)NULL) {
		return FALSE;
	}
	
	paskFnInf->userData = user_data;
	
	paskFnInf->dialog = gtk_file_selection_new (title);
	
	g_signal_connect (GTK_OBJECT (GTK_FILE_SELECTION (paskFnInf->dialog)->ok_button),
					"clicked", G_CALLBACK (okevent), paskFnInf);
	
  /* Ensure that the dialog box is destroyed when the user clicks a button. */
	g_signal_connect (GTK_OBJECT (GTK_FILE_SELECTION (paskFnInf->dialog)->ok_button),
					"clicked", G_CALLBACK (closeAskFilename), paskFnInf); 
	
	g_signal_connect (GTK_OBJECT (GTK_FILE_SELECTION (paskFnInf->dialog)->cancel_button),
					"clicked", G_CALLBACK (closeAskFilename), paskFnInf); 
  
	if (filename != (gchar *)NULL) {
		gtk_file_selection_set_filename (GTK_FILE_SELECTION (paskFnInf->dialog), filename);
	}
	
	/* Display that dialog */
	gtk_widget_show (paskFnInf->dialog);
	
	return TRUE;
}

void closeAskFilename (GtkWidget *widget, gpointer user_data) {
	p_askFilenameInfos paskFnInf = (p_askFilenameInfos)user_data;
	
	gtk_widget_destroy(paskFnInf->dialog);
	
	g_free(paskFnInf);
}

gchar * gmlc_helpers_protect_underscore (const gchar * str) {
	GString * strRet = NULL;
	gchar * pcRet = NULL;
	int i = 0;
	
	strRet = g_string_new(str);
	
	for (i = 0; i < strRet->len; i++) {
		if (strRet->str[i] == '_') {
			g_string_insert_c(strRet, i, '_');
			i++;
		}
	}
	
	pcRet = g_strdup(strRet->str);
	g_string_free(strRet, TRUE);
	
	return pcRet;
}


