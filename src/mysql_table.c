
#include "mysql_db_all.h"

p_mysql_table mysql_table_new(p_mysql_database mysql_db, const gchar * tbl_name) {
	p_mysql_table mysql_tbl;
	
	mysql_tbl = (p_mysql_table) g_try_malloc(sizeof(s_mysql_table));
	
	if (mysql_tbl == (p_mysql_table)NULL) {
		return (p_mysql_table)NULL; /* return NULL pointer */
	}
	
	mysql_tbl->mysql_db = mysql_db;
	mysql_tbl->name = g_strdup(tbl_name);
	mysql_tbl->nbrRow = (gchar *)NULL;
	mysql_tbl->size = (gchar *)NULL;
	mysql_tbl->type = (gchar *)NULL;
	
	return mysql_tbl;
}

gboolean mysql_table_delete(p_mysql_table mysql_tbl) {
	
	if (mysql_tbl == (p_mysql_table)NULL) {
		return TRUE;
	}
	
	g_free(mysql_tbl->name);
	g_free(mysql_tbl->nbrRow);
	g_free(mysql_tbl->size);
	g_free(mysql_tbl->type);

	g_free(mysql_tbl);
	return TRUE;
}

p_mysql_query mysql_table_query (p_mysql_table mysql_tbl) {
	return mysql_database_query(mysql_tbl->mysql_db);	
}

gboolean mysql_table_dump_direct (p_mysql_table mysql_tbl, const p_dump_table_params params, GIOChannel * file, const gchar * charset) {
	p_mysql_query mysql_qry;
	GString * strSql;
	GIOChannel * dumpFile;
	GError * err = (GError *)NULL;
	
	mysql_qry = mysql_table_query(mysql_tbl);
	
	if (file == (GIOChannel *)NULL) {
		dumpFile = g_io_channel_new_file(params->sql_filename, "w", &err);
		g_io_channel_set_encoding(dumpFile, charset, &err);
	} else {
		dumpFile = file;
	}
	
	if (params->structure) {
		if (!mysql_dump_table_struct_direct (mysql_qry, mysql_tbl->name, params->drop_table, dumpFile)) {
			return FALSE;
		}
	}
	
	if (params->data) {
		mysql_query_free_query(mysql_qry);
		strSql = g_string_new("");
		g_string_printf(strSql, "SELECT * FROM `%s`.`%s`", mysql_qry->db_name, mysql_tbl->name);
		g_print("mysql_table_dump_direct - with_data - sql : '%s'\n", strSql->str);
		if (mysql_query_execute_query(mysql_qry, strSql->str)) {
			if (!mysql_dump_query_sql_direct (mysql_qry, params->data_complete_insert, file)) {
				return FALSE;
			}
		}
		g_string_free (strSql, TRUE);
		mysql_query_free_query(mysql_qry);
	}
	
	mysql_query_delete(mysql_qry);
	
	if (file == (GIOChannel *)NULL) {
		g_io_channel_unref(dumpFile);		
}
	
	return TRUE;
}

GString * mysql_table_dump (p_mysql_table mysql_tbl, const p_dump_table_params params) {
	GString * strRet, * strTmp, *strSql;
	p_mysql_query mysql_qry;
	
	strRet = g_string_new("");
	mysql_qry = mysql_table_query(mysql_tbl);
	
	if (params->structure) {
		strTmp = mysql_dump_table_struct (mysql_qry, mysql_tbl->name, params->drop_table);
		g_string_append(strRet, strTmp->str);
		g_string_free(strTmp, TRUE);
	}
	
	if (params->data) {
		mysql_query_free_query(mysql_qry);
		strSql = g_string_new("");
		g_string_printf(strSql, "SELECT * FROM `%s`.`%s`", mysql_qry->db_name, mysql_tbl->name);
		g_print("mysql_table_dump - with_data - sql : '%s'\n", strSql->str);
		if (mysql_query_execute_query(mysql_qry, strSql->str)) {
			strTmp = mysql_dump_table_data (mysql_qry, mysql_tbl->name, params->data_complete_insert);
			g_string_append(strRet, strTmp->str);
			g_string_free(strTmp, TRUE);
		}
		g_string_free (strSql, TRUE);
		mysql_query_free_query(mysql_qry);
	}
	
	mysql_query_delete(mysql_qry);
	
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
gboolean mysql_table_set_status_infos (p_mysql_table mysql_tbl, guint64 nbrRow, guint64 size, const gchar * type) {
	return TRUE;
}
