#ifndef __GMYSQL_CONF_H__
#define __GMYSQL_CONF_H__

#include <glib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "mysql_db_all.h"
/*#include "gmysql_history.h"*/

#define CONF_DIR ".gmysqlcc"
#define XML_FILE "gmysqlcc_serv.xml"
#define QUERY_HISTORY_FILE "gmysqlcc_history"

typedef struct _s_gmysql_config {
	gchar * configFileName;
	GList * lstServers;
	/*p_gmysql_history queryHistory;*/
	void * queryHistory;
} s_gmysql_config;

typedef s_gmysql_config * p_gmysql_config;

p_gmysql_config gmysql_config_new ();
gboolean gmysql_config_delete (p_gmysql_config gmsql_conf);

gboolean gmysql_config_init (p_gmysql_config gmysql_conf);

gboolean gmysql_config_add_server (p_gmysql_config gmsql_conf, gchar * name, gchar * host, gint port, gchar * login, gchar * passwd, gchar * dbAllowedList, gchar * localSock);
gboolean gmysql_config_update_server (p_gmysql_config gmsql_conf, const gchar * oldname, gchar * name, gchar * host, gint port, gchar * login, gchar * passwd, gchar * dbAllowedList, gchar * localSock);
gboolean gmysql_config_del_server (p_gmysql_config gmsql_conf, const gchar * name);
p_mysql_server gmysql_config_get_server (p_gmysql_config gmsql_conf, const gchar * name);

gboolean gmysql_config_read_xml (p_gmysql_config gmsql_conf);
gboolean gmysql_config_write_xml (p_gmysql_config gmsql_conf);


#endif /* __GMYSQL_CONF_H__ */
