#include <sys/stat.h>
#include <sys/types.h>

#include "mysql_db_all.h"

gboolean mysql_dump_do_to_disk_xml (p_mysql_dump mysql_dmp);

p_mysql_dump mysql_dump_new (e_dumpFormat format, e_dumpLevel level) {
	p_mysql_dump mysql_dmp;
	
	mysql_dmp = (p_mysql_dump) g_try_malloc(sizeof(s_mysql_dump));
	
	if (mysql_dmp == (p_mysql_dump)NULL) {
		return (p_mysql_dump)NULL; /* return NULL pointer */
	}
	
	mysql_dmp->format = format;
	mysql_dmp->level = level;
	
	mysql_dmp->filename = NULL;
	mysql_dmp->charset = NULL;
	mysql_dmp->file = NULL;
	
	mysql_dmp->mysql_svr = NULL;
	mysql_dmp->svr_separate_file = FALSE;
	mysql_dmp->svr_base_directory = NULL;
	mysql_dmp->svr_group_in_directory = FALSE;
	mysql_dmp->svr_group_directory = NULL;
	
	mysql_dmp->mysql_db = NULL;
	mysql_dmp->db_drop_database = FALSE;
	mysql_dmp->db_use_database = FALSE;
	
	mysql_dmp->mysql_tbl = NULL;
	mysql_dmp->tbl_drop_table = FALSE;
	mysql_dmp->tbl_structure = FALSE;
	mysql_dmp->tbl_data = FALSE;
	mysql_dmp->tbl_data_complete_insert = FALSE;
	
	mysql_dmp->mysql_qry = NULL;
	mysql_dmp->qry_string = NULL;
	return mysql_dmp;
}

gboolean mysql_dump_free (p_mysql_dump mysql_dmp) {
	
	if (mysql_dmp == (p_mysql_dump)NULL) {
		return TRUE;
	}
	
	g_free(mysql_dmp->filename);
	g_free(mysql_dmp->charset);
	g_free(mysql_dmp->svr_base_directory);
	g_free(mysql_dmp->svr_group_directory);
	mysql_query_delete(mysql_dmp->mysql_qry);
	g_free(mysql_dmp->qry_string);
	
	g_free(mysql_dmp);
	return TRUE;
}

gboolean mysql_dump_set_filename (p_mysql_dump mysql_dmp, const gchar * filename, const gchar * charset) {
	
	g_free(mysql_dmp->filename);
	mysql_dmp->filename = g_strdup(filename);
	if (mysql_dmp->file != NULL) {
		g_io_channel_unref(mysql_dmp->file);
		mysql_dmp->file = NULL;
	}
	
	if (charset != NULL) {
		g_free(mysql_dmp->charset);
		mysql_dmp->charset = g_strdup(charset);
	}
	
	return TRUE;
}

gboolean mysql_dump_set_params_server (p_mysql_dump mysql_dmp, p_mysql_server mysql_svr, gboolean separate_file, const gchar * base_directory, gboolean group_in_directory, const gchar * group_directory) {
	
	
	g_free(mysql_dmp->svr_base_directory);
	g_free(mysql_dmp->svr_group_directory);
	
	mysql_dmp->mysql_svr = mysql_svr;
	mysql_dmp->svr_separate_file = separate_file;
	mysql_dmp->svr_base_directory = g_strdup(base_directory);
	mysql_dmp->svr_group_in_directory = group_in_directory;
	mysql_dmp->svr_group_directory = g_strdup(group_directory);
	
	return TRUE;
}

gboolean mysql_dump_set_params_database (p_mysql_dump mysql_dmp, p_mysql_database mysql_db, gboolean drop_database, gboolean use_database) {
	
	mysql_dmp->mysql_db = mysql_db;
	mysql_dmp->db_drop_database = drop_database;
	mysql_dmp->db_use_database = use_database;
	
	return TRUE;
}

gboolean mysql_dump_set_database (p_mysql_dump mysql_dmp, p_mysql_database mysql_db) {
	
	mysql_dmp->mysql_db = mysql_db;
	
	return TRUE;
}

gboolean mysql_dump_set_params_table (p_mysql_dump mysql_dmp, p_mysql_table mysql_tbl, gboolean drop_table, gboolean structure, gboolean data, gboolean data_complete_insert) {
	
	mysql_dmp->mysql_tbl = mysql_tbl;
	mysql_dmp->tbl_drop_table = drop_table;
	mysql_dmp->tbl_structure = structure;
	mysql_dmp->tbl_data = data;
	mysql_dmp->tbl_data_complete_insert = data_complete_insert;
	
	return TRUE;
}

gboolean mysql_dump_set_table (p_mysql_dump mysql_dmp, p_mysql_table mysql_tbl) {
	
	mysql_dmp->mysql_tbl = mysql_tbl;
	
	return TRUE;
}

gboolean mysql_dump_set_query (p_mysql_dump mysql_dmp, p_mysql_query mysql_qry) {
	
	mysql_query_delete(mysql_dmp->mysql_qry);
	
	mysql_dmp->mysql_qry = mysql_qry;
	
	return TRUE;
}

gboolean mysql_dump_set_query_string (p_mysql_dump mysql_dmp, const gchar * qry_string) {
	
	g_free(mysql_dmp->qry_string);
	
	mysql_dmp->qry_string = g_strdup(qry_string);
	
	return TRUE;
}

gboolean mysql_dump_do_to_disk (p_mysql_dump mysql_dmp) {
	
	switch (mysql_dmp->format) {
		case DumpFormat_Sql :
			return mysql_dump_sql_do_to_disk(mysql_dmp);
			break;
		case DumpFormat_Xml :
			return mysql_dump_xml_do_to_disk(mysql_dmp);
			break;
		case DumpFormat_Csv :
			return mysql_dump_csv_do_to_disk(mysql_dmp);
			break;
		default :
			return FALSE;
			break;
	}
	
	return FALSE;
}

gchar * mysql_dump_do_to_memory (p_mysql_dump mysql_dmp) {
	
	switch (mysql_dmp->format) {
		case DumpFormat_Sql :
			break;
		case DumpFormat_Xml :
			break;
		case DumpFormat_Csv :
			return mysql_dump_csv_do_to_memory(mysql_dmp);
			break;
		default :
			return NULL;
			break;
	}
	
	return NULL;
}
