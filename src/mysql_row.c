
#include "mysql_db_all.h"


p_mysql_row mysql_row_new_next_record (p_mysql_query mysql_qry) {
	GArray * retData;
	GArray * retHeaders;
	p_mysql_row mysql_rw;
	
	retData = mysql_query_get_next_record(mysql_qry);
	
	if (retData == (GArray *)NULL) {
		return (p_mysql_row)NULL;
	}
	
	retHeaders = mysql_query_get_headers(mysql_qry);
	
	mysql_rw = (p_mysql_row) g_try_malloc(sizeof(s_mysql_row));
	
	if (mysql_rw == (p_mysql_row)NULL) {
		return (p_mysql_row)NULL;
	}
	
	mysql_rw->results = retData;
	mysql_rw->mysql_qry = mysql_qry;
	mysql_rw->abs_tbl_name = mysql_query_get_absolute_table_name(mysql_qry);
	if (mysql_rw->abs_tbl_name != (gchar *)NULL) {
		mysql_rw->primary_where_part = mysql_query_get_primary_where(mysql_qry, mysql_rw->results);
	} else {
		mysql_rw->primary_where_part = (gchar *)NULL;
	}
	
	return mysql_rw;
}

gboolean mysql_row_free (p_mysql_row mysql_rw) {
	guint i;
	
	for (i = 0; i < mysql_rw->results->len; i++) {
		g_free(g_array_index(mysql_rw->results, gchar *, i));
	}
	g_array_free(mysql_rw->results, TRUE);
	g_free(mysql_rw->abs_tbl_name);
	g_free(mysql_rw->primary_where_part);
	g_free(mysql_rw);
	
	return TRUE;
}

gboolean mysql_row_is_editable (p_mysql_row mysql_rw) {
	return (mysql_rw->abs_tbl_name != (gchar *) NULL);
}

gchar * mysql_row_get_field_value(p_mysql_row mysql_rw, int idx) {
	return g_array_index(mysql_rw->results, gchar *, idx);
}

gchar * mysql_row_set_field_value(p_mysql_row mysql_rw, int idx, const gchar * new_value) {
	p_mysql_query mysql_qry;
	MYSQL_FIELD * field;
	GString * strSql;
	gboolean ret;
	gchar * str;
	
	field = g_array_index(mysql_rw->mysql_qry->rawHeaders, MYSQL_FIELD *, idx);
	mysql_qry = mysql_query_duplicate(mysql_rw->mysql_qry);
	
	strSql = g_string_new ("UPDATE ");
	g_string_append_printf(strSql, "%s SET `%s` = '%s' WHERE %s", mysql_rw->abs_tbl_name, field->name, new_value, mysql_rw->primary_where_part);
	g_print("Update SQL : '%s'\n", strSql->str);
	
	ret = mysql_query_execute_query(mysql_qry, strSql->str, FALSE);
	mysql_query_delete(mysql_qry);
	
	g_string_free(strSql, TRUE);
	
	if (ret) {
		str = g_strdup(new_value);
		g_array_insert_val(mysql_rw->results, idx, str);
		g_array_remove_index(mysql_rw->results, idx + 1);
		return str;
	} else {
		return (gchar *)NULL;
	}
}

gboolean mysql_row_delete(p_mysql_row mysql_rw) {
	p_mysql_query mysql_qry;
	GString * strSql;
	gboolean ret;
	
	mysql_qry = mysql_query_duplicate(mysql_rw->mysql_qry);
	
	strSql = g_string_new ("");
	g_string_printf(strSql, "DELETE FROM %s WHERE %s", mysql_rw->abs_tbl_name, mysql_rw->primary_where_part);
	g_print("Delete SQL : '%s'\n", strSql->str);
	
	ret = mysql_query_execute_query(mysql_qry, strSql->str, FALSE);
	mysql_query_delete(mysql_qry);
	
	g_string_free(strSql, TRUE);
	
	return mysql_row_free(mysql_rw);
}
