
#include "gmysql_gui.h"

static void saveContent (GtkWidget *widget, gpointer user_data);

static void btntlbrsave_clicked (GtkWidget *widget, gpointer user_data) {
	p_textWnd pTxtWnd = (p_textWnd)user_data;
	
	askFilename(_("Save Text"), pTxtWnd->filename, saveContent, user_data);
}

static void saveContent (GtkWidget *widget, gpointer user_data) {
	p_askFilenameInfos paskFnInf = (p_askFilenameInfos)user_data;
	p_textWnd pTxtWnd = (p_textWnd)paskFnInf->userData;
	gchar * filename = (gchar *)NULL;
	gchar * content = (gchar *)NULL;
	
	GtkTextBuffer * txtBuffer;
	GtkTextIter begin, end;
	
	GIOChannel * saveFile;
	GError * err = (GError *)NULL;
	gssize nbBytes;
	
	txtBuffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(pTxtWnd->txtContent));
	gtk_text_buffer_get_start_iter (GTK_TEXT_BUFFER(txtBuffer), &begin);
	gtk_text_buffer_get_end_iter (GTK_TEXT_BUFFER(txtBuffer), &end);
	content = (gchar *)gtk_text_buffer_get_text (GTK_TEXT_BUFFER(txtBuffer), &begin, &end, FALSE);

	filename = (gchar *)gtk_file_selection_get_filename(GTK_FILE_SELECTION (paskFnInf->dialog));
	
	saveFile = g_io_channel_new_file(filename, "w", &err);
	g_io_channel_set_encoding(saveFile, FALSE, &err);
	g_io_channel_write_chars(saveFile, content, -1, &nbBytes, &err);
	g_io_channel_unref(saveFile);
}

static void btntlbrclose_clicked (GtkWidget *widget, gpointer user_data) {
	p_textWnd pTxtWnd = (p_textWnd)user_data;
	
	gtk_widget_destroy(GTK_WIDGET(pTxtWnd->wndText));
}

static void destroy(GtkWidget *widget, gpointer user_data) {
	p_textWnd pTxtWnd = (p_textWnd)user_data;
	
	/* Destroy GUI */
	gtk_widget_hide (GTK_WIDGET(pTxtWnd->wndText));
	gtk_widget_destroy (GTK_WIDGET(pTxtWnd->wndText));
	
	/* Destroy Datas */
	g_free(pTxtWnd->filename);
	g_free(pTxtWnd);
	
	/* Destroy Application if needed */
	NbrWnd--;
	g_printerr("Destruction Server window - nbrWnd : %d\n", NbrWnd);
	if (NbrWnd <= 0) {
		g_printerr("Destroy App\n");
		gtk_main_quit();
	}
}

p_textWnd create_wndText (gboolean display, gchar * content, gchar * filename){
  p_textWnd p_txt;
	
  GtkWidget *vbox;
  GtkWidget *toolbar;
  GtkWidget *scrolledwindow;
 	GtkWidget * imgToolbar;
  GtkToolItem * btnTlbrClose, * btnTlbrSave;
	GtkTooltips * tooltips;
	GtkTextBuffer * txtBuffer;
	GString * sTitle;

 	/* Init structure */
	p_txt = (p_textWnd) g_try_malloc(sizeof(textWnd));
	
	if (p_txt == (p_textWnd)NULL) {
		return (p_textWnd)NULL; /* return NULL pointer */
	}
	
	p_txt->filename = g_strdup(filename);
	
	tooltips = gtk_tooltips_new();
	
	p_txt->wndText = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	sTitle = g_string_new("");
	g_string_printf(sTitle, _("Text Window - File name : %s"), p_txt->filename);
  gtk_window_set_title (GTK_WINDOW (p_txt->wndText), sTitle->str);
	g_string_free(sTitle, TRUE);
 	gtk_window_set_default_size (GTK_WINDOW (p_txt->wndText), 400, 400);

  vbox = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox);
  gtk_container_add (GTK_CONTAINER (p_txt->wndText), vbox);
	g_signal_connect (G_OBJECT (p_txt->wndText), "destroy", G_CALLBACK (destroy), p_txt);

  toolbar = gtk_toolbar_new ();
  gtk_widget_show (toolbar);
  gtk_box_pack_start (GTK_BOX (vbox), toolbar, FALSE, FALSE, 0);
  gtk_toolbar_set_style (GTK_TOOLBAR (toolbar), GTK_TOOLBAR_BOTH_HORIZ);

	imgToolbar = gtk_image_new_from_stock(GTK_STOCK_SAVE, GTK_ICON_SIZE_LARGE_TOOLBAR);
	gtk_widget_show(imgToolbar);
	btnTlbrSave = gtk_tool_button_new (imgToolbar, _("Save"));
	gtk_tool_item_set_is_important (GTK_TOOL_ITEM(btnTlbrSave), TRUE);
	g_signal_connect (G_OBJECT (btnTlbrSave), "clicked", G_CALLBACK (btntlbrsave_clicked), p_txt);
	gtk_widget_show(GTK_WIDGET(btnTlbrSave));
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar), GTK_TOOL_ITEM(btnTlbrSave), -1);
	gtk_tool_item_set_tooltip (GTK_TOOL_ITEM(btnTlbrSave), tooltips, _("Save text"), NULL);
	
	imgToolbar = gtk_image_new_from_stock(GTK_STOCK_CLOSE, GTK_ICON_SIZE_LARGE_TOOLBAR);
	gtk_widget_show(imgToolbar);
	btnTlbrClose = gtk_tool_button_new (imgToolbar, _("Close"));
	gtk_tool_item_set_is_important (GTK_TOOL_ITEM(btnTlbrClose), TRUE);
	g_signal_connect (G_OBJECT (btnTlbrClose), "clicked", G_CALLBACK (btntlbrclose_clicked), p_txt);
	gtk_widget_show(GTK_WIDGET(btnTlbrClose));
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar), GTK_TOOL_ITEM(btnTlbrClose), -1);
	gtk_tool_item_set_tooltip (GTK_TOOL_ITEM(btnTlbrClose), tooltips, _("Close window"), NULL);
	
  scrolledwindow = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (scrolledwindow);
  gtk_box_pack_start (GTK_BOX (vbox), scrolledwindow, TRUE, TRUE, 0);

  p_txt->txtContent = gtk_text_view_new ();
  gtk_widget_show (p_txt->txtContent);
  gtk_container_add (GTK_CONTAINER (scrolledwindow), p_txt->txtContent);
  /*gtk_text_view_set_wrap_mode (GTK_TEXT_VIEW (p_txt->txtContent), GTK_WRAP_WORD);*/

	if (content != (gchar *) NULL) {
		txtBuffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(p_txt->txtContent));
		gtk_text_buffer_set_text(GTK_TEXT_BUFFER(txtBuffer), content, -1);
	}
	
	if (display) {
		gtk_widget_show (p_txt->wndText);
		NbrWnd ++;	
	}

  return p_txt;
}
