
#include "mysql_db_all.h"

p_mysql_right mysql_right_new (p_mysql_server mysql_srv);
gboolean mysql_right_read_user (p_mysql_right mysql_rght);
gboolean mysql_right_update_user (p_mysql_right mysql_rght, const gchar * right, const gchar * new_value);
gboolean mysql_right_read_database (p_mysql_right mysql_rght);
gboolean mysql_right_update_database (p_mysql_right mysql_rght, const gchar * right, const gchar * new_value);

p_mysql_right mysql_right_new (p_mysql_server mysql_srv) {
	p_mysql_right mysql_rght;
	
	mysql_rght = (p_mysql_right) g_try_malloc(sizeof(s_mysql_right));
	
	if (mysql_rght == NULL) {
		return NULL; /* return NULL pointer */
	}
	
	mysql_rght->type = UserSubRightType_Undefined;
	mysql_rght->mysql_srv = mysql_srv;
	mysql_rght->host = NULL;
	mysql_rght->login = NULL;
	mysql_rght->db = NULL;
	mysql_rght->table = NULL;
	mysql_rght->column = NULL;
	mysql_rght->routine_name = NULL;
	mysql_rght->routine_type = NULL;
	
	mysql_rght->hsh_rights = g_hash_table_new_full(&g_str_hash, &g_str_equal, &g_free, &g_free);
	
	return mysql_rght;
}

p_mysql_right mysql_right_new_user (p_mysql_server mysql_srv, const gchar * Host, const gchar * Login) {
	p_mysql_right mysql_rght;
	
	mysql_rght = mysql_right_new(mysql_srv);
	
	if (mysql_rght == NULL) {
		return NULL; /* return NULL pointer */
	}
	
	mysql_rght->type = UserSubRightType_User;
	mysql_rght->host = g_strdup(Host);
	mysql_rght->login = g_strdup(Login);
	
	if (!mysql_right_read_user(mysql_rght)) {
		mysql_right_delete(mysql_rght);
		mysql_rght = NULL;
	}
	
	return mysql_rght;
}

p_mysql_right mysql_right_new_database (p_mysql_server mysql_srv, const gchar * Host, const gchar * Login, const gchar * Db) {
	p_mysql_right mysql_rght;
	
	mysql_rght = mysql_right_new(mysql_srv);
	
	if (mysql_rght == NULL) {
		return NULL; /* return NULL pointer */
	}
	
	mysql_rght->type = UserSubRightType_Database;
	mysql_rght->host = g_strdup(Host);
	mysql_rght->login = g_strdup(Login);
	mysql_rght->db = g_strdup(Db);
	
	if (!mysql_right_read_user(mysql_rght)) {
		mysql_right_delete(mysql_rght);
		mysql_rght = NULL;
	}
	
	return mysql_rght;
}

gboolean mysql_right_delete (p_mysql_right mysql_rght) {
	
	if (mysql_rght == NULL) {
		return TRUE;
	}
	
	g_free(mysql_rght->host);
	g_free(mysql_rght->login);
	g_free(mysql_rght->db);
	g_free(mysql_rght->table);
	g_free(mysql_rght->column);
	g_free(mysql_rght->routine_name);
	g_free(mysql_rght->routine_type);
	
	g_hash_table_destroy(mysql_rght->hsh_rights);
	
	g_free(mysql_rght);
	return TRUE;
}

gboolean mysql_right_read (p_mysql_right mysql_rght) {
	gboolean ret = FALSE;
	
	if (mysql_rght == NULL) {
		return FALSE;
	}
	
	switch (mysql_rght->type) {
		case UserSubRightType_Undefined :
			ret = FALSE;
			break;
		case UserSubRightType_Host :
			
			break;
		case UserSubRightType_User :
			ret = mysql_right_read_user(mysql_rght);
			break;
		case UserSubRightType_Database :
			
			break;
		case UserSubRightType_Table :
			
			break;
		case UserSubRightType_TableColumn :
			
			break;
		case UserSubRightType_Column :
			
			break;
		case UserSubRightType_Proc :
			
			break;
	}
	
	return ret;
}

gboolean mysql_right_update (p_mysql_right mysql_rght, const gchar * right, const gchar * new_value) {
	gboolean ret = FALSE;
	
	if (mysql_rght == NULL) {
		return FALSE;
	}
	
	switch (mysql_rght->type) {
		case UserSubRightType_Undefined :
			ret = FALSE;
			break;
		case UserSubRightType_Host :
			
			break;
		case UserSubRightType_User :
			ret = mysql_right_update_user(mysql_rght, right, new_value);
			break;
		case UserSubRightType_Database :
			
			break;
		case UserSubRightType_Table :
			
			break;
		case UserSubRightType_TableColumn :
			
			break;
		case UserSubRightType_Column :
			
			break;
		case UserSubRightType_Proc :
			
			break;
	}
	
	if (ret) {
		mysql_right_read(mysql_rght);
	}
	
	return ret;
}

gboolean mysql_right_read_user (p_mysql_right mysql_rght) {
	p_mysql_query mysql_qry;
	GArray * arRow, * arHeaders;
	GString * strSql;
	gchar * right, * value;
	gint idx;
	
	if (mysql_rght == NULL) {
		return FALSE;
	}
	
	g_hash_table_destroy(mysql_rght->hsh_rights);
	mysql_rght->hsh_rights = g_hash_table_new_full(&g_str_hash, &g_str_equal, &g_free, &g_free);
	
	mysql_qry = mysql_server_query(mysql_rght->mysql_srv, "mysql");
	
	strSql = g_string_new("");
	g_string_printf(strSql, "SELECT * FROM `mysql`.`user` WHERE Host = '%s' AND User = '%s'", mysql_rght->host, mysql_rght->login);
	
	if (mysql_query_execute_query(mysql_qry, strSql->str, FALSE)) {
	
		arRow = mysql_query_get_next_record(mysql_qry);
		arHeaders = mysql_query_get_headers(mysql_qry);
		
		if (arRow != NULL && arHeaders != NULL) {
			
			for (idx = 0; idx < arRow->len; idx++) {
				right = g_array_index(arHeaders, gchar *, idx);
				value = g_array_index(arRow, gchar *, idx);
				
				if (g_ascii_strncasecmp(right, "Host", 4) != 0 && g_ascii_strncasecmp(right, "User", 4) != 0 && g_ascii_strncasecmp(right, "Password", 8) != 0) {
					g_hash_table_insert(mysql_rght->hsh_rights, g_strdup(right), g_strdup(value));
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

gboolean mysql_right_update_user (p_mysql_right mysql_rght, const gchar * right, const gchar * new_value) {
	p_mysql_query mysql_qry;
	GString * strSql;
	gboolean ret_update;

	mysql_qry = mysql_server_query(mysql_rght->mysql_srv, "mysql");
	
	strSql = g_string_new("");
	g_string_printf(strSql, "UPDATE `mysql`.`user` SET `%s`  = '%s' WHERE Host = '%s' AND User = '%s'", right, new_value, mysql_rght->host, mysql_rght->login);
	
	ret_update = (mysql_query_execute_query(mysql_qry, strSql->str, FALSE));
	
	g_string_free(strSql, TRUE);
	mysql_query_delete(mysql_qry);
	
	return ret_update;
}

gboolean mysql_right_read_database (p_mysql_right mysql_rght) {
	p_mysql_query mysql_qry;
	GArray * arRow, * arHeaders;
	GString * strSql;
	gchar * right, * value;
	gint idx;
	
	if (mysql_rght == NULL) {
		return FALSE;
	}
	
	g_hash_table_destroy(mysql_rght->hsh_rights);
	mysql_rght->hsh_rights = g_hash_table_new_full(&g_str_hash, &g_str_equal, &g_free, &g_free);
	
	mysql_qry = mysql_server_query(mysql_rght->mysql_srv, "mysql");
	
	strSql = g_string_new("");
	g_string_printf(strSql, "SELECT * FROM `mysql`.`db` WHERE Host = '%s' AND User = '%s' AND Db = '%s'", mysql_rght->host, mysql_rght->login, mysql_rght->db);
	
	if (mysql_query_execute_query(mysql_qry, strSql->str, FALSE)) {
	
		arRow = mysql_query_get_next_record(mysql_qry);
		arHeaders = mysql_query_get_headers(mysql_qry);
		
		if (arRow != NULL && arHeaders != NULL) {
			
			for (idx = 0; idx < arRow->len; idx++) {
				right = g_array_index(arHeaders, gchar *, idx);
				value = g_array_index(arRow, gchar *, idx);
				
				if (g_ascii_strncasecmp(right, "Host", 4) != 0 && g_ascii_strncasecmp(right, "User", 4) != 0 && g_ascii_strncasecmp(right, "Db", 2) != 0) {
					g_hash_table_insert(mysql_rght->hsh_rights, g_strdup(right), g_strdup(value));
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

gboolean mysql_right_update_database (p_mysql_right mysql_rght, const gchar * right, const gchar * new_value) {
	p_mysql_query mysql_qry;
	GString * strSql;
	gboolean ret_update;

	mysql_qry = mysql_server_query(mysql_rght->mysql_srv, "mysql");
	
	strSql = g_string_new("");
	g_string_printf(strSql, "UPDATE `mysql`.`db` SET `%s`  = '%s' WHERE Host = '%s' AND User = '%s' AND Db = '%s'", right, new_value, mysql_rght->host, mysql_rght->login, mysql_rght->db);
	
	ret_update = (mysql_query_execute_query(mysql_qry, strSql->str, FALSE));
	
	g_string_free(strSql, TRUE);
	mysql_query_delete(mysql_qry);
	
	return ret_update;
}
