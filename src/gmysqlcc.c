#include <sys/stat.h>
#include <sys/types.h>

#include <gtk/gtk.h>

#include "mysql_db_all.h"
#include "gmysql_conf.h"
#include "gmysql_utils.h"
#include "gListServer.h"

#define CONF_DIR ".gmysqlcc"
#define XML_FILE "gmysqlcc_serv.xml"

int NbrWnd = 0;

gboolean doBeforeEnd(gpointer data) {
	p_gmysql_config gmsql_conf = (p_gmysql_config)data;
	GString * configFilePath;

	configFilePath = g_string_new("");
	
	g_string_printf(configFilePath, "%s/%s", g_get_home_dir(), CONF_DIR);
	if (!g_file_test(configFilePath->str, (G_FILE_TEST_IS_DIR | G_FILE_TEST_EXISTS))) {
		mkdir (configFilePath->str, 0700);
	}
	
	g_string_printf(configFilePath, "%s/%s/%s", g_get_home_dir(), CONF_DIR, XML_FILE);
	gmysql_config_write_xml(gmsql_conf, configFilePath->str);
	g_string_free(configFilePath, TRUE);

	return TRUE;
}

int main(int argc, char *argv[]) {
  p_gmysql_config gmsql_conf;
	listServWnd * p_lstsvr;
	GString * configFilePath;

	gtk_init (&argc, &argv);
	
	initFieldTypeCapabilities();
	
	g_printerr("Client librarie version : %s\n", mysql_get_client_info());

	configFilePath = g_string_new("");
	gmsql_conf = gmysql_config_new();

	g_string_printf(configFilePath, "%s/%s/%s", g_get_home_dir(), CONF_DIR, XML_FILE);
	gmysql_config_read_xml(gmsql_conf, configFilePath->str);
	g_string_free(configFilePath, TRUE);
	
	gtk_quit_add(0, doBeforeEnd, gmsql_conf);
	
	p_lstsvr = create_wndListServer(TRUE, gmsql_conf);
	
	gtk_main ();
	
	return 0;
}
