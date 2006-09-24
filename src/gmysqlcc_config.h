#ifndef __GMYSQL_CONF_H__
#define __GMYSQL_CONF_H__

#include <glib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "mysql_db_all.h"

#define CONF_DIR ".config"
#define GMYSQLCC_CONF_DIR "gmysqlcc"

#define SERVERS_FILE "gmysqlcc_serv.xml"
#define CONFIG_FILE "gmysqlcc_conf.xml"

typedef struct _s_gmysqlcc_config {
	gchar * config_filename;
	gchar * servers_filename;
	
	gchar * pcQueryFontName;
	gchar * pcHelpFontName;
	
	GList * lst_servers;
} s_gmysqlcc_config;

typedef s_gmysqlcc_config * p_gmysqlcc_config;

p_gmysqlcc_config gmysqlcc_config_new ();
gboolean gmysqlcc_config_delete (p_gmysqlcc_config gmysqlcc_conf);

gboolean gmysqlcc_config_init (p_gmysqlcc_config gmysqlcc_conf);

gboolean gmysqlcc_config_add_server (p_gmysqlcc_config gmysqlcc_conf, const gchar * name, const gchar * host, const gint port, const gchar * login, const gchar * passwd, const gchar * dbAllowedList, const gchar * localSock, const gboolean read_only, const gboolean write_warning);
gboolean gmysqlcc_config_update_server (p_gmysqlcc_config gmysqlcc_conf, const gchar * oldname, const gchar * name, const gchar * host, const gint port, const gchar * login, const gchar * passwd, const gchar * dbAllowedList, const gchar * localSock, const gboolean read_only, const gboolean write_warning);
gboolean gmysqlcc_config_del_server (p_gmysqlcc_config gmysqlcc_conf, const gchar * name);
p_mysql_server gmysqlcc_config_get_server (p_gmysqlcc_config gmysqlcc_conf, const gchar * name);

gboolean gmysqlcc_config_read (p_gmysqlcc_config gmysqlcc_conf);
gboolean gmysqlcc_config_write (p_gmysqlcc_config gmysqlcc_conf);


#endif /* __GMYSQL_CONF_H__ */
