
#include "gmysql_conf.h"

/* gmysql_config functions */
p_gmysql_config gmysql_config_new () {
	p_gmysql_config gmysql_conf;
	GString * filePath;

	gmysql_conf = (p_gmysql_config) g_try_malloc(sizeof(s_gmysql_config));
	
	if (gmysql_conf == (p_gmysql_config)NULL) {
		return (p_gmysql_config)NULL; /* return NULL pointer */
	}
	
	filePath = g_string_new("");
	
	g_string_printf(filePath, "%s/%s/%s", g_get_home_dir(), CONF_DIR, XML_FILE);
	gmysql_conf->configFileName = g_strdup(filePath->str);

	/*g_string_printf(filePath, "%s/%s/%s", g_get_home_dir(), CONF_DIR, QUERY_HISTORY_FILE);
	gmysql_conf->queryHistory = gmysql_history_new(filePath->str);*/

	gmysql_conf->lstServers = (GList *) NULL;
	
	g_string_free(filePath, TRUE);
	
	return gmysql_conf;
}

gboolean gmysql_config_init (p_gmysql_config gmysql_conf) {
	GString * filePath;

	if (gmysql_conf == (p_gmysql_config)NULL) {
		return FALSE;
	}
	
	filePath = g_string_new("");
	
	g_string_printf(filePath, "%s/%s", g_get_home_dir(), CONF_DIR);
	if (!g_file_test(filePath->str, (G_FILE_TEST_IS_DIR | G_FILE_TEST_EXISTS))) {
		mkdir (filePath->str, 0700);
	}
	
	g_string_free(filePath, TRUE);
	
	gmysql_config_read_xml(gmysql_conf);
	
	/*gmysql_history_read(gmysql_conf->queryHistory);*/
	
	return TRUE;
}

gboolean gmysql_config_delete (p_gmysql_config gmysql_conf) {
	GList * lstServersIdx;
	
	if (gmysql_conf != (p_gmysql_config)NULL) {
		if (gmysql_conf->lstServers != (GList *) NULL) {
			lstServersIdx = g_list_first(gmysql_conf->lstServers);
			while (lstServersIdx != (GList *)NULL) {
				 mysql_server_delete((p_mysql_server)lstServersIdx->data); 
				lstServersIdx = g_list_next(lstServersIdx);
			}
		}
		
		g_list_free(gmysql_conf->lstServers);
		
		g_free(gmysql_conf->configFileName);
		g_free(gmysql_conf);
	}
	
	return TRUE;
}

gboolean gmysql_config_add_server (p_gmysql_config gmysql_conf, gchar * name, gchar * host, gint port, gchar * login, gchar * passwd, gchar * dbAllowedList, gchar * localSock) {
	p_mysql_server msql_srv;
	
	msql_srv = gmysql_config_get_server(gmysql_conf, name);
	
	if (msql_srv != (p_mysql_server)NULL) {
		return FALSE;
	}
	
	msql_srv = mysql_server_new();
	if (msql_srv == FALSE) {
		return FALSE;
	}
	
	msql_srv->name = g_strdup(name);
	msql_srv->host = g_strdup(host);
	msql_srv->port = port;
	msql_srv->user = g_strdup(login);
	msql_srv->passwd = g_strdup(passwd);
	msql_srv->allowedDbs = g_strdup(dbAllowedList);
	msql_srv->localSock= g_strdup(localSock);
	
	gmysql_conf->lstServers = g_list_append(gmysql_conf->lstServers, msql_srv);
	
	return TRUE;
}

gboolean gmysql_config_update_server (p_gmysql_config gmysql_conf, const gchar * oldname, gchar * name, gchar * host, gint port, gchar * login, gchar * passwd, gchar * dbAllowedList, gchar * localSock) {
	p_mysql_server msql_srv;
	
	msql_srv = gmysql_config_get_server(gmysql_conf, oldname);
	
	if (msql_srv != (p_mysql_server)NULL) {

		/* Free old strings */
		g_free(msql_srv->name);
		g_free(msql_srv->host);
		g_free(msql_srv->user);
		g_free(msql_srv->passwd);
		g_free(msql_srv->allowedDbs);
		g_free(msql_srv->localSock);

		/* Add new ones */
		msql_srv->name = g_strdup(name);
		msql_srv->host = g_strdup(host);
		msql_srv->port = port;
		msql_srv->user = g_strdup(login);
		msql_srv->passwd = g_strdup(passwd);
		msql_srv->allowedDbs = g_strdup(passwd);
		msql_srv->localSock= g_strdup(localSock);
		
		return TRUE;
	}
	
	return FALSE;
}

gboolean gmysql_config_del_server (p_gmysql_config gmysql_conf, const gchar * name) {
	p_mysql_server msql_srv;
	GList * lstServersIdx;
	
	lstServersIdx = g_list_first(gmysql_conf->lstServers);
	
	while (lstServersIdx != (GList *)NULL) {
		msql_srv = (p_mysql_server) lstServersIdx->data;
		if (g_ascii_strcasecmp(msql_srv->name, name) == 0) {
			mysql_server_delete(msql_srv);
			gmysql_conf->lstServers = g_list_delete_link(gmysql_conf->lstServers, lstServersIdx);
			return TRUE;
		}
		lstServersIdx = g_list_next(lstServersIdx);
	}
	
	return FALSE;
}

p_mysql_server gmysql_config_get_server (p_gmysql_config gmysql_conf, const gchar * name) {
	p_mysql_server msql_srv;
	GList * lstServersIdx;
	
	lstServersIdx = g_list_first(gmysql_conf->lstServers);
	
	while (lstServersIdx != (GList *)NULL) {
		msql_srv = (p_mysql_server) lstServersIdx->data;
		if (g_ascii_strcasecmp(msql_srv->name, name) == 0) {
			return msql_srv;
		}
		lstServersIdx = g_list_next(lstServersIdx);
	}
	
	return (p_mysql_server)NULL;

}

/* xml read structures and datas */

struct _xmlReadState{
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
			X_IN_OPTIONS = 10
		} state;
	GString * name;
	GString * host;
	gint port;
	GString * login;
	GString * passwd;
	GString * allowedDbs;
	GString * localSock;
} xmlReadState;

char * defaultConf = "<?xml version=\"1.0\" ?><servers><server><name><![CDATA[Localhost]]></name><host><![CDATA[localhost]]></host><port>3306</port><login><![CDATA[root]]></login><passwd><![CDATA[]]></passwd><alloweddbs><![CDATA[]]></alloweddbs><localsock><![CDATA[/var/run/mysqld/mysqld.sock]]></localsock><options></options></server></servers>";

/* xml read functions */

void xmlConfStart (GMarkupParseContext *context, const gchar * element_name, const gchar ** attribute_names, const gchar ** attribute_values, gpointer user_data, GError ** error);
void xmlConfEnd (GMarkupParseContext *context, const gchar * element_name, gpointer user_data, GError ** error);
void xmlConfText (GMarkupParseContext *context, const gchar * text, gsize text_len, gpointer user_data, GError ** error);
void xmlConfPassthrough (GMarkupParseContext *context, const gchar * passthrough_text, gsize text_len, gpointer user_data, GError **error);

gboolean gmysql_config_read_xml (p_gmysql_config gmysql_conf) {

	GMarkupParser xmlParse;
	GMarkupParseContext * xmlContext;
	GError * gerr = (GError *)NULL;
	FILE * xmlFile;
	gchar sbuf[257];
	int szRead;
	
	if (gmysql_conf == (p_gmysql_config)NULL) {
		return FALSE; 
	}
	
	if (gmysql_conf->configFileName == (gchar *)NULL) {
		return FALSE;
	}
	
	/* Init XMl parser Struct */
	xmlParse.start_element = xmlConfStart;
	xmlParse.end_element = xmlConfEnd;
	xmlParse.text = xmlConfText;
	xmlParse.passthrough = xmlConfPassthrough;
	xmlParse.error = NULL;
	
	/* Init XML read status struct */
	xmlReadState.state = X_OUT_ROOT;
	xmlReadState.name = g_string_sized_new(16);
	xmlReadState.host = g_string_sized_new(16);
	xmlReadState.port = 0;
	xmlReadState.login = g_string_sized_new(16);
	xmlReadState.passwd = g_string_sized_new(16);
	xmlReadState.allowedDbs = g_string_sized_new(48);
	xmlReadState.localSock = g_string_sized_new(48);

	/* Create XML Parser */
	xmlContext = g_markup_parse_context_new(&xmlParse, 0, (gpointer)gmysql_conf, (GDestroyNotify)NULL);
	
	/*g_print("File open : '%s'\n", gmysql_conf->configFileName);*/
	xmlFile = fopen(gmysql_conf->configFileName, "r");
	if (xmlFile == (FILE *)NULL) { /* Not found conf file -> use default conf */
		g_printerr("Error file open : %s. -> read default configuration\n", strerror(errno));
		g_markup_parse_context_parse(xmlContext, defaultConf, strlen(defaultConf), &gerr);
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

gboolean gmysql_config_write_xml (p_gmysql_config gmysql_conf) {
	GString * xmlContent;
	GList * lstServersIdx;
	p_mysql_server msql_srv;
	FILE * xmlFile;
	
	if (gmysql_conf == (p_gmysql_config)NULL) {
		return FALSE; 
	}
	
	if (gmysql_conf->configFileName == (gchar *)NULL) {
		return FALSE;
	}
	
	xmlContent = g_string_new("<?xml version=\"1.0\" ?>\n<servers>\n");
	
	lstServersIdx = g_list_first(gmysql_conf->lstServers);
	
	while (lstServersIdx != (GList *)NULL) {
		msql_srv = (p_mysql_server) lstServersIdx->data;
		g_string_append(xmlContent, "\t<server>\n");
		g_string_append_printf(xmlContent, "\t\t<name><![CDATA[%s]]></name>\n", msql_srv->name);
		g_string_append_printf(xmlContent, "\t\t<host><![CDATA[%s]]></host>\n", msql_srv->host);
		g_string_append_printf(xmlContent, "\t\t<port>%d</port>\n", msql_srv->port);
		g_string_append_printf(xmlContent, "\t\t<login><![CDATA[%s]]></login>\n", msql_srv->user);
		g_string_append_printf(xmlContent, "\t\t<passwd><![CDATA[%s]]></passwd>\n", msql_srv->passwd);
		if (msql_srv->allowedDbs != (gchar *)NULL) {
			g_string_append_printf(xmlContent, "\t\t<alloweddbs><![CDATA[%s]]></alloweddbs>\n", msql_srv->allowedDbs);
		} else {
			g_string_append_printf(xmlContent, "\t\t<alloweddbs><![CDATA[]]></alloweddbs>\n");
		}
		if (msql_srv->localSock != (gchar *)NULL) {
			g_string_append_printf(xmlContent, "\t\t<localsock><![CDATA[%s]]></localsock>\n", msql_srv->localSock);
		} else {
			g_string_append_printf(xmlContent, "\t\t<localsock><![CDATA[]]></localsock>\n");
		}
		g_string_append(xmlContent, "\t\t<options></options>\n\t</server>\n");
		lstServersIdx = g_list_next(lstServersIdx);
	}

	g_string_append(xmlContent, "</servers>\n");
	
	xmlFile = fopen(gmysql_conf->configFileName, "w");
	if (xmlFile == (FILE *)NULL) {
		g_printerr("Error file open : %s.\n", strerror(errno));
		return FALSE;
	}
	fwrite(xmlContent->str, 1, xmlContent->len, xmlFile);
	fclose(xmlFile);
	
	g_string_free(xmlContent, TRUE);
	
	return TRUE;
}



void xmlConfStart (GMarkupParseContext *context, const gchar * element_name, const gchar ** attribute_names, const gchar ** attribute_values, gpointer user_data, GError ** error) {
	if (g_ascii_strcasecmp(element_name, "servers") == 0 && xmlReadState.state == X_OUT_ROOT) {
		xmlReadState.state = X_IN_ROOT;
	} else if (g_ascii_strcasecmp(element_name, "server") == 0 && xmlReadState.state == X_IN_ROOT) {
		g_string_erase(xmlReadState.name, 0, -1);
		g_string_erase(xmlReadState.host, 0, -1);
		g_string_erase(xmlReadState.login, 0, -1);
		g_string_erase(xmlReadState.passwd, 0, -1);
		g_string_erase(xmlReadState.allowedDbs, 0, -1);
		g_string_erase(xmlReadState.localSock, 0, -1);
		xmlReadState.port = 0;
		xmlReadState.state = X_IN_SERVER;
	} else if (g_ascii_strcasecmp(element_name, "name") == 0 && xmlReadState.state == X_IN_SERVER) {
		xmlReadState.state = X_IN_NAME;
	} else if (g_ascii_strcasecmp(element_name, "host") == 0 && xmlReadState.state == X_IN_SERVER) {
		xmlReadState.state = X_IN_HOST;
	} else if (g_ascii_strcasecmp(element_name, "port") == 0 && xmlReadState.state == X_IN_SERVER) {
		xmlReadState.state = X_IN_PORT;
	} else if (g_ascii_strcasecmp(element_name, "login") == 0 && xmlReadState.state == X_IN_SERVER) {
		xmlReadState.state = X_IN_LOGIN;
	} else if (g_ascii_strcasecmp(element_name, "passwd") == 0 && xmlReadState.state == X_IN_SERVER) {
		xmlReadState.state = X_IN_PASSWD;
	} else if (g_ascii_strcasecmp(element_name, "alloweddbs") == 0 && xmlReadState.state == X_IN_SERVER) {
		xmlReadState.state = X_IN_ALLOWED_DBS;
	} else if (g_ascii_strcasecmp(element_name, "localsock") == 0 && xmlReadState.state == X_IN_SERVER) {
		xmlReadState.state = X_IN_LOCAL_SOCK;
	} else if (g_ascii_strcasecmp(element_name, "options") == 0 && xmlReadState.state == X_IN_SERVER) {
		xmlReadState.state = X_IN_OPTIONS;
	}
}

void xmlConfEnd (GMarkupParseContext *context, const gchar * element_name, gpointer user_data, GError ** error) {
	p_gmysql_config gmysql_conf = (p_gmysql_config)user_data;

	if (g_ascii_strcasecmp(element_name, "servers") == 0 && xmlReadState.state == X_IN_ROOT) {
		xmlReadState.state = X_OUT_ROOT;
	} else if (g_ascii_strcasecmp(element_name, "server") == 0 && xmlReadState.state == X_IN_SERVER) {
		gmysql_config_add_server(gmysql_conf, xmlReadState.name->str, xmlReadState.host->str,  xmlReadState.port, xmlReadState.login->str, xmlReadState.passwd->str, xmlReadState.allowedDbs->str, xmlReadState.localSock->str);
		xmlReadState.state = X_IN_ROOT;
	} else if (g_ascii_strcasecmp(element_name, "name") == 0 && xmlReadState.state == X_IN_NAME) {
		xmlReadState.state = X_IN_SERVER;
	} else if (g_ascii_strcasecmp(element_name, "host") == 0 && xmlReadState.state == X_IN_HOST) {
		xmlReadState.state = X_IN_SERVER;
	} else if (g_ascii_strcasecmp(element_name, "port") == 0 && xmlReadState.state == X_IN_PORT) {
		xmlReadState.state = X_IN_SERVER;
	} else if (g_ascii_strcasecmp(element_name, "login") == 0 && xmlReadState.state == X_IN_LOGIN) {
		xmlReadState.state = X_IN_SERVER;
	} else if (g_ascii_strcasecmp(element_name, "passwd") == 0 && xmlReadState.state == X_IN_PASSWD) {
		xmlReadState.state = X_IN_SERVER;
	} else if (g_ascii_strcasecmp(element_name, "alloweddbs") == 0 && xmlReadState.state == X_IN_ALLOWED_DBS) {
		xmlReadState.state = X_IN_SERVER;
	} else if (g_ascii_strcasecmp(element_name, "localsock") == 0 && xmlReadState.state == X_IN_LOCAL_SOCK) {
		xmlReadState.state = X_IN_SERVER;
	} else if (g_ascii_strcasecmp(element_name, "options") == 0 && xmlReadState.state == X_IN_OPTIONS) {
		xmlReadState.state = X_IN_SERVER;
	}
}

void xmlConfText (GMarkupParseContext *context, const gchar * text, gsize text_len, gpointer user_data, GError ** error) {
	if (xmlReadState.state == X_IN_PORT) {
		xmlReadState.port = g_ascii_strtoull(text, (gchar **)NULL, 10);
	}
}

void xmlConfPassthrough (GMarkupParseContext *context, const gchar * passthrough_text, gsize text_len, gpointer user_data, GError **error) {
	if (!(g_ascii_strncasecmp(passthrough_text, "<![CDATA[", 9) == 0 && g_ascii_strncasecmp(passthrough_text + (text_len - 3), "]]>", 3) == 0)) {
		return;
	}
	
	switch (xmlReadState.state) {
		case X_IN_NAME :
			g_string_append_len(xmlReadState.name, passthrough_text + 9, text_len - 12);
			break;
		case X_IN_HOST :
			g_string_append_len(xmlReadState.host, passthrough_text + 9, text_len - 12);
			break;
		case X_IN_LOGIN :
			g_string_append_len(xmlReadState.login, passthrough_text + 9, text_len - 12);
			break;
		case X_IN_PASSWD :
			g_string_append_len(xmlReadState.passwd, passthrough_text + 9, text_len - 12);
			break;
		case X_IN_ALLOWED_DBS :
			g_string_append_len(xmlReadState.allowedDbs, passthrough_text + 9, text_len - 12);
			break;
		case X_IN_LOCAL_SOCK :
			g_string_append_len(xmlReadState.localSock, passthrough_text + 9, text_len - 12);
			break;
		default :
			break;
	}
}
