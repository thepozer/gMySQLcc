
#include <sys/stat.h>
#include <sys/types.h>

#include "mysql_db_all.h"
#include "gmysqlcc_helpers.h"

gboolean mysql_dump_xml_server_to_disk (p_mysql_dump mysql_dmp);
gboolean mysql_dump_xml_database_to_disk (p_mysql_dump mysql_dmp);
gboolean mysql_dump_xml_table_to_disk (p_mysql_dump mysql_dmp);
gboolean mysql_dump_xml_query_to_disk (p_mysql_dump mysql_dmp);
gboolean mysql_dump_xml_struct_database_to_disk (p_mysql_dump mysql_dmp);
gboolean mysql_dump_xml_struct_table_to_disk (p_mysql_dump mysql_dmp);
gboolean mysql_dump_xml_data_query_to_disk (p_mysql_dump mysql_dmp);

/* Internal XML dump functions */

gboolean mysql_dump_xml_do_to_disk (p_mysql_dump mysql_dmp) {
	GError * err = (GError *)NULL;
	gssize nbBytes;
	
	if (mysql_dmp->filename == NULL) {
		return FALSE;
	}

	if (!mysql_dmp->svr_separate_file) {
		/* all datas in the same file */
		g_print("Write unique file : %s\n", mysql_dmp->filename);
		mysql_dmp->file = g_io_channel_new_file(mysql_dmp->filename, "w", &err);
		g_io_channel_set_encoding(mysql_dmp->file, mysql_dmp->charset, &err);
		g_io_channel_write_chars(mysql_dmp->file, "<?xml version=\"1.0\" ?>\n<mysqldump>\n", -1, &nbBytes, &err);
	}
	
	switch (mysql_dmp->level) {
		case DumpLevel_Server :
			return mysql_dump_xml_server_to_disk(mysql_dmp);
			break;
		case DumpLevel_Database :
			return mysql_dump_xml_database_to_disk(mysql_dmp);
			break;
		case DumpLevel_Table :
			return mysql_dump_xml_table_to_disk(mysql_dmp);
			break;
		case DumpLevel_Query :
			return mysql_dump_xml_query_to_disk(mysql_dmp);
			break;
		default :
			return FALSE;
			break;
	}
	
	if (!mysql_dmp->svr_separate_file && mysql_dmp->file != NULL) {
		g_io_channel_write_chars(mysql_dmp->file, "</mysqldump>\n", -1, &nbBytes, &err);
		g_io_channel_unref(mysql_dmp->file);
		mysql_dmp->file = NULL;
	}
	
	return TRUE;
}

gboolean mysql_dump_xml_server_to_disk (p_mysql_dump mysql_dmp) {
	GString * strTmp;
	
	void ht_dump_database(gpointer key, gpointer value, gpointer user_data) {
		GString * strTmp;
		p_mysql_dump mysql_dmp = (p_mysql_dump)user_data;
		p_mysql_database mysql_db = (p_mysql_database)value;
		GError * err = (GError *)NULL;
		gssize nbBytes;
		
		mysql_database_refresh_table_list(mysql_db);
		
		if (mysql_dmp->svr_separate_file) {
			strTmp = g_string_new("");
			
			if (mysql_dmp->svr_group_in_directory) {
				g_string_printf(strTmp, "%s/%s/%s.xml", mysql_dmp->svr_base_directory, mysql_dmp->svr_group_directory, mysql_dmp->mysql_db->name);
			} else {
				g_string_printf(strTmp, "%s/%s.xml", mysql_dmp->svr_base_directory, mysql_dmp->mysql_db->name);
			}
			
			g_print("Write separate files : %s\n", strTmp->str);
			mysql_dump_set_filename(mysql_dmp, strTmp->str, NULL);
			
			g_string_free(strTmp, TRUE);
			
			mysql_dmp->file = g_io_channel_new_file(mysql_dmp->filename, "w", &err);
			g_io_channel_set_encoding(mysql_dmp->file, mysql_dmp->charset, &err);
			g_io_channel_write_chars(mysql_dmp->file, "<?xml version=\"1.0\" ?>\n<mysqldump>\n", -1, &nbBytes, &err);
		}
		
		mysql_dump_set_database(mysql_dmp, mysql_db);
		
		mysql_dump_xml_database_to_disk(mysql_dmp);
		
		if (mysql_dmp->svr_separate_file) {
			g_io_channel_write_chars(mysql_dmp->file, "</mysqldump>\n", -1, &nbBytes, &err);
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

gboolean mysql_dump_xml_database_to_disk (p_mysql_dump mysql_dmp) {
	GString * strXml;
	GError * err = (GError *)NULL;
	gssize nbBytes;
	
	void htr_dump_table(gpointer key, gpointer value, gpointer user_data) {
		p_mysql_dump mysql_dmp = (p_mysql_dump)user_data;
		p_mysql_table mysql_tbl = (p_mysql_table)value;
		
		mysql_dump_set_table(mysql_dmp, mysql_tbl);
		
		mysql_dump_xml_table_to_disk(mysql_dmp);
	}


	strXml = g_string_new("");
	g_string_printf(strXml, "\t<database name=\"%s\">\n", mysql_dmp->mysql_db->name);
	g_io_channel_write_chars(mysql_dmp->file, strXml->str, -1, &nbBytes, &err);
	g_string_free(strXml, TRUE);
	
	if (!mysql_dump_xml_struct_database_to_disk (mysql_dmp)) {
		return FALSE;
	}
	
	g_hash_table_foreach(mysql_dmp->mysql_db->hshTables, &htr_dump_table, (gpointer)mysql_dmp);

	g_io_channel_write_chars(mysql_dmp->file, "\t</database>", -1, &nbBytes, &err);
	
	return TRUE;
}

gboolean mysql_dump_xml_table_to_disk (p_mysql_dump mysql_dmp) {
	gboolean bRet;
	GString * strSql, * strXml;
	GError * err = (GError *)NULL;
	gssize nbBytes;
	
	strXml = g_string_new("");
	
	if (mysql_dmp->level == DumpLevel_Table) {
		g_string_printf(strXml, "\t<database name=\"%s\">\n\t\t<table name=\"%s\">\n", mysql_dmp->mysql_db->name, mysql_dmp->mysql_tbl->name);
	} else {
		g_string_printf(strXml, "\t\t<table name=\"%s\">\n", mysql_dmp->mysql_tbl->name);
	}
	
	g_io_channel_write_chars(mysql_dmp->file, strXml->str, -1, &nbBytes, &err);
	g_string_free(strXml, TRUE);
	
	if (mysql_dmp->tbl_data) {
		strSql = g_string_new("");
		g_string_printf(strSql, "SELECT * FROM `%s`.`%s`", mysql_dmp->mysql_db->name, mysql_dmp->mysql_tbl->name);
		g_print("mysql_table_dump_direct - with_data - sql : '%s'\n", strSql->str);
		
		/* Set query string and query object */
		mysql_dump_set_query_string(mysql_dmp, strSql->str);
		
		g_string_free (strSql, TRUE);
		
		bRet = mysql_dump_xml_data_query_to_disk(mysql_dmp);
		
		/* Free query string and query object */
		mysql_dump_set_query_string(mysql_dmp, NULL);
	}
	
	if (mysql_dmp->level == DumpLevel_Table) {
		g_io_channel_write_chars(mysql_dmp->file, "\t\t</table>\n\t</database>\n", -1, &nbBytes, &err);
	} else {
		g_io_channel_write_chars(mysql_dmp->file, "\t\t</table>\n", -1, &nbBytes, &err);
	}
	
	return bRet;
}

gboolean mysql_dump_xml_query_to_disk (p_mysql_dump mysql_dmp) {
	gboolean bRet;
	GString * strXml;
	GError * err = (GError *)NULL;
	gssize nbBytes;
	
	strXml = g_string_new("");
	g_string_printf(strXml, "\t<database name=\"%s\">\n\t\t<table name=\"%s\">\n", mysql_dmp->mysql_db->name, mysql_dmp->mysql_tbl->name);
	g_io_channel_write_chars(mysql_dmp->file, strXml->str, -1, &nbBytes, &err);
	g_string_free(strXml, TRUE);
	
	g_print("mysql_dump_xml_query_to_disk - with_data - sql : '%s'\n", mysql_dmp->qry_string);
	
	bRet = mysql_dump_xml_data_query_to_disk(mysql_dmp);
	
	g_io_channel_write_chars(mysql_dmp->file, "\t\t</table>\n\t</database>\n", -1, &nbBytes, &err);
	
	return bRet;
}

gboolean mysql_dump_xml_struct_database_to_disk (p_mysql_dump mysql_dmp) {
	return TRUE;
}

gboolean mysql_dump_xml_struct_table_to_disk (p_mysql_dump mysql_dmp) {
	return TRUE;
}

gboolean mysql_dump_xml_data_query_to_disk (p_mysql_dump mysql_dmp) {
	p_mysql_query mysql_qry;
	GString * strRet, * fieldName, * fieldValue;
	GArray * arRow, * arHeaders;
	GError * err = (GError *)NULL;
	gssize nbBytes;
	int i;
	
	mysql_qry = mysql_table_query(mysql_dmp->mysql_tbl);

	g_print("mysql_dump_xml_data_query_to_disk - sql : '%s'\n", mysql_dmp->qry_string);
	
	if (mysql_query_execute_query(mysql_qry, mysql_dmp->qry_string, FALSE)) {

		arHeaders = mysql_query_get_headers(mysql_qry);
		
		strRet = g_string_new("");
		arRow = mysql_query_get_next_record(mysql_qry);
		while (arRow != (GArray *)NULL) {
			
			g_string_assign(strRet, "\t\t\t<row>\n");
			
			for (i = 0; i < arRow->len; i++) {
				fieldName = gmysqlcc_helpers_add_slashes(g_array_index(arRow, gchar *, i));
				fieldValue = g_string_new(g_array_index(arHeaders, gchar *, i));
				g_string_append_printf(strRet, "\t\t\t\t<field name=\"%s\">%s</field>\n", fieldName->str, fieldValue->str);
				g_string_free(fieldName, TRUE);
				g_string_free(fieldValue, TRUE);
			}
			
			g_string_append(strRet, "\t\t\t</row>\n");
			g_io_channel_write_chars(mysql_dmp->file, strRet->str, -1, &nbBytes, &err);
			
			g_array_free(arRow, TRUE);
			arRow = mysql_query_get_next_record(mysql_qry);
		}
		
		g_array_free(arHeaders, TRUE);
		g_string_free(strRet, TRUE);
	}
	
	mysql_query_delete(mysql_qry);
	
	return TRUE;
}

