
#include "gmysqlcc_gui_all.h"


void gmysqlcc_gui_text_create_widget (p_gmysqlcc_gui_text gui_text);
void gmysqlcc_gui_text_init_widget (p_gmysqlcc_gui_text gui_text);
void gmysqlcc_gui_text_save_to_file (p_gmysqlcc_gui_text gui_text, const gchar * filename);

void gmysqlcc_gui_text_evt_btnTlbrSave_clicked (GtkWidget *widget, gpointer user_data);
void gmysqlcc_gui_text_evt_btnTlbrClose_clicked (GtkWidget *widget, gpointer user_data);
void gmysqlcc_gui_text_evt_destroy (GtkWidget *widget, gpointer user_data);

p_gmysqlcc_gui_text gmysqlcc_gui_text_new () {
	p_gmysqlcc_gui_text gui_text;
	
	/* Init structure */
	gui_text = (p_gmysqlcc_gui_text) g_try_malloc(sizeof(s_gmysqlcc_gui_text));
	
	if (gui_text == NULL) {
		return NULL; /* return NULL pointer */
	}
	
	gui_text->filename = NULL;
	
	gmysqlcc_gui_text_create_widget(gui_text);
	gmysqlcc_gui_text_init_widget(gui_text);
	
	NbrWnd ++;	
	
	return gui_text;
}

gboolean gmysqlcc_gui_text_set_content (p_gmysqlcc_gui_text gui_text, const gchar * content, const gchar * filename) {
	GString * sTitle;
	GtkTextBuffer * txtBuffer;
	
	if (content != NULL) {
		txtBuffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(gui_text->txtContent));
		gtk_text_buffer_set_text(GTK_TEXT_BUFFER(txtBuffer), content, -1);
	} else {
		return FALSE;
	}
	
	if (filename != NULL) {
		g_free(gui_text->filename);
		gui_text->filename = g_strdup(filename);
		
		sTitle = g_string_new("");
		g_string_printf(sTitle, _("Text Window - File name : %s"), gui_text->filename);
		gtk_window_set_title (GTK_WINDOW (gui_text->window), sTitle->str);
		g_string_free(sTitle, TRUE);
	}
	
	return TRUE;
}

gboolean gmysqlcc_gui_text_display (p_gmysqlcc_gui_text gui_text, gboolean display) {
	if (display) {
		gtk_widget_show (gui_text->window);
	} else {
		gtk_widget_hide (gui_text->window);
	}
	
	return TRUE;
}

gboolean gmysqlcc_gui_text_delete (p_gmysqlcc_gui_text gui_text) {
	/* Destroy Datas */
	g_free(gui_text->filename);
	g_free(gui_text);
	
	/* Destroy Application if needed */
	NbrWnd--;
	g_printerr("Destruction Server window - nbrWnd : %d\n", NbrWnd);
	if (NbrWnd <= 0) {
		g_printerr("Destroy App\n");
		gtk_main_quit();
	}
	
	return TRUE;
}

void gmysqlcc_gui_text_create_widget (p_gmysqlcc_gui_text gui_text) {
	GtkWidget *vbox;
  GtkWidget *toolbar;
  GtkWidget *scrolledwindow;
 	GtkWidget * imgToolbar;
  GtkToolItem * btnTlbrClose, * btnTlbrSave;
	GtkTooltips * tooltips;
	GtkTextBuffer * txtBuffer;
	
	gui_text->window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title (GTK_WINDOW (gui_text->window), _("Text Window"));
 	gtk_window_set_default_size (GTK_WINDOW (gui_text->window), 400, 400);
	g_signal_connect (G_OBJECT (gui_text->window), "destroy", G_CALLBACK (gmysqlcc_gui_text_evt_destroy), gui_text);

  vbox = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox);
  gtk_container_add (GTK_CONTAINER (gui_text->window), vbox);

  toolbar = gtk_toolbar_new ();
  gtk_widget_show (toolbar);
  gtk_box_pack_start (GTK_BOX (vbox), toolbar, FALSE, FALSE, 0);
  gtk_toolbar_set_style (GTK_TOOLBAR (toolbar), GTK_TOOLBAR_BOTH_HORIZ);

	imgToolbar = gtk_image_new_from_stock(GTK_STOCK_SAVE, GTK_ICON_SIZE_LARGE_TOOLBAR);
	gtk_widget_show(imgToolbar);
	btnTlbrSave = gtk_tool_button_new (imgToolbar, _("Save"));
	gtk_tool_item_set_is_important (GTK_TOOL_ITEM(btnTlbrSave), TRUE);
	g_signal_connect (G_OBJECT (btnTlbrSave), "clicked", G_CALLBACK (gmysqlcc_gui_text_evt_btnTlbrSave_clicked), gui_text);
	gtk_widget_show(GTK_WIDGET(btnTlbrSave));
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar), GTK_TOOL_ITEM(btnTlbrSave), -1);
	gtk_tool_item_set_tooltip (GTK_TOOL_ITEM(btnTlbrSave), tooltips, _("Save text"), NULL);
	
	imgToolbar = gtk_image_new_from_stock(GTK_STOCK_CLOSE, GTK_ICON_SIZE_LARGE_TOOLBAR);
	gtk_widget_show(imgToolbar);
	btnTlbrClose = gtk_tool_button_new (imgToolbar, _("Close"));
	gtk_tool_item_set_is_important (GTK_TOOL_ITEM(btnTlbrClose), TRUE);
	g_signal_connect (G_OBJECT (btnTlbrClose), "clicked", G_CALLBACK (gmysqlcc_gui_text_evt_btnTlbrClose_clicked), gui_text);
	gtk_widget_show(GTK_WIDGET(btnTlbrClose));
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar), GTK_TOOL_ITEM(btnTlbrClose), -1);
	gtk_tool_item_set_tooltip (GTK_TOOL_ITEM(btnTlbrClose), tooltips, _("Close window"), NULL);
	
  scrolledwindow = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (scrolledwindow);
  gtk_box_pack_start (GTK_BOX (vbox), scrolledwindow, TRUE, TRUE, 0);

#ifdef USE_GTKSOURCEVIEW
  gui_text->txtContent = gtk_source_view_new ();
	gtk_source_view_set_show_line_numbers (GTK_SOURCE_VIEW(gui_text->txtContent), TRUE);
	gtk_source_view_set_tabs_width (GTK_SOURCE_VIEW(gui_text->txtContent), 2);
	txtBuffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(gui_text->txtContent));
	gtk_source_buffer_set_highlight (GTK_SOURCE_BUFFER(txtBuffer), TRUE);
	gtk_source_buffer_set_language(GTK_SOURCE_BUFFER(txtBuffer), 
			gtk_source_languages_manager_get_language_from_mime_type(LangManager, "text/x-sql"));
#else /* USE_GTKSOURCEVIEW */
  gui_text->txtContent = gtk_text_view_new ();
#endif /* USE_GTKSOURCEVIEW */
  gtk_widget_show (gui_text->txtContent);
  gtk_container_add (GTK_CONTAINER (scrolledwindow), gui_text->txtContent);
	
}

void gmysqlcc_gui_text_init_widget (p_gmysqlcc_gui_text gui_text) {
	/* Do nothing */
}


void gmysqlcc_gui_text_save_to_file (p_gmysqlcc_gui_text gui_text, const gchar * filename) {
	gchar * content = NULL;
	GtkTextBuffer * txtBuffer;
	GtkTextIter begin, end;
	GIOChannel * saveFile;
	GError * err = NULL;
	gssize nbBytes;
	
	g_return_if_fail (filename != NULL);
	
	/*g_print("Save file : '%s'\n", filename);*/
	
	g_free(gui_text->filename);
	gui_text->filename = g_strdup(filename);
	
	txtBuffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(gui_text->txtContent));
	gtk_text_buffer_get_start_iter (GTK_TEXT_BUFFER(txtBuffer), &begin);
	gtk_text_buffer_get_end_iter (GTK_TEXT_BUFFER(txtBuffer), &end);
	content = (gchar *)gtk_text_buffer_get_text (GTK_TEXT_BUFFER(txtBuffer), &begin, &end, FALSE);

	saveFile = g_io_channel_new_file(filename, "w", &err);
	g_io_channel_set_encoding(saveFile, "ISO-8859-15", &err);
	g_io_channel_write_chars(saveFile, content, -1, &nbBytes, &err);
	g_io_channel_unref(saveFile);

}

void gmysqlcc_gui_text_evt_btnTlbrSave_clicked (GtkWidget *widget, gpointer user_data) {
	p_gmysqlcc_gui_text gui_text = (p_gmysqlcc_gui_text)user_data;
	GtkWidget *chooser;
	gint response;
	
	chooser = gtk_file_chooser_dialog_new (_("Save text"), GTK_WINDOW(gui_text->window), GTK_FILE_CHOOSER_ACTION_SAVE,
		GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, GTK_STOCK_SAVE, GTK_RESPONSE_OK, NULL);
	
	gtk_file_chooser_set_filename (GTK_FILE_CHOOSER (chooser), gui_text->filename);
	
	response = gtk_dialog_run (GTK_DIALOG (chooser));
	if (response == GTK_RESPONSE_OK) {
		gchar *filename;
		
		filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (chooser));
		if (filename != NULL) {
			gmysqlcc_gui_text_save_to_file (gui_text, filename);
			g_free (filename);
		}
	}
	
	gtk_widget_destroy (chooser);
}

void gmysqlcc_gui_text_evt_btnTlbrClose_clicked (GtkWidget *widget, gpointer user_data) {
	p_gmysqlcc_gui_text gui_text = (p_gmysqlcc_gui_text)user_data;
	
	gtk_widget_destroy(GTK_WIDGET(gui_text->window));
}

void gmysqlcc_gui_text_evt_destroy (GtkWidget *widget, gpointer user_data) {
	p_gmysqlcc_gui_text gui_text = (p_gmysqlcc_gui_text)user_data;
	
	gmysqlcc_gui_text_display(gui_text, FALSE);
	gmysqlcc_gui_text_delete(gui_text);
}

