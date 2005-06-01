
#include "mysql_db_all.h"

p_mysql_user mysql_user_new (p_mysql_server mysql_srv, const gchar * login, const gchar * host) {
	p_mysql_user mysql_usr;
	
	mysql_usr = (p_mysql_user) g_try_malloc(sizeof(s_mysql_user));
	
	if (mysql_usr == NULL) {
		return NULL;
	}
	
	mysql_usr->mysql_srv = mysql_srv;
	mysql_usr->login = g_strdup(login);
	mysql_usr->host = g_strdup(host);
	mysql_usr->passwd = NULL;
	mysql_usr->hshRights = g_hash_table_new_full(&g_str_hash, &g_str_equal, &g_free, &g_free);

	return mysql_usr;
}

gboolean mysql_user_delete (p_mysql_user mysql_usr) {
	
	if (mysql_usr == NULL) {
		return TRUE;
	}
	
	g_free(mysql_usr->login);
	g_free(mysql_usr->host);
	g_free(mysql_usr->passwd);
	
	g_hash_table_destroy(mysql_usr->hshRights);
	
	g_free(mysql_usr);
	return TRUE;
}

gboolean mysql_user_read_rights (p_mysql_user mysql_usr) {
	p_mysql_query mysql_qry;
	GArray * arRow, * arHeaders;
	GString * strSql;
	gchar * right, * value;
	gint idx;
	
	g_hash_table_destroy(mysql_usr->hshRights);
	mysql_usr->hshRights = g_hash_table_new_full(&g_str_hash, &g_str_equal, &g_free, &g_free);
	
	mysql_qry = mysql_server_query(mysql_usr->mysql_srv, "mysql");
	
	strSql = g_string_new("");
	g_string_printf(strSql, "SELECT * FROM `mysql`.`user` WHERE Host = '%s' AND User = '%s'", mysql_usr->host, mysql_usr->login);
	
	if (mysql_query_execute_query(mysql_qry, strSql->str, FALSE)) {
	
		arRow = mysql_query_get_next_record(mysql_qry);
		arHeaders = mysql_query_get_headers(mysql_qry);
		
		if (arRow != NULL && arHeaders != NULL) {
			
			for (idx = 0; idx < arRow->len; idx++) {
				right = g_array_index(arHeaders, gchar *, idx);
				value = g_array_index(arRow, gchar *, idx);
				
				if (g_ascii_strncasecmp(right, "Password", 8) == 0) {
					mysql_usr->passwd = g_strdup(value);
				} else if (g_ascii_strncasecmp(right, "Host", 4) != 0 && g_ascii_strncasecmp(right, "User", 4) != 0) {
					g_hash_table_insert(mysql_usr->hshRights, g_strdup(right), g_strdup(value));
				}
			}
			
		}
		
		if (arHeaders != NULL) {
			g_array_free(arHeaders, TRUE);
		}
		if (arRow != NULL) {
			g_array_free(arRow, TRUE);
		}
	} else {
		return FALSE;
	}
	
	g_string_free(strSql, TRUE);
	mysql_query_delete(mysql_qry);
	
	return TRUE;
}

gboolean mysql_user_read_accesses (p_mysql_user mysql_usr) {
	return TRUE;
}

gboolean mysql_user_update_from_db (p_mysql_user mysql_usr) {
	mysql_user_read_rights(mysql_usr);
	mysql_user_read_accesses(mysql_usr);
	return TRUE;
}

gboolean mysql_user_set_password (p_mysql_user mysql_usr, const gchar * new_password, gboolean crypted) {
	gboolean updated = FALSE;
	GString * str_sql;
	p_mysql_query mysql_qry;
	
	str_sql = g_string_new("");
	mysql_qry = mysql_server_query(mysql_usr->mysql_srv, "mysql");
	
	if (mysql_usr->mysql_srv->version >= 40000) { /* Verison >= 4.0.0 */
		if (!crypted) {
			g_string_printf(str_sql, "SET PASSWORD '%s'@'%s' = PASSWORD('%s')", mysql_usr->login, mysql_usr->host, new_password);
		} else {
			g_string_printf(str_sql, "SET PASSWORD '%s'@'%s' = '%s'", mysql_usr->login, mysql_usr->host, new_password);
		}
		if (mysql_query_execute_query(mysql_qry, str_sql->str, FALSE)) {
			updated = TRUE;
		}
	} else {
		if (!crypted) {
			g_string_printf(str_sql, "UPDATE mysql.user SET Password = PASSWORD('%s') WHERE User = '%s' AND Host = '%s'", new_password, mysql_usr->login, mysql_usr->host);
		} else {
			g_string_printf(str_sql, "UPDATE mysql.user SET Password = '%s' WHERE User = '%s' AND Host = '%s'", new_password, mysql_usr->login, mysql_usr->host);
		}
		if (mysql_query_execute_query(mysql_qry, str_sql->str, FALSE)) {
			updated = TRUE;
		}
		g_string_printf(str_sql, "FLUSH PRIVILEGES");
		if (updated && mysql_query_execute_query(mysql_qry, str_sql->str, FALSE)) {
			updated = TRUE;
		} else if (updated && mysql_usr->passwd != NULL) {
			g_string_printf(str_sql, "UPDATE mysql.user SET Password = '%s' WHERE User = '%s' AND Host = '%s'", mysql_usr->passwd, mysql_usr->login, mysql_usr->host);
			mysql_query_execute_query(mysql_qry, str_sql->str, FALSE);
			updated = FALSE;
		} else {
			updated = FALSE;
		}
	}
	
	if (updated) {
		g_free(mysql_usr->passwd);
		mysql_usr->passwd = g_strdup(new_password);
	}
	
	g_string_free(str_sql, TRUE);
	mysql_query_delete(mysql_qry);
	
	return updated;
}
