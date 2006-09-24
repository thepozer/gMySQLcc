
#include "gmysqlcc_gui_all.h"


void gmysqlcc_gui_text_create_widget (p_gmysqlcc_gui_text gui_text);
void gmysqlcc_gui_text_init_widget (p_gmysqlcc_gui_text gui_text);
void gmysqlcc_gui_text_save_to_file (p_gmysqlcc_gui_text gui_text, const gchar * filename);
void gmysqlcc_gui_text_load_from_file (p_gmysqlcc_gui_text gui_text, const gchar * filename);

void gmysqlcc_gui_text_evt_btnTlbrLoad_clicked (GtkWidget *widget, gpointer user_data);
void gmysqlcc_gui_text_evt_btnTlbrSave_clicked (GtkWidget *widget, gpointer user_data);
void gmysqlcc_gui_text_evt_btnTlbrExecute_clicked (GtkWidget *widget, gpointer user_data);
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
	if (NbrWnd <= 1) { /* There is always the window server list */
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
	GtkToolItem * btnTlbrLoad, * btnTlbrSave, * btnTlbrExecute, * btnTlbrClose;
	GtkTooltips * tooltips;
#ifdef USE_GTKSOURCEVIEW
	GtkTextBuffer * txtBuffer;
#endif /* USE_GTKSOURCEVIEW */
	
	PangoFontDescription * pCourierFontDesc = NULL;
	
	tooltips = gtk_tooltips_new();
	
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

	imgToolbar = gtk_image_new_from_stock(GTK_STOCK_OPEN, GTK_ICON_SIZE_LARGE_TOOLBAR);
	gtk_widget_show(imgToolbar);
	btnTlbrLoad = gtk_tool_button_new (imgToolbar, _("Open"));
	gtk_tool_item_set_is_important (GTK_TOOL_ITEM(btnTlbrLoad), TRUE);
	g_signal_connect (G_OBJECT (btnTlbrLoad), "clicked", G_CALLBACK (gmysqlcc_gui_text_evt_btnTlbrLoad_clicked), gui_text);
	gtk_widget_show(GTK_WIDGET(btnTlbrLoad));
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar), GTK_TOOL_ITEM(btnTlbrLoad), -1);
	gtk_tool_item_set_tooltip (GTK_TOOL_ITEM(btnTlbrLoad), tooltips, _("Load sql file"), NULL);
	
	imgToolbar = gtk_image_new_from_stock(GTK_STOCK_SAVE, GTK_ICON_SIZE_LARGE_TOOLBAR);
	gtk_widget_show(imgToolbar);
	btnTlbrSave = gtk_tool_button_new (imgToolbar, _("Save"));
	gtk_tool_item_set_is_important (GTK_TOOL_ITEM(btnTlbrSave), TRUE);
	g_signal_connect (G_OBJECT (btnTlbrSave), "clicked", G_CALLBACK (gmysqlcc_gui_text_evt_btnTlbrSave_clicked), gui_text);
	gtk_widget_show(GTK_WIDGET(btnTlbrSave));
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar), GTK_TOOL_ITEM(btnTlbrSave), -1);
	gtk_tool_item_set_tooltip (GTK_TOOL_ITEM(btnTlbrSave), tooltips, _("Save sql file"), NULL);
	
	imgToolbar = gtk_image_new_from_stock(GTK_STOCK_EXECUTE, GTK_ICON_SIZE_LARGE_TOOLBAR);
	gtk_widget_show(imgToolbar);
	btnTlbrExecute = gtk_tool_button_new (imgToolbar, _("Execute"));
	gtk_tool_item_set_is_important (GTK_TOOL_ITEM(btnTlbrExecute), TRUE);
	g_signal_connect (G_OBJECT (btnTlbrExecute), "clicked", G_CALLBACK (gmysqlcc_gui_text_evt_btnTlbrExecute_clicked), gui_text);
	gtk_widget_show(GTK_WIDGET(btnTlbrExecute));
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar), GTK_TOOL_ITEM(btnTlbrExecute), -1);
	gtk_tool_item_set_tooltip (GTK_TOOL_ITEM(btnTlbrExecute), tooltips, _("Execute sql file"), NULL);
	
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
	pCourierFontDesc = pango_font_description_from_string(gmysqlcc_conf->pcQueryFontName);
	gtk_widget_modify_font(gui_text->txtContent, pCourierFontDesc);
	pango_font_description_free(pCourierFontDesc);
	gtk_widget_show (gui_text->txtContent);
	gtk_container_add (GTK_CONTAINER (scrolledwindow), gui_text->txtContent);

}

void gmysqlcc_gui_text_init_widget (p_gmysqlcc_gui_text gui_text) {
	/* Do nothing */
}


void gmysqlcc_gui_text_load_from_file (p_gmysqlcc_gui_text gui_text, const gchar * filename) {
	gchar * content = NULL;
	GtkTextBuffer * txtBuffer;
	GIOChannel * loadFile;
	GError * err = NULL;
	gsize nbBytes;
	
	g_return_if_fail (filename != NULL);
	
	g_free(gui_text->filename);
	gui_text->filename = g_strdup(filename);
	
	loadFile = g_io_channel_new_file(filename, "r", &err);
	g_io_channel_set_encoding(loadFile, "ISO-8859-15", &err);
	if (g_io_channel_read_to_end(loadFile, &content, &nbBytes, &err) != G_IO_STATUS_NORMAL) {
		content = NULL;
	}
	g_io_channel_unref(loadFile);
	
	txtBuffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(gui_text->txtContent));
	gtk_text_buffer_set_text(GTK_TEXT_BUFFER(txtBuffer), content, -1);
}

void gmysqlcc_gui_text_save_to_file (p_gmysqlcc_gui_text gui_text, const gchar * filename) {
	gchar * content = NULL;
	GtkTextBuffer * txtBuffer;
	GtkTextIter begin, end;
	GIOChannel * saveFile;
	GError * err = NULL;
	gsize nbBytes;
	
	g_return_if_fail (filename != NULL);
	
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

void gmysqlcc_gui_text_evt_btnTlbrLoad_clicked (GtkWidget *widget, gpointer user_data){
	p_gmysqlcc_gui_text gui_text = (p_gmysqlcc_gui_text)user_data;
	GtkWidget *chooser;
	gint response;
	
	chooser = gtk_file_chooser_dialog_new (_("Load sql file"), GTK_WINDOW(gui_text->window), GTK_FILE_CHOOSER_ACTION_OPEN,
		GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, GTK_STOCK_OPEN, GTK_RESPONSE_OK, NULL);
	
	gtk_file_chooser_set_filename (GTK_FILE_CHOOSER (chooser), gui_text->filename);
	
	response = gtk_dialog_run (GTK_DIALOG (chooser));
	if (response == GTK_RESPONSE_OK) {
		gchar *filename;
		
		filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (chooser));
		if (filename != NULL) {
			gmysqlcc_gui_text_load_from_file (gui_text, filename);
			g_free (filename);
		}
	}
	
	gtk_widget_destroy (chooser);
}

void gmysqlcc_gui_text_evt_btnTlbrSave_clicked (GtkWidget *widget, gpointer user_data) {
	p_gmysqlcc_gui_text gui_text = (p_gmysqlcc_gui_text)user_data;
	GtkWidget *chooser;
	gint response;
	
	chooser = gtk_file_chooser_dialog_new (_("Save sql file"), GTK_WINDOW(gui_text->window), GTK_FILE_CHOOSER_ACTION_SAVE,
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

void gmysqlcc_gui_text_evt_btnTlbrExecute_clicked (GtkWidget *widget, gpointer user_data){
	p_gmysqlcc_gui_text gui_text = (p_gmysqlcc_gui_text)user_data;
	p_gmysqlcc_gui_exec_file gui_xcfl;
	const gchar * content;
	GtkTextBuffer * txtBuffer;
	GtkTextIter begin, end;
	
	txtBuffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(gui_text->txtContent));
	gtk_text_buffer_get_start_iter (GTK_TEXT_BUFFER(txtBuffer), &begin);
	gtk_text_buffer_get_end_iter (GTK_TEXT_BUFFER(txtBuffer), &end);
	content = (gchar *)gtk_text_buffer_get_text (GTK_TEXT_BUFFER(txtBuffer), &begin, &end, FALSE);
	
	gui_xcfl = gmysqlcc_gui_exec_file_new();
	gmysqlcc_gui_exec_file_set_content(gui_xcfl, content);
	gmysqlcc_gui_exec_file_display(gui_xcfl, TRUE);
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

