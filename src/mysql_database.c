
#include "mysql_db_all.h"

p_mysql_database mysql_database_new(p_mysql_server mysql_srv, const gchar * db_name) {
	p_mysql_database mysql_db;
	
	mysql_db = (p_mysql_database) g_try_malloc(sizeof(s_mysql_database));
	
	if (mysql_db == (p_mysql_database)NULL) {
		return (p_mysql_database)NULL; /* return NULL pointer */
	}
	
	mysql_db->mysql_srv = mysql_srv;
	mysql_db->name = g_strdup(db_name);
	mysql_db->lstTables = (GList *) NULL;
	mysql_db->hshTables = g_hash_table_new(&g_str_hash, &g_str_equal);
	mysql_db->found = TRUE;
		
	return mysql_db;
}

gboolean mysql_database_delete(p_mysql_database mysql_db) {
	
	if (mysql_db == (p_mysql_database)NULL) {
		return TRUE;
	}
	
	mysql_database_clean_table_list(mysql_db);
	
	g_free(mysql_db->name);

	g_free(mysql_db);
	return TRUE;
	
}

p_mysql_query mysql_database_query (p_mysql_database mysql_db) {
	return mysql_query_new(mysql_db->mysql_srv, mysql_db->name);	
}

gboolean mysql_database_clean_table_list (p_mysql_database mysql_db) {
	
	gboolean htr_remove_table(gpointer key, gpointer value, gpointer user_data) {
		mysql_table_delete((p_mysql_table)value);
		return TRUE;
	}

	if (mysql_db->hshTables != (GHashTable *) NULL) {
		g_hash_table_foreach_steal(mysql_db->hshTables, &htr_remove_table, (gpointer)NULL);
		g_hash_table_destroy(mysql_db->hshTables);
	}
	
	return TRUE;
}

gboolean mysql_database_refresh_table_list (p_mysql_database mysql_db) {
	p_mysql_query mysql_qry;
	p_mysql_table mysql_tbl;
	GArray * arRow;
	GString * ssql;
	gchar * tbl_name;
	
	mysql_qry = mysql_database_query(mysql_db);
	
	ssql = g_string_new("");
	g_string_printf(ssql, "SHOW TABLE STATUS FROM `%s`", mysql_db->name);
	
	if (mysql_query_execute_query(mysql_qry, ssql->str)) {
	
		arRow = mysql_query_get_next_record(mysql_qry);
		
		while (arRow != (GArray *)NULL) {
		
			tbl_name = g_array_index(arRow, gchar *, 0);
			if ((mysql_tbl = (p_mysql_table)g_hash_table_lookup(mysql_db->hshTables, tbl_name)) == (p_mysql_table)NULL) {
				mysql_tbl = mysql_table_new(mysql_db, tbl_name);
				mysql_tbl->size = g_strdup(g_array_index(arRow, gchar *, 5));
				mysql_tbl->nbrRow = g_strdup(g_array_index(arRow, gchar *, 3));
				mysql_tbl->type = g_strdup(g_array_index(arRow, gchar *, 1));
				g_hash_table_insert(mysql_db->hshTables, tbl_name, mysql_tbl);
			}
			
			g_array_free(arRow, TRUE);
			arRow = mysql_query_get_next_record(mysql_qry);
		}
		
		g_string_free(ssql, TRUE);
		mysql_query_delete(mysql_qry);
		
		return TRUE;
	} else {
	
		g_string_free(ssql, TRUE);
		mysql_query_delete(mysql_qry);
		
		return FALSE;
	}
}

p_mysql_table mysql_database_get_table (p_mysql_database mysql_db, const gchar * tbl_name) {
	return (p_mysql_table)g_hash_table_lookup(mysql_db->hshTables, tbl_name);
}


typedef struct _s_dump_db_info {
	p_dump_database_params params;
	GString * strRet;
	GIOChannel * file;
	gchar * charset;
} s_dump_db_info;

GString * mysql_database_dump (p_mysql_database mysql_db, const p_dump_database_params params) {
	GString * strRet, * strTmp;
	s_dump_db_info dumpInfo;

	void htr_dump_table(gpointer key, gpointer value, gpointer user_data) {
		GString * strTmp;
		s_dump_db_info * dumpInfo = (s_dump_db_info *)user_data;
		
		strTmp = mysql_table_dump((p_mysql_table)value, &dumpInfo->params->table);
		g_string_append(dumpInfo->strRet, strTmp->str);
		g_string_free(strTmp, TRUE);
	}
	
	strRet = g_string_new("");
	
	strTmp = mysql_dump_database_struct(mysql_db->name, params->drop_database, params->use_database);
	g_string_append(strRet, strTmp->str);
	g_string_free(strTmp, TRUE);
	
	dumpInfo.params = params;
	dumpInfo.strRet = strRet;

	g_hash_table_foreach(mysql_db->hshTables, &htr_dump_table, (gpointer)&dumpInfo);

	if (params->sql_filename != (gchar *)NULL) {
		GIOChannel * sqlFile;
		GError * err = (GError *)NULL;
		gssize nbBytes;
		
		sqlFile = g_io_channel_new_file(params->sql_filename, "w", &err);
		g_io_channel_set_encoding(sqlFile, (gchar *)NULL, &err);
		g_io_channel_write_chars(sqlFile, strRet->str, -1, &nbBytes, &err);
		g_io_channel_unref(sqlFile);
		g_string_free(strRet, TRUE);
		return (GString *)NULL;
	} else {
		return strRet;
	}
}

gboolean mysql_database_dump_direct (p_mysql_database mysql_db, const p_dump_database_params params, GIOChannel * file, const gchar * charset) {
	GIOChannel * dumpFile;
	GError * err = (GError *)NULL;
	s_dump_db_info dumpInfo;

	void htr_dump_table(gpointer key, gpointer value, gpointer user_data) {
		s_dump_db_info * dumpInfo = (s_dump_db_info *)user_data;
		
		mysql_table_dump_direct((p_mysql_table)value, &dumpInfo->params->table, dumpInfo->file, dumpInfo->charset);
	}
	
	if (file == (GIOChannel *)NULL) {
		dumpFile = g_io_channel_new_file(params->sql_filename, "w", &err);
		g_io_channel_set_encoding(dumpFile, charset, &err);
	} else {
		dumpFile = file;
	}
	
	if (!mysql_dump_database_struct_direct (mysql_db->name, params->drop_database, params->use_database, dumpFile)) {
		return FALSE;
	}
	
	dumpInfo.params = params;
	dumpInfo.strRet = (GString *)NULL;
	dumpInfo.file = dumpFile;
	dumpInfo.charset = (gchar *)charset;

	g_hash_table_foreach(mysql_db->hshTables, &htr_dump_table, (gpointer)&dumpInfo);

	if (file == (GIOChannel *)NULL) {
		g_io_channel_unref(dumpFile);		
	}
	
	return TRUE;
}
