/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include "gmlc_misc_config.h"

struct _s_xml_config_read_state {
	enum {
			X_CFG_OUT = 0,
			X_CFG_IN_GMYSQLCC, /* = 1, */
			X_CFG_IN_GMYSQLCC_FONTS, /* = 2, */
			X_CFG_IN_GMYSQLCC_FONTS_QUERY, /* = 3, */
			X_CFG_IN_GMYSQLCC_FONTS_HELP, /* = 4, */
		} state;
	GString * text;
} s_xml_config_read_state;

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

char * default_gmysqlcc_conf = "<?xml version=\"1.0\"?>"
"<gmysqlcc>"
	"<fonts>"
		"<query><![CDATA[courier 9]]></query>"
		"<help><![CDATA[courier 9]]></help>"
	"</fonts>"
"</gmysqlcc>";

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

gboolean gmlc_misc_config_read_config_file (GmlcMiscConfig * pGmlcMscCfg);
gboolean gmlc_misc_config_write_config_file (GmlcMiscConfig * pGmlcMscCfg);
void xml_config_conf_start (GMarkupParseContext *context, const gchar * element_name, const gchar ** attribute_names, const gchar ** attribute_values, gpointer user_data, GError ** error);
void xml_config_conf_end (GMarkupParseContext *context, const gchar * element_name, gpointer user_data, GError ** error);
void xml_config_conf_text (GMarkupParseContext *context, const gchar * text, gsize text_len, gpointer user_data, GError ** error);
void xml_config_conf_passthrough (GMarkupParseContext *context, const gchar * passthrough_text, gsize text_len, gpointer user_data, GError **error);

gboolean gmlc_misc_config_read_servers_file (GmlcMiscConfig * pGmlcMscCfg);
gboolean gmlc_misc_config_write_servers_file (GmlcMiscConfig * pGmlcMscCfg);
void xml_servers_conf_start (GMarkupParseContext *context, const gchar * element_name, const gchar ** attribute_names, const gchar ** attribute_values, gpointer user_data, GError ** error);
void xml_servers_conf_end (GMarkupParseContext *context, const gchar * element_name, gpointer user_data, GError ** error);
void xml_servers_conf_text (GMarkupParseContext *context, const gchar * text, gsize text_len, gpointer user_data, GError ** error);
void xml_servers_conf_passthrough (GMarkupParseContext *context, const gchar * passthrough_text, gsize text_len, gpointer user_data, GError **error);


static void gmlc_misc_config_finalize (GmlcMiscConfig * pGmlcMscCfg);
static void gmlc_misc_config_set_property (GObject * object, guint prop_id, const GValue * value, GParamSpec * pspec);
static void gmlc_misc_config_get_property (GObject * object, guint prop_id, GValue * value, GParamSpec * pspec);


enum {
	PROP_0,
	PROP_NAME,
	PROP_HOST,
	PROP_PORT,
	PROP_LOGIN,
	PROP_PASSWORD,
	PROP_LOCAL_SOCK,
	PROP_ALLOWED_DBS,
	PROP_READ_ONLY,
	PROP_WRITE_WARNING,
	PROP_VERSION
};

G_DEFINE_TYPE (GmlcMiscConfig, gmlc_misc_config, G_TYPE_OBJECT);

static void gmlc_misc_config_class_init (GmlcMiscConfigClass * pClass) {
	GObjectClass *pObjClass = G_OBJECT_CLASS(pClass);
	
	pObjClass->finalize = (GObjectFinalizeFunc) gmlc_misc_config_finalize;
	
	pObjClass->get_property = gmlc_misc_config_get_property;
	pObjClass->set_property = gmlc_misc_config_set_property;
	
/*
	g_object_class_install_property(pObjClass, PROP_NAME, 
		g_param_spec_string("name", "Server name", "Name (not host) of the mysql server", "", G_PARAM_READWRITE | G_PARAM_CONSTRUCT));
	g_object_class_install_property(pObjClass, PROP_HOST, 
		g_param_spec_string("hostname", "Server host name", "Host name of the mysql server", "", G_PARAM_READWRITE | G_PARAM_CONSTRUCT));
	g_object_class_install_property(pObjClass, PROP_PORT, 
		g_param_spec_uint("port", "Server port", "Port of the mysql server", 0, 65535, 3306, G_PARAM_READWRITE | G_PARAM_CONSTRUCT));
	g_object_class_install_property(pObjClass, PROP_LOGIN, 
		g_param_spec_string("login", "Server login", "Login for the mysql server", "", G_PARAM_READWRITE | G_PARAM_CONSTRUCT));
	g_object_class_install_property(pObjClass, PROP_PASSWORD, 
		g_param_spec_string("password", "Server password", "Password for the mysql server", "", G_PARAM_READWRITE | G_PARAM_CONSTRUCT));
	g_object_class_install_property(pObjClass, PROP_LOCAL_SOCK, 
		g_param_spec_string("local_socket", "Local socket file name", "Local socket file name used for access to the mysql server", "", G_PARAM_READWRITE | G_PARAM_CONSTRUCT));
	g_object_class_install_property(pObjClass, PROP_ALLOWED_DBS, 
		g_param_spec_string("allowed_dbs", "Allowed databases", "Databases allowed to the login for the mysql server", "", G_PARAM_READWRITE | G_PARAM_CONSTRUCT));
	g_object_class_install_property(pObjClass, PROP_READ_ONLY, 
		g_param_spec_boolean("read_only", "Read only server", "The mysql server is marked as read only", FALSE, G_PARAM_READWRITE | G_PARAM_CONSTRUCT));
	g_object_class_install_property(pObjClass, PROP_WRITE_WARNING, 
		g_param_spec_boolean("write_warning", "Write warning server", "Display write warning for this mysql server", FALSE, G_PARAM_READWRITE | G_PARAM_CONSTRUCT));
	g_object_class_install_property(pObjClass, PROP_VERSION, 
		g_param_spec_ulong("version", "Server version", "Version of the mysql server", 0, G_MAXULONG, 0, G_PARAM_READABLE));
*/	
	
}

static void gmlc_misc_config_init (GmlcMiscConfig * pGmlcMscCfg) {
	pGmlcMscCfg->pcServersFilename = NULL;
	pGmlcMscCfg->pcConfigFilename = NULL;
	
	pGmlcMscCfg->pcQueryFontName = NULL;
	pGmlcMscCfg->pcHelpFontName = NULL;

	pGmlcMscCfg->lstServers = NULL;
}

static void gmlc_misc_config_finalize (GmlcMiscConfig * pGmlcMscCfg) {
	GList * lstServersIdx;
	
	if (pGmlcMscCfg->lstServers != NULL) {
		lstServersIdx = g_list_first(pGmlcMscCfg->lstServers);
		while (lstServersIdx != NULL) {
			g_object_unref(G_OBJECT(lstServersIdx->data));
			lstServersIdx = g_list_next(lstServersIdx);
		}
	}
	
	g_list_free(pGmlcMscCfg->lstServers);
	
	g_free(pGmlcMscCfg->pcQueryFontName);
	g_free(pGmlcMscCfg->pcHelpFontName);

	g_free(pGmlcMscCfg->pcServersFilename);
	g_free(pGmlcMscCfg->pcConfigFilename);
}

static void gmlc_misc_config_set_property (GObject * object, guint prop_id, const GValue * value, GParamSpec * pspec) {
	GmlcMiscConfig * pGmlcMscCfg = GMLC_MISC_CONFIG(object);
	UNUSED_VAR(pGmlcMscCfg);
	
	switch (prop_id) {
/*
		case PROP_NAME :
			g_free(pGmlcMscCfg->pcName);
			pGmlcMscCfg->pcName = g_value_dup_string(value);
			break;
		case PROP_HOST :
			g_free(pGmlcMscCfg->pcHost);
			pGmlcMscCfg->pcHost = g_value_dup_string(value);
			break;
		case PROP_PORT :
			pGmlcMscCfg->iPort = g_value_get_uint (value);
			break;
		case PROP_LOGIN :
			g_free(pGmlcMscCfg->pcLogin);
			pGmlcMscCfg->pcLogin = g_value_dup_string(value);
			break;
		case PROP_PASSWORD :
			g_free(pGmlcMscCfg->pcPassword);
			pGmlcMscCfg->pcPassword = g_value_dup_string(value);
			break;
		case PROP_LOCAL_SOCK :
			g_free(pGmlcMscCfg->pcLocalSock);
			pGmlcMscCfg->pcLocalSock = g_value_dup_string(value);
			break;
		case PROP_ALLOWED_DBS :
			g_free(pGmlcMscCfg->pcAllowedDbs);
			pGmlcMscCfg->pcAllowedDbs = g_value_dup_string(value);
			break;
		case PROP_READ_ONLY :
			pGmlcMscCfg->bReadOnly = g_value_get_boolean(value);
			break;
		case PROP_WRITE_WARNING :
			pGmlcMscCfg->bWriteWarning = g_value_get_boolean(value);
			break;
*/
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
			break;
	}
}

static void gmlc_misc_config_get_property (GObject * object, guint prop_id, GValue * value, GParamSpec * pspec) {
	GmlcMiscConfig * pGmlcMscCfg = GMLC_MISC_CONFIG(object);
	UNUSED_VAR(pGmlcMscCfg);
	
	switch (prop_id) {
/*
		case PROP_NAME :
			g_value_set_string(value, pGmlcMscCfg->pcName);
			break;
		case PROP_HOST :
			g_value_set_string(value, pGmlcMscCfg->pcHost);
			break;
		case PROP_PORT :
			g_value_set_uint(value, pGmlcMscCfg->iPort);
			break;
		case PROP_LOGIN :
			g_value_set_string(value, pGmlcMscCfg->pcLogin);
			break;
		case PROP_PASSWORD :
			g_value_set_string(value, pGmlcMscCfg->pcPassword);
			break;
		case PROP_LOCAL_SOCK :
			g_value_set_string(value, pGmlcMscCfg->pcLocalSock);
			break;
		case PROP_ALLOWED_DBS :
			g_value_set_string(value, pGmlcMscCfg->pcAllowedDbs);
			break;
		case PROP_READ_ONLY :
			g_value_set_boolean(value, pGmlcMscCfg->bReadOnly);
			break;
		case PROP_WRITE_WARNING :
			g_value_set_boolean(value, pGmlcMscCfg->bWriteWarning);
			break;
		case PROP_VERSION :
			if (pGmlcMscCfg->lVersion == 0) {
				GmlcMysqlQuery * pGmlcMysqlQry = NULL;
				pGmlcMysqlQry = gmlc_mysql_query_new(G_OBJECT(pGmlcMscCfg), NULL);
				pGmlcMscCfg->lVersion = gmlc_mysql_query_get_version(pGmlcMysqlQry);
				g_object_unref(G_OBJECT(pGmlcMysqlQry));
			}
			g_value_set_ulong(value, pGmlcMscCfg->lVersion);
			break;
*/
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
			break;
	}
}

GmlcMiscConfig * gmlc_misc_config_new (void) {
	return GMLC_MISC_CONFIG(g_object_new (GMLC_TYPE_MISC_CONFIG, NULL));
}

gboolean gmlc_misc_config_read (GmlcMiscConfig * pGmlcMscCfg) {

	if (pGmlcMscCfg == NULL) {
		return FALSE;
	}
	
	/* Check new config file */
	pGmlcMscCfg->pcConfigFilename = g_build_path("/", g_get_user_config_dir(), GMYSQLCC_CONF_DIR, CONFIG_FILE, NULL);
	if (!g_file_test(pGmlcMscCfg->pcConfigFilename, (G_FILE_TEST_EXISTS | G_FILE_TEST_IS_REGULAR))) {
		g_free(pGmlcMscCfg->pcConfigFilename);
		pGmlcMscCfg->pcConfigFilename = NULL;
	}
	
	gmlc_misc_config_read_config_file(pGmlcMscCfg);
	
	/* Check new servers file */
	pGmlcMscCfg->pcServersFilename = g_build_path("/", g_get_user_config_dir(), GMYSQLCC_CONF_DIR, SERVERS_FILE, NULL);
	if (!g_file_test(pGmlcMscCfg->pcServersFilename, (G_FILE_TEST_EXISTS | G_FILE_TEST_IS_REGULAR))) {
		g_free(pGmlcMscCfg->pcServersFilename);
		pGmlcMscCfg->pcServersFilename = NULL;
	}
	
	gmlc_misc_config_read_servers_file(pGmlcMscCfg);
	
	return TRUE;
}

gboolean gmlc_misc_config_write (GmlcMiscConfig * pGmlcMscCfg) {
	gchar * pcFilePath = NULL;

	if (pGmlcMscCfg == NULL) {
		return FALSE;
	}
	
	/* Check config directories and create them if needed */
	pGmlcMscCfg->pcConfigFilename = g_build_path("/", g_get_user_config_dir(), GMYSQLCC_CONF_DIR, CONFIG_FILE, NULL);
	
	pcFilePath = (gchar *)g_get_user_config_dir();
	if (!g_file_test(pcFilePath, (G_FILE_TEST_EXISTS | G_FILE_TEST_IS_DIR))) {
		mkdir (pcFilePath, 0700);
	}
	
	pcFilePath = g_build_path("/", g_get_user_config_dir(), GMYSQLCC_CONF_DIR, NULL);
	if (!g_file_test(pcFilePath, (G_FILE_TEST_EXISTS | G_FILE_TEST_IS_DIR))) {
		mkdir (pcFilePath, 0700);
	}
	g_free(pcFilePath);
	
	/* Check new config file */
	if (pGmlcMscCfg->pcConfigFilename == NULL) {
		pGmlcMscCfg->pcConfigFilename = g_build_path("/", g_get_user_config_dir(), GMYSQLCC_CONF_DIR, CONFIG_FILE, NULL);
	}
	
	gmlc_misc_config_write_config_file(pGmlcMscCfg);
	
	/* Check new servers file */
	if (pGmlcMscCfg->pcServersFilename == NULL) {
		pGmlcMscCfg->pcServersFilename = g_build_path("/", g_get_user_config_dir(), GMYSQLCC_CONF_DIR, SERVERS_FILE, NULL);
	}
	gmlc_misc_config_write_servers_file(pGmlcMscCfg);
	
	return TRUE;
}

gboolean gmlc_misc_config_add_server (GmlcMiscConfig * pGmlcMscCfg, const gchar * name, const gchar * host, const gint port, const gchar * login, const gchar * passwd, const gchar * dbAllowedList, const gchar * localSock, const gboolean read_only, const gboolean write_warning) {
	GmlcMysqlServer * pGmlcMysqlSrv;
	
	pGmlcMysqlSrv = gmlc_misc_config_get_server(pGmlcMscCfg, name);
	
	if (pGmlcMysqlSrv != NULL) {
		return FALSE;
	}
	
	pGmlcMysqlSrv = gmlc_mysql_server_new();
	if (pGmlcMysqlSrv == NULL) {
		return FALSE;
	}
	
	g_object_set(pGmlcMysqlSrv, 
		"name", name, 
		"hostname", host, 
		"port", port, 
		"login", login, 
		"password", passwd, 
		"allowed_dbs", dbAllowedList, 
		"local_socket", localSock, 
		"read_only", read_only, 
		"write_warning", write_warning, 
		NULL);
	
	pGmlcMscCfg->lstServers = g_list_append(pGmlcMscCfg->lstServers, pGmlcMysqlSrv);
	
	return TRUE;
}

gboolean gmlc_misc_config_update_server (GmlcMiscConfig * pGmlcMscCfg, const gchar * oldname, const gchar * name, const gchar * host, const gint port, const gchar * login, const gchar * passwd, const gchar * dbAllowedList, const gchar * localSock, const gboolean read_only, const gboolean write_warning) {
	GmlcMysqlServer * pGmlcMysqlSrv;
	
	pGmlcMysqlSrv = gmlc_misc_config_get_server(pGmlcMscCfg, oldname);
	
	if (pGmlcMysqlSrv != NULL) {

		g_object_set(pGmlcMysqlSrv, 
			"name", name, 
			"hostname", host, 
			"port", port, 
			"login", login, 
			"password", passwd, 
			"allowed_dbs", dbAllowedList, 
			"local_socket", localSock, 
			"read_only", read_only, 
			"write_warning", write_warning,
			NULL);
		
		return TRUE;
	}
	
	return FALSE;
}

gboolean gmlc_misc_config_del_server (GmlcMiscConfig * pGmlcMscCfg, const gchar * name) {
	GmlcMysqlServer * pGmlcMysqlSrv = NULL;
	GList * lstServersIdx = NULL;
	gchar * pcSrvName = NULL;
	
	lstServersIdx = g_list_first(pGmlcMscCfg->lstServers);
	
	while (lstServersIdx != NULL) {
		pGmlcMysqlSrv = (GmlcMysqlServer *) lstServersIdx->data;
		g_object_get(pGmlcMysqlSrv, "name", &pcSrvName, NULL);
		if (g_ascii_strcasecmp(pcSrvName, name) == 0) {
			g_object_unref(G_OBJECT(pGmlcMysqlSrv));
			pGmlcMscCfg->lstServers = g_list_delete_link(pGmlcMscCfg->lstServers, lstServersIdx);
			return TRUE;
		}
		g_free(pcSrvName);
		lstServersIdx = g_list_next(lstServersIdx);
	}
	
	return FALSE;
}

GmlcMysqlServer * gmlc_misc_config_get_server (GmlcMiscConfig * pGmlcMscCfg, const gchar * name) {
	GmlcMysqlServer * pGmlcMysqlSrv = NULL;
	GList * lstServersIdx = NULL;
	gchar * pcSrvName = NULL;
	
	lstServersIdx = g_list_first(pGmlcMscCfg->lstServers);
	
	while (lstServersIdx != NULL) {
		pGmlcMysqlSrv = (GmlcMysqlServer *)lstServersIdx->data;
		g_object_get(pGmlcMysqlSrv, "name", &pcSrvName, NULL);
		if (g_ascii_strcasecmp(pcSrvName, name) == 0) {
			return pGmlcMysqlSrv;
		}
		g_free(pcSrvName);
		lstServersIdx = g_list_next(lstServersIdx);
	}
	
	return NULL;
}

/* xml read structures and datas */

/* xml read functions - Config file */

gboolean gmlc_misc_config_read_config_file (GmlcMiscConfig * pGmlcMscCfg) {
	GMarkupParser xmlParse;
	GMarkupParseContext * xmlContext;
	GError * gErr = NULL;
	gchar * pcBuffer = NULL;
	gsize szBuffer = 0;
	
	if (pGmlcMscCfg == NULL) {
		return FALSE; 
	}
	
	/* Init XML parser Struct */
	xmlParse.start_element = xml_config_conf_start;
	xmlParse.end_element = xml_config_conf_end;
	xmlParse.text = NULL; /*xml_config_conf_text;*/
	xmlParse.passthrough = xml_config_conf_passthrough;
	xmlParse.error = NULL;
	
	/* Init XML read status struct */
	s_xml_config_read_state.state = X_CFG_OUT;
	s_xml_config_read_state.text = g_string_sized_new(16);

	/* Create XML Parser */
	xmlContext = g_markup_parse_context_new(&xmlParse, 0, (gpointer)pGmlcMscCfg, (GDestroyNotify)NULL);
	
	if (pGmlcMscCfg->pcConfigFilename != NULL) {
		if (!g_file_get_contents(pGmlcMscCfg->pcConfigFilename, &pcBuffer, &szBuffer, &gErr)) {
			g_printerr("Error config file open : %s. -> read default configuration\n", gErr->message);
			g_clear_error(&gErr);
			pcBuffer = g_strdup(default_gmysqlcc_conf);
			szBuffer = strlen(pcBuffer);
		}
	} else {
		g_printerr("Config file doesn't exist. -> read default configuration\n");
		pcBuffer = g_strdup(default_gmysqlcc_conf);
		szBuffer = strlen(pcBuffer);
	}
	
	g_markup_parse_context_parse(xmlContext, pcBuffer, szBuffer, &gErr);
	g_markup_parse_context_end_parse (xmlContext, &gErr);
	
	g_markup_parse_context_free(xmlContext);
	
	return TRUE;
}

gboolean gmlc_misc_config_write_config_file (GmlcMiscConfig * pGmlcMscCfg) {
	GString * xmlContent;
	GError * gErr = NULL;
	
	if (pGmlcMscCfg == NULL) {
		return FALSE; 
	}
	
	if (pGmlcMscCfg->pcConfigFilename == NULL) {
		return FALSE;
	}
	
	xmlContent = g_string_new("<?xml version=\"1.0\" ?>\n<gmysqlcc>\n");
	g_string_append_printf(xmlContent, "\t<fonts>\n\t\t<query><![CDATA[%s]]></query>\n", pGmlcMscCfg->pcQueryFontName);
	g_string_append_printf(xmlContent, "\t\t<help><![CDATA[%s]]></help>\n\t</fonts>\n", pGmlcMscCfg->pcHelpFontName);
	g_string_append(xmlContent, "</gmysqlcc>\n");
	
	if (!g_file_set_contents(pGmlcMscCfg->pcConfigFilename, xmlContent->str, xmlContent->len, &gErr)) {
		g_printerr("Error can't write config file : %s\n", gErr->message);
		g_clear_error(&gErr);
		g_string_free(xmlContent, TRUE);
		return FALSE;
	}
	
	g_string_free(xmlContent, TRUE);

	return TRUE;
}

void xml_config_conf_start (GMarkupParseContext *context, const gchar * element_name, const gchar ** attribute_names, const gchar ** attribute_values, gpointer user_data, GError ** error) {
	if (g_ascii_strcasecmp(element_name, "gmysqlcc") == 0 && s_xml_config_read_state.state == X_CFG_OUT) {
		s_xml_config_read_state.state = X_CFG_IN_GMYSQLCC;
	} else if (g_ascii_strcasecmp(element_name, "fonts") == 0 && s_xml_config_read_state.state == X_CFG_IN_GMYSQLCC) {
		s_xml_config_read_state.state = X_CFG_IN_GMYSQLCC_FONTS;
	} else if (g_ascii_strcasecmp(element_name, "query") == 0 && s_xml_config_read_state.state == X_CFG_IN_GMYSQLCC_FONTS) {
		g_string_erase(s_xml_config_read_state.text, 0, -1);
		s_xml_config_read_state.state = X_CFG_IN_GMYSQLCC_FONTS_QUERY;
	} else if (g_ascii_strcasecmp(element_name, "help") == 0 && s_xml_config_read_state.state == X_CFG_IN_GMYSQLCC_FONTS) {
		g_string_erase(s_xml_config_read_state.text, 0, -1);
		s_xml_config_read_state.state = X_CFG_IN_GMYSQLCC_FONTS_HELP;
	}
}

void xml_config_conf_end (GMarkupParseContext *context, const gchar * element_name, gpointer user_data, GError ** error) {
	GmlcMiscConfig * pGmlcMscCfg = GMLC_MISC_CONFIG(user_data);

	if (g_ascii_strcasecmp(element_name, "gmysqlcc") == 0 && s_xml_config_read_state.state == X_CFG_IN_GMYSQLCC) {
		s_xml_config_read_state.state = X_CFG_OUT;
	} else if (g_ascii_strcasecmp(element_name, "fonts") == 0 && s_xml_config_read_state.state == X_CFG_IN_GMYSQLCC_FONTS) {
		s_xml_config_read_state.state = X_CFG_IN_GMYSQLCC;
	} else if (g_ascii_strcasecmp(element_name, "query") == 0 && s_xml_config_read_state.state == X_CFG_IN_GMYSQLCC_FONTS_QUERY) {
		pGmlcMscCfg->pcQueryFontName = strdup(s_xml_config_read_state.text->str);
		/*g_print("+ SET Font query : '%s'\n", gmysqlcc_conf->pcQueryFontName);*/
		s_xml_config_read_state.state = X_CFG_IN_GMYSQLCC_FONTS;
	} else if (g_ascii_strcasecmp(element_name, "help") == 0 && s_xml_config_read_state.state == X_CFG_IN_GMYSQLCC_FONTS_HELP) {
		pGmlcMscCfg->pcHelpFontName = strdup(s_xml_config_read_state.text->str);
		/*g_print("+ SET Font help : '%s'\n", gmysqlcc_conf->pcHelpFontName);*/
		s_xml_config_read_state.state = X_CFG_IN_GMYSQLCC_FONTS;
	}
}

void xml_config_conf_text (GMarkupParseContext *context, const gchar * text, gsize text_len, gpointer user_data, GError ** error) {
}

void xml_config_conf_passthrough (GMarkupParseContext *context, const gchar * passthrough_text, gsize text_len, gpointer user_data, GError **error) {

	if (!(g_ascii_strncasecmp(passthrough_text, "<![CDATA[", 9) == 0 && g_ascii_strncasecmp(passthrough_text + (text_len - 3), "]]>", 3) == 0)) {
		return;
	}
	
	switch (s_xml_config_read_state.state) {
		case X_CFG_IN_GMYSQLCC_FONTS_QUERY :
			g_string_append_len(s_xml_config_read_state.text, passthrough_text + 9, text_len - 12);
			/*g_print("+ Read Font query : '%s'\n", s_xml_config_read_state.text->str);*/
			break;
		case X_CFG_IN_GMYSQLCC_FONTS_HELP :
			g_string_append_len(s_xml_config_read_state.text, passthrough_text + 9, text_len - 12);
			/*g_print("+ Read Font help : '%s'\n", s_xml_config_read_state.text->str);*/
			break;
		default :
			break;
	}
}

/* xml read functions - Servers file */

gboolean gmlc_misc_config_read_servers_file (GmlcMiscConfig * pGmlcMscCfg) {
	GMarkupParser xmlParse;
	GMarkupParseContext * xmlContext;
	GError * gErr = NULL;
	gchar * pcBuffer = NULL;
	gsize szBuffer = 0;
	
	if (pGmlcMscCfg == NULL) {
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
	xmlContext = g_markup_parse_context_new(&xmlParse, 0, (gpointer)pGmlcMscCfg, (GDestroyNotify)NULL);
	
	if (pGmlcMscCfg->pcServersFilename != NULL) {
		if (!g_file_get_contents(pGmlcMscCfg->pcServersFilename, &pcBuffer, &szBuffer, &gErr)) {
			g_printerr("Error servers file open : %s. -> read default configuration\n", gErr->message);
			g_clear_error(&gErr);
			pcBuffer = g_strdup(default_servers_conf);
			szBuffer = strlen(pcBuffer);
		}
	} else {
		g_printerr("Server file doesn't exist. -> read default configuration\n");
		pcBuffer = g_strdup(default_servers_conf);
		szBuffer = strlen(pcBuffer);
	}
	
	g_markup_parse_context_parse(xmlContext, pcBuffer, szBuffer, &gErr);
	g_markup_parse_context_end_parse (xmlContext, &gErr);
	
	g_markup_parse_context_free(xmlContext);
	
	return TRUE;
}

gboolean gmlc_misc_config_write_servers_file (GmlcMiscConfig * pGmlcMscCfg) {
	GmlcMysqlServer * pGmlcMysqlSrv = NULL;
	GString * xmlContent = NULL;
	GList * lstServersIdx = NULL;
	GError * gErr = NULL;
	gchar * pcXmlServer = NULL;
	
	if (pGmlcMscCfg == NULL) {
		return FALSE; 
	}
	
	if (pGmlcMscCfg->pcServersFilename == NULL) {
		return FALSE;
	}
	
	xmlContent = g_string_new("<?xml version=\"1.0\" ?>\n<servers>\n");
	
	lstServersIdx = g_list_first(pGmlcMscCfg->lstServers);
	
	while (lstServersIdx != NULL) {
		pGmlcMysqlSrv = GMLC_MYSQL_SERVER(lstServersIdx->data);
		
		pcXmlServer = gmlc_mysql_server_generate_xml_config(pGmlcMysqlSrv);
		g_string_append(xmlContent, pcXmlServer);
		g_free(pcXmlServer);
		
		lstServersIdx = g_list_next(lstServersIdx);
	}

	g_string_append(xmlContent, "</servers>\n");
	
	
	if (!g_file_set_contents(pGmlcMscCfg->pcServersFilename, xmlContent->str, xmlContent->len, &gErr)) {
		g_printerr("Error can't write servers file : %s\n", gErr->message);
		g_clear_error(&gErr);
		g_string_free(xmlContent, TRUE);
		return FALSE;
	}
	
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
	GmlcMiscConfig * pGmlcMscCfg = GMLC_MISC_CONFIG(user_data);

	if (g_ascii_strcasecmp(element_name, "servers") == 0 && xml_servers_read_state.state == X_IN_ROOT) {
		xml_servers_read_state.state = X_OUT_ROOT;
	} else if (g_ascii_strcasecmp(element_name, "server") == 0 && xml_servers_read_state.state == X_IN_SERVER) {
		gmlc_misc_config_add_server(pGmlcMscCfg, xml_servers_read_state.name->str, xml_servers_read_state.host->str, xml_servers_read_state.port, xml_servers_read_state.login->str, xml_servers_read_state.passwd->str, xml_servers_read_state.allowedDbs->str, xml_servers_read_state.localSock->str, xml_servers_read_state.read_only, xml_servers_read_state.write_warning);
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


