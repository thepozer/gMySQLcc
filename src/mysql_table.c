
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

GString * mysql_table_get_sql_structure (p_mysql_table mysql_tbl) {
	p_mysql_query mysql_qry;
	GString * strRet, * strSql;
	GArray * arRow;
	
	strRet = g_string_new("\n# Dump structure\n\n");
	
	strSql = g_string_new("");
	g_string_printf(strSql, "SHOW CREATE TABLE `%s`.`%s`", mysql_tbl->mysql_db->name, mysql_tbl->name);
	g_print("mysql_table_get_sql_structure - sql : '%s'\n", strSql->str);

	mysql_qry = mysql_table_query(mysql_tbl);
	
	if (mysql_query_execute_query(mysql_qry, strSql->str, FALSE)) {
		arRow = mysql_query_get_next_record(mysql_qry);
		if (arRow != (GArray *)NULL) {
			g_string_append(strRet, g_array_index(arRow, gchar *, 1));
			g_string_append(strRet, " ;\n");
		}
		g_array_free(arRow, TRUE);
	}

	mysql_query_delete(mysql_qry);
	g_string_free(strSql, TRUE);
	
	return strRet;
}
