
#include "mysql_db_all.h"

#include "gmysql_utils.h"

#include <errno.h>
#include <string.h>

p_mysql_query mysql_query_new(p_mysql_server mysql_srv, const gchar * db_name) {
	p_mysql_query mysql_qry;
	
	mysql_qry = (p_mysql_query) g_try_malloc(sizeof(s_mysql_query));
	
	if (mysql_qry == (p_mysql_query)NULL) {
		return (p_mysql_query)NULL;
	}
	
	mysql_qry->query = (gchar *)NULL;
	mysql_qry->nbrField = 0;
	mysql_qry->editResult = 0;
	mysql_qry->errCode = 0;
	mysql_qry->errMsg = (gchar *)NULL;
	mysql_qry->rawHeaders = (GArray *)NULL;
	mysql_qry->can_edit = TRUE;
	mysql_qry->mysql_srv = mysql_srv;
	mysql_qry->db_name = (gchar *)g_strdup(db_name);
	mysql_qry->abs_tbl_name = (gchar *)NULL;
	mysql_qry->mysql_link = (MYSQL *)NULL;
	mysql_qry->mysql_result = (MYSQL_RES *)NULL;
	mysql_qry->charset = (gchar *)NULL;
	mysql_qry->iconv_from = 0;
	mysql_qry->iconv_to = 0;
	
	mysql_qry->mysql_link = mysql_init((MYSQL *)NULL);
	if (mysql_qry->mysql_link == (MYSQL *)NULL) {
		mysql_qry->errCode = -1;
		mysql_qry->errMsg = "**** Can't prepare mysql connection ****";
		g_printerr("Failed to connect to database: Error (%d) : %s\n", mysql_qry->errCode, mysql_qry->errMsg);
		return mysql_qry;
	}
	
	if (!mysql_real_connect(mysql_qry->mysql_link, mysql_srv->host, mysql_srv->user, mysql_srv->passwd, db_name, mysql_srv->port, mysql_srv->localSock, 0)) {
		mysql_qry->errCode = mysql_errno(mysql_qry->mysql_link);
		mysql_qry->errMsg = (gchar *)mysql_error(mysql_qry->mysql_link);
		mysql_qry->mysql_link = (MYSQL *)NULL;
		g_printerr("Failed to connect to database: Error (%d) : %s\n", mysql_qry->errCode, mysql_qry->errMsg);
		return mysql_qry;
	}
	
	mysql_qry->charset = g_strdup(mysql_character_set_name(mysql_qry->mysql_link));
	
	mysql_qry->iconv_from = g_iconv_open("UTF-8", mysql_qry->charset);
	if (mysql_qry->iconv_from < 0) {
		g_printerr("Iconv from open error (%d) : '%s'\n", errno, strerror(errno));
	}
	
	mysql_qry->iconv_to = g_iconv_open(mysql_qry->charset, "UTF-8");
	if (mysql_qry->iconv_to < 0) {
		g_printerr("Iconv to open error (%d) : '%s'\n", errno, strerror(errno));
	}

	return mysql_qry;
}

gboolean mysql_query_delete(p_mysql_query mysql_qry) {
	
	if (mysql_qry == (p_mysql_query)NULL) {
		return TRUE;
	}
	
	mysql_query_free_query(mysql_qry);
	
	/* Close DB connection */
	if (mysql_qry->mysql_link != (MYSQL *)NULL) {
		mysql_close(mysql_qry->mysql_link);
	}
	
	/* Destroy iconv converter */
	if (mysql_qry->iconv_from < 0) {
		g_iconv_close(mysql_qry->iconv_from);
	}
	if (mysql_qry->iconv_to < 0) {
		g_iconv_close(mysql_qry->iconv_to);
	}
	
	/* Release pointers */
	if (mysql_qry->rawHeaders != (GArray *)NULL) {
		g_array_free(mysql_qry->rawHeaders, TRUE);
	}
	g_free(mysql_qry->abs_tbl_name);
	g_free(mysql_qry->db_name);
	g_free(mysql_qry->charset);
	
	g_free(mysql_qry);
	
	return TRUE;
}

p_mysql_query mysql_query_duplicate(p_mysql_query base_mysql_qry) {
	p_mysql_query mysql_qry;
	
	mysql_qry = (p_mysql_query) g_try_malloc(sizeof(s_mysql_query));
	
	if (mysql_qry == (p_mysql_query)NULL) {
		return (p_mysql_query)NULL;
	}
	
	mysql_qry->query = (gchar *)NULL;
	mysql_qry->nbrField = 0;
	mysql_qry->editResult = 0;
	mysql_qry->errCode = 0;
	mysql_qry->errMsg = (gchar *)NULL;
	mysql_qry->rawHeaders = (GArray *)NULL;
	mysql_qry->can_edit = base_mysql_qry->can_edit;
	mysql_qry->mysql_srv = base_mysql_qry->mysql_srv;
	mysql_qry->db_name = (gchar *)g_strdup(base_mysql_qry->db_name);
	mysql_qry->abs_tbl_name = (gchar *)NULL;
	mysql_qry->mysql_link = (MYSQL *)NULL;
	mysql_qry->mysql_result = (MYSQL_RES *)NULL;
	mysql_qry->charset = (gchar *)NULL;
	mysql_qry->iconv_from = 0;
	mysql_qry->iconv_to = 0;
	
	mysql_qry->mysql_link = mysql_init((MYSQL *)NULL);
	if (mysql_qry->mysql_link == (MYSQL *)NULL) {
		mysql_qry->errCode = -1;
		mysql_qry->errMsg = "**** Can't prepare mysql connection ****";
		g_printerr("Failed to connect to database: Error (%d) : %s\n", mysql_qry->errCode, mysql_qry->errMsg);
		return mysql_qry;
	}
	
	if (!mysql_real_connect(mysql_qry->mysql_link, mysql_qry->mysql_srv->host, mysql_qry->mysql_srv->user, mysql_qry->mysql_srv->passwd, mysql_qry->db_name, mysql_qry->mysql_srv->port, mysql_qry->mysql_srv->localSock, 0)) {
		mysql_qry->errCode = mysql_errno(mysql_qry->mysql_link);
		mysql_qry->errMsg = (gchar *)mysql_error(mysql_qry->mysql_link);
		mysql_qry->mysql_link = (MYSQL *)NULL;
		g_printerr("Failed to connect to database: Error (%d) : %s\n", mysql_qry->errCode, mysql_qry->errMsg);
		return mysql_qry;
	}
	
	mysql_qry->charset = g_strdup(mysql_character_set_name(mysql_qry->mysql_link));
	
	mysql_qry->iconv_from = g_iconv_open("UTF-8", mysql_qry->charset);
	if (mysql_qry->iconv_from < 0) {
		g_printerr("Iconv open error (%d) : '%s'\n", errno, strerror(errno));
	}
	
	mysql_qry->iconv_to = g_iconv_open(mysql_qry->charset, "UTF-8");
	if (mysql_qry->iconv_to < 0) {
		g_printerr("Iconv open error (%d) : '%s'\n", errno, strerror(errno));
	}
	
	return mysql_qry;
}

p_mysql_database mysql_query_get_database(p_mysql_query mysql_qry) {
	return mysql_server_get_database (mysql_qry->mysql_srv, mysql_qry->db_name);
}
gboolean mysql_query_execute_query(p_mysql_query mysql_qry, const gchar * query, gboolean user_query) {
	int errcode;
	
	if (mysql_qry == (p_mysql_query)NULL) {
		return FALSE;
	}
	
	if (mysql_qry->mysql_link == (MYSQL *)NULL) {
		return FALSE;
	}
	
	mysql_qry->query = gmysql_alloc_iconv(mysql_qry->iconv_to, query);
	
	/* Ping mysql host to reconnect if needeed */
	mysql_ping (mysql_qry->mysql_link);
	
	/* Clear old query infos */
	mysql_query_free_query(mysql_qry);
	
	/* Execute Query */
	errcode = mysql_real_query(mysql_qry->mysql_link, mysql_qry->query, strlen(mysql_qry->query));
	if (errcode != 0 ) {
		mysql_qry->errCode = mysql_errno(mysql_qry->mysql_link);
		mysql_qry->errMsg = (gchar *)mysql_error(mysql_qry->mysql_link);
		g_printerr("mysql_field_count - Failed to query 0 : '%s'\nError: (%d - %d) %s\n", query, errcode, mysql_qry->errCode, mysql_qry->errMsg);
		return FALSE;
	}
	
	mysql_qry->nbrField = mysql_field_count(mysql_qry->mysql_link);

	if (errcode != 0 && mysql_qry->nbrField < 0) {
		mysql_qry->errCode = mysql_errno(mysql_qry->mysql_link);
		mysql_qry->errMsg = (gchar *)mysql_error(mysql_qry->mysql_link);
		g_printerr("mysql_field_count - Failed to query : '%s'\nError: (%d - %d) %s\n", query, errcode, mysql_qry->errCode, mysql_qry->errMsg);
		return FALSE;
	} else if (errcode == 0 && mysql_qry->nbrField == 0) {
		mysql_qry->editResult = mysql_affected_rows(mysql_qry->mysql_link);
		mysql_qry->errCode = 0;
		mysql_qry->errMsg = (gchar *)NULL;
		return TRUE;
	}
	
	mysql_qry->mysql_result = mysql_use_result(mysql_qry->mysql_link);
	
	if (mysql_qry->mysql_result == (MYSQL_RES *)NULL) {
		mysql_qry->errCode = mysql_errno(mysql_qry->mysql_link);
		mysql_qry->errMsg = (gchar *)mysql_error(mysql_qry->mysql_link);
		g_printerr("mysql_use_result - Failed to use results : '%s'\nError: %s\n", query, mysql_qry->errMsg);
		return FALSE;
	}
	
	mysql_qry->errCode = 0;
	mysql_qry->errMsg = (gchar *)NULL;
	return TRUE;
}

gboolean mysql_query_free_query(p_mysql_query mysql_qry) {

	if (mysql_qry == (p_mysql_query)NULL) {
		return TRUE;
	}
	
	if (mysql_qry->mysql_result == (MYSQL_RES *)NULL) {
		return FALSE;
	}
	
	mysql_free_result(mysql_qry->mysql_result);
	g_free(mysql_qry->query);
	
	if (mysql_qry->rawHeaders != (GArray *)NULL) {
		g_array_free(mysql_qry->rawHeaders, TRUE);
		mysql_qry->rawHeaders = (GArray *)NULL;
	}
	
	mysql_qry->mysql_result = (MYSQL_RES *)NULL;
	mysql_qry->query = (gchar *)NULL;
	
	return TRUE;
}

GArray * mysql_query_get_headers(p_mysql_query mysql_qry) {
	MYSQL_FIELD *fields, *field;
	GArray * headers;
	gchar * tmpstr;
	int i = 0;
	
	if (mysql_qry == (p_mysql_query)NULL) {
		return (GArray *) NULL;
	}
	
	if (mysql_qry->nbrField == 0) {
		return (GArray *) NULL;
	}
	
	headers = g_array_sized_new (FALSE, TRUE, sizeof (char *), mysql_qry->nbrField);
	if (mysql_qry->rawHeaders != NULL) {

		/* List headers from array */
		for(i = 0; i < mysql_qry->rawHeaders->len; i++) {
			field = g_array_index(mysql_qry->rawHeaders, MYSQL_FIELD *, i);
			tmpstr = gmysql_alloc_iconv(mysql_qry->iconv_from, field->name);
			g_array_append_val (headers, tmpstr);
		}
		
	} else {
		
		mysql_qry->rawHeaders = g_array_sized_new (FALSE, TRUE, sizeof (char *), mysql_qry->nbrField);

		/* List headers from database */
		fields = mysql_fetch_fields(mysql_qry->mysql_result);
		
		for(i = 0; i < mysql_qry->nbrField; i++) {
			field = &fields[i];
			g_array_append_val (mysql_qry->rawHeaders, field);
			tmpstr = gmysql_alloc_iconv(mysql_qry->iconv_from, field->name);
			g_array_append_val (headers, tmpstr);
		}
	}
	
	return headers;
}

GArray * mysql_query_get_next_record(p_mysql_query mysql_qry) {
	MYSQL_ROW currRow;
	GArray * arRow = (GArray *) NULL;
	gchar * tmpstr;
	int i = 0;
	
	if (mysql_qry == (p_mysql_query)NULL) {
		return (GArray *) NULL;
	}
	
	if (mysql_qry->nbrField == 0) {
		return (GArray *) NULL;
	}
	
	currRow = mysql_fetch_row(mysql_qry->mysql_result);
	if (currRow == (MYSQL_ROW)NULL) {
		return (GArray *) NULL;
	}
	
	arRow = g_array_sized_new (FALSE, TRUE, sizeof (char *), mysql_qry->nbrField);
	
	for(i = 0; i < mysql_qry->nbrField; i++) {
		if (currRow[i] != (gchar *)NULL) {
			tmpstr = gmysql_alloc_iconv(mysql_qry->iconv_from, currRow[i]);
		} else {
			tmpstr = (gchar *)g_strdup("NULL");
		}
		
		g_array_append_val (arRow, tmpstr);
	}
	
	return arRow;

}

GArray * mysql_query_get_all_records(p_mysql_query mysql_qry) {
	GArray * arAllRows, * currRow;
	int cntr;
	if (mysql_qry == (p_mysql_query)NULL) {
		return (GArray *) NULL;
	}
	
	if (mysql_qry->nbrField == 0) {
		return (GArray *) NULL;
	}
	
	arAllRows = g_array_new (FALSE, TRUE, sizeof (GArray *));
	
	cntr = 0;
	currRow = mysql_query_get_next_record(mysql_qry);
	while (currRow != (GArray *)NULL) {
		g_array_append_val (arAllRows, currRow);
		cntr ++;
		currRow = mysql_query_get_next_record(mysql_qry);
	}
	
	if (cntr == 0) {
		return (GArray *) NULL;
	} else {
		return arAllRows;
	}
}

gchar * mysql_query_get_absolute_table_name (p_mysql_query mysql_qry) {
	MYSQL_FIELD * field;
	int i;
	GString * abs_tbl_name, * abs_tbl_name_old;
	GArray * hdrs;
	
	if (mysql_qry->abs_tbl_name != (gchar *)NULL) {
		return mysql_qry->abs_tbl_name;
	}

	if ((hdrs = mysql_query_get_headers(mysql_qry)) == (GArray *)NULL) {
		return (gchar *)NULL;
	}
	g_array_free(hdrs, TRUE);

	abs_tbl_name = g_string_new("");
	abs_tbl_name_old = (GString *) NULL;
	
	for(i = 0; i < mysql_qry->rawHeaders->len; i++) {
		
		field = g_array_index(mysql_qry->rawHeaders, MYSQL_FIELD *, i);
		g_string_printf(abs_tbl_name, "`%s`.`%s`", mysql_qry->db_name, field->table);
		
		if (abs_tbl_name_old != (GString *) NULL) {
			
			if (!g_string_equal(abs_tbl_name, abs_tbl_name_old)) {
				mysql_qry->abs_tbl_name = (gchar *)NULL;
				g_string_free(abs_tbl_name, TRUE);
				g_string_free(abs_tbl_name_old, TRUE);
				return (gchar *) NULL;
			}
			
		} else {
			abs_tbl_name_old = g_string_new(abs_tbl_name->str);
		}
		
	}
	
	mysql_qry->abs_tbl_name = abs_tbl_name->str;
	g_string_free(abs_tbl_name, FALSE);
	g_string_free(abs_tbl_name_old, TRUE);
	return mysql_qry->abs_tbl_name;
}

gchar * mysql_query_get_primary_where (p_mysql_query mysql_qry, GArray * datas) {
	MYSQL_FIELD * field;
	int i;
	GString * strWhere;
	gchar * retStr;
	GArray * hdrs;
	
	if ((hdrs = mysql_query_get_headers(mysql_qry)) == (GArray *)NULL) {
		return (gchar *)NULL;
	}
	g_array_free(hdrs, TRUE);
	
	strWhere = g_string_new("");
	
	for(i = 0; i < mysql_qry->rawHeaders->len; i++) {
		
		field = g_array_index(mysql_qry->rawHeaders, MYSQL_FIELD *, i);
		if (field->flags & PRI_KEY_FLAG) {
			if (strWhere->len == 0) {
				g_string_append_printf(strWhere, "`%s` = '%s'", field->name, g_array_index(datas, gchar *, i));
			} else {
				g_string_append_printf(strWhere, " AND `%s` = '%s'", field->name, g_array_index(datas, gchar *, i));
			}
		}
	}
	
	retStr = strWhere->str;
	g_string_free(strWhere, FALSE);
	return retStr;
}

gboolean mysql_query_is_editable (p_mysql_query mysql_qry) {
	return (mysql_qry->abs_tbl_name != (gchar *) NULL) && mysql_qry->can_edit;
}

gboolean mysql_query_set_can_edit (p_mysql_query mysql_qry, gboolean new_value) {
	gboolean old_value;
	
	old_value = mysql_qry->can_edit;
	mysql_qry->can_edit = new_value;
	
	return old_value;
}

gboolean mysql_query_get_can_edit (p_mysql_query mysql_qry) {
	return mysql_qry->can_edit;
}
gchar * mysql_query_get_charset(p_mysql_query mysql_qry) {

	if (mysql_qry == (p_mysql_query)NULL) {
		return (gchar *)NULL;
	}
	
	return mysql_qry->charset;
}

gboolean mysql_query_change_charset (p_mysql_query mysql_qry, const gchar * charset) {
	GIConv iconv_from;
	GIConv iconv_to;
	
	/* check mysql query object */
	if (mysql_qry == (p_mysql_query)NULL) {
		return FALSE;
	}
	
	/* Create iconv converter based on given charset */
	iconv_from = g_iconv_open("UTF-8", charset);
	if (iconv_from < 0) {
		g_printerr("Iconv from open error (%d) : '%s'\n", errno, strerror(errno));
		return FALSE;
	}
	
	iconv_to = g_iconv_open(charset, "UTF-8");
	if (iconv_to < 0) {
		g_printerr("Iconv to open error (%d) : '%s'\n", errno, strerror(errno));
		g_iconv_close(iconv_from);
		return FALSE;
	}
	
	/* Destroy iconv converter */
	g_free(mysql_qry->charset);
	if (mysql_qry->iconv_from < 0) {
		g_iconv_close(mysql_qry->iconv_from);
	}
	if (mysql_qry->iconv_to < 0) {
		g_iconv_close(mysql_qry->iconv_to);
	}
	
	/* Update converter */
	mysql_qry->charset = g_strdup(charset);
	mysql_qry->iconv_from = iconv_from;
	mysql_qry->iconv_to = iconv_to;
	
	return TRUE;
}
