
#include <sys/stat.h>
#include <sys/types.h>

#include "mysql_db_all.h"
#include "gmysqlcc_helpers.h"

gboolean mysql_dump_sql_server_to_disk (p_mysql_dump mysql_dmp);
gboolean mysql_dump_sql_database_to_disk (p_mysql_dump mysql_dmp);
gboolean mysql_dump_sql_table_to_disk (p_mysql_dump mysql_dmp);
gboolean mysql_dump_sql_query_to_disk (p_mysql_dump mysql_dmp);
gboolean mysql_dump_sql_struct_database_to_disk (p_mysql_dump mysql_dmp);
gboolean mysql_dump_sql_struct_table_to_disk (p_mysql_dump mysql_dmp);
gboolean mysql_dump_sql_data_query_to_disk (p_mysql_dump mysql_dmp);

/* Internal SQL dump functions */

gboolean mysql_dump_sql_do_to_disk (p_mysql_dump mysql_dmp) {
	GError * err = (GError *)NULL;
	gboolean ret;
	
	if (mysql_dmp->filename == NULL) {
		return FALSE;
	}

	if (!mysql_dmp->svr_separate_file) {
		/* all datas in the same file */
		g_print("Write unique file : %s\n", mysql_dmp->filename);
		mysql_dmp->file = g_io_channel_new_file(mysql_dmp->filename, "w", &err);
		g_io_channel_set_encoding(mysql_dmp->file, mysql_dmp->charset, &err);
	}
	
	switch (mysql_dmp->level) {
		case DumpLevel_Server :
			ret = mysql_dump_sql_server_to_disk(mysql_dmp);
			break;
		case DumpLevel_Database :
			ret = mysql_dump_sql_database_to_disk(mysql_dmp);
			break;
		case DumpLevel_Table :
			ret = mysql_dump_sql_table_to_disk(mysql_dmp);
			break;
		case DumpLevel_Query :
			ret = mysql_dump_sql_query_to_disk(mysql_dmp);
			break;
		default :
			ret = FALSE;
			break;
	}
	
	if (!mysql_dmp->svr_separate_file && mysql_dmp->file != NULL) {
		g_io_channel_unref(mysql_dmp->file);
		mysql_dmp->file = NULL;
	}
	
	return ret;
}

gboolean mysql_dump_sql_server_to_disk (p_mysql_dump mysql_dmp) {
	GString * strTmp;
	GError * err = (GError *)NULL;
	
	void ht_dump_database(gpointer key, gpointer value, gpointer user_data) {
		GString * strTmp;
		p_mysql_dump mysql_dmp = (p_mysql_dump)user_data;
		p_mysql_database mysql_db = (p_mysql_database)value;
		
		mysql_database_refresh_table_list(mysql_db);
		
		if (mysql_dmp->svr_separate_file) {
			strTmp = g_string_new("");
			
			if (mysql_dmp->svr_group_in_directory) {
				g_string_printf(strTmp, "%s/%s/%s.sql", mysql_dmp->svr_base_directory, mysql_dmp->svr_group_directory, mysql_dmp->mysql_db->name);
			} else {
				g_string_printf(strTmp, "%s/%s.sql", mysql_dmp->svr_base_directory, mysql_dmp->mysql_db->name);
			}
			
			g_print("Write separate files : %s\n", strTmp->str);
			mysql_dump_set_filename(mysql_dmp, strTmp->str, NULL);
			
			g_string_free(strTmp, TRUE);
			
			mysql_dmp->file = g_io_channel_new_file(mysql_dmp->filename, "w", &err);
			g_io_channel_set_encoding(mysql_dmp->file, mysql_dmp->charset, &err);
		}
		
		mysql_dump_set_database(mysql_dmp, mysql_db);
		
		mysql_dump_sql_database_to_disk(mysql_dmp);
		
		if (mysql_dmp->svr_separate_file) {
			g_io_channel_unref(mysql_dmp->file);
			mysql_dmp->file = NULL;
		}
	}
	
	if (mysql_dmp->svr_separate_file && mysql_dmp->svr_group_in_directory) {
		strTmp = g_string_new("");
		g_string_printf(strTmp, "%s/%s", mysql_dmp->svr_base_directory, mysql_dmp->svr_group_directory);
		g_print("New directory : %s\n", strTmp->str);
		if (mkdir((char *)strTmp->str, 0755) != 0) {
			mysql_dmp->svr_group_in_directory = FALSE;
		}
		g_string_free(strTmp, TRUE);
	}
	
	g_hash_table_foreach(mysql_dmp->mysql_svr->hshDbs, &ht_dump_database, (gpointer)mysql_dmp);
	
	return TRUE;
}

gboolean mysql_dump_sql_database_to_disk (p_mysql_dump mysql_dmp) {
	
	void htr_dump_table(gpointer key, gpointer value, gpointer user_data) {
		p_mysql_dump mysql_dmp = (p_mysql_dump)user_data;
		p_mysql_table mysql_tbl = (p_mysql_table)value;
		
		mysql_dump_set_table(mysql_dmp, mysql_tbl);
		
		mysql_dump_sql_table_to_disk(mysql_dmp);
	}
	
	if (!mysql_dump_sql_struct_database_to_disk (mysql_dmp)) {
		return FALSE;
	}
	
	g_hash_table_foreach(mysql_dmp->mysql_db->hshTables, &htr_dump_table, (gpointer)mysql_dmp);

	return TRUE;
}

gboolean mysql_dump_sql_table_to_disk (p_mysql_dump mysql_dmp) {
	gboolean bRet;
	GString * strSql;
	
	g_print("mysql_dump_sql_table_to_disk - table : '%s'\n", mysql_dmp->mysql_tbl->name);
	
	if (mysql_dmp->tbl_structure) {
		if (!mysql_dump_sql_struct_table_to_disk (mysql_dmp)) {
			return FALSE;
		}
	}
	
	if (mysql_dmp->tbl_data) {
		strSql = g_string_new("");
		g_string_printf(strSql, "SELECT * FROM `%s`.`%s`", mysql_dmp->mysql_db->name, mysql_dmp->mysql_tbl->name);
		g_print("mysql_table_dump_direct - with_data - sql : '%s'\n", strSql->str);
		
		/* Set query string and query object */
		mysql_dump_set_query_string(mysql_dmp, strSql->str);
		
		g_string_free (strSql, TRUE);
		
		bRet = mysql_dump_sql_data_query_to_disk(mysql_dmp);
		
		/* Free query string and query object */
		mysql_dump_set_query_string(mysql_dmp, NULL);
	}
	
	return bRet;
}

gboolean mysql_dump_sql_query_to_disk (p_mysql_dump mysql_dmp) {
	p_mysql_query mysql_qry;
	GString * strRet, * strInsert, * tmpField;
	GArray * arRow;
	GError * err = NULL;
	gsize nbBytes;
	gchar * s_absTableName = NULL;
	int i;
	
	mysql_qry = mysql_database_query(mysql_dmp->mysql_db);
	strInsert = g_string_new("");

	g_io_channel_write_chars(mysql_dmp->file, "\n# Dump datas\n\n", -1, &nbBytes, &err);
	
	g_print("mysql_dump_sql_query_to_disk - with_data : '%s'\n", mysql_dmp->qry_string);
	
	if (mysql_query_execute_query(mysql_qry, mysql_dmp->qry_string, FALSE)) {
		
		s_absTableName = mysql_query_get_absolute_table_name(mysql_qry, FALSE);
		
		if (s_absTableName == NULL) {
			s_absTableName = g_strdup("`DUMMY_DB`.`DUMMY_TBL`");
		}
		
		if (mysql_dmp->tbl_data_complete_insert) {
			g_string_printf(strInsert, "INSERT INTO %s (", s_absTableName);
			arRow = mysql_query_get_headers(mysql_qry);
			for (i = 0; i < arRow->len; i++) {
					g_string_append_printf(strInsert, (i == 0) ? "`%s`" : ", `%s`" , g_array_index(arRow, gchar *, i));
			}
			g_string_append(strInsert, ")");
			g_array_free(arRow, TRUE);
		} else {
			g_string_printf(strInsert, "INSERT INTO %s", s_absTableName);
		}
		
		strRet = g_string_new("");
		arRow = mysql_query_get_next_record(mysql_qry);
		while (arRow != (GArray *)NULL) {
			g_string_assign(strRet, strInsert->str);
			g_string_append(strRet, " VALUES (");
			
			for (i = 0; i < arRow->len; i++) {
				tmpField = gmysqlcc_helpers_add_slashes(g_array_index(arRow, gchar *, i));
				g_string_append_printf(strRet, (i == 0) ? "'%s'" : ", '%s'" , tmpField->str);
				g_string_free(tmpField, TRUE);
			}
			
			g_string_append(strRet, ");\n");
			g_io_channel_write_chars(mysql_dmp->file, strRet->str, -1, &nbBytes, &err);
			
			g_array_free(arRow, TRUE);
			arRow = mysql_query_get_next_record(mysql_qry);
		}
		
		g_string_free(strRet, TRUE);
		g_free(s_absTableName);
	}
	
	g_string_free (strInsert, TRUE);
	mysql_query_delete(mysql_qry);
	
	return TRUE;
}

gboolean mysql_dump_sql_struct_database_to_disk (p_mysql_dump mysql_dmp) {
	GString * strRet;
	GError * err = (GError *)NULL;
	gsize nbBytes;
	
	strRet = g_string_new("");
	
	g_io_channel_write_chars(mysql_dmp->file, "\n# Dump Database\n\n", -1, &nbBytes, &err);
	
	if (mysql_dmp->db_drop_database) {
		g_string_printf(strRet, "DROP DATABASE IF EXISTS `%s`;\n", mysql_dmp->mysql_db->name);
		g_io_channel_write_chars(mysql_dmp->file, strRet->str, -1, &nbBytes, &err);
	}
	
	g_string_printf(strRet, "CREATE DATABASE `%s`;\n", mysql_dmp->mysql_db->name);
	g_io_channel_write_chars(mysql_dmp->file, strRet->str, -1, &nbBytes, &err);
	
	if (mysql_dmp->db_use_database) {
		g_string_printf(strRet, "USE %s ;\n", mysql_dmp->mysql_db->name);
		g_io_channel_write_chars(mysql_dmp->file, strRet->str, -1, &nbBytes, &err);
	}
	
	g_string_free(strRet, TRUE);
	
	return TRUE;
}

gboolean mysql_dump_sql_struct_table_to_disk (p_mysql_dump mysql_dmp) {
	GString * strRet;
	GError * err = (GError *)NULL;
	gsize nbBytes;
	
	strRet = g_string_new("");
	
	g_io_channel_write_chars(mysql_dmp->file, "\n# Dump structure\n\n", -1, &nbBytes, &err);
	
	if (mysql_dmp->tbl_drop_table) {
		strRet = g_string_new("");
		g_string_printf(strRet, "DROP TABLE IF EXISTS `%s`;\n", mysql_dmp->mysql_tbl->name);
		g_io_channel_write_chars(mysql_dmp->file, strRet->str, -1, &nbBytes, &err);
		g_string_free(strRet, TRUE);
	}
	
	strRet = mysql_table_get_sql_structure(mysql_dmp->mysql_tbl);
	g_io_channel_write_chars(mysql_dmp->file, strRet->str, -1, &nbBytes, &err);
	g_string_free(strRet, TRUE);
	
	return TRUE;
}

gboolean mysql_dump_sql_data_query_to_disk (p_mysql_dump mysql_dmp) {
	p_mysql_query mysql_qry;
	GString * strRet, * strInsert, * tmpField;
	GArray * arRow;
	GError * err = (GError *)NULL;
	gsize nbBytes;
	int i;
	
	mysql_qry = mysql_table_query(mysql_dmp->mysql_tbl);
	strInsert = g_string_new("");

	g_io_channel_write_chars(mysql_dmp->file, "\n# Dump datas\n\n", -1, &nbBytes, &err);
	
	g_print("mysql_dump_sql_data_query_to_disk - sql : '%s'\n", mysql_dmp->qry_string);
	
	if (mysql_query_execute_query(mysql_qry, mysql_dmp->qry_string, FALSE)) {

		if (mysql_dmp->tbl_data_complete_insert) {
			g_string_printf(strInsert, "INSERT INTO `%s` (", mysql_dmp->mysql_tbl->name);
			arRow = mysql_query_get_headers(mysql_qry);
			for (i = 0; i < arRow->len; i++) {
					g_string_append_printf(strInsert, (i == 0) ? "`%s`" : ", `%s`" , g_array_index(arRow, gchar *, i));
			}
			g_string_append(strInsert, ")");
			g_array_free(arRow, TRUE);
		} else {
			g_string_printf(strInsert, "INSERT INTO `%s`", mysql_dmp->mysql_tbl->name);
		}
		
		strRet = g_string_new("");
		arRow = mysql_query_get_next_record(mysql_qry);
		while (arRow != (GArray *)NULL) {
			g_string_assign(strRet, strInsert->str);
			g_string_append(strRet, " VALUES (");
			
			for (i = 0; i < arRow->len; i++) {
				tmpField = gmysqlcc_helpers_add_slashes(g_array_index(arRow, gchar *, i));
				g_string_append_printf(strRet, (i == 0) ? "'%s'" : ", '%s'" , tmpField->str);
				g_string_free(tmpField, TRUE);
			}
			
			g_string_append(strRet, ");\n");
			g_io_channel_write_chars(mysql_dmp->file, strRet->str, -1, &nbBytes, &err);
			
			g_array_free(arRow, TRUE);
			arRow = mysql_query_get_next_record(mysql_qry);
		}
		
		g_string_free(strRet, TRUE);
	}
	
	g_string_free (strInsert, TRUE);
	mysql_query_delete(mysql_qry);
	
	return TRUE;
}

