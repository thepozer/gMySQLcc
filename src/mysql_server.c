
#include "mysql_db_all.h"

#include <sys/stat.h>
#include <sys/types.h>

p_mysql_server mysql_server_new () {
	p_mysql_server mysql_srv;
	
	mysql_srv = (p_mysql_server) g_try_malloc(sizeof(s_mysql_server));
	
	if (mysql_srv == (p_mysql_server)NULL) {
		return (p_mysql_server)NULL; /* return NULL pointer */
	}
	
	mysql_srv->name = (gchar *) NULL;
	mysql_srv->host = (gchar *) NULL;
	mysql_srv->port = 3306;
	mysql_srv->user = (gchar *) NULL;
	mysql_srv->passwd = (gchar *) NULL;
	mysql_srv->allowedDbs = (gchar *) NULL;
	mysql_srv->localSock = (gchar *) NULL;
	mysql_srv->hshDbs = g_hash_table_new(&g_str_hash, &g_str_equal);
	
	return mysql_srv;
}

gboolean mysql_server_delete (p_mysql_server mysql_srv) {
	
	if (mysql_srv == (p_mysql_server)NULL) {
		return TRUE;
	}
	
	g_free(mysql_srv->name);
	g_free(mysql_srv->host);
	g_free(mysql_srv->user);
	g_free(mysql_srv->passwd);
	g_free(mysql_srv->allowedDbs);
	g_free(mysql_srv->localSock);
	
	mysql_server_clean_database_list(mysql_srv, FALSE);
	g_hash_table_destroy(mysql_srv->hshDbs);
	
	g_free(mysql_srv);
	return TRUE;
}

p_mysql_query mysql_server_query (p_mysql_server mysql_srv, const gchar * db_name) {
	return mysql_query_new(mysql_srv, db_name);
}

gboolean mysql_server_clean_database_list (p_mysql_server mysql_srv, gboolean only_not_found) {
	
	gboolean htr_remove_database(gpointer key, gpointer value, gpointer user_data) {
		p_mysql_database mysql_db = (p_mysql_database)value;
		
		if (!*((gboolean *)user_data) || (*((gboolean *)user_data) && !mysql_db->found)) {
			mysql_database_delete(mysql_db);
			return TRUE;
		}
		return FALSE;
	}
	
	if (mysql_srv->hshDbs != (GHashTable *) NULL) {
		g_hash_table_foreach_steal(mysql_srv->hshDbs, &htr_remove_database, (gpointer)&only_not_found);
	}
	
}

void mysql_server_mark_found_all_databases (p_mysql_server mysql_srv, gboolean found) {
	p_mysql_database mysql_db;

	void ht_mark_found_database(gpointer key, gpointer value, gpointer user_data) {
		((p_mysql_database)value)->found = *((gboolean *)user_data);
	}
	
	if (mysql_srv->hshDbs != (GHashTable *) NULL) {
		g_hash_table_foreach(mysql_srv->hshDbs, &ht_mark_found_database, (gpointer)&found);
	}
}
gboolean mysql_server_refresh_database_list (p_mysql_server mysql_srv) {
	p_mysql_query mysql_qry;
	p_mysql_database mysql_db;
	GArray * arRow;
	gchar * db_name;
	
	mysql_server_mark_found_all_databases(mysql_srv, FALSE);
	
	mysql_qry = mysql_server_query(mysql_srv, (gchar *)NULL);
	
	if (mysql_query_execute_query(mysql_qry, "SHOW DATABASES")) {
	
		arRow = mysql_query_get_next_record(mysql_qry);
		
		while (arRow != (GArray *)NULL) {
		
			db_name = g_array_index(arRow, gchar *, 0);
			if ((mysql_db = (p_mysql_database)g_hash_table_lookup(mysql_srv->hshDbs, db_name)) == (p_mysql_database)NULL) {
				mysql_db = mysql_database_new(mysql_srv, db_name);
				g_hash_table_insert(mysql_srv->hshDbs, db_name, mysql_db);
			} else {
				mysql_db->found = TRUE;
			}
			
			g_array_free(arRow, TRUE);
			arRow = mysql_query_get_next_record(mysql_qry);
			
		}
	} else {
		return FALSE;
	}
	
	mysql_server_clean_database_list(mysql_srv, TRUE);
	
	mysql_query_delete(mysql_qry);
	
	return TRUE;
}

p_mysql_database mysql_server_get_database (p_mysql_server mysql_srv, const gchar * db_name){
	return (p_mysql_database)g_hash_table_lookup(mysql_srv->hshDbs, db_name);
}

typedef struct _s_dump_svr_info {
	p_dump_server_params params;
	GString * strRet;
	GIOChannel * file;
	gchar * charset;
} s_dump_svr_info;

GString * mysql_server_dump (p_mysql_server mysql_srv, const p_dump_server_params params) {
	GString * strTmp;
	s_dump_svr_info dumpInfo;
	GIOChannel * sqlFile;
	GError * err = (GError *)NULL;
	gssize nbBytes;
		
	void ht_dump_database(gpointer key, gpointer value, gpointer user_data) {
		GString * strTmp;
		s_dump_svr_info * dumpInfo = (s_dump_svr_info *)user_data;
		p_mysql_database mysql_db = (p_mysql_database)value;
		
		mysql_database_refresh_table_list(mysql_db);
		
		if (dumpInfo->params->separate_file) {
			strTmp = g_string_new("");
			if (dumpInfo->params->group_in_directory) {
				g_string_printf(strTmp, "%s/%s/%s.sql", dumpInfo->params->base_directory, dumpInfo->params->group_directory, mysql_db->name);
			} else {
				g_string_printf(strTmp, "%s/%s.sql", dumpInfo->params->base_directory, mysql_db->name);
			}
		g_printf("Write separate files : %s\n", strTmp->str);
			dumpInfo->params->database.sql_filename = strTmp->str;
			mysql_database_dump(mysql_db, &dumpInfo->params->database);
			g_string_free(strTmp, TRUE);
		} else {
			dumpInfo->params->database.sql_filename = (gchar *)NULL;
			strTmp = mysql_database_dump(mysql_db, &dumpInfo->params->database);
			g_string_append(dumpInfo->strRet, strTmp->str);
			g_string_free(strTmp, TRUE);
		}
	}
	
	dumpInfo.params = params;
	dumpInfo.strRet = g_string_new("");
	
	if (params->separate_file && params->group_in_directory) {
		strTmp = g_string_new("");
		g_string_printf(strTmp, "%s/%s", params->base_directory, params->group_directory);
		g_printf("New directory : %s\n", strTmp->str);
		if (mkdir((char *)strTmp->str, 0755) != 0) {
			params->group_in_directory = FALSE;
		}
		g_string_free(strTmp, TRUE);
	}
	
	g_hash_table_foreach(mysql_srv->hshDbs, &ht_dump_database, (gpointer)&dumpInfo);
	
	if (!params->separate_file) {
		/* all databases in the same file */
		g_printf("Write unique file : %s\n", params->sql_filename);
		sqlFile = g_io_channel_new_file(params->sql_filename, "w", &err);
		g_io_channel_set_encoding(sqlFile, (gchar *)NULL, &err);
		g_io_channel_write_chars(sqlFile, dumpInfo.strRet->str, -1, &nbBytes, &err);
		g_io_channel_unref(sqlFile);
		g_string_free(dumpInfo.strRet, TRUE);
	}
	
	return (GString *)NULL;
}

gboolean mysql_server_dump_direct (p_mysql_server mysql_srv, const p_dump_server_params params, GIOChannel * file, const gchar * charset) {
	GString * strTmp;
	GError * err = (GError *)NULL;
	s_dump_svr_info dumpInfo;
		
	void ht_dump_database(gpointer key, gpointer value, gpointer user_data) {
		GString * strTmp;
		s_dump_svr_info * dumpInfo = (s_dump_svr_info *)user_data;
		p_mysql_database mysql_db = (p_mysql_database)value;
		
		mysql_database_refresh_table_list(mysql_db);
		
		if (dumpInfo->params->separate_file) {
			strTmp = g_string_new("");
			
			if (dumpInfo->params->group_in_directory) {
				g_string_printf(strTmp, "%s/%s/%s.sql", dumpInfo->params->base_directory, dumpInfo->params->group_directory, mysql_db->name);
			} else {
				g_string_printf(strTmp, "%s/%s.sql", dumpInfo->params->base_directory, mysql_db->name);
			}
			
			g_printf("Write separate files : %s\n", strTmp->str);
			dumpInfo->params->database.sql_filename = strTmp->str;
			
			mysql_database_dump_direct(mysql_db, &dumpInfo->params->database, (GIOChannel *)NULL, dumpInfo->charset);
			
			g_string_free(strTmp, TRUE);
		} else {
			mysql_database_dump_direct(mysql_db, &dumpInfo->params->database, dumpInfo->file, dumpInfo->charset);
		}
	}
	
	dumpInfo.params = params;
	dumpInfo.strRet = (GString *)NULL;
	dumpInfo.charset = (gchar *)charset;

	if (!params->separate_file && file == (GIOChannel *)NULL) {
		/* all databases in the same file */
		g_printf("Write unique file : %s\n", params->sql_filename);
		dumpInfo.file = g_io_channel_new_file(params->sql_filename, "w", &err);
		g_io_channel_set_encoding(dumpInfo.file, charset, &err);
	} else {
		dumpInfo.file = file;
	}
	
	if (params->separate_file && params->group_in_directory) {
		strTmp = g_string_new("");
		g_string_printf(strTmp, "%s/%s", params->base_directory, params->group_directory);
		g_printf("New directory : %s\n", strTmp->str);
		if (mkdir((char *)strTmp->str, 0755) != 0) {
			params->group_in_directory = FALSE;
		}
		g_string_free(strTmp, TRUE);
	}
	
	if (!params->separate_file) {
		/* all databases in the same file */
		g_printf("Write unique file : %s\n", params->sql_filename);
		dumpInfo.file = g_io_channel_new_file(params->sql_filename, "w", &err);
		g_io_channel_set_encoding(dumpInfo.file, charset, &err);
	}
	
	g_hash_table_foreach(mysql_srv->hshDbs, &ht_dump_database, (gpointer)&dumpInfo);
	
	if (!params->separate_file && file == (GIOChannel *)NULL) {
		g_io_channel_unref(dumpInfo.file);
	}
	
	return TRUE;
}
