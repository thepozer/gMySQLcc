#include <gtk/gtk.h>

#include <libintl.h>

#define __GMYSQL_MAIN_PART__

#include "../config.h"
#include "mysql_db_all.h"
#include "gmysql_conf.h"
#include "gmysql_utils.h"
#include "gmysql_gui.h"

int NbrWnd = 0;

p_gmysql_config gmysql_conf;
	
gboolean doBeforeEnd(gpointer data) {
	p_gmysql_config gmysql_conf = (p_gmysql_config)data;

	gmysql_config_write_xml(gmysql_conf);
	
	return TRUE;
}

int main(int argc, char *argv[]) {
	listServWnd * p_lstsvr;

	gtk_init (&argc, &argv);
	
  bindtextdomain (PACKAGE, PACKAGE_LOCALE_DIR);
  textdomain (PACKAGE);
	bind_textdomain_codeset(PACKAGE, "UTF-8");

	initFieldTypeCapabilities();
	
	gmysql_conf = gmysql_config_new();
	
	if (!gmysql_config_init(gmysql_conf)) {
		g_printerr("Can't start gmysqlcc - Error during configuration ... \n");
		return 1;
	}
	
	gtk_quit_add(0, doBeforeEnd, gmysql_conf);
	
	p_lstsvr = create_wndListServer(TRUE, gmysql_conf);
	
	gtk_main ();
	
	return 0;
}
