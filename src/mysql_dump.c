
#include "mysql_db_all.h"

GString * addSlashes (const gchar * str) {
	GString * pgstr;
	int i;
	
	pgstr = g_string_new(str);
	
	for (i = 0; i < pgstr->len; i++) {
		if (pgstr->str[i] == '\'' || pgstr->str[i] == '\"') {
			g_string_insert_c(pgstr, i, '\\');
			i++;
		}
	}
	
	return pgstr;
}

GString * mysql_dump_database_struct (const gchar * db_name, gboolean drop_database, gboolean use_database) {
	GString * strRet;
	
	strRet = g_string_new("\n# Dump Database\n\n");
	
	if (drop_database) {
		g_string_append_printf(strRet, "DROP DATABASE IF EXISTS `%s`;\n", db_name);
	}
	g_string_append_printf(strRet, "CREATE DATABASE `%s`;\n", db_name);
	if (use_database) {
		g_string_append_printf(strRet, "USE %s ;\n", db_name);
	}

	return strRet;

}

GString * mysql_dump_table_struct (p_mysql_query mysql_qry, const gchar * tbl_name, gboolean drop_table) {
	GString * strRet, * strSql;
	GArray * arRow;
	
	strRet = g_string_new("\n# Dump structure\n\n");
	strSql = g_string_new("");
	
	mysql_query_free_query(mysql_qry);
	
	if (drop_table) {
		g_string_append_printf(strRet, "DROP TABLE IF EXISTS `%s`;\n", tbl_name);
	}
	g_string_printf(strSql, "SHOW CREATE TABLE `%s`.`%s`", mysql_qry->db_name, tbl_name);
	g_print("gmysql_dump_query_table - with_struct - sql : '%s'\n", strSql->str);
	if (mysql_query_execute_query(mysql_qry, strSql->str, FALSE)) {
		arRow = mysql_query_get_next_record(mysql_qry);
		if (arRow != (GArray *)NULL) {
			g_string_append(strRet, g_array_index(arRow, gchar *, 1));
			g_string_append(strRet, " ;\n");
		}
	}

	mysql_query_free_query(mysql_qry);
	g_string_free(strSql, TRUE);
	
	return strRet;
}

GString * mysql_dump_table_data (p_mysql_query mysql_qry, const gchar * tbl_name, gboolean complete_insert) {
	GString * strRet, * strInsert, * strSql, * tmpField;
	GArray * arRow;
	int i;
	
	strRet = g_string_new("\n# Dump datas\n\n");
	strInsert = g_string_new("");
	strSql = g_string_new("");

	mysql_query_free_query(mysql_qry);
	
	g_string_printf(strSql, "SELECT * FROM `%s`.`%s`", mysql_qry->db_name, tbl_name);
	g_print("gmysql_dump_query_table - with_data - sql : '%s'\n", strSql->str);
	
	if (mysql_query_execute_query(mysql_qry, strSql->str, FALSE)) {

		if (complete_insert) {
			g_string_printf(strInsert, "INSERT INTO `%s` (", tbl_name);
			arRow = mysql_query_get_headers(mysql_qry);
			for (i = 0; i < arRow->len; i++) {
					g_string_append_printf(strInsert, (i == 0) ? "`%s`" : ", `%s`" , g_array_index(arRow, gchar *, i));
			}
			g_string_append(strInsert, ")");
			g_array_free(arRow, TRUE);
		} else {
			g_string_printf(strInsert, "INSERT INTO `%s`", tbl_name);
		}
		
		arRow = mysql_query_get_next_record(mysql_qry);
		while (arRow != (GArray *)NULL) {
			g_string_append_printf(strRet, "%s VALUES (", strInsert->str);
			
			for (i = 0; i < arRow->len; i++) {
				tmpField = addSlashes(g_array_index(arRow, gchar *, i));
				g_string_append_printf(strRet, (i == 0) ? "'%s'" : ", '%s'" , tmpField->str);
				g_string_free(tmpField, TRUE);
			}
			
			g_string_append(strRet, ");\n");
			g_array_free(arRow, TRUE);
			arRow = mysql_query_get_next_record(mysql_qry);
		}
	}
	
	mysql_query_free_query(mysql_qry);
	
	return strRet;
}

GString * mysql_dump_query_csv (p_mysql_query mysql_qry) {
	GString * strRet, * tmpField;
	GArray * arRow;
	int i;
	
	strRet = g_string_new("");
	
	arRow = mysql_query_get_next_record(mysql_qry);
	while (arRow != (GArray *)NULL) {

		for (i = 0; i < arRow->len; i++) {
			tmpField = addSlashes(g_array_index(arRow, gchar *, i));
			g_string_append_printf(strRet, (i == 0) ? "\"%s\"" : ";\"%s\"" , tmpField->str);
			g_string_free(tmpField, TRUE);
		}
		
		g_string_append(strRet, "\n");

		g_array_free(arRow, TRUE);
		arRow = mysql_query_get_next_record(mysql_qry);
	}
	
	return strRet;
}

GString * mysql_dump_query_xml (p_mysql_query mysql_qry, gboolean bGlobal, gboolean bDatabase, gboolean bTable) {
	GString * strRet;
	
	strRet = g_string_new("");
	
	return strRet;
}

GString * mysql_dump_query_sql (p_mysql_query mysql_qry, gboolean complete_insert) {
	GString * strRet, * strInsert, * tmpField;
	GArray * arRow;
	gchar * tbl_name;
	int i;
	
	tbl_name = mysql_query_get_absolute_table_name(mysql_qry);
	if (tbl_name == (gchar *)NULL) {
		return (GString *)NULL;
	}
	
	strRet = g_string_new("\n# Dump datas\n\n");
	
	
	if (complete_insert) {
		g_string_printf(strInsert, "INSERT INTO `%s` (", tbl_name);
		arRow = mysql_query_get_headers(mysql_qry);
		for (i = 0; i < arRow->len; i++) {
				g_string_append_printf(strInsert, (i == 0) ? "`%s`" : ", `%s`" , g_array_index(arRow, gchar *, i));
		}
		g_string_append(strInsert, ")");
		g_array_free(arRow, TRUE);
	} else {
		g_string_printf(strInsert, "INSERT INTO `%s`", tbl_name);
	}
	
	arRow = mysql_query_get_next_record(mysql_qry);
	while (arRow != (GArray *)NULL) {
		g_string_append_printf(strRet, "%s VALUES (", strInsert->str);
		
		for (i = 0; i < arRow->len; i++) {
			tmpField = addSlashes(g_array_index(arRow, gchar *, i));
			g_string_append_printf(strRet, (i == 0) ? "'%s'" : ", '%s'" , tmpField->str);
			g_string_free(tmpField, TRUE);
		}
		
		g_string_append(strRet, ");\n");
		g_array_free(arRow, TRUE);
		arRow = mysql_query_get_next_record(mysql_qry);
	}
	
	return strRet;
}





gboolean mysql_dump_database_struct_direct (const gchar * db_name, gboolean drop_database, gboolean use_database, GIOChannel * file) {
	GString * strRet;
	GError * err = (GError *)NULL;
	gssize nbBytes;
	
	strRet = g_string_new("");
	
	g_io_channel_write_chars(file, "\n# Dump Database\n\n", -1, &nbBytes, &err);
	
	if (drop_database) {
		g_string_printf(strRet, "DROP DATABASE IF EXISTS `%s`;\n", db_name);
		g_io_channel_write_chars(file, strRet->str, -1, &nbBytes, &err);
	}
	
	g_string_printf(strRet, "CREATE DATABASE `%s`;\n", db_name);
	g_io_channel_write_chars(file, strRet->str, -1, &nbBytes, &err);
	
	if (use_database) {
		g_string_printf(strRet, "USE %s ;\n", db_name);
		g_io_channel_write_chars(file, strRet->str, -1, &nbBytes, &err);
	}

	return TRUE;

}

gboolean mysql_dump_table_struct_direct (p_mysql_query mysql_qry, const gchar * tbl_name, gboolean drop_table, GIOChannel * file) {
	GString * strRet, * strSql;
	GArray * arRow;
	GError * err = (GError *)NULL;
	gssize nbBytes;
	
	g_io_channel_write_chars(file, "\n# Dump structure\n\n", -1, &nbBytes, &err);
	strSql = g_string_new("");
	
	mysql_query_free_query(mysql_qry);
	
	if (drop_table) {
		strRet = g_string_new("");
		g_string_printf(strRet, "DROP TABLE IF EXISTS `%s`;\n", tbl_name);
		g_io_channel_write_chars(file, strRet->str, -1, &nbBytes, &err);
		g_string_free(strRet, TRUE);
	}
	
	g_string_printf(strSql, "SHOW CREATE TABLE `%s`.`%s`", mysql_qry->db_name, tbl_name);
	g_print("gmysql_dump_query_table - with_struct - sql : '%s'\n", strSql->str);
	if (mysql_query_execute_query(mysql_qry, strSql->str, FALSE)) {
		arRow = mysql_query_get_next_record(mysql_qry);
		if (arRow != (GArray *)NULL) {
			strRet = g_string_new(g_array_index(arRow, gchar *, 1));
			g_string_append(strRet, " ;\n");
			g_io_channel_write_chars(file, strRet->str, -1, &nbBytes, &err);
			g_string_free(strRet, TRUE);
		}
	}

	g_string_free(strSql, TRUE);
	mysql_query_free_query(mysql_qry);
	
	return TRUE;
}

gboolean mysql_dump_table_data_direct (p_mysql_query mysql_qry, const gchar * tbl_name, gboolean complete_insert, GIOChannel * file) {
	GString * strRet, * strInsert, * strSql, * tmpField;
	GArray * arRow;
	GError * err = (GError *)NULL;
	gssize nbBytes;
	int i;
	
	/*strRet = g_string_new("\n# Dump datas\n\n");*/
	g_io_channel_write_chars(file, "\n# Dump datas\n\n", -1, &nbBytes, &err);
	strInsert = g_string_new("");
	strSql = g_string_new("");

	mysql_query_free_query(mysql_qry);
	
	g_string_printf(strSql, "SELECT * FROM `%s`.`%s`", mysql_qry->db_name, tbl_name);
	g_print("gmysql_dump_query_table - with_data - sql : '%s'\n", strSql->str);
	
	if (mysql_query_execute_query(mysql_qry, strSql->str, FALSE)) {

		if (complete_insert) {
			g_string_printf(strInsert, "INSERT INTO `%s` (", tbl_name);
			arRow = mysql_query_get_headers(mysql_qry);
			for (i = 0; i < arRow->len; i++) {
					g_string_append_printf(strInsert, (i == 0) ? "`%s`" : ", `%s`" , g_array_index(arRow, gchar *, i));
			}
			g_string_append(strInsert, ")");
			g_array_free(arRow, TRUE);
		} else {
			g_string_printf(strInsert, "INSERT INTO `%s`", tbl_name);
		}
		
		strRet = g_string_new("");
		arRow = mysql_query_get_next_record(mysql_qry);
		while (arRow != (GArray *)NULL) {
			/*g_string_append_printf(strRet, "%s VALUES (", strInsert->str);*/
			g_string_assign(strRet, strInsert->str);
			g_string_append(strRet, " VALUES (");
			
			for (i = 0; i < arRow->len; i++) {
				tmpField = addSlashes(g_array_index(arRow, gchar *, i));
				g_string_append_printf(strRet, (i == 0) ? "'%s'" : ", '%s'" , tmpField->str);
				g_string_free(tmpField, TRUE);
			}
			
			g_string_append(strRet, ");\n");
			g_io_channel_write_chars(file, strRet->str, -1, &nbBytes, &err);
			
			g_array_free(arRow, TRUE);
			arRow = mysql_query_get_next_record(mysql_qry);
		}
		
		g_string_free(strRet, TRUE);
	}
	
	g_string_free (strInsert, TRUE);
	g_string_free (strSql, TRUE);
	mysql_query_free_query(mysql_qry);
	
	return TRUE;
}

gboolean mysql_dump_query_csv_direct (p_mysql_query mysql_qry, GIOChannel * file) {
	GString * strRet, * tmpField;
	GArray * arRow;
	GError * err = (GError *)NULL;
	gssize nbBytes;
	int i;
	
	strRet = g_string_new("");
	
	arRow = mysql_query_get_next_record(mysql_qry);
	while (arRow != (GArray *)NULL) {
		g_string_assign (strRet, "");

		for (i = 0; i < arRow->len; i++) {
			tmpField = addSlashes(g_array_index(arRow, gchar *, i));
			g_string_append_printf(strRet, (i == 0) ? "\"%s\"" : ";\"%s\"" , tmpField->str);
			g_string_free(tmpField, TRUE);
		}
		
		g_string_append(strRet, "\n");
		g_io_channel_write_chars(file, strRet->str, -1, &nbBytes, &err);

		g_array_free(arRow, TRUE);
		arRow = mysql_query_get_next_record(mysql_qry);
	}
	
	g_string_free (strRet, TRUE);
	
	return TRUE;
}

gboolean mysql_dump_query_xml_direct (p_mysql_query mysql_qry, GIOChannel * file, gboolean bGlobal, gboolean bDatabase, gboolean bTable, gboolean cdata) {
	GString * strXML;
	GArray * arRow, * arHeader;
	GError * err = (GError *)NULL;
	gssize nbBytes;
	gint i;
/*
<?xml version="1.0"?>
<mysqldump>
<database name="mysql">
        <table name="user">
        <row>
                <field name="Host">localhost</field>
                <field name="User">root</field>
                <field name="Password">6b8a9d6f4312ddb4</field>
        </row>
        </table>
</database>
</mysqldump>
*/
	strXML = g_string_new("");
	
	if (bGlobal) {
		g_string_append(strXML, "<?xml version=\"1.0\"?>\n<mysqldump>\n");
	}
	
	if (bDatabase) {
		g_string_append_printf(strXML, "\t<database name=\"%s\">\n", mysql_qry->db_name);
	}
	
	if (bTable) {
		if (mysql_qry->abs_tbl_name != NULL) {
			g_string_append_printf(strXML, "\t\t<table name=\"%s\">\n", mysql_qry->abs_tbl_name);
		} else {
			g_string_append(strXML, "\t\t<table>\n");
		}
	}
	
	if (strXML->len > 0) {
		g_io_channel_write_chars(file, strXML->str, -1, &nbBytes, &err);
		g_string_erase(strXML, 0, -1);
	}
	
	arHeader = mysql_query_get_headers(mysql_qry);

	arRow = mysql_query_get_next_record(mysql_qry);
	while (arRow != (GArray *)NULL) {
		
		g_string_assign (strXML, "\t\t\t<row>\n");

		for (i = 0; i < arRow->len; i++) {
			g_string_append_printf(strXML, (cdata) ? "\t\t\t\t<field name=\"%s\"><![CDATA[%s]]></field>\n" : "<field name=\"%s\">%s</field>\n" , 
				g_array_index(arHeader, gchar *, i), g_array_index(arRow, gchar *, i));
		}
		
		g_string_append(strXML, "</row>\n");
		g_io_channel_write_chars(file, strXML->str, -1, &nbBytes, &err);

		g_array_free(arRow, TRUE);
		arRow = mysql_query_get_next_record(mysql_qry);
	}
	
	g_string_erase(strXML, 0, -1);

	if (bTable) {
		g_string_append(strXML, "\t\t</table>\n");
	}
	
	if (bDatabase) {
		g_string_append(strXML, "\t</database>\n");
	}
	
	if (bGlobal) {
		g_string_append(strXML, "</mysqldump>\n");
	}
	
	if (strXML->len > 0) {
		g_io_channel_write_chars(file, strXML->str, -1, &nbBytes, &err);
		g_string_erase(strXML, 0, -1);
	}
	
	g_string_free(strXML, TRUE);
	return TRUE;
}

gboolean mysql_dump_query_sql_direct (p_mysql_query mysql_qry, gboolean complete_insert, GIOChannel * file) {
	GString * strRet, * strInsert, * tmpField;
	GArray * arRow;
	gchar * tbl_name;
	GError * err = (GError *)NULL;
	gssize nbBytes;
	int i;
	
	tbl_name = mysql_query_get_absolute_table_name(mysql_qry);
	if (tbl_name == (gchar *)NULL) {
		return FALSE;
	}
	
	g_io_channel_write_chars(file, "\n# Dump datas\n\n", -1, &nbBytes, &err);
	
	strInsert = g_string_new("");
	if (complete_insert) {
		g_string_printf(strInsert, "INSERT INTO `%s` (", tbl_name);
		arRow = mysql_query_get_headers(mysql_qry);
		for (i = 0; i < arRow->len; i++) {
				g_string_append_printf(strInsert, (i == 0) ? "`%s`" : ", `%s`" , g_array_index(arRow, gchar *, i));
		}
		g_string_append(strInsert, ")");
		g_array_free(arRow, TRUE);
	} else {
		g_string_printf(strInsert, "INSERT INTO `%s`", tbl_name);
	}
	
	strRet = g_string_new("");
	arRow = mysql_query_get_next_record(mysql_qry);
	while (arRow != (GArray *)NULL) {
		g_string_printf(strRet, "%s VALUES (", strInsert->str);
		
		for (i = 0; i < arRow->len; i++) {
			tmpField = addSlashes(g_array_index(arRow, gchar *, i));
			g_string_append_printf(strRet, (i == 0) ? "'%s'" : ", '%s'" , tmpField->str);
			g_string_free(tmpField, TRUE);
		}
		
		g_string_append(strRet, ");\n");
		g_io_channel_write_chars(file, strRet->str, -1, &nbBytes, &err);
		
		g_array_free(arRow, TRUE);
		arRow = mysql_query_get_next_record(mysql_qry);
	}
	
	g_string_free (strRet, TRUE);
	g_string_free (strInsert, TRUE);
	return TRUE;
}
