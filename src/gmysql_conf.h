#ifndef __GMYSQL_CONF_H__
#define __GMYSQL_CONF_H__

#include <glib.h>
#include <stdio.h>
#include <errno.h>

#include "mysql_db_all.h"

typedef struct _gmysql_config {
	GList * lstServers;
} gmysql_config;

typedef gmysql_config * p_gmysql_config;

p_gmysql_config gmysql_config_new ();
gboolean gmysql_config_delete (p_gmysql_config gmsql_conf);

gboolean gmysql_config_add_server (p_gmysql_config gmsql_conf, gchar * name, gchar * host, gint port, gchar * login, gchar * passwd, gchar * dbAllowedList);
gboolean gmysql_config_update_server (p_gmysql_config gmsql_conf, const gchar * oldname, gchar * name, gchar * host, gint port, gchar * login, gchar * passwd, gchar * dbAllowedList);
gboolean gmysql_config_del_server (p_gmysql_config gmsql_conf, const gchar * name);
p_mysql_server gmysql_config_get_server (p_gmysql_config gmsql_conf, const gchar * name);

gboolean gmysql_config_read_xml (p_gmysql_config gmsql_conf, const gchar * filename);
gboolean gmysql_config_write_xml (p_gmysql_config gmsql_conf, const gchar * filename);


#endif /* __GMYSQL_CONF_H__ */
