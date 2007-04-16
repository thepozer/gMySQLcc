
#include "gmlc_mysql_server.h"
#include "gmlc_mysql_database.h"
#include "gmlc_mysql_query.h"

#include "gmlc_misc_hashtable.h"

static void gmlc_mysql_server_finalize (GmlcMysqlServer * pGmlcMysqlSrv);
static void gmlc_mysql_server_set_property (GObject * object, guint prop_id, const GValue * value, GParamSpec * pspec);
static void gmlc_mysql_server_get_property (GObject * object, guint prop_id, GValue * value, GParamSpec * pspec);

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

G_DEFINE_TYPE (GmlcMysqlServer, gmlc_mysql_server, G_TYPE_OBJECT);

static void gmlc_mysql_server_class_init (GmlcMysqlServerClass * pClass) {
	GObjectClass *pObjClass = G_OBJECT_CLASS(pClass);
	
	pObjClass->finalize = (GObjectFinalizeFunc) gmlc_mysql_server_finalize;
	
	pObjClass->get_property = gmlc_mysql_server_get_property;
	pObjClass->set_property = gmlc_mysql_server_set_property;
	
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
	
	
}

static void gmlc_mysql_server_init (GmlcMysqlServer * pGmlcMysqlSrv) {
	pGmlcMysqlSrv->pcName = NULL;
	pGmlcMysqlSrv->pcHost = NULL;
	pGmlcMysqlSrv->iPort = 3306;
	pGmlcMysqlSrv->pcLogin = NULL;
	pGmlcMysqlSrv->pcPassword = NULL;
	pGmlcMysqlSrv->pcLocalSock = NULL;
	pGmlcMysqlSrv->pcAllowedDbs = NULL;
	pGmlcMysqlSrv->bReadOnly = FALSE;
	pGmlcMysqlSrv->bWriteWarning = FALSE;
	pGmlcMysqlSrv->lVersion = 0;
	pGmlcMysqlSrv->htbDatabases = g_hash_table_new_full(&g_str_hash, &g_str_equal, &g_free, &g_object_unref);
}

static void gmlc_mysql_server_finalize (GmlcMysqlServer * pGmlcMysqlSrv) {
	g_hash_table_destroy(pGmlcMysqlSrv->htbDatabases);
	g_free(pGmlcMysqlSrv->pcName);
	g_free(pGmlcMysqlSrv->pcHost);
	g_free(pGmlcMysqlSrv->pcLogin);
	g_free(pGmlcMysqlSrv->pcPassword);
	g_free(pGmlcMysqlSrv->pcLocalSock);
	g_free(pGmlcMysqlSrv->pcAllowedDbs);
}

static void gmlc_mysql_server_set_property (GObject * object, guint prop_id, const GValue * value, GParamSpec * pspec) {
	GmlcMysqlServer * pGmlcMysqlSrv = GMLC_MYSQL_SERVER(object);
	
	switch (prop_id) {
		case PROP_NAME :
			g_free(pGmlcMysqlSrv->pcName);
			pGmlcMysqlSrv->pcName = g_value_dup_string(value);
			break;
		case PROP_HOST :
			g_free(pGmlcMysqlSrv->pcHost);
			pGmlcMysqlSrv->pcHost = g_value_dup_string(value);
			break;
		case PROP_PORT :
			pGmlcMysqlSrv->iPort = g_value_get_uint (value);
			break;
		case PROP_LOGIN :
			g_free(pGmlcMysqlSrv->pcLogin);
			pGmlcMysqlSrv->pcLogin = g_value_dup_string(value);
			break;
		case PROP_PASSWORD :
			g_free(pGmlcMysqlSrv->pcPassword);
			pGmlcMysqlSrv->pcPassword = g_value_dup_string(value);
			break;
		case PROP_LOCAL_SOCK :
			g_free(pGmlcMysqlSrv->pcLocalSock);
			pGmlcMysqlSrv->pcLocalSock = g_value_dup_string(value);
			break;
		case PROP_ALLOWED_DBS :
			g_free(pGmlcMysqlSrv->pcAllowedDbs);
			pGmlcMysqlSrv->pcAllowedDbs = g_value_dup_string(value);
			break;
		case PROP_READ_ONLY :
			pGmlcMysqlSrv->bReadOnly = g_value_get_boolean(value);
			break;
		case PROP_WRITE_WARNING :
			pGmlcMysqlSrv->bWriteWarning = g_value_get_boolean(value);
			break;
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
			break;
	}
}

static void gmlc_mysql_server_get_property (GObject * object, guint prop_id, GValue * value, GParamSpec * pspec) {
	GmlcMysqlServer * pGmlcMysqlSrv = GMLC_MYSQL_SERVER(object);
	
	switch (prop_id) {
		case PROP_NAME :
			g_value_set_string(value, pGmlcMysqlSrv->pcName);
			break;
		case PROP_HOST :
			g_value_set_string(value, pGmlcMysqlSrv->pcHost);
			break;
		case PROP_PORT :
			g_value_set_uint(value, pGmlcMysqlSrv->iPort);
			break;
		case PROP_LOGIN :
			g_value_set_string(value, pGmlcMysqlSrv->pcLogin);
			break;
		case PROP_PASSWORD :
			g_value_set_string(value, pGmlcMysqlSrv->pcPassword);
			break;
		case PROP_LOCAL_SOCK :
			g_value_set_string(value, pGmlcMysqlSrv->pcLocalSock);
			break;
		case PROP_ALLOWED_DBS :
			g_value_set_string(value, pGmlcMysqlSrv->pcAllowedDbs);
			break;
		case PROP_READ_ONLY :
			g_value_set_boolean(value, pGmlcMysqlSrv->bReadOnly);
			break;
		case PROP_WRITE_WARNING :
			g_value_set_boolean(value, pGmlcMysqlSrv->bWriteWarning);
			break;
		case PROP_VERSION :
			if (pGmlcMysqlSrv->lVersion == 0) {
				GmlcMysqlQuery * pGmlcMysqlQry = NULL;
				pGmlcMysqlQry = gmlc_mysql_query_new(G_OBJECT(pGmlcMysqlSrv), NULL);
				pGmlcMysqlSrv->lVersion = gmlc_mysql_query_get_version(pGmlcMysqlQry);
				g_object_unref(G_OBJECT(pGmlcMysqlQry));
			}
			g_value_set_ulong(value, pGmlcMysqlSrv->lVersion);
			break;
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
			break;
	}
}

GmlcMysqlServer * gmlc_mysql_server_new (void) {
	return GMLC_MYSQL_SERVER(g_object_new (GMLC_TYPE_MYSQL_SERVER, NULL));
}

gchar * gmlc_mysql_server_generate_xml_config(GmlcMysqlServer * pGmlcMysqlSrv) {
	GString * xmlContent = NULL;
	gchar * pcRet = NULL;
	
	xmlContent = g_string_new("\t<server>\n");
	g_string_append_printf(xmlContent, "\t\t<name><![CDATA[%s]]></name>\n", pGmlcMysqlSrv->pcName);
	g_string_append_printf(xmlContent, "\t\t<host><![CDATA[%s]]></host>\n", pGmlcMysqlSrv->pcHost);
	g_string_append_printf(xmlContent, "\t\t<port>%d</port>\n", pGmlcMysqlSrv->iPort);
	g_string_append_printf(xmlContent, "\t\t<login><![CDATA[%s]]></login>\n", pGmlcMysqlSrv->pcLogin);
	g_string_append_printf(xmlContent, "\t\t<passwd><![CDATA[%s]]></passwd>\n", pGmlcMysqlSrv->pcPassword);
	
	if (pGmlcMysqlSrv->pcAllowedDbs != NULL) {
		g_string_append_printf(xmlContent, "\t\t<alloweddbs><![CDATA[%s]]></alloweddbs>\n", pGmlcMysqlSrv->pcAllowedDbs);
	} else {
		g_string_append_printf(xmlContent, "\t\t<alloweddbs><![CDATA[]]></alloweddbs>\n");
	}
	
	if (pGmlcMysqlSrv->pcLocalSock != NULL) {
		g_string_append_printf(xmlContent, "\t\t<localsock><![CDATA[%s]]></localsock>\n", pGmlcMysqlSrv->pcLocalSock);
	} else {
		g_string_append_printf(xmlContent, "\t\t<localsock><![CDATA[]]></localsock>\n");
	}
	
	g_string_append(xmlContent, "\t\t<options>\n");
	if (pGmlcMysqlSrv->bReadOnly) {
		g_string_append_printf(xmlContent, "\t\t\t<read_only/>\n");
	}
	if (pGmlcMysqlSrv->bWriteWarning) {
		g_string_append_printf(xmlContent, "\t\t\t<write_warning/>\n");
	}
	g_string_append(xmlContent, "\t\t</options>\n\t</server>\n");

	pcRet = g_strdup(xmlContent->str);
	g_string_free(xmlContent, TRUE);
	
	return pcRet;
}

GArray * gmlc_mysql_server_databases_name_list (GmlcMysqlServer * pGmlcMysqlSrv, gboolean bUpdateList) {
	GArray * arList = NULL;
	
	void internal_gmlc_mysql_server_database_name_list_append_name (gpointer key, gpointer value, gpointer user_data) {
		gchar * pcName = (gchar *) key;
		GArray * arList = (GArray *) user_data;
		UNUSED_VAR(value);
		
		g_array_append_val(arList, pcName);
	}
	
	if (bUpdateList) {
		gmlc_mysql_server_update_databases_list(pGmlcMysqlSrv);
	}
	
	arList = g_array_new(TRUE, TRUE, sizeof(gchar *));
	g_hash_table_foreach(pGmlcMysqlSrv->htbDatabases, &internal_gmlc_mysql_server_database_name_list_append_name, arList);
	
	return arList;
}

gboolean gmlc_mysql_server_update_databases_list(GmlcMysqlServer * pGmlcMysqlSrv) {
	GmlcMysqlDatabase * pGmlcMysqlDb = NULL;
	GmlcMysqlQuery * pGmlcMysqlQry = NULL;
	GArray * arListDb = NULL;
	GArray * arRow = NULL;
	gchar * pcName = NULL;
	gint i = 0;
	
	pGmlcMysqlQry = gmlc_mysql_query_new (G_OBJECT(pGmlcMysqlSrv), NULL);
	
	if (gmlc_mysql_query_execute(pGmlcMysqlQry, "SHOW DATABASES;", 15, TRUE)) {
		arListDb = g_array_new(TRUE, TRUE, sizeof(gchar *));
		while ((arRow = gmlc_mysql_query_next_record(pGmlcMysqlQry)) != NULL) {
			pcName = g_array_index(arRow, gchar *, 0);
			g_array_append_val(arListDb, pcName);
			
			g_array_free(arRow, TRUE);
		}
	}
	
	g_object_unref(pGmlcMysqlQry);
	
	if (arListDb != NULL) {
		gmlc_misc_hash_table_set_all_flag(pGmlcMysqlSrv->htbDatabases, FALSE);
		
		for (i = 0; i < arListDb->len; i++) {
			pcName = g_array_index(arListDb, gchar *, i);
			pGmlcMysqlDb = GMLC_MYSQL_DATABASE(g_hash_table_lookup(pGmlcMysqlSrv->htbDatabases, pcName));
			
			if (pGmlcMysqlDb == NULL) {
				pGmlcMysqlDb = gmlc_mysql_database_new(pGmlcMysqlSrv, pcName);
				g_hash_table_insert(pGmlcMysqlSrv->htbDatabases, g_strdup(pcName), pGmlcMysqlDb);
			}
			g_object_set(G_OBJECT(pGmlcMysqlDb), "flagged", TRUE, NULL);
			
			g_free(pcName);
		}
		
		gmlc_misc_hash_table_remove_unflag_object(pGmlcMysqlSrv->htbDatabases);
		
		g_array_free(arListDb, TRUE);
	}
	
	return TRUE;
}

GmlcMysqlDatabase * gmlc_mysql_server_get_database(GmlcMysqlServer * pGmlcMysqlSrv, const gchar * pcName) {
	
	g_return_val_if_fail(pcName != NULL, NULL);
	
	return GMLC_MYSQL_DATABASE(g_hash_table_lookup(pGmlcMysqlSrv->htbDatabases, pcName));
}
