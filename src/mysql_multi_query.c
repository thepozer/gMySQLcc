
#include "mysql_db_all.h"

#include <string.h>

gboolean mysql_multi_query_execute_query(p_mysql_multi_query mysql_mlt_qry, const char * query);

p_mysql_multi_query mysql_multi_query_new (p_mysql_database mysql_db) {
	p_mysql_multi_query mysql_mlt_qry;
	
	mysql_mlt_qry = (p_mysql_multi_query) g_try_malloc(sizeof(s_mysql_multi_query));
	
	if (mysql_mlt_qry == NULL) {
		return NULL;
	}
	
	mysql_mlt_qry->mysql_db = mysql_db;
	mysql_mlt_qry->status_callback = NULL;
	mysql_mlt_qry->status_user_data = NULL;
	mysql_mlt_qry->report = g_string_new("");
	
	mysql_mlt_qry->mysql_qry = mysql_database_query(mysql_db);
	if (mysql_mlt_qry->mysql_qry == NULL) {
		mysql_multi_query_delete(mysql_mlt_qry);
		return NULL;
	}
	
	return mysql_mlt_qry;
}

gboolean mysql_multi_query_delete(p_mysql_multi_query mysql_mlt_qry) {
	
	if (mysql_mlt_qry == NULL) {
		return TRUE;
	}
	
	g_string_free(mysql_mlt_qry->report, TRUE);
	
	mysql_query_delete(mysql_mlt_qry->mysql_qry);
	
	g_free(mysql_mlt_qry);
	
	return TRUE;
}

gboolean mysql_multi_query_from_file(p_mysql_multi_query mysql_mlt_qry, const char * filename, gboolean b_stop_on_error) {
	GString * content, * sql_line;
	GIOChannel * sqlFile;
	GError * err = (GError *)NULL;
	gboolean ret = TRUE;
	
	/* Read SQL file */
	content = g_string_new("");
	sql_line = g_string_new("");
	
	sqlFile = g_io_channel_new_file(filename, "r", &err);
	if (err != (GError *)NULL) {
		g_print("Error : '%s'\n", err->message);
		g_string_append_printf(mysql_mlt_qry->report, "Access to file error : '%s'\n", err->message);
		return FALSE;
	}
	
	if (sqlFile != (GIOChannel *)NULL) {
		while (g_io_channel_read_line_string(sqlFile, sql_line, (gsize *)NULL, &err) != G_IO_STATUS_EOF) {
			if (err != (GError *)NULL) {
				g_print("Error : '%s'\n", err->message);
				g_string_append_printf(mysql_mlt_qry->report, "Reading file error : '%s'\n", err->message);
			}
			g_string_append(content, sql_line->str);
		}
		g_io_channel_unref(sqlFile);
	}
	
	ret = mysql_multi_query_from_string(mysql_mlt_qry, content->str, b_stop_on_error);
	
	g_string_free(sql_line, TRUE);
	g_string_free(content, TRUE);
	
	return ret;
}

gboolean mysql_multi_query_from_string(p_mysql_multi_query mysql_mlt_qry, const char * content, gboolean b_stop_on_error) {
	GString * sql_query;
	gint32 pos = 0, step = 0, content_len = 0;
	gchar car, prevcar;
	gboolean ret = TRUE;
	
	/* Parse SQL content */
	content_len = strlen(content);
	sql_query = g_string_new("");
	car = 0;
	
	while (pos < content_len) {
		prevcar = car;
		car = content[pos];
		switch (step) {
			case 0:
				switch (car) {
					case '\'' :
						g_string_append_c(sql_query, car);
						step = 1;
						break;
					case '\"' :
						g_string_append_c(sql_query, car);
						step = 2;
						break;
					case ';' :
						step = 3;
						break;
					case '-' :
						step = 4;
						break;
					case '#' :
						step = 5;
						break;
					default :
						if (!(sql_query->len == 0 && (car == ' ' || car == '\t' || car == '\n'))) {
							g_string_append_c(sql_query, car);
						}
						break;
				}
				break;
			case 1:
				g_string_append_c(sql_query, car);
				if (car == '\'' && prevcar != '\\') {
					step = 0;
				}
				break;
			case 2:
				g_string_append_c(sql_query, car);
				if (car == '\"' && prevcar != '\\') {
					step = 0;
				}
				break;
			case 3:
				/* Execute query */
				if (!mysql_multi_query_execute_query(mysql_mlt_qry, sql_query->str) && b_stop_on_error) {
					/* Error on query and Stop on error flag set ... stop NOW !!! */
					pos = content_len;
					ret = FALSE;
					break;
				}
				
				/* Look to the next query */
				sql_query = g_string_assign(sql_query, "");
				car = prevcar;
				pos --;
				step = 0;
				break;
			case 4:
				if (car != '-') {
					g_string_append_c(sql_query, prevcar);
					g_string_append_c(sql_query, car);
					step = 0;
				} else {
					step = 5;
				}
				break;
			case 5:
				if (car == '\n') {
					step = 0;
				}
				break;
		}
		pos ++;
	}
	
	g_string_free(sql_query, TRUE);
	
	return ret;
}

gboolean mysql_multi_query_execute_query(p_mysql_multi_query mysql_mlt_qry, const char * query) {
	gboolean ret;
	
	ret = mysql_query_execute_query(mysql_mlt_qry->mysql_qry, query, TRUE);
	
	if (!ret) {
		mysql_mlt_qry->nbr_error ++;
		g_string_append_printf(mysql_mlt_qry->report, _("----\nError during the query '%s'\nError : (%d) %s\n"), query, mysql_mlt_qry->mysql_qry->errCode, mysql_mlt_qry->mysql_qry->errMsg);
	} else {
		mysql_mlt_qry->nbr_query ++;
		if (mysql_mlt_qry->mysql_qry->errCode == 0 && mysql_mlt_qry->mysql_qry->nbrField == 0) {
			g_string_append_printf(mysql_mlt_qry->report, _("Affected row : %d\n"), mysql_mlt_qry->mysql_qry->editResult);
		}
	}
	
	if (mysql_mlt_qry->status_callback != NULL) {
		(*mysql_mlt_qry->status_callback)(mysql_mlt_qry, ret, mysql_mlt_qry->status_user_data);
	}
	
	return TRUE;
}

const gchar * mysql_multi_query_get_report(p_mysql_multi_query mysql_mlt_qry) {
	return mysql_mlt_qry->report->str;
}

void mysql_multi_query_set_status_callback(p_mysql_multi_query mysql_mlt_qry, void (* callback) (p_mysql_multi_query mysql_mlt_qry, gboolean error, gpointer user_data), gpointer user_data) {
	mysql_mlt_qry->status_callback = callback;
	
	if (mysql_mlt_qry->status_callback != NULL) {
		mysql_mlt_qry->status_user_data = user_data;
	} else {
		mysql_mlt_qry->status_user_data = NULL;
	}
}

