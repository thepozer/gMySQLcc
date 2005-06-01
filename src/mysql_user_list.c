
#include "mysql_db_all.h"

p_data_list mysql_user_list_new (p_mysql_server	mysql_srv) {
	return mysql_data_list_new(mysql_srv, &g_free, (GDestroyNotify)&mysql_user_delete);
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

p_mysql_user mysql_user_list_create_user (p_data_list mysql_usr_lst, const gchar * login, const gchar * host, const gchar * password, gboolean crypted_password) {
	gboolean created = FALSE;
	GString * str_sql;
	p_mysql_server mysql_srv;
	p_mysql_query mysql_qry;
	p_mysql_user mysql_usr;
	
	str_sql = g_string_new("");
	mysql_srv = (p_mysql_server *)mysql_data_list_get_data(mysql_usr_lst);
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
	
	return mysql_usr;
}
