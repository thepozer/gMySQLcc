#include <gtk/gtk.h>

#include <libintl.h>

#define __GMYSQLCC_MAIN_PART__

#include "../config.h"
#include "mysql_db_all.h"
#include "gmysqlcc_helpers.h"
#include "gmysqlcc_gui_all.h"

int NbrWnd = 0;

p_gmysqlcc_config gmysqlcc_conf;
	
#ifdef USE_GTKSOURCEVIEW
GtkSourceLanguagesManager * LangManager;
#endif /* USE_GTKSOURCEVIEW */

gboolean gmysqlcc_main_before_end(gpointer data);

int main(int argc, char *argv[]) {
	p_gmysqlcc_gui_list_server gui_list_server;
	
	/* Init gtk library */
	gtk_init (&argc, &argv);
	
	/* Init gettext library */
  bindtextdomain (PACKAGE, PACKAGE_LOCALE_DIR);
  textdomain (PACKAGE);
	bind_textdomain_codeset(PACKAGE, "UTF-8");
	
	/* Init types list */
	gmysqlcc_helpers_field_capability_init_array();
	
	/* Read configuration */
	gmysqlcc_conf = gmysqlcc_config_new();
	
	if (!gmysqlcc_config_read(gmysqlcc_conf)) {
		g_printerr("Can't start gmysqlcc - Error during configuration ... \n");
		return 1;
	}
	
	/* Add trigger to save configuration before the end of gmysqlcc */
	gtk_quit_add(0, gmysqlcc_main_before_end, gmysqlcc_conf);
	
#ifdef USE_GTKSOURCEVIEW
	/* Get Language Manager to select sql type */
	LangManager = gtk_source_languages_manager_new();
#endif /* USE_GTKSOURCEVIEW */
	
	/* Create server list windows */
	gui_list_server = gmysqlcc_gui_list_server_new(gmysqlcc_conf);
	gmysqlcc_gui_list_server_display(gui_list_server, TRUE);
	
	/* Start the application */
	gtk_main ();
	
	return 0;
}

gboolean gmysqlcc_main_before_end(gpointer data) {
	p_gmysqlcc_config gmysqlcc_conf = (p_gmysqlcc_config)data;

	gmysqlcc_config_write(gmysqlcc_conf);
	
	return TRUE;
}
