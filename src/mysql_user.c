
#include "mysql_db_all.h"

p_mysql_user mysql_user_create (p_mysql_server mysql_srv, const gchar * login, const gchar * host, const gchar * password, gboolean crypted_password) {
	gboolean created = FALSE;
	GString * str_sql;
	p_mysql_query mysql_qry;
	p_mysql_user mysql_usr;
	
	str_sql = g_string_new("");
	mysql_qry = mysql_server_query(mysql_srv, "mysql");
	
	if (mysql_srv->version >= 50002) { /* Verison >= 5.0.2 */
		if (password != NULL) {
			if (!crypted_password) {
				g_string_printf(str_sql, "CREATE USER '%s'@'%s' IDENTIFIED BY '%s'", login, host, password);
			} else {
				g_string_printf(str_sql, "CREATE USER '%s'@'%s' IDENTIFIED BY PASSWORD '%s'", login, host, password);
			}
		} else {
			g_string_printf(str_sql, "CREATE USER '%s'@'%s'", login, host);
		}
		
		if (mysql_query_execute_query(mysql_qry, str_sql->str, FALSE)) {
			created = TRUE;
		}
		
	} else {
		if (password != NULL) {
			if (!crypted_password) {
				g_string_printf(str_sql, "INSERT INTO `mysql`.`user` (User, Host, Password) VALUES ('%s', '%s', PASSWORD('%s'))", login, host, password);
			} else {
				g_string_printf(str_sql, "INSERT INTO `mysql`.`user` (User, Host, Password) VALUES ('%s', '%s', '%s')", login, host, password);
			}
		} else {
			g_string_printf(str_sql, "INSERT INTO `mysql`.`user` (User, Host) VALUES ('%s', '%s')", login, host);
		}
		
		if (mysql_query_execute_query(mysql_qry, str_sql->str, FALSE)) {
			created = TRUE;
			g_string_printf(str_sql, "FLUSH PRIVILEGES");
			mysql_query_execute_query(mysql_qry, str_sql->str, FALSE);
		}
	}
	
	if (created) {
		mysql_usr = mysql_user_new(mysql_srv, login, host);
		mysql_user_update_from_db(mysql_usr);
	} else {
		mysql_usr = NULL;
	}
	
	g_string_free(str_sql, TRUE);
	mysql_query_delete(mysql_qry);

	return mysql_usr;
}

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

gchar * mysql_user_get_key (p_mysql_user mysql_usr) {
	GString * str_key;
	gchar * retKey;
	
	str_key = g_string_new("");
	g_string_printf(str_key, "'%s'@'%s'", mysql_usr->login, mysql_usr->host);
	retKey = g_strdup(str_key->str);
	g_string_free(str_key, TRUE);
	
	return retKey;
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

gboolean mysql_user_set_right (p_mysql_user mysql_usr, const gchar * right, const gchar * new_value) {
	p_mysql_query mysql_qry;
	GString * strSql;
	gboolean ret_update;

	mysql_qry = mysql_server_query(mysql_usr->mysql_srv, "mysql");
	
	strSql = g_string_new("");
	g_string_printf(strSql, "UPDATE `mysql`.`user` SET `%s`  = '%s' WHERE Host = '%s' AND User = '%s'", right, new_value, mysql_usr->host, mysql_usr->login);
	
	if (mysql_query_execute_query(mysql_qry, strSql->str, FALSE)) {
		mysql_user_read_rights(mysql_usr);
		ret_update = TRUE;
	} else {
		ret_update = FALSE;
	}
	
	g_string_free(strSql, TRUE);
	mysql_query_delete(mysql_qry);
	
	return ret_update;
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
			g_string_printf(str_sql, "SET PASSWORD FOR '%s'@'%s' = PASSWORD('%s')", mysql_usr->login, mysql_usr->host, new_password);
		} else {
			g_string_printf(str_sql, "SET PASSWORD FOR '%s'@'%s' = '%s'", mysql_usr->login, mysql_usr->host, new_password);
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

gboolean mysql_user_update_key_values (p_mysql_user mysql_usr, const gchar * new_login, const gchar * new_host) {
	gboolean updated = FALSE;
	GString * str_sql;
	p_mysql_query mysql_qry;
	
	str_sql = g_string_new("");
	mysql_qry = mysql_server_query(mysql_usr->mysql_srv, "mysql");
	
	g_string_printf(str_sql, "UPDATE mysql.user SET User = '%s', Host = '%s' WHERE User = '%s' AND Host = '%s'", new_login, new_host, mysql_usr->login, mysql_usr->host);
	if (mysql_query_execute_query(mysql_qry, str_sql->str, FALSE)) {
		updated = TRUE;
	}
	
	g_string_printf(str_sql, "FLUSH PRIVILEGES");
	mysql_query_execute_query(mysql_qry, str_sql->str, FALSE);
	
	if (updated) {
		g_free(mysql_usr->login);
		mysql_usr->login = g_strdup(new_login);
		g_free(mysql_usr->host);
		mysql_usr->host = g_strdup(new_host);
	}
	
	g_string_free(str_sql, TRUE);
	mysql_query_delete(mysql_qry);
	
	return updated;
}

gboolean mysql_user_remove (p_mysql_user mysql_usr) {
	gboolean deleted = FALSE;
	GString * str_sql;
	p_mysql_query mysql_qry;
	
	str_sql = g_string_new("");
	mysql_qry = mysql_server_query(mysql_usr->mysql_srv, "mysql");
	
	if (mysql_usr->mysql_srv->version >= 50002) { /* Verison >= 5.0.2 */
		g_string_printf(str_sql, "DROP USER '%s'@'%s' ", mysql_usr->login, mysql_usr->host);
		
		if (mysql_query_execute_query(mysql_qry, str_sql->str, FALSE)) {
			deleted = TRUE;
		}
		
	} else {
		deleted = TRUE;
		
		g_string_printf(str_sql, "DELETE FROM `mysql`.`user` WHERE User = '%s' AND Host = '%s'", mysql_usr->login, mysql_usr->host);
		if (deleted && !mysql_query_execute_query(mysql_qry, str_sql->str, FALSE)) {
			deleted = FALSE;
		}
		
		g_string_printf(str_sql, "DELETE FROM `mysql`.`db` WHERE User = '%s' AND Host = '%s'", mysql_usr->login, mysql_usr->host);
		if (deleted && !mysql_query_execute_query(mysql_qry, str_sql->str, FALSE)) {
			deleted = FALSE;
		}
		
		g_string_printf(str_sql, "DELETE FROM `mysql`.`tables_priv` WHERE User = '%s' AND Host = '%s'", mysql_usr->login, mysql_usr->host);
		if (deleted && !mysql_query_execute_query(mysql_qry, str_sql->str, FALSE)) {
			deleted = FALSE;
		}
		
		g_string_printf(str_sql, "DELETE FROM `mysql`.`columns_priv` WHERE User = '%s' AND Host = '%s'", mysql_usr->login, mysql_usr->host);
		if (deleted && !mysql_query_execute_query(mysql_qry, str_sql->str, FALSE)) {
			deleted = FALSE;
		}
		
		if (deleted) {
			g_string_printf(str_sql, "FLUSH PRIVILEGES");
			mysql_query_execute_query(mysql_qry, str_sql->str, FALSE);
		}
	}
	
	g_string_free(str_sql, TRUE);
	mysql_query_delete(mysql_qry);
	
	return deleted;
}
