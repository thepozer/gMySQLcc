
#include "gmysql_gui.h"

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

GtkWidget * createIconButton(const char * icon, const char * title) {
	GtkWidget * btn, * align, * hbox, * img, * lbl;
	
  btn = gtk_button_new ();
  gtk_widget_show (btn);

  align = gtk_alignment_new (0.5, 0.5, 0, 0);
  gtk_widget_show (align);
  gtk_container_add (GTK_CONTAINER (btn), align);

  hbox = gtk_hbox_new (FALSE, 2);
  gtk_widget_show (hbox);
  gtk_container_add (GTK_CONTAINER (align), hbox);

  img = gtk_image_new_from_stock (icon, GTK_ICON_SIZE_BUTTON);
  gtk_widget_show (img);
  gtk_box_pack_start (GTK_BOX (hbox), img, FALSE, FALSE, 0);

  lbl = gtk_label_new_with_mnemonic (title);
  gtk_widget_show (lbl);
  gtk_box_pack_start (GTK_BOX (hbox), lbl, FALSE, FALSE, 0);
  gtk_label_set_justify (GTK_LABEL (lbl), GTK_JUSTIFY_LEFT);
	
	return btn;
}
