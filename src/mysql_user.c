
#include "mysql_db_all.h"

void internal_mysql_user_list_clean_value (gpointer data);

/* mysql user list functions */

p_data_list mysql_user_list_new (p_mysql_server	mysql_srv) {
	return mysql_data_list_new(mysql_srv, &g_free, &internal_mysql_user_list_clean_value);
}

gboolean mysql_user_list_delete (p_data_list mysql_usr_lst) {
	return mysql_data_list_delete(mysql_usr_lst);
}

gboolean mysql_user_list_refresh (p_data_list mysql_usr_lst) {
	p_mysql_query mysql_qry;
	p_mysql_user mysql_usr;
	p_mysql_server mysql_srv;
	GArray * arRow;
	gchar * login;
	gchar * host;
	GString * userName;
	
	void htr_remove_user(gpointer key, gpointer value, gpointer user_data) {
		p_mysql_user mysql_usr = (p_mysql_user)value;
		
		mysql_user_delete(mysql_usr);
	}
	
	userName = g_string_new("");
	mysql_srv = (p_mysql_server) mysql_usr_lst->data;
	
	mysql_data_list_mark_all(mysql_usr_lst, FALSE);
	
	mysql_qry = mysql_server_query(mysql_srv, (gchar *)NULL);
	
	if (mysql_query_execute_query(mysql_qry, "SELECT Host, User AS UserName FROM `mysql`.`user` ORDER BY Host, User", FALSE)) {
		
		arRow = mysql_query_get_next_record(mysql_qry);
		
		while (arRow != NULL) {
			mysql_usr = NULL;
			login = g_array_index(arRow, gchar *, 1);
			host = g_array_index(arRow, gchar *, 0);
			
			g_string_printf(userName, "'%s'@'%s'", login, host);
			
			if ((mysql_usr = (p_mysql_user)mysql_data_list_get(mysql_usr_lst, userName->str)) == NULL) {
				mysql_usr = mysql_user_new(mysql_srv, login, host);
				mysql_data_list_add(mysql_usr_lst, g_strdup(userName->str), mysql_usr, TRUE);
			} else {
				mysql_data_list_mark(mysql_usr_lst, userName->str, TRUE);
			}
			
			mysql_user_update_from_db(mysql_usr);
			
			g_array_free(arRow, TRUE);
			arRow = mysql_query_get_next_record(mysql_qry);
			
		}
	} else {
		return FALSE;
	}
	
	mysql_data_list_clean_mark(mysql_usr_lst, FALSE);
	
	mysql_query_delete(mysql_qry);
	g_string_free(userName, TRUE);
	
	return TRUE;
}

void internal_mysql_user_list_clean_value (gpointer data) {
	p_data_list_item data_item = (p_data_list_item)data;
	
	mysql_user_delete((p_mysql_user)data_item->data);
	g_free(data_item);
}

/* mysql user functions */

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
	mysql_usr->user_rights = NULL;
	mysql_usr->hsh_db_list_rights = g_hash_table_new_full(&g_str_hash, &g_str_equal, &g_free, (GDestroyNotify)&mysql_right_delete);
	mysql_usr->hsh_tbl_list_rights = g_hash_table_new_full(&g_str_hash, &g_str_equal, &g_free, (GDestroyNotify)&mysql_right_delete);
	mysql_usr->hsh_tbl_col_list_rights = g_hash_table_new_full(&g_str_hash, &g_str_equal, &g_free, (GDestroyNotify)&mysql_right_delete);
	
	return mysql_usr;
}

gboolean mysql_user_delete (p_mysql_user mysql_usr) {
	
	if (mysql_usr == NULL) {
		return TRUE;
	}
	
	g_free(mysql_usr->login);
	g_free(mysql_usr->host);
	g_free(mysql_usr->passwd);
	
	mysql_right_delete(mysql_usr->user_rights);
	g_hash_table_destroy(mysql_usr->hsh_db_list_rights);
	
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
	
	mysql_right_delete(mysql_usr->user_rights);
	mysql_usr->user_rights = mysql_right_new_user(mysql_usr->mysql_srv, mysql_usr->host, mysql_usr->login);
	
	return (mysql_usr->user_rights != NULL);
}

gboolean mysql_user_set_right (p_mysql_user mysql_usr, const gchar * right, const gchar * new_value) {
	return mysql_right_update(mysql_usr->user_rights, right, new_value);
}

gboolean mysql_user_update_from_db (p_mysql_user mysql_usr) {
	mysql_user_read_rights(mysql_usr);
	mysql_user_read_database_rights(mysql_usr);
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

gboolean mysql_user_read_database_rights (p_mysql_user mysql_usr) {
	p_mysql_query mysql_qry;
	p_mysql_right mysql_rght;
	GArray * arRow;
	GString * strSql;
	gchar * name;
	
	g_hash_table_destroy(mysql_usr->hsh_db_list_rights);
	mysql_usr->hsh_db_list_rights = g_hash_table_new_full(&g_str_hash, &g_str_equal, &g_free, (GDestroyNotify)&mysql_right_delete);

	mysql_qry = mysql_server_query(mysql_usr->mysql_srv, "mysql");
	
	strSql = g_string_new("");
	g_string_printf(strSql, "SELECT Db FROM `mysql`.`db` WHERE Host = '%s' AND User = '%s' ORDER BY Db", mysql_usr->host, mysql_usr->login);
	
	if (mysql_query_execute_query(mysql_qry, strSql->str, FALSE)) {
		while ((arRow = mysql_query_get_next_record(mysql_qry)) != NULL) {
			name = (gchar *) g_array_index(arRow, gchar *, 0);
			mysql_rght = mysql_right_new_database(mysql_usr->mysql_srv, mysql_usr->host, mysql_usr->login, name);
			
			if (mysql_rght != NULL) {
				g_hash_table_insert(mysql_usr->hsh_db_list_rights, g_strdup(name), mysql_rght);
			}
			
			g_array_free(arRow, TRUE);
		}
	}
	
	g_string_free(strSql, TRUE);
	mysql_query_delete(mysql_qry);
	
	return TRUE;
}

gboolean mysql_user_read_table_rights (p_mysql_user mysql_usr) {
	p_mysql_query mysql_qry;
	p_mysql_right mysql_rght;
	GArray * arRow;
	GString * strSql, * str_key;
	gchar * db_name, * tbl_name;
	
	g_hash_table_destroy(mysql_usr->hsh_tbl_list_rights);
	mysql_usr->hsh_tbl_list_rights = g_hash_table_new_full(&g_str_hash, &g_str_equal, &g_free, (GDestroyNotify)&mysql_right_delete);

	g_hash_table_destroy(mysql_usr->hsh_tbl_col_list_rights);
	mysql_usr->hsh_tbl_col_list_rights = g_hash_table_new_full(&g_str_hash, &g_str_equal, &g_free, (GDestroyNotify)&mysql_right_delete);
	
	mysql_qry = mysql_server_query(mysql_usr->mysql_srv, "mysql");
	
	str_key = g_string_new("");
	strSql = g_string_new("");
	g_string_printf(strSql, "SELECT Db, Table_name FROM `mysql`.`tables_priv` WHERE Host = '%s' AND User = '%s' ORDER BY Db, Table_name", mysql_usr->host, mysql_usr->login);
	
	if (mysql_query_execute_query(mysql_qry, strSql->str, FALSE)) {
		while ((arRow = mysql_query_get_next_record(mysql_qry)) != NULL) {
			db_name = (gchar *) g_array_index(arRow, gchar *, 0);
			tbl_name = (gchar *) g_array_index(arRow, gchar *, 1);
			g_string_printf(str_key, "`%s`.`%s`", db_name, tbl_name);
			
			mysql_rght = mysql_right_new_table(mysql_usr->mysql_srv, mysql_usr->host, mysql_usr->login, db_name, tbl_name);
			
			if (mysql_rght != NULL) {
				g_hash_table_insert(mysql_usr->hsh_tbl_list_rights, g_strdup(str_key->str), mysql_rght);
			}
			/*
			mysql_rght = mysql_right_new_table_column(mysql_usr->mysql_srv, mysql_usr->host, mysql_usr->login, db_name, tbl_name);
			
			if (mysql_rght != NULL) {
				g_hash_table_insert(mysql_usr->hsh_tbl_col_list_rights, g_strdup(str_key->str), mysql_rght);
			}
			*/
			g_array_free(arRow, TRUE);
		}
	}
	
	g_string_free(strSql, TRUE);
	g_string_free(str_key, TRUE);
	mysql_query_delete(mysql_qry);
	
	return TRUE;
}
