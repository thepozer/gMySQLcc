
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
	mysql_usr->hshRights = g_hash_table_new(&g_str_hash, &g_str_equal);
	mysql_usr->found = TRUE;
	mysql_usr->updated = FALSE;

	return mysql_usr;
}

gboolean mysql_user_delete (p_mysql_user mysql_usr) {
	
	if (mysql_usr == NULL) {
		return TRUE;
	}
	
	g_free(mysql_usr->login);
	g_free(mysql_usr->host);
	g_free(mysql_usr->passwd);
	
	mysql_user_clean_rights_list(mysql_usr);
	g_hash_table_destroy(mysql_usr->hshRights);
	
	g_free(mysql_usr);
	return TRUE;
}

gboolean mysql_user_clean_rights_list (p_mysql_user mysql_usr) {
	
	gboolean htr_remove_user_right(gpointer key, gpointer value, gpointer user_data) {
		g_free(key);
		g_free(value);
		return TRUE;
	}
	
	if (mysql_usr->hshRights != NULL) {
		g_hash_table_foreach_steal(mysql_usr->hshRights, &htr_remove_user_right, NULL);
	}
	
	return TRUE;
}

gboolean mysql_user_read_rights (p_mysql_user mysql_usr) {
	p_mysql_query mysql_qry;
	GArray * arRow, * arHeaders;
	GString * strSql;
	gchar * right, * value;
	gint idx;
	
	mysql_user_clean_rights_list(mysql_usr);
	
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

gboolean mysql_user_update_from_db (p_mysql_user mysql_usr) {
	mysql_user_read_rights(mysql_usr);
	
	return TRUE;
}
