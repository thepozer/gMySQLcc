
#include "gmysqlcc_gui_all.h"
#include "gmysqlcc_gui_server.h"

void gmysqlcc_gui_server_create_widget_tab_procedure (p_gmysqlcc_gui_server gui_server, GtkWidget * notebook) {
	GtkWidget *label, *label_tmp;
	
	label_tmp = gtk_label_new (_("In construction"));
	gtk_widget_show (label_tmp);

	label = gtk_label_new (_("Procedures"));
	gtk_widget_show (label);
	gtk_label_set_justify (GTK_LABEL (label), GTK_JUSTIFY_LEFT);
	
	gtk_notebook_append_page(GTK_NOTEBOOK(notebook), label_tmp, label);
}

void gmysqlcc_gui_server_init_widget_tab_procedure (p_gmysqlcc_gui_server gui_server) {

}
