
#include "gmysqlcc_config.h"

struct _s_xml_servers_read_state {
	enum {
			X_OUT_ROOT = 0,
			X_IN_ROOT = 1,
			X_IN_SERVER = 2,
			X_IN_NAME = 3,
			X_IN_HOST = 4,
			X_IN_PORT = 5,
			X_IN_LOGIN = 6,
			X_IN_PASSWD = 7,
			X_IN_ALLOWED_DBS = 8,
			X_IN_LOCAL_SOCK = 9,
			X_IN_OPTIONS = 10,
			X_IN_READ_ONLY = 11,
			X_IN_WRITE_WARNING = 12
		} state;
	GString * name;
	GString * host;
	gint port;
	GString * login;
	GString * passwd;
	GString * allowedDbs;
	GString * localSock;
	gboolean read_only;
	gboolean write_warning;
} xml_servers_read_state;

char * default_servers_conf = "<?xml version=\"1.0\"?>"
"<servers>"
	"<server>"
		"<name><![CDATA[Localhost]]></name>"
		"<host><![CDATA[localhost]]></host>"
		"<port>3306</port>"
		"<login><![CDATA[root]]></login>"
		"<passwd><![CDATA[]]></passwd>"
		"<alloweddbs><![CDATA[]]></alloweddbs>"
		"<localsock><![CDATA[/var/run/mysqld/mysqld.sock]]></localsock>"
		"<options>"
			"<read_only/>"
			"<write_warning/>"
		"</options>"
	"</server>"
"</servers>";

gboolean gmysqlcc_config_read_servers_file (p_gmysqlcc_config gmysqlcc_conf);
gboolean gmysqlcc_config_write_servers_file (p_gmysqlcc_config gmysqlcc_conf);
void xml_servers_conf_start (GMarkupParseContext *context, const gchar * element_name, const gchar ** attribute_names, const gchar ** attribute_values, gpointer user_data, GError ** error);
void xml_servers_conf_end (GMarkupParseContext *context, const gchar * element_name, gpointer user_data, GError ** error);
void xml_servers_conf_text (GMarkupParseContext *context, const gchar * text, gsize text_len, gpointer user_data, GError ** error);
void xml_servers_conf_passthrough (GMarkupParseContext *context, const gchar * passthrough_text, gsize text_len, gpointer user_data, GError **error);


/* 
	gmysqlcc_config functions 
*/

p_gmysqlcc_config gmysqlcc_config_new () {
	p_gmysqlcc_config gmysqlcc_conf;
	GString * filePath;

	gmysqlcc_conf = (p_gmysqlcc_config) g_try_malloc(sizeof(s_gmysqlcc_config));
	
	if (gmysqlcc_conf == NULL) {
		return NULL; /* return NULL pointer */
	}
	
	gmysqlcc_conf->servers_filename = NULL;
	gmysqlcc_conf->config_filename = NULL;
	gmysqlcc_conf->lst_servers = NULL;
	
	g_string_free(filePath, TRUE);
	
	return gmysqlcc_conf;
}

gboolean gmysqlcc_config_read (p_gmysqlcc_config gmysqlcc_conf) {
	GString * filePath;

	if (gmysqlcc_conf == NULL) {
		return FALSE;
	}
	
	filePath = g_string_new("");
	
	/* Check new config file */
	gmysqlcc_conf->config_filename = NULL;
	
	g_string_printf(filePath, "%s/%s/%s/%s", g_get_home_dir(), CONF_DIR, GMYSQLCC_CONF_DIR, CONFIG_FILE);
	if (g_file_test(filePath->str, (G_FILE_TEST_EXISTS | G_FILE_TEST_IS_REGULAR))) {
		gmysqlcc_conf->config_filename = g_strdup(filePath->str);
	} else {
		g_string_printf(filePath, "%s/.%s/%s", g_get_home_dir(), GMYSQLCC_CONF_DIR, CONFIG_FILE);
		if (g_file_test(filePath->str, (G_FILE_TEST_EXISTS | G_FILE_TEST_IS_REGULAR))) {
			gmysqlcc_conf->config_filename = g_strdup(filePath->str);
		}
	}
	
	/*gmysqlcc_config_read_config_file(gmysqlcc_conf);*/
	
	/* Check new servers file */
	gmysqlcc_conf->servers_filename = NULL;
	
	g_string_printf(filePath, "%s/%s/%s/%s", g_get_home_dir(), CONF_DIR, GMYSQLCC_CONF_DIR, SERVERS_FILE);
	if (g_file_test(filePath->str, (G_FILE_TEST_EXISTS | G_FILE_TEST_IS_REGULAR))) {
		gmysqlcc_conf->servers_filename = g_strdup(filePath->str);
	} else {
		g_string_printf(filePath, "%s/.%s/%s", g_get_home_dir(), GMYSQLCC_CONF_DIR, SERVERS_FILE);
		if (g_file_test(filePath->str, (G_FILE_TEST_EXISTS | G_FILE_TEST_IS_REGULAR))) {
			gmysqlcc_conf->servers_filename = g_strdup(filePath->str);
		}
	}
	
	gmysqlcc_config_read_servers_file(gmysqlcc_conf);
	
	
	g_string_free(filePath, TRUE);
	
	return TRUE;
}

gboolean gmysqlcc_config_write (p_gmysqlcc_config gmysqlcc_conf) {
	GString * filePath;
	
	if (gmysqlcc_conf == NULL) {
		return FALSE;
	}
	
	filePath = g_string_new("");
	
	/* Check config directories and create them if needed */
	g_string_printf(filePath, "%s/%s", g_get_home_dir(), CONF_DIR);
	if (!g_file_test(filePath->str, (G_FILE_TEST_EXISTS | G_FILE_TEST_IS_DIR))) {
		mkdir (filePath->str, 0700);
	}
	
	g_string_printf(filePath, "%s/%s/%s", g_get_home_dir(), CONF_DIR, GMYSQLCC_CONF_DIR);
	if (!g_file_test(filePath->str, (G_FILE_TEST_EXISTS | G_FILE_TEST_IS_DIR))) {
		mkdir (filePath->str, 0700);
	}
	
	/* Check new config file */
	gmysqlcc_conf->config_filename = NULL;
	
	g_string_printf(filePath, "%s/%s/%s/%s", g_get_home_dir(), CONF_DIR, GMYSQLCC_CONF_DIR, CONFIG_FILE);
	gmysqlcc_conf->config_filename = g_strdup(filePath->str);
	
	/*gmysqlcc_config_write_config_file(gmysqlcc_conf);*/
	
	/* Check new servers file */
	gmysqlcc_conf->servers_filename = NULL;
	
	g_string_printf(filePath, "%s/%s/%s/%s", g_get_home_dir(), CONF_DIR, GMYSQLCC_CONF_DIR, SERVERS_FILE);
	gmysqlcc_conf->servers_filename = g_strdup(filePath->str);
	
	gmysqlcc_config_write_servers_file(gmysqlcc_conf);
	
	
	g_string_free(filePath, TRUE);
	
	return TRUE;
}

gboolean gmysqlcc_config_delete (p_gmysqlcc_config gmysqlcc_conf) {
	GList * lstServersIdx;
	
	if (gmysqlcc_conf != NULL) {
		if (gmysqlcc_conf->lst_servers != NULL) {
			lstServersIdx = g_list_first(gmysqlcc_conf->lst_servers);
			while (lstServersIdx != NULL) {
				mysql_server_delete((p_mysql_server)lstServersIdx->data); 
				lstServersIdx = g_list_next(lstServersIdx);
			}
		}
		
		g_list_free(gmysqlcc_conf->lst_servers);
		
		g_free(gmysqlcc_conf->servers_filename);
		g_free(gmysqlcc_conf->config_filename);
		g_free(gmysqlcc_conf);
	}
	
	return TRUE;
}

gboolean gmysqlcc_config_add_server (p_gmysqlcc_config gmysqlcc_conf, const gchar * name, const gchar * host, const gint port, const gchar * login, const gchar * passwd, const gchar * dbAllowedList, const gchar * localSock, const gboolean read_only, const gboolean write_warning) {
	p_mysql_server mysql_srv;
	
	mysql_srv = gmysqlcc_config_get_server(gmysqlcc_conf, name);
	
	if (mysql_srv != NULL) {
		return FALSE;
	}
	
	mysql_srv = mysql_server_new();
	if (mysql_srv == NULL) {
		return FALSE;
	}
	
	mysql_srv->name = g_strdup(name);
	mysql_srv->host = g_strdup(host);
	mysql_srv->port = port;
	mysql_srv->user = g_strdup(login);
	mysql_srv->passwd = g_strdup(passwd);
	mysql_srv->allowedDbs = g_strdup(dbAllowedList);
	mysql_srv->localSock= g_strdup(localSock);
	
	mysql_srv->read_only = read_only;
	mysql_srv->write_warning = write_warning;
	
	gmysqlcc_conf->lst_servers = g_list_append(gmysqlcc_conf->lst_servers, mysql_srv);
	
	return TRUE;
}

gboolean gmysqlcc_config_update_server (p_gmysqlcc_config gmysqlcc_conf, const gchar * oldname, const gchar * name, const gchar * host, const gint port, const gchar * login, const gchar * passwd, const gchar * dbAllowedList, const gchar * localSock, const gboolean read_only, const gboolean write_warning) {
	p_mysql_server mysql_srv;
	
	mysql_srv = gmysqlcc_config_get_server(gmysqlcc_conf, oldname);
	
	if (mysql_srv != NULL) {

		/* Free old strings */
		g_free(mysql_srv->name);
		g_free(mysql_srv->host);
		g_free(mysql_srv->user);
		g_free(mysql_srv->passwd);
		g_free(mysql_srv->allowedDbs);
		g_free(mysql_srv->localSock);

		/* Add new ones */
		mysql_srv->name = g_strdup(name);
		mysql_srv->host = g_strdup(host);
		mysql_srv->port = port;
		mysql_srv->user = g_strdup(login);
		mysql_srv->passwd = g_strdup(passwd);
		mysql_srv->allowedDbs = g_strdup(passwd);
		mysql_srv->localSock= g_strdup(localSock);
		
		mysql_srv->read_only = read_only;
		mysql_srv->write_warning = write_warning;
		
		return TRUE;
	}
	
	return FALSE;
}

gboolean gmysqlcc_config_del_server (p_gmysqlcc_config gmysqlcc_conf, const gchar * name) {
	p_mysql_server mysql_srv;
	GList * lstServersIdx;
	
	lstServersIdx = g_list_first(gmysqlcc_conf->lst_servers);
	
	while (lstServersIdx != NULL) {
		mysql_srv = (p_mysql_server) lstServersIdx->data;
		if (g_ascii_strcasecmp(mysql_srv->name, name) == 0) {
			mysql_server_delete(mysql_srv);
			gmysqlcc_conf->lst_servers = g_list_delete_link(gmysqlcc_conf->lst_servers, lstServersIdx);
			return TRUE;
		}
		lstServersIdx = g_list_next(lstServersIdx);
	}
	
	return FALSE;
}

p_mysql_server gmysqlcc_config_get_server (p_gmysqlcc_config gmysqlcc_conf, const gchar * name) {
	p_mysql_server mysql_srv;
	GList * lstServersIdx;
	
	lstServersIdx = g_list_first(gmysqlcc_conf->lst_servers);
	
	while (lstServersIdx != NULL) {
		mysql_srv = (p_mysql_server)lstServersIdx->data;
		if (g_ascii_strcasecmp(mysql_srv->name, name) == 0) {
			return mysql_srv;
		}
		lstServersIdx = g_list_next(lstServersIdx);
	}
	
	return NULL;
}

/* xml read structures and datas */

/* xml read functions */

gboolean gmysqlcc_config_read_servers_file (p_gmysqlcc_config gmysqlcc_conf) {
	GMarkupParser xmlParse;
	GMarkupParseContext * xmlContext;
	GError * gerr = NULL;
	FILE * xmlFile;
	gchar sbuf[257];
	int szRead;
	
	if (gmysqlcc_conf == NULL) {
		return FALSE; 
	}
	
	/* Init XMl parser Struct */
	xmlParse.start_element = xml_servers_conf_start;
	xmlParse.end_element = xml_servers_conf_end;
	xmlParse.text = xml_servers_conf_text;
	xmlParse.passthrough = xml_servers_conf_passthrough;
	xmlParse.error = NULL;
	
	/* Init XML read status struct */
	xml_servers_read_state.state = X_OUT_ROOT;
	xml_servers_read_state.name = g_string_sized_new(16);
	xml_servers_read_state.host = g_string_sized_new(16);
	xml_servers_read_state.port = 0;
	xml_servers_read_state.login = g_string_sized_new(16);
	xml_servers_read_state.passwd = g_string_sized_new(16);
	xml_servers_read_state.allowedDbs = g_string_sized_new(48);
	xml_servers_read_state.localSock = g_string_sized_new(48);

	/* Create XML Parser */
	xmlContext = g_markup_parse_context_new(&xmlParse, 0, (gpointer)gmysqlcc_conf, (GDestroyNotify)NULL);
	
	if (gmysqlcc_conf->servers_filename != NULL) {
		xmlFile = fopen(gmysqlcc_conf->servers_filename, "r");
	} else {
		xmlFile = NULL;
		errno = ENOENT;
	}
	
	if (xmlFile == (FILE *)NULL) { /* Not found conf file -> use default conf */
		g_printerr("Error file open : %s. -> read default configuration\n", strerror(errno));
		g_markup_parse_context_parse(xmlContext, default_servers_conf, strlen(default_servers_conf), &gerr);
		g_markup_parse_context_end_parse (xmlContext, &gerr);
	} else { /* Read user configuration */
		do {
			szRead = fread(sbuf, 1, 256, xmlFile);
			if (!g_markup_parse_context_parse(xmlContext, sbuf, szRead, &gerr)) {
				szRead = -1;
			}
		} while (szRead > 0);
		if (szRead > -1) {
			g_markup_parse_context_end_parse (xmlContext, &gerr);
		}
		fclose(xmlFile);
	}
	
	g_markup_parse_context_free(xmlContext);

	return TRUE;
}

gboolean gmysqlcc_config_write_servers_file (p_gmysqlcc_config gmysqlcc_conf) {
	GString * xmlContent;
	GList * lstServersIdx;
	p_mysql_server mysql_srv;
	FILE * xmlFile;
	
	if (gmysqlcc_conf == NULL) {
		return FALSE; 
	}
	
	if (gmysqlcc_conf->servers_filename == NULL) {
		return FALSE;
	}
	
	xmlContent = g_string_new("<?xml version=\"1.0\" ?>\n<servers>\n");
	
	lstServersIdx = g_list_first(gmysqlcc_conf->lst_servers);
	
	while (lstServersIdx != NULL) {
		mysql_srv = (p_mysql_server) lstServersIdx->data;
		g_string_append(xmlContent, "\t<server>\n");
		g_string_append_printf(xmlContent, "\t\t<name><![CDATA[%s]]></name>\n", mysql_srv->name);
		g_string_append_printf(xmlContent, "\t\t<host><![CDATA[%s]]></host>\n", mysql_srv->host);
		g_string_append_printf(xmlContent, "\t\t<port>%d</port>\n", mysql_srv->port);
		g_string_append_printf(xmlContent, "\t\t<login><![CDATA[%s]]></login>\n", mysql_srv->user);
		g_string_append_printf(xmlContent, "\t\t<passwd><![CDATA[%s]]></passwd>\n", mysql_srv->passwd);
		if (mysql_srv->allowedDbs != NULL) {
			g_string_append_printf(xmlContent, "\t\t<alloweddbs><![CDATA[%s]]></alloweddbs>\n", mysql_srv->allowedDbs);
		} else {
			g_string_append_printf(xmlContent, "\t\t<alloweddbs><![CDATA[]]></alloweddbs>\n");
		}
		if (mysql_srv->localSock != NULL) {
			g_string_append_printf(xmlContent, "\t\t<localsock><![CDATA[%s]]></localsock>\n", mysql_srv->localSock);
		} else {
			g_string_append_printf(xmlContent, "\t\t<localsock><![CDATA[]]></localsock>\n");
		}
		g_string_append(xmlContent, "\t\t<options>\n");
		if (mysql_srv->read_only) {
			g_string_append_printf(xmlContent, "\t\t\t<read_only/>\n");
		}
		if (mysql_srv->write_warning) {
			g_string_append_printf(xmlContent, "\t\t\t<write_warning/>\n");
		}
		
		g_string_append(xmlContent, "\t\t</options>\n\t</server>\n");
		lstServersIdx = g_list_next(lstServersIdx);
	}

	g_string_append(xmlContent, "</servers>\n");
	
	xmlFile = fopen(gmysqlcc_conf->servers_filename, "w");
	if (xmlFile == NULL) {
		g_printerr("Error file open : %s.\n", strerror(errno));
		return FALSE;
	}
	fwrite(xmlContent->str, 1, xmlContent->len, xmlFile);
	fclose(xmlFile);
	
	g_string_free(xmlContent, TRUE);
	
	return TRUE;
}



void xml_servers_conf_start (GMarkupParseContext *context, const gchar * element_name, const gchar ** attribute_names, const gchar ** attribute_values, gpointer user_data, GError ** error) {
	if (g_ascii_strcasecmp(element_name, "servers") == 0 && xml_servers_read_state.state == X_OUT_ROOT) {
		xml_servers_read_state.state = X_IN_ROOT;
	} else if (g_ascii_strcasecmp(element_name, "server") == 0 && xml_servers_read_state.state == X_IN_ROOT) {
		g_string_erase(xml_servers_read_state.name, 0, -1);
		g_string_erase(xml_servers_read_state.host, 0, -1);
		g_string_erase(xml_servers_read_state.login, 0, -1);
		g_string_erase(xml_servers_read_state.passwd, 0, -1);
		g_string_erase(xml_servers_read_state.allowedDbs, 0, -1);
		g_string_erase(xml_servers_read_state.localSock, 0, -1);
		xml_servers_read_state.port = 0;
		xml_servers_read_state.read_only = FALSE;
		xml_servers_read_state.write_warning = FALSE;
		xml_servers_read_state.state = X_IN_SERVER;
	} else if (g_ascii_strcasecmp(element_name, "name") == 0 && xml_servers_read_state.state == X_IN_SERVER) {
		xml_servers_read_state.state = X_IN_NAME;
	} else if (g_ascii_strcasecmp(element_name, "host") == 0 && xml_servers_read_state.state == X_IN_SERVER) {
		xml_servers_read_state.state = X_IN_HOST;
	} else if (g_ascii_strcasecmp(element_name, "port") == 0 && xml_servers_read_state.state == X_IN_SERVER) {
		xml_servers_read_state.state = X_IN_PORT;
	} else if (g_ascii_strcasecmp(element_name, "login") == 0 && xml_servers_read_state.state == X_IN_SERVER) {
		xml_servers_read_state.state = X_IN_LOGIN;
	} else if (g_ascii_strcasecmp(element_name, "passwd") == 0 && xml_servers_read_state.state == X_IN_SERVER) {
		xml_servers_read_state.state = X_IN_PASSWD;
	} else if (g_ascii_strcasecmp(element_name, "alloweddbs") == 0 && xml_servers_read_state.state == X_IN_SERVER) {
		xml_servers_read_state.state = X_IN_ALLOWED_DBS;
	} else if (g_ascii_strcasecmp(element_name, "localsock") == 0 && xml_servers_read_state.state == X_IN_SERVER) {
		xml_servers_read_state.state = X_IN_LOCAL_SOCK;
	} else if (g_ascii_strcasecmp(element_name, "options") == 0 && xml_servers_read_state.state == X_IN_SERVER) {
		xml_servers_read_state.state = X_IN_OPTIONS;
	} else if (g_ascii_strcasecmp(element_name, "read_only") == 0 && xml_servers_read_state.state == X_IN_OPTIONS) {
		/* Nothing to do */
	} else if (g_ascii_strcasecmp(element_name, "write_warning") == 0 && xml_servers_read_state.state == X_IN_OPTIONS) {
		/* Nothing to do */
	}
}

void xml_servers_conf_end (GMarkupParseContext *context, const gchar * element_name, gpointer user_data, GError ** error) {
	p_gmysqlcc_config gmysqlcc_conf = (p_gmysqlcc_config)user_data;

	if (g_ascii_strcasecmp(element_name, "servers") == 0 && xml_servers_read_state.state == X_IN_ROOT) {
		xml_servers_read_state.state = X_OUT_ROOT;
	} else if (g_ascii_strcasecmp(element_name, "server") == 0 && xml_servers_read_state.state == X_IN_SERVER) {
		gmysqlcc_config_add_server(gmysqlcc_conf, xml_servers_read_state.name->str, xml_servers_read_state.host->str, xml_servers_read_state.port, xml_servers_read_state.login->str, xml_servers_read_state.passwd->str, xml_servers_read_state.allowedDbs->str, xml_servers_read_state.localSock->str, xml_servers_read_state.read_only, xml_servers_read_state.write_warning);
		xml_servers_read_state.state = X_IN_ROOT;
	} else if (g_ascii_strcasecmp(element_name, "name") == 0 && xml_servers_read_state.state == X_IN_NAME) {
		xml_servers_read_state.state = X_IN_SERVER;
	} else if (g_ascii_strcasecmp(element_name, "host") == 0 && xml_servers_read_state.state == X_IN_HOST) {
		xml_servers_read_state.state = X_IN_SERVER;
	} else if (g_ascii_strcasecmp(element_name, "port") == 0 && xml_servers_read_state.state == X_IN_PORT) {
		xml_servers_read_state.state = X_IN_SERVER;
	} else if (g_ascii_strcasecmp(element_name, "login") == 0 && xml_servers_read_state.state == X_IN_LOGIN) {
		xml_servers_read_state.state = X_IN_SERVER;
	} else if (g_ascii_strcasecmp(element_name, "passwd") == 0 && xml_servers_read_state.state == X_IN_PASSWD) {
		xml_servers_read_state.state = X_IN_SERVER;
	} else if (g_ascii_strcasecmp(element_name, "alloweddbs") == 0 && xml_servers_read_state.state == X_IN_ALLOWED_DBS) {
		xml_servers_read_state.state = X_IN_SERVER;
	} else if (g_ascii_strcasecmp(element_name, "localsock") == 0 && xml_servers_read_state.state == X_IN_LOCAL_SOCK) {
		xml_servers_read_state.state = X_IN_SERVER;
	} else if (g_ascii_strcasecmp(element_name, "options") == 0 && xml_servers_read_state.state == X_IN_OPTIONS) {
		xml_servers_read_state.state = X_IN_SERVER;
	} else if (g_ascii_strcasecmp(element_name, "read_only") == 0 && xml_servers_read_state.state == X_IN_OPTIONS) {
		xml_servers_read_state.read_only = TRUE;
	} else if (g_ascii_strcasecmp(element_name, "write_warning") == 0 && xml_servers_read_state.state == X_IN_OPTIONS) {
		xml_servers_read_state.write_warning = TRUE;
	}
}

void xml_servers_conf_text (GMarkupParseContext *context, const gchar * text, gsize text_len, gpointer user_data, GError ** error) {
	if (xml_servers_read_state.state == X_IN_PORT) {
		xml_servers_read_state.port = g_ascii_strtoull(text, NULL, 10);
	}
}

void xml_servers_conf_passthrough (GMarkupParseContext *context, const gchar * passthrough_text, gsize text_len, gpointer user_data, GError **error) {
	if (!(g_ascii_strncasecmp(passthrough_text, "<![CDATA[", 9) == 0 && g_ascii_strncasecmp(passthrough_text + (text_len - 3), "]]>", 3) == 0)) {
		return;
	}
	
	switch (xml_servers_read_state.state) {
		case X_IN_NAME :
			g_string_append_len(xml_servers_read_state.name, passthrough_text + 9, text_len - 12);
			break;
		case X_IN_HOST :
			g_string_append_len(xml_servers_read_state.host, passthrough_text + 9, text_len - 12);
			break;
		case X_IN_LOGIN :
			g_string_append_len(xml_servers_read_state.login, passthrough_text + 9, text_len - 12);
			break;
		case X_IN_PASSWD :
			g_string_append_len(xml_servers_read_state.passwd, passthrough_text + 9, text_len - 12);
			break;
		case X_IN_ALLOWED_DBS :
			g_string_append_len(xml_servers_read_state.allowedDbs, passthrough_text + 9, text_len - 12);
			break;
		case X_IN_LOCAL_SOCK :
			g_string_append_len(xml_servers_read_state.localSock, passthrough_text + 9, text_len - 12);
			break;
		default :
			break;
	}
}
