
#include "mysql_db_all.h"

p_mysql_user mysql_user_new (p_mysql_server mysql_srv, const gchar * login, const gchar * host) {
	p_mysql_user mysql_usr;
	
	mysql_usr = (p_mysql_user) g_try_malloc(sizeof(p_mysql_user));
	
	if (mysql_usr == (p_mysql_user)NULL) {
		return (p_mysql_user)NULL; /* return NULL pointer */
	}
	
	mysql_usr->mysql_srv = mysql_srv;
	mysql_usr->login = g_strdup(login);
	mysql_usr->host = g_strdup(host);
	mysql_usr->hshRights = g_hash_table_new(&g_str_hash, &g_str_equal);
	
	return mysql_usr;
}

gboolean mysql_user_delete (p_mysql_user mysql_usr) {
	
	if (mysql_usr == (p_mysql_user)NULL) {
		return TRUE;
	}
	
	g_free(mysql_usr->login);
	g_free(mysql_usr->host);
	
	/*mysql_user_clean_rights_list(mysql_usr);*/
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
	
	if (mysql_usr->hshRights != (GHashTable *) NULL) {
		g_hash_table_foreach_steal(mysql_usr->hshRights, &htr_remove_user_right, (void *)NULL);
	}
	
	return TRUE;
}

gboolean mysql_user_read_rights (p_mysql_user mysql_usr) {
	p_mysql_query mysql_qry;
	GArray * arRow;
	gchar * right, * value;
	GString * strSql;
	
	mysql_user_clean_rights_list(mysql_usr);
	
	mysql_qry = mysql_server_query(mysql_usr->mysql_srv, "mysql");
	
	strSql = g_string_new("");
	g_string_printf(strSql, "SELECT * FROM `mysql`.`user` WHERE Host = '%s' AND User = '%s'", mysql_usr->host, mysql_usr->login);
	
	if (mysql_query_execute_query(mysql_qry, strSql->str, FALSE)) {
	
		arRow = mysql_query_get_next_record(mysql_qry);
		
		while (arRow != (GArray *)NULL) {
			
			right = g_array_index(arRow, gchar *, 0);
			value = g_array_index(arRow, gchar *, 1);
			
			g_array_free(arRow, TRUE);
			arRow = mysql_query_get_next_record(mysql_qry);
		}
	} else {
		return FALSE;
	}
	
	g_string_free(strSql, TRUE);
	mysql_query_delete(mysql_qry);
	
	return TRUE;
}
